#define FUSE_USE_VERSION 31

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <ftw.h>
#include <stdlib.h>

static const char* dirpath = "/Users/macbook/Kuliah/Sistem Operasi/modul-sisop-4/vfs/portofolio";

static int hello_getattr(const char* path, struct stat* stbuf) {
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  int res;

  res = lstat(fpath, stbuf);
  if (res == -1)
    return -errno;

  return 0;
  }

static int hello_mkdir(const char* path, mode_t mode) {
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  int res;

  char* prefix = "wm-";
  if (strncmp(path, "/gallery/", 9) == 0 && strncmp(path + 9, prefix, 3) == 0) {
    res = mkdir(fpath, mode);
    if (res == -1)
      return -errno;
    }
  else {
    res = mkdir(fpath, mode);
    if (res == -1)
      return -errno;
    }

  return 0;
  }

static int hello_create(const char* path, mode_t mode, struct fuse_file_info* fi) {
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  int res;

  res = open(fpath, fi->flags, mode);
  if (res == -1)
    return -errno;

  close(res);
  return 0;
  }



static int hello_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi) {
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);

  DIR* dp;
  struct dirent* de;

  dp = opendir(fpath);
  if (dp == NULL)
    return -errno;

  while ((de = readdir(dp)) != NULL) {
    struct stat st;
    memset(&st, 0, sizeof(st));
    st.st_ino = de->d_ino;
    st.st_mode = de->d_type << 12;

    char* filename = de->d_name;

    if (strcmp(path, "/gallery") == 0 && strncmp(filename, "wm-", 3) == 0) {
      char src_path[1000];
      sprintf(src_path, "%s/%s", fpath, filename);

      char dest_path[1000];
      sprintf(dest_path, "%s/%s_watermarked.jpg", fpath, filename);

      char cmd[1000];
      sprintf(cmd, "convert '%s' -fill white -gravity South -annotate +0+5 inikaryakita.id '%s'", src_path, dest_path);
      system(cmd);
      }
    else if (strcmp(path, "/bahaya") == 0 && strcmp(filename, "script.sh") == 0) {
      char script_path[1000];
      sprintf(script_path, "%s/script.sh", fpath);
      chmod(script_path, 0755);
      }

    if (filler(buf, de->d_name, &st, 0))
      break;
    }

  closedir(dp);
  return 0;
  }

static int hello_unlink(const char* path) {
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  int res;

  res = unlink(fpath);
  if (res == -1)
    return -errno;

  return 0;
  }

static int hello_read(const char* path, char* buf, size_t size, off_t offset, struct fuse_file_info* fi) {
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  int fd = open(fpath, O_RDONLY);
  if (fd == -1)
    return -errno;

  // Read the file content
  char* temp_buf = (char*)malloc(size);
  int res = pread(fd, temp_buf, size, offset);
  if (res == -1) {
    free(temp_buf);
    close(fd);
    return -errno;
    }

  // Reverse the file content
  for (size_t i = 0; i < res; i++) {
    buf[i] = temp_buf[res - 1 - i];
    }

  free(temp_buf);
  close(fd);
  return res;
  }

static int hello_chmod(const char* path, mode_t mode) {
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  int res;

  res = chmod(fpath, mode);
  if (res == -1)
    return -errno;

  return 0;
  }

static int hello_rename(const char* from, const char* to) {
  char from_path[1000], to_path[1000];
  sprintf(from_path, "%s%s", dirpath, from);
  sprintf(to_path, "%s%s", dirpath, to);
  int res;

  res = rename(from_path, to_path);
  if (res == -1)
    return -errno;

  // Check if the destination path is in a "wm-" prefixed directory inside "gallery"
  if (strncmp(to, "/gallery/wm-", 12) == 0) {
    // Add watermark to the file
    char cmd[1000];
    sprintf(cmd, "convert '%s' -fill white -gravity South -annotate +0+5 inikaryakita.id '%s'", to_path, to_path);
    system(cmd);
    }

  return 0;
  }

static struct fuse_operations hello_oper = {
    .getattr = hello_getattr,
    .mkdir = hello_mkdir,
    .create = hello_create,
    .readdir = hello_readdir,
    .unlink = hello_unlink,
    .read = hello_read,
    .chmod = hello_chmod,
    .rename = hello_rename,
  };

int main(int argc, char* argv[]) {
  umask(0);
  return fuse_main(argc, argv, &hello_oper, NULL);
  }
