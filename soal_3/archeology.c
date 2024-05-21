// defines
#define _XOPEN_SOURCE 700 // for S_IFREG
#define FUSE_USE_VERSION 28 // fuse

// debug mode - printing and using foreground fuse with -f
#define DEBUG

// libraries
#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

// constants
static const char *dirpath = "/home/etern1ty/sisop_works/modul_4/soal_3/relics";
const int MAX_PATH = 1024;
const int PART_SIZE = 10000;

static int artifact_getattr(const char *path, struct stat *stbuf) {
    memset(stbuf, 0, sizeof(struct stat));

    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755; // all permissions for owner, read and execute for others
        stbuf->st_nlink = 2;
        return 0;
    }

    char fpath[MAX_PATH]; // full path
    sprintf(fpath, "%s%s", dirpath, path);

    stbuf->st_mode = S_IFREG | 0755; // same thing
    stbuf->st_nlink = 1;
    stbuf->st_size = 0;

    char part[MAX_PATH]; // path of divided file (parts)
    FILE *fp;
    
    int i = 0;

    #ifdef DEBUG
    printf("getattr: [%s][%d]\n", path, i);
    #endif

    while (1) {
        sprintf(part, "%s.%03d", fpath, i++); // looping through parts
        fp = fopen(part, "rb");
        if (!fp) break;

        fseek(fp, 0L, SEEK_END);
        stbuf->st_size += ftell(fp); // adds file size to the total size
        fclose(fp);
    }
    if (i == 1) return -errno;
    return 0;
}

static int artifact_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    (void) offset;
    (void) fi;

    filler(buf, ".", NULL, 0); // skips cur dir
    filler(buf, "..", NULL, 0); // skips parent dir

    DIR *dp;
    struct dirent *de;
    dp = opendir(dirpath);
    if (dp == NULL) return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        if (strstr(de->d_name, ".000") == NULL) continue; // skips if it isnt a file's part

        char relic[MAX_PATH];
        strcpy(relic, de->d_name);
        relic[strlen(de->d_name) - 4] = '\0'; // removes the ".000-.nnn" part
        
        #ifdef DEBUG
        printf("readdir: [%s] as [%s]\n", de->d_name, relic);
        #endif

        if (filler(buf, relic, &st, 0)) break;
    }
    closedir(dp);
    return 0;
}

static int artifact_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    (void) fi;
    char fpath[MAX_PATH]; 
    sprintf(fpath, "%s%s", dirpath, path); // full path

    // reading variables
    char part[MAX_PATH];
    size_t read_len;
    size_t read_size = 0;

    int i = 0;
    while (size > 0) {
        sprintf(part, "%s.%03d", fpath, i++); // looping through parts
        FILE *fp = fopen(part, "rb");
        if (!fp) break;

        #ifdef DEBUG
        printf("read: [%s]\n", part);
        #endif

        fseek(fp, 0L, SEEK_END); 
        size_t part_size = ftell(fp); // get the size of the part
        fseek(fp, 0L, SEEK_SET);

        if (offset >= part_size) { // skip if offset is larger than part's size
            offset -= part_size;
            fclose(fp);
            continue;
        }

        fseek(fp, offset, SEEK_SET);
        read_len = fread(buf, 1, size, fp); // read file
        fclose(fp);

        // buffer increases, size decreases, read_size increases for each iteration
        buf += read_len;
        size -= read_len;
        read_size += read_len;

        offset = 0;
    }
    return read_size;
}

static int artifact_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    (void) fi;
    char fpath[MAX_PATH];
    sprintf(fpath, "%s%s", dirpath, path); // full path

    #ifdef DEBUG
    printf("init: [%s]\n", path);
    #endif
    
    // writing variables
    int cur_part = offset / PART_SIZE;
    size_t part_offset = offset % PART_SIZE;
    size_t write_size_total = 0;
    char part[MAX_PATH]; 

    while (size > 0) {
        sprintf(part, "%s.%03d", fpath, cur_part++); // looping through parts
        FILE *fp = fopen(part, "r+b");
        if (!fp) {
            fp = fopen(part, "wb");
            if (!fp) return -errno;
        }

        #ifdef DEBUG
        printf("write: [%s][%d] size: %ld offset: %ld\n", path, cur_part, size, offset);
        #endif

        fseek(fp, part_offset, SEEK_SET);
        size_t write_size;
        // find size    
        if (size > (PART_SIZE - part_offset)) write_size = PART_SIZE - part_offset; 
        else write_size = size;

        fwrite(buf, 1, write_size, fp); // write to the file
        fclose(fp);

        // buffer decreases, size decreases, write_size_total increases for each iteration
        buf += write_size;
        size -= write_size;
        write_size_total += write_size;
        part_offset = 0; // reset offset for next iteration
    }
    return write_size_total;
}

static int artifact_unlink(const char *path) {
    char fpath[MAX_PATH];
    sprintf(fpath, "%s%s", dirpath, path); // full path

    // unlinking variables
    int cur_part = 0;
    char part[MAX_PATH];
    int res = 0;

    while (1) {
        #ifdef DEBUG
        printf("unlink: [%s][%d]\n", path, cur_part);
        #endif

        sprintf(part, "%s.%03d", fpath, cur_part++); // looping through parts
        res = unlink(part); // unlink file
        
        if (res == -1) {
            if (errno == ENOENT) break;
            else return -errno;
        }
    }
    return 0;
}

static int artifact_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    (void) fi;
    char fpath[MAX_PATH];
    sprintf(fpath, "%s%s.000", dirpath, path); // full path

    #ifdef DEBUG
    printf("arccreate (:eyes:) [%s]\n", path);
    #endif

    int res = creat(fpath, mode); // create file
    if (res == -1) return -errno;

    close(res);
    return 0;
}

static int artifact_truncate(const char *path, off_t size) { // writing and reading needs truncation
    char fpath[MAX_PATH];
    sprintf(fpath, "%s%s", dirpath, path); // full path

    // truncating variables
    int cur_part = 0;
    char part[MAX_PATH];
    off_t size_remain = size;

    while (size_remain > 0) {
        #ifdef DEBUG
        printf("truncate: [%s][%d]\n", path, cur_part);
        #endif

        sprintf(part, "%s.%03d", fpath, cur_part++); // looping through parts
        size_t part_size;
        // find size
        if (size_remain > PART_SIZE) part_size = PART_SIZE;
        else part_size = size_remain;

        int res = truncate(part, part_size); // truncate file
        if (res == -1) return -errno;

        size_remain -= part_size; // decrease remaining size
    }
    while (1) {
        #ifdef DEBUG
        printf("truncate - unlink: [%s][%d]\n", path, cur_part);
        #endif

        sprintf(part, "%s.%03d", fpath, cur_part++); // looping through parts
        int res = unlink(part); // unlink file
        if (res == -1) {
            if (errno == ENOENT) break;
            else return -errno;
        }
    }
    return 0;
}

// fuse's lib
static struct fuse_operations artifact_oper = {
    .getattr = artifact_getattr,
    .readdir = artifact_readdir,
    .read = artifact_read,
    .write = artifact_write,
    .unlink = artifact_unlink,
    .create = artifact_create,
    .truncate = artifact_truncate,
};

int main(int argc, char *argv[]) {
    umask(0);
    return fuse_main(argc, argv, &artifact_oper, NULL);
}

// Point 3
// Samba conf:
// [<>]
//     comment = Report - Soal 4 Modul 3 Sisop
//     path = /home/etern1ty/sisop_works/modul_4/soal_3/report
//     read only = yes
//     browsable = yes
//     writable = no
//     guest ok = yes

// How to connect: smb://<ip addr>/<>
// ex: smb://10.4.66.198/mod4soal3
// username: etern1ty, password: a