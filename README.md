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

Adfi merupakan seorang CEO agency creative bernama Ini Karya Kita. Ia sedang melakukan inovasi pada manajemen project photography Ini Karya Kita. Salah satu ide yang dia kembangkan adalah tentang pengelolaan foto project dalam sistem arsip Ini Karya Kita. Dalam membangun sistem ini, Adfi tidak bisa melakukannya sendirian, dia perlu bantuan mahasiswa Departemen Teknologi Informasi angkatan 2023 untuk membahas konsep baru yang akan mengubah project fotografinya lebih menarik untuk dilihat. Adfi telah menyiapkan portofolio hasil project fotonya yang bisa didownload dan diakses di www.inikaryakita.id . Silahkan eksplorasi web Ini Karya Kita dan temukan halaman untuk bisa mendownload projectnya. Setelah kalian download terdapat folder gallery dan bahaya.

## Pengerjaan

## Virtual File System dengan FUSE

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

Jika path file dimulai dengan `/bahaya/test`, maka isi file akan dibalik urutannya sebelum disalin ke buffer `buf` yang diberikan sebagai argumen. Jika tidak, isi file akan disalin secara langsung ke buffer `buf`.

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

![image setelah watermark](assets/soal1_1.png)
![setelah reverse](assets/soal1_2.png)
![setelah menjalankan script.sh](assets/soal1_3.png)

# Soal 2

### Dikerjakan oleh Muhammad Ida Bagus Rafi Habibibie(5027221059)


## Deskripsi Soal

Masih dengan Ini Karya Kita, sang CEO ingin melakukan tes keamanan pada folder sensitif Ini Karya Kita. Karena Teknologi Informasi merupakan departemen dengan salah satu fokus di Cyber Security, maka dia kembali meminta bantuan mahasiswa Teknologi Informasi angkatan 2023 untuk menguji dan mengatur keamanan pada folder sensitif tersebut. Untuk mendapatkan folder sensitif itu, mahasiswa IT 23 harus kembali mengunjungi website Ini Karya Kita pada [www.inikaryakita.id/schedule](https://www.inikaryakita.id/schedule) . Silahkan isi semua formnya, tapi pada form subject isi dengan nama kelompok_SISOP24 , ex: IT01_SISOP24 . Lalu untuk form Masukkan Pesanmu, ketik “Mau Foldernya” . Tunggu hingga 1x24 jam, maka folder sensitif tersebut akan dikirimkan melalui email kalian. Apabila folder tidak dikirimkan ke email kalian, maka hubungi sang [CEO](http://wa.me/6282140759677) untuk meminta bantuan.

## Pengerjaan

### Pendahuluan

Adfi dan timnya menginginkan sebuah sistem file bernama pastibisa.c yang mampu melakukan decoding otomatis pada beberapa format encoding tertentu dan menerapkan keamanan pada folder dengan prefix tertentu. Dokumentasi ini menjelaskan langkah-langkah implementasi kode FUSE yang memenuhi permintaan tersebut.

#### Persiapan

1.  Instalasi FUSE:
    

Pastikan FUSE telah terinstal pada sistem Anda. Gunakan perintah berikut untuk instalasi:  
```  
sudo apt-get install fuse libfuse-dev
```

     
    

2.  Header dan Konstanta:
    

-   Menyiapkan versi FUSE dan meng-include berbagai header yang diperlukan untuk operasi sistem file dan manipulasi string.
    
```c 
#define FUSE_USE_VERSION 28

#include <fuse.h>

#include <stdio.h>

#include <string.h>

#include <errno.h>

#include <fcntl.h>

#include <unistd.h>

#include <dirent.h>

#include <sys/stat.h>

#include <time.h>

#include <stdlib.h>

#include <ctype.h>

  
const char *mount_path = "/Modul4/sensitif/Pesan-4";

```   
    

### Langkah Pengerjaan

1.  Implementasi Fungsi Logging:
    
-   Menulis log setiap proses yang dilakukan, mencatat status, waktu, tag, dan informasi tambahan.
    

```c  
void write_log(const char *status, const char *action, const char *details) {

FILE *log_file = fopen("logs-fuse.log", "a");

if (log_file != NULL) {

time_t now = time(NULL);

struct tm *local_time = localtime(&now);

char time_buffer[80];

strftime(time_buffer, sizeof(time_buffer), "%d/%m/%Y-%H:%M:%S", local_time);

fprintf(log_file, "[%s]::%s::[%s]::[%s]\n", status, time_buffer, action, details);

fclose(log_file);

}

}
```
  
    
2.  Implementasi Fungsi Dekode:
    

-   Base64:
  
 ```c  
char *decode_base64(const char *input) {

static const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int len = strlen(input);

char *decoded = (char *)malloc(len);

if (decoded == NULL) return NULL;

int i, j = 0, k = 0, val = 0;

for (i = 0; i < len; i++) {

if (input[i] == '=') break;

val = strchr(base64_chars, input[i]) - base64_chars;

val = val << ((i % 4) * 2);

k = k | val;

if (i % 4 == 3) {

decoded[j++] = (char)((k & 0x00FF0000) >> 16);

decoded[j++] = (char)((k & 0x0000FF00) >> 8);

decoded[j++] = (char)(k & 0x000000FF);

k = 0;

}

}

decoded[j] = '\0';

return decoded;

}
```
   
    

-   ROT13:
    

```c  
char *decode_rot13(const char *input) {

char *output = strdup(input);

if (output == NULL) return NULL;

for (char *c = output; *c; c++) {

if (isalpha(*c)) {

if ((*c >= 'a' && *c <= 'm') || (*c >= 'A' && *c <= 'M')) *c += 13;

else *c -= 13;

}

}

return output;

}

``` 
    

-   Hexadecimal:
    

```c
char *decode_hex(const char *input) {

int len = strlen(input);

char *output = (char *)malloc(len / 2 + 1);

if (output == NULL) return NULL;

for (int i = 0; i < len; i += 2) {

char byte[3] = {input[i], input[i+1], '\0'};

output[i/2] = (char)strtol(byte, NULL, 16);

}

output[len/2] = '\0';

return output;

}

```   
    

-   Reverse Text:
    
```c  
char *reverse_text(const char *input) {

int len = strlen(input);

char *output = strdup(input);

if (output == NULL) return NULL;

for (int i = 0; i < len / 2; i++) {

char temp = output[i];

output[i] = output[len - i - 1];

output[len - i - 1] = temp;

}

return output;

}

```   
    
3. Helper Function:
    

-   Memeriksa apakah sebuah string memiliki prefix tertentu.
    

```c
int has_prefix(const char *str, const char *prefix) {

return strncmp(str, prefix, strlen(prefix)) == 0;

}

``` 
    
4.  Implementasi FUSE Operations:
    

-   Getattr: Mengambil atribut file/directory.
    
```c  
static int custom_getattr(const char *path, struct stat *stbuf) {

char full_path[1000];

sprintf(full_path, "%s%s", mount_path, path);

int res = lstat(full_path, stbuf);

return (res == -1) ? -errno : 0;

}
``` 
    

-   Readdir: Membaca isi directory.
    

```c  
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

```  
    

-   Open: Membuka file dengan pengecekan password untuk file yang sensitif.
    

```c  
static int custom_open(const char *path, struct fuse_file_info *fi) {

if (strncmp(path, "/Modul4/sensitif/rahasia-berkas", strlen("/Modul4/sensitif/rahasia-berkas")) == 0) {

char password[256];

printf("Enter password to access %s: ", path);

scanf("%255s", password);

if (strcmp(password, "12345678") != 0) {

write_log("FAILED", "access", path);

return -EACCES;

}

write_log("SUCCESS", "access", path);

}

return 0;

}

```  
    

-   Read: Membaca isi file dengan berbagai operasi dekode tergantung pada prefix dari nama file.
    

```c
static int custom_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {

char full_path[1000];

sprintf(full_path, "%s%s", mount_path, path);

  

int fd = open(full_path, O_RDONLY);

if (fd == -1) return -errno;

  

struct stat st;

fstat(fd, &st);

char *content = (char *)malloc(st.st_size + 1);

if (content == NULL) {

close(fd);

return -ENOMEM;

}

pread(fd, content, st.st_size, 0);

content[st.st_size] = '\0';

close(fd);

  

char *filename = strdup(path + 1);

if (filename == NULL) {

free(content);

return -ENOMEM;

}

  

if (has_prefix(filename, "base64")) {

char *decoded = decode_base64(content);

free(content);

content = decoded;

} else if (has_prefix(filename, "rot13")) {

char *decoded = decode_rot13(content);

free(content);

content = decoded;

} else if (has_prefix(filename, "hex")) {

char *decoded = decode_hex(content);

free(content);

content = decoded;

} else if (has_prefix(filename, "rev")) {

char *reversed = reverse_text(content);

free(content);

content = reversed;

}

  

size_t len = strlen(content);

if (offset < len) {

if (offset + size > len) size = len - offset;

memcpy(buf, content + offset, size);

} else {

size = 0;

}

  

free(content);

free(filename);

  

return size;

}

```  
    
5.  Main Function dan FUSE Operations Struct:
    

-   struct fuse_operations custom_oper: Mendefinisikan operasi yang diimplementasikan oleh file system ini.
    
```c
static struct fuse_operations custom_oper = {

.getattr = custom_getattr,

.readdir = custom_readdir,

.open = custom_open,

.read = custom_read,

};

```
    

-   main: Fungsi utama yang menjalankan FUSE.
    

```c 
int main(int argc, char *argv[]) {

umask(0);

return fuse_main(argc, argv, &custom_oper, NULL);

}

```  
    

### Menjalankan Program

1.  Kompilasi Program:
    

-   Kompilasi program menggunakan GCC.
    

```  
fuse --cflags` -o pastibisa pastibisa.c `pkg-config fuse --libs`

```   
    
2.  Menjalankan Program:
    

-   Buat direktori mount point dan jalankan filesystem.
    

```
mkdir -p ~/Modul4/sensitif/Pesan

./pastibisa ~/Modul4/sensitif/Pesan

```  
    

### Kesimpulan

Kode FUSE ini berhasil diimplementasikan untuk memenuhi permintaan dengan menyediakan fitur-fitur berikut:

-   Dekoding otomatis untuk format Base64, ROT13, Hexadecimal, dan reverse text berdasarkan prefix nama file.
    
-   Perlindungan akses untuk file dan folder dengan prefix "rahasia" menggunakan mekanisme password.
    
-   Logging semua operasi dengan format yang sesuai, mencatat status, waktu, tindakan, dan informasi tambahan.
    

Dengan ini, file system dapat berfungsi sesuai dengan persyaratan yang diberikan oleh Adfi dan timnya.

hasil output:
![setelah menjalankan pastibisa](assets/soal2.png)

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

Untuk readdir, pertama kita skip `.` dan `..` terlebih dahulu. Kemudian kita opendir ke dirpath, inisialisasi struct st dan set ke 0.

Dilakukan iterasi atas semua entry di dir. Misal nama file yang di read tidak memiliki `.000`, maka skip sehingga file hanya ada 1 ketika dilakukan `ls`. Setelah itu, kita rename / menghapus bagian .000 dengan melakukan `'\0'` di relic `(strlen(de->d_name) - 4)`. Iterasi akan terus dilakukan sampai semua entry diproses.

> > Ketika dilakukan copy (dari direktori [nama_bebas] ke tujuan manapun), file yang disalin adalah file dari direktori relics yang sudah tergabung.

Untuk melakukan copy, kita memerlukan fungsi lain, yaitu read.

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

Untuk read, setelah deklarasi path, ada loop while yang berjalan selama `size > 0`. Di setiap iterasi yang dilakukan, fungsi membuat part untuk setiap file yang dibaca.

Kemudian kita mendapat part_size melalui seeking ke fp. Misal `offset >= part_size` maka kita skip dan mengurangi offset berdasarkan part_size.

Jika offset lebih kecil dari part_size, maka kita menggeser posisi seek ke offset dan membaca dari buf. File ditutup, dan dilakukan penambahan buf dan read_size dengan read_len, dan juga pengurangan size dengan read_len. Fungsi mengembalikan read_size yaitu jumlah byte yang dibaca.

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

Untuk write, seperti biasa ada deklarasi path. Setelah itu, kita membuat beberapa variabel seperti cur_part, part_offset, part, dan write_size_total.

Masuk ke fungsi while yang akan berjalan selama `size > 0`, di setiap iterasi, part = path ke bagian file yang akan diwrite (menggunakan r+b untuk read + write, wb = write). Kemudian, kita seek berdasarkan part_offset, dan menentukan write_size dengan cara:

- Misal `size > (PART_SIZE - part_offset))` dimana `(PART_SIZE - part_offset))` adalah ruang yang tersisa dalam part file, `write_size = (PART_SIZE - part_offset))` atau ruang yang tersisa.
- Jika kondisi diatas tidak terpenuhi, maka `write_size = size`.

Setelah itu kita write data dari buf, melakukan penambahan buf dan write_size_total dengan write_size, melakukan pengurangan size dengan write_size, dan terakhir reset part_offset.

Fungsi mengembalikan write_size_total yaitu jumlah byte yang telah ditulis.

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

Untuk create (membuat file baru), pertama kita deklarasi path seperti biasa, kemudian kita lakukan create ke full path yang ditambah dengan .000.

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

Untuk unlink, setelah deklarasi path, kita membuat beberapa variabel yaitu cur_part, part, dan res. Masuk ke while loop, disetiap iterasi dilakukan unlink (penghapusan) terhadap file yang ditambah dengan `.%03d` yang menunjuk ke cur_part. Iterasi terus dilakukan ulang sampai file yang ditunjuk tidak ada / sudah selesai melakukan unlink terhadap semua part.

Kemudian semua fungsi dimasukkan kedalam struct fuse_operations:

```c
static struct fuse_operations artifact_oper = {
    .getattr = artifact_getattr,
    .readdir = artifact_readdir,
    .read = artifact_read,
    .write = artifact_write,
    .unlink = artifact_unlink,
    .create = artifact_create,
};
```

**int main**

```c
int main(int argc, char *argv[]) {
    umask(0);
    return fuse_main(argc, argv, &artifact_oper, NULL);
}
```

`define DEBUG` digunakan untuk memasukkan `#ifdef DEBUG` pada semua fungsi agar mudah mengetahui apa yang terjadi jika dilakukan suatu command sehingga mempermudah proses debugging.

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

**ls**

![output ls](assets/soal3_1.png)

**Copy dari fuse_dir ke luar**

![output ls](assets/soal3_2.png)

**Copy dari luar ke fuse_dir**

![output ls](assets/soal3_3.png)

**Hasil copy dari luar di relics** (ls)

![output ls](assets/soal3_4.png)

**Penghapusan fiile di fuse_dir**

![output ls](assets/soal3_5.png)

**Copy ke report**

![output ls](assets/soal3_6.png)

**Konfigurasi Samba**

![output ls](assets/soal3_7.png)

**Penggunaan folder report/ di Samba**

![output ls](assets/soal3_8.png)
