# ⚡ warki toolbox

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-20-blue.svg" alt="C++ Standard">
  <img src="https://img.shields.io/badge/Framework-Qt%206-green.svg" alt="Qt 6">
  <img src="https://img.shields.io/badge/Platform-Linux%20%7C%20Windows-lightgrey.svg" alt="Platform">
  <img src="https://img.shields.io/badge/License-MIT-yellow.svg" alt="License">
</p>

**Warki Toolbox** adalah aplikasi utilitas desktop *high-performance*, super ringan, dan **100% offline** yang dirancang menggunakan C++20 dan Qt 6. Aplikasi ini berfokus pada optimasi file, kompresi PDF, *image resizing*, serta pemrosesan media dengan kecepatan tinggi tanpa ketergantungan pada koneksi internet.

---

## 🌟 Fitur Utama

- 📄 **PDF Optimizer & Engine Manager:**
  - Kompresi berbasis *DPI Downsampling* dengan beberapa preset target (`100–300 KB`, `300–500 KB`, `1–2 MB`, hingga `Upscale/HQ`).
  - **Perlindungan Ketajaman Teks:** Seluruh font, teks, dan elemen vektor terkunci aman agar dokumen tetap crisp & nyaman dibaca; kompresi difokuskan pada elemen raster/gambar.
  - Peringatan cerdas (*Smart Warning*) otomatis saat mengompresi file berukuran besar (> 10 MB).
- 🖼️ **Image Resizer & Compressor:** *(Dalam Pengembangan)*
- ✨ **AI Background Removal (Offline):** *(Dalam Pengembangan)*
- 📋 **Task History List:**
  - Merekam riwayat file yang telah diproses secara *real-time*.
  - Akses cepat tombol **"📂 Buka Folder"** untuk membuka lokasi file hasil secara langsung di File Manager sistem.

---

## 🏛️ Arsitektur Sistem & Teknis

Warki Toolbox dibangun dengan arsitektur modular (*layered architecture*) untuk memastikan keterpisahan antara antarmuka pengguna (UI) dan logika pemrosesan (*Engine*):

```text
warki-toolbox/
├── CMakeLists.txt        # Konfigurasi Build System
├── Makefile             # Shortcut kompilasi & eksekusi cepat
└── src/
    ├── main.cpp         # Entry point aplikasi
    ├── MainWindow.h/cpp # Antarmuka Utama & Task Manager UI
    └── PdfEngine.h/cpp  # Core Engine Kompresi PDF (Asynchronous Processing)

    Bahasa: C++20 Standard

    Framework UI: Qt 6 (Widgets)

    Engine PDF: Ghostscript Integration (Via QProcess & QTimer Asynchronous)

    Build System: CMake & Makefile

```
🚀 Cara Build & Instalasi (Linux Mint / Ubuntu)
1. Prasyarat System (Dependencies)

Pastikan compiler C++, CMake, Qt 6, dan Ghostscript telah terpasang di sistem kamu.
Bash

sudo apt update
sudo apt install -y build-essential cmake git ninja-build \
                    qt6-base-dev qt6-tools-dev libqt6svg6-dev \
                    ghostscript

2. Clone Repository
Bash

git clone [https://github.com/username-kamu/warki-toolbox.git](https://github.com/username-kamu/warki-toolbox.git)
cd warki-toolbox

3. Build & Jalankan Aplikasi

Aplikasi dilengkapi dengan Makefile untuk mempermudah proses kompilasi:
Bash

# Build dan langsung jalankan aplikasi
make run

Atau secara manual menggunakan CMake:
Bash

mkdir -p build && cd build
cmake ..
make -j$(nproc)
./WarkiToolbox