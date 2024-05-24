# Repository Praktikum Sistem Operasi Modul 3 - IT04

## Anggota

| Nama                      | NRP        |
| ------------------------- | ---------- |
| Nathan Kho Pancras        | 5027231002 |
| Athalla Barka Fadhil      | 5027231018 |
| Muhammad Ida Bagus Rafi H | 5027221059 |

## Daftar Isi

- [Soal 1](#soal-1)
- [Soal 2](#soal-2)
- [Soal 3](#soal-3)

# Soal 1

### Dikerjakan oleh Athalla Barka Fadhil (5027231018)

## Deskripsi Soal

### Catatan

## Pengerjaan

Berikut adalah dokumentasi kode dalam format Markdown, dengan penjelasan pada setiap blok kode yang relevan. Kode akan dikelompokkan sesuai dengan fungsinya.

# Virtual File System dengan FUSE

Kode ini merupakan implementasi dari sebuah virtual file system menggunakan FUSE (Filesystem in Userspace). Fungsi-fungsi yang diimplementasikan meliputi operasi file dasar seperti membaca, menulis, membuat direktori, menghapus file, dan lain-lain.

## Inisialisasi dan Konfigurasi

```c
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
```

Bagian ini mendefinisikan versi FUSE yang digunakan dan meng-include header file yang diperlukan untuk implementasi virtual file system. Variabel `dirpath` menyimpan lokasi direktori fisik yang akan digunakan sebagai basis untuk file system virtual.

## Operasi File

### Mendapatkan Atribut File/Direktori

```c
static int hello_getattr(const char* path, struct stat* stbuf) {
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  int res;

  res = lstat(fpath, stbuf);
  if (res == -1)
    return -errno;

  return 0;
}
```

Fungsi `hello_getattr` digunakan untuk mendapatkan atribut dari sebuah file atau direktori, seperti ukuran, waktu modifikasi, dan hak akses. Fungsi ini memanggil `lstat` pada `fpath`, yang merupakan gabungan dari `dirpath` dan `path` (path relatif dalam file system virtual). Atribut yang didapatkan disimpan dalam struktur `stat` yang diteruskan sebagai argumen.

### Membuat Direktori Baru

```c
static int hello_mkdir(const char* path, mode_t mode) {
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  int res;

  res = mkdir(fpath, mode);
  if (res == -1)
    return -errno;

  return 0;
}
```

Fungsi `hello_mkdir` digunakan untuk membuat direktori baru dalam file system virtual. Fungsi ini memanggil `mkdir` pada `fpath`, yang merupakan gabungan dari `dirpath` dan `path` (path relatif dalam file system virtual). `mode` menentukan hak akses yang akan diberikan pada direktori yang baru dibuat.

### Membuat File Baru

```c
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
```

Fungsi `hello_create` digunakan untuk membuat file baru dalam file system virtual. Fungsi ini memanggil `open` pada `fpath`, yang merupakan gabungan dari `dirpath` dan `path` (path relatif dalam file system virtual). `mode` menentukan hak akses yang akan diberikan pada file yang baru dibuat, dan `fi->flags` menentukan mode pembukaan file (seperti read-only, write-only, atau read-write).

### Membuka File

```c
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
```

Fungsi `hello_open` digunakan untuk membuka file dalam file system virtual. Fungsi ini memanggil `open` pada `fpath`, yang merupakan gabungan dari `dirpath` dan `path` (path relatif dalam file system virtual). `fi->flags` menentukan mode pembukaan file (seperti read-only, write-only, atau read-write).

### Membaca Daftar Direktori

```c
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
```

Fungsi `hello_readdir` digunakan untuk membaca daftar file dan direktori dalam sebuah direktori pada file system virtual. Fungsi ini memanggil `opendir` pada `fpath`, yang merupakan gabungan dari `dirpath` dan `path` (path relatif dalam file system virtual).

Selain itu, fungsi ini juga mengandung kode khusus untuk memberikan izin eksekusi (`chmod +x`) pada file `script.sh` yang berada di direktori `/bahaya/` setiap kali direktori tersebut dibaca.

Fungsi `hello_readdir` akan memanggil `filler` untuk setiap entry dalam direktori, yang akan menyediakan informasi seperti nama file/direktori dan atribut file (seperti inode dan jenis file).

### Menghapus File

```c
static int hello_unlink(const char* path) {
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  int res;

  res = unlink(fpath);
  if (res == -1)
    return -errno;

  return 0;
}
```

Fungsi `hello_unlink` digunakan untuk menghapus file dalam file system virtual. Fungsi ini memanggil `unlink` pada `fpath`, yang merupakan gabungan dari `dirpath` dan `path` (path relatif dalam file system virtual).

### Menghapus Direktori

```c
static int hello_rmdir(const char* path) {
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  int res;

  res = rmdir(fpath);
  if (res == -1)
    return -errno;

  return 0;
}
```

Fungsi `hello_rmdir` digunakan untuk menghapus direktori dalam file system virtual. Fungsi ini memanggil `rmdir` pada `fpath`, yang merupakan gabungan dari `dirpath` dan `path` (path relatif dalam file system virtual). Jika operasi penghapusan direktori berhasil, fungsi akan mengembalikan 0. Jika terjadi kesalahan, fungsi akan mengembalikan nilai error yang sesuai.

### Membaca Isi File

```c
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
```

Fungsi `hello_read` digunakan untuk membaca isi dari sebuah file dalam file system virtual. Fungsi ini membuka file menggunakan `open` pada `fpath`, yang merupakan gabungan dari `dirpath` dan `path` (path relatif dalam file system virtual). Kemudian, menggunakan `pread` untuk membaca isi file ke dalam buffer sementara `temp_buf`.

Jika path file dimulai dengan `/bahaya/test-`, maka isi file akan dibalik urutannya sebelum disalin ke buffer `buf` yang diberikan sebagai argumen. Jika tidak, isi file akan disalin secara langsung ke buffer `buf`.

### Mengubah Hak Akses File/Direktori

```c
static int hello_chmod(const char* path, mode_t mode) {
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  int res;

  res = chmod(fpath, mode);
  if (res == -1)
    return -errno;

  return 0;
}
```

Fungsi `hello_chmod` digunakan untuk mengubah hak akses (mode) dari sebuah file atau direktori dalam file system virtual. Fungsi ini memanggil `chmod` pada `fpath`, yang merupakan gabungan dari `dirpath` dan `path` (path relatif dalam file system virtual). `mode` adalah hak akses baru yang akan diberikan.

### Mengubah Nama File/Direktori

```c
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
```

Fungsi `hello_rename` digunakan untuk mengubah nama file atau direktori dalam file system virtual. Fungsi ini memanggil `rename` pada `from_path` dan `to_path`, yang merupakan gabungan dari `dirpath` dan `from` serta `to` (path relatif lama dan baru dalam file system virtual).

Selain itu, fungsi ini juga mengandung kode khusus untuk menambahkan watermark pada file yang namanya dimulai dengan `/gallery/wm-` menggunakan utilitas `magick`.

### Menulis ke File

```c
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
```

Fungsi `hello_write` digunakan untuk menulis data ke sebuah file dalam file system virtual. Fungsi ini membuka file menggunakan `open` pada `fpath`, yang merupakan gabungan dari `dirpath` dan `path` (path relatif dalam file system virtual). Jika file tidak ada dan `O_CREAT` diatur dalam `fi->flags`, maka file baru akan dibuat dengan hak akses tertentu.

Jika path file dimulai dengan `/bahaya/test`, maka data yang akan ditulis akan dibalik urutannya sebelum ditulis ke file menggunakan `pwrite`. Jika tidak, data akan ditulis secara langsung ke file.

## Struktur Operasi FUSE

```c
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
```

Bagian ini mendefinisikan struktur `fuse_operations` yang berisi pointer ke fungsi-fungsi operasi file yang telah diimplementasikan sebelumnya. Struktur ini akan digunakan oleh FUSE untuk menangani permintaan operasi file dari pengguna atau aplikasi lain yang mengakses file system virtual ini.

## Fungsi Main

```c
int main(int argc, char* argv[]) {
  umask(0);
  return fuse_main(argc, argv, &hello_oper, NULL);
}
```

Fungsi `main` merupakan titik masuk utama dari program ini. Fungsi ini memanggil `fuse_main` dengan argumen `hello_oper` (struktur operasi FUSE yang telah didefinisikan sebelumnya) dan `argc` serta `argv` yang diteruskan dari baris perintah. `umask(0)` digunakan untuk memastikan bahwa tidak ada pembatasan hak akses default yang diterapkan pada file atau direktori yang dibuat.

## Output

# Soal 2

### Dikerjakan oleh Muhammad Ida Bagus Rafi Habibie (5027221059)

## Deskripsi Soal

### Catatan

## Pengerjaan

## Output

# Soal 3

### Dikerjakan oleh Nathan Kho Pancras (5027231002)

## Deskripsi Soal

Seorang arkeolog menemukan sebuah gua yang didalamnya tersimpan banyak relik dari zaman praaksara, sayangnya semua barang yang ada pada gua tersebut memiliki bentuk yang terpecah belah akibat bencana yang tidak diketahui. Sang arkeolog ingin menemukan cara cepat agar ia bisa menggabungkan relik-relik yang terpecah itu, namun karena setiap pecahan relik itu masih memiliki nilai tersendiri, ia memutuskan untuk membuat sebuah file system yang mana saat ia mengakses file system tersebut ia dapat melihat semua relik dalam keadaan utuh, sementara relik yang asli tidak berubah sama sekali.

### Catatan

Directory tree:

```
.
├── fuse_dir
├── relics
│   ├── relic_1.png.000
│   ├── relic_1.png.001
│   ├── ...
│   └── relic_9.png.010
└── report
```

[nama_bebas] = fuse_dir

Define yang digunakan:

```c
// defines
#define _XOPEN_SOURCE 700 // for S_IFREG
#define FUSE_USE_VERSION 28 // fuse

// debug mode - printing and using foreground fuse with -f
#define DEBUG
```

Constants yang digunakan:

```c
static const char *dirpath = "/home/etern1ty/sisop_works/modul_4/soal_3/relics";
const int MAX_PATH = 1024;
const int PART_SIZE = 10000;
```

## Pengerjaan

> a. Buatlah sebuah direktori dengan ketentuan seperti pada tree berikut

```
.
├── [nama_bebas]
├── relics
│   ├── relic_1.png.000
│   ├── relic_1.png.001
│   ├── dst dst…
│   └── relic_9.png.010
└── report
```

Untuk [nama_bebas], saya memakai fuse_dir sebagai nama folder.

> b. Direktori [nama_bebas] adalah direktori FUSE dengan direktori asalnya adalah direktori relics. Ketentuan Direktori [nama_bebas] adalah sebagai berikut:
>
> > Ketika dilakukan listing, isi dari direktori [nama_bebas] adalah semua relic dari relics yang telah tergabung.

Pertama, agar FUSE sendiri bekerja untuk listing di fuse_dir, kita memerlukan beberapa fungsi, yaitu:

- getattr
- readdir

Untuk deklarasi fungsi didapatkan dari `fuse.h`.

**getattr**

```c
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
```

Untuk getattr, pertama kita inisialisasi struct stbuf dan set mem ke 0, ditambah setting permissionsnya dengan S_IFREG dan mode 0755. Kita juga membuat fpath (full path) yang diambil dari dirpath dan path. Kemudian di main loop, kita loop semua part dari 000 (i = 0) sampai selesai menggunakan seek, dan menambahkan sizenya ke size total.

**readdir**

```c
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

```

Untuk readdir,

> > Ketika dilakukan copy (dari direktori [nama_bebas] ke tujuan manapun), file yang disalin adalah file dari direktori relics yang sudah tergabung.

Untuk melakukan copy, kita memerlukan fungsi lain, yaitu:

- read
- truncate

**read**

```c
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
```

Untuk read,

> > Ketika ada file dibuat, maka pada direktori asal (direktori relics) file tersebut akan dipecah menjadi sejumlah pecahan dengan ukuran maksimum tiap pecahan adalah 10kb.

Jika ada file baru yang di cp, dan kita memecahnya dengan ukuran max 10kb, kita memerlukan fungsi baru, yaitu:

- write
- create

**write**

```c
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
```

Untuk write,

**create**

```c
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
```

Untuk create,

> > File yang dipecah akan memiliki nama [namafile].000 dan seterusnya sesuai dengan jumlah pecahannya.
> > Ketika dilakukan penghapusan, maka semua pecahannya juga ikut terhapus.

Terakhir, untuk melakukan penghapusan, kita memerlukan fungsi unlink.

**unlink**

```c
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
```

Untuk unlink,

Kemudian semua fungsi dimasukkan kedalam struct fuse_operations:

```c
static struct fuse_operations artifact_oper = {
    .getattr = artifact_getattr,
    .readdir = artifact_readdir,
    .read = artifact_read,
    .write = artifact_write,
    .unlink = artifact_unlink,
    .create = artifact_create,
    .truncate = artifact_truncate,
};
```

**int main**

```c
int main(int argc, char *argv[]) {
    umask(0);
    return fuse_main(argc, argv, &artifact_oper, NULL);
}
```

> c. Direktori report adalah direktori yang akan dibagikan menggunakan Samba File Server. Setelah kalian berhasil membuat direktori [nama_bebas], jalankan FUSE dan salin semua isi direktori [nama_bebas] pada direktori report

1. Salin file secara manual: cp fuse_dir/. report/
2. Input konfigurasi samba

```conf
Samba conf:
[<>]
    comment = Report - Soal 4 Modul 3 Sisop
    path = /home/etern1ty/sisop_works/modul_4/soal_3/report
    read only = yes
    browsable = yes
    writable = no
    guest ok = yes
```

3. Cari ip address dengan ip addr
4. Input ke smb://<ip addr>/<>
5. Masuk dengan username etern1ty, password a

## Output
