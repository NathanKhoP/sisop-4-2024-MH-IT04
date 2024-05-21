# Repository Praktikum Sistem Operasi Modul 3 - IT04
## Anggota

| Nama                      | NRP        |
|---------------------------|------------|
|Nathan Kho Pancras         | 5027231002 |
|Athalla Barka Fadhil       | 5027231018 |
|Muhammad Ida Bagus Rafi H  | 5027221059 |


## Daftar Isi

- [Soal 1](#soal-1)
- [Soal 2](#soal-2)
- [Soal 3](#soal-3)
- [Soal 4](#soal-4)

# Soal 1

### Dikerjakan oleh Athalla Barka Fadhil (5027231018)

## Deskripsi Soal

### Catatan

## Pengerjaan

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
>> Ketika dilakukan listing, isi dari direktori [nama_bebas] adalah semua relic dari relics yang telah tergabung.

-

>> Ketika dilakukan copy (dari direktori [nama_bebas] ke tujuan manapun), file yang disalin adalah file dari direktori relics yang sudah tergabung.

-

>> Ketika ada file dibuat, maka pada direktori asal (direktori relics) file tersebut akan dipecah menjadi sejumlah pecahan dengan ukuran maksimum tiap pecahan adalah 10kb.

-

>> File yang dipecah akan memiliki nama [namafile].000 dan seterusnya sesuai dengan jumlah pecahannya.
Ketika dilakukan penghapusan, maka semua pecahannya juga ikut terhapus.

-

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

