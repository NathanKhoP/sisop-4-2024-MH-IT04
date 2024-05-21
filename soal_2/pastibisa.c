#define FUSE_USE_VERSION 30
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include <iostream>
#include <ctime>
#include <algorithm>

const char *mount_path = "/modul4/";

void write_log(const char *status, const char *action, const char *details) {
    std::ofstream log_file("logs-fuse.log", std::ios_base::app);
    if (log_file.is_open()) {
        time_t now = time(nullptr);
        struct tm *local_time = localtime(&now);
        char time_buffer[80];
        strftime(time_buffer, sizeof(time_buffer), "%d/%m/%Y-%H:%M:%S", local_time);
        log_file << "[" << status << "]::" << time_buffer << "::[" << action << "]::[" << details << "]\n";
        log_file.close();
    }
}

std::string decode_base64(const std::string &input) {
    static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string decoded_string;
    size_t len = input.size();
    int i = 0, j = 0, k = 0, val = 0;
    for (i = 0; i < len; i++) {
        val = base64_chars.find(input[i]);
        val = val << ((i % 4) * 2);
        k = k | val;
        if (i % 4 == 3) {
            decoded_string.push_back((char)((k & 0x00FF0000) >> 16));
            decoded_string.push_back((char)((k & 0x0000FF00) >> 8));
            decoded_string.push_back((char)(k & 0x000000FF));
            k = 0;
        }
    }
    return decoded_string;
}

std::string decode_rot13(const std::string &input) {
    std::string output = input;
    for (char &c : output) {
        if (isalpha(c)) {
            if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M')) c += 13;
            else c -= 13;
        }
    }
    return output;
}

std::string decode_hex(const std::string &input) {
    std::string output;
    for (size_t i = 0; i < input.length(); i += 2) {
        std::string byte = input.substr(i, 2);
        char chr = (char)(int)strtol(byte.c_str(), nullptr, 16);
        output.push_back(chr);
    }
    return output;
}

std::string reverse_text(const std::string &input) {
    std::string output = input;
    std::reverse(output.begin(), output.end());
    return output;
}

bool has_prefix(const std::string &str, const std::string &prefix) {
    return str.substr(0, prefix.size()) == prefix;
}

static int custom_getattr(const char *path, struct stat *stbuf) {
    char full_path[1000];
    sprintf(full_path, "%s%s", mount_path, path);
    int res = lstat(full_path, stbuf);
    return (res == -1) ? -errno : 0;
}

static int custom_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    char full_path[1000];
    if (strcmp(path, "/") == 0) {
        sprintf(full_path, "%s", mount_path);
    } else {
        sprintf(full_path, "%s%s", mount_path, path);
    }

    DIR *dp;
    struct dirent *de;
    dp = opendir(full_path);
    if (!dp) return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, de->d_name, &st, 0)) break;
    }

    closedir(dp);
    return 0;
}

static int custom_open(const char *path, struct fuse_file_info *fi) {
    if (strncmp(path, "/rahasia-berkas", strlen("/rahasia-berkas")) == 0) {
        std::string password;
        std::cout << "Enter password to access " << path << ": ";
        std::cin >> password;
        if (password != "password") {
            write_log("FAILED", "access", path);
            return -EACCES;
        }
        write_log("SUCCESS", "access", path);
    }
    return 0;
}

static int custom_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    char full_path[1000];
    sprintf(full_path, "%s%s", mount_path, path);

    int fd = open(full_path, O_RDONLY);
    if (fd == -1) return -errno;

    std::string content;
    std::string filename = std::string(path + 1);

    struct stat st;
    fstat(fd, &st);
    content.resize(st.st_size);
    pread(fd, &content[0], st.st_size, 0);
    close(fd);

    if (has_prefix(filename, "base64_")) {
        content = decode_base64(content);
    } else if (has_prefix(filename, "rot13_")) {
        content = decode_rot13(content);
    } else if (has_prefix(filename, "hex_")) {
        content = decode_hex(content);
    } else if (has_prefix(filename, "rev_")) {
        content = reverse_text(content);
    }

    size_t len = content.size();
    if (offset < len) {
        if (offset + size > len) size = len - offset;
        memcpy(buf, content.c_str() + offset, size);
    } else {
        size = 0;
    }

    return size;
}

static struct fuse_operations custom_oper = {
    .getattr = custom_getattr,
    .readdir = custom_readdir,
    .open = custom_open,
    .read = custom_read,
};

int main(int argc, char *argv[]) {
    umask(0);
    return fuse_main(argc, argv, &custom_oper, nullptr);
}
