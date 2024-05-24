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
#include <stdlib.h>

static const char* dirpath = "/Users/macbook/Kuliah/Sistem Operasi/sisop4/soal_1/vfs/portofolio";
// static const char* dirpath = "/vagrant/vfs/portofolio";

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

  res = mkdir(fpath, mode);
  if (res == -1)
    return -errno;

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

static int hello_open(const char* path, struct fuse_file_info* fi) {
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  int res;

  res = open(fpath, fi->flags);
  if (res == -1)
    return -errno;

  close(res);
  return 0;
  }

static int hello_readdir(const char* path, void* buf, fuse_fill_dir_t filler,
  off_t offset, struct fuse_file_info* fi) {
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);

  DIR* dp;
  struct dirent* de;

  dp = opendir(fpath);
  if (dp == NULL)
    return -errno;

  if (strcmp(path, "/bahaya") == 0) {
    char cmd[1000];
    sprintf(cmd, "chmod +x %s/bahaya/script.sh", dirpath);
    system(cmd);
    }

  while ((de = readdir(dp)) != NULL) {
    struct stat st;
    memset(&st, 0, sizeof(st));
    st.st_ino = de->d_ino;
    st.st_mode = de->d_type << 12;
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

static int hello_rmdir(const char* path) {
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  int res;

  res = rmdir(fpath);
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

  int should_reverse = (strncmp(path, "/bahaya/test", 12) == 0);

  char* temp_buf = (char*)malloc(size);
  int res = pread(fd, temp_buf, size, offset);
  if (res == -1) {
    free(temp_buf);
    close(fd);
    return -errno;
    }

  if (should_reverse) {
    for (size_t i = 0; i < res; i++) {
      buf[i] = temp_buf[res - 1 - i];
      }
    }
  else {
    memcpy(buf, temp_buf, res);
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

  if (strncmp(to, "/gallery/wm-", 12) == 0) {
    char cmd[1000];
    sprintf(cmd, "magick '%s' -fill white -pointsize 50 -gravity South -annotate +0+5 inikaryakita.id '%s'", to_path, to_path);
    system(cmd);
    }

  return 0;
  }

static int hello_write(const char* path, const char* buf, size_t size, off_t offset,
  struct fuse_file_info* fi) {
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  int fd;

  if (fi->flags & O_CREAT) {
    fd = open(fpath, fi->flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
    if (fd == -1)
      return -errno;
    }
  else {
    fd = open(fpath, fi->flags);
    if (fd == -1)
      return -errno;
    }

  int should_reverse = (strncmp(path, "/bahaya/test", 12) == 0);

  char* temp_buf = (char*)malloc(size);
  if (should_reverse) {
    memcpy(temp_buf, buf, size);
    for (size_t i = 0; i < size; i++) {
      temp_buf[i] = buf[size - 1 - i];
      }
    }
  else {
    memcpy(temp_buf, buf, size);
    }

  int res = pwrite(fd, temp_buf, size, offset);
  if (res == -1)
    res = -errno;

  free(temp_buf);
  close(fd);
  return res;
  }

static struct fuse_operations hello_oper = {
    .getattr = hello_getattr,
    .mkdir = hello_mkdir,
    .create = hello_create,
    .open = hello_open,
    .readdir = hello_readdir,
    .unlink = hello_unlink,
    .rmdir = hello_rmdir,
    .read = hello_read,
    .chmod = hello_chmod,
    .rename = hello_rename,
    .write = hello_write,
  };

int main(int argc, char* argv[]) {
  umask(0);
  return fuse_main(argc, argv, &hello_oper, NULL);
  }
