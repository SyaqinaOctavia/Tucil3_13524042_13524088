# Tucil3_13524042_13524088

## Deskripsi Singkat

Program ini adalah solver untuk permainan **Ice Sliding Puzzle**. Pemain bergerak dengan cara meluncur ke arah atas, bawah, kiri, atau kanan hingga berhenti karena terhalang batu. Program mencari rute dari titik awal `Z` menuju tujuan `O` dengan memperhatikan checkpoint bernomor `0` sampai `9` yang harus dilewati secara berurutan.

Algoritma pencarian yang tersedia:

- Uniform Cost Search (UCS)
- Greedy Best First Search (GBFS)
- A* Search

Heuristik yang tersedia untuk GBFS dan A*:

- Manhattan Distance
- Euclidean Distance
- Diagonal Distance

Program dilengkapi GUI berbasis raylib untuk memilih file input, algoritma, heuristik, menjalankan pencarian, melihat visualisasi solusi, dan menyimpan hasil.

## Requirement

Requirement utama:

- C++17
- CMake versi 3.15 atau lebih baru
- Compiler C++ seperti GCC, Clang, atau MSVC
- raylib

Instalasi raylib dapat dilakukan sesuai sistem operasi masing-masing, misalnya:

- Windows dengan MSYS2: `pacman -S mingw-w64-x86_64-raylib`
- Linux Debian/Ubuntu: `sudo apt install libraylib-dev`
- Alternatif lain: menggunakan vcpkg atau instalasi manual raylib

## Cara Kompilasi

Dari root folder proyek, jalankan:

```bash
cmake -S . -B build
cmake --build build
```

Executable akan dibuat pada folder:

```text
bin/main.exe
```

Pada Linux atau macOS, nama executable dapat berupa:

```text
bin/main
```

## Cara Menjalankan Program

Setelah dikompilasi, jalankan:

```bash
./bin/main
```

Pada Windows PowerShell:

```powershell
.\bin\main.exe
```

Cara menggunakan program:

1. Masukkan nama file input pada kolom input file.
2. Pilih algoritma pencarian: UCS, GBFS, atau A*.
3. Jika memilih GBFS atau A*, pilih heuristik yang ingin digunakan.
4. Tekan tombol run untuk menjalankan pencarian.
5. Hasil pencarian akan ditampilkan pada GUI.
6. Gunakan tombol save untuk menyimpan hasil ke folder output.

Program akan mencari file input dari path yang dimasukkan atau dari folder:

```text
test/input
```

Contoh file input tersedia pada folder tersebut.

## Format Input

Format file input:

```text
rows cols
baris_peta_1
baris_peta_2
...
baris_peta_rows
baris_biaya_1
baris_biaya_2
...
baris_biaya_rows
```

Keterangan simbol peta:

- `Z` = titik awal
- `O` = tujuan
- `X` = batu atau dinding
- `L` = lava
- `*` atau `.` = tile biasa
- `0` sampai `9` = checkpoint

Contoh:

```text
7 7
XXXXXXX
X0****X
X**X**X
X****OX
X1***LX
XZ**X*X
XXXXXXX
999 999 999 999 999 999 999
999 3   5   2   8   1   999
999 7   4   999 6   9   999
999 2   8   3   5   4   999
999 6   1   7   2   999 999
999 9   3   4   999 8   999
999 999 999 999 999 999 999
```

## Struktur Folder

```text
src/          kode sumber program
src/include/  header file
test/input/   file input pengujian
test/output/  hasil output pencarian
bin/          executable hasil kompilasi
build/        file hasil build CMake
assets/       aset GUI
doc/          laporan
```

## Author

13524042 - Suryani Mulia Utami <br>
13524088 - Syaqina Octavia Rizha <br>
