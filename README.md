# ⚡ Warki Toolbox

**Warki Toolbox** adalah aplikasi *desktop utility suite* berkinerja tinggi, ringan, dan **100% offline** yang dirancang untuk mengolah dokumen, gambar, serta pemrosesan file secara cepat dan presisi. 

Dibangun menggunakan **C++20 Native** dan kerangka kerja **Qt6**, Warki Toolbox memberikan alternatif lokal berkualitas profesional (*Nitro Pro / Adobe Acrobat style*) tanpa ketergantungan cloud, pelacakan data, maupun keterlambatan *proses FFI/heavy runtime*.

---

## 🌟 Fitur Utama

### 1. 📄 PDF Suite & Document Converter
* **Multi-Format Conversion**: Konversi file PDF ke **Word (`.docx`)**, **Excel (`.xlsx`)**, **PowerPoint (`.pptx`)**, **Gambar (`.png`, `.jpg`)**, dan **Teks Polos (`.txt`)**.
* **Combine & Merge PDF**: Gabungkan beberapa file PDF menjadi satu dokumen utuh secara instan dengan fitur *re-order* (naik/turun urutan file) yang fleksibel.
* **Kecepatan Tinggi & Ringan**: Pemrosesan *asynchronous C++ native* memanfaatkan eksekusi *multi-threading* (`QtConcurrent`) dan *toolchain* Poppler tanpa beban *runtime* GUI LibreOffice yang berat.
* **Indikator & Operasi Aman**: Dikelengkapi *progress bar* dinamis, indikator status animasi, serta tombol **Batal (Cancel)** untuk menghentikan pemrosesan kapan saja secara aman.

### 2. 📷 Photo to Document & Smart OCR
* **Image Preprocessing (OpenCV)**: Memproses foto dokumen, buku, atau nota menggunakan teknik *Otsu Binarization* dan *Automatic Contrast Adjustment* untuk hasil ekstraksi teks yang bersih.
* **Tesseract OCR Integration**: Ekstraksi teks berbasis *hOCR layout analysis* dengan dukungan Bahasa Indonesia (`ind`) dan Bahasa Inggris (`eng`).
* **Rich Text / Office Preview**: Hasil ekstraksi langsung dapat dilihat dan diedit dalam tampilan pratinjau bergaya Microsoft Word/Office 2021 sebelum disimpan.

### 3. ✨ Background Removal (`RMBG-1.4`)
* **Hapus Latar Belakang Foto**: Mengisolasi subjek foto secara presisi menggunakan integrasi AI lokal tanpa memerlukan koneksi internet.
* **Preset Warna & Transparansi**: Pilih latar belakang transparan (PNG) atau terapkan warna pasfoto standar (Merah, Biru, Putih) secara instan.

### 4. 🖼️ Image Resizer & Quality Compressor
* **Dimensi Kustom**: Ubah ukuran lebar (*width*) dan tinggi (*height*) gambar (`.jpg`, `.png`, `.webp`) dalam piksel.
* **Kompresi Kualitas**: Atur tingkat kompresi file untuk menghemat ruang penyimpanan tanpa merusak estetika visual.

### 5. 📄 PDF Compressor
* **Preset Target Ukuran**: Pilihan kompresi ultra (*Target 100 KB - 300 KB*), ekstrem, hingga standar web untuk kebutuhan pendaftaran dokumen resmi.

---

## 🎨 Antarmuka Pengguna (UI & Themes)

Dikembangkan menggunakan prinsip antarmuka modern yang nyaman di mata (*eye-pleasing*) dan tidak kaku:
* **Tema Bawaan**: *Dark Catppuccin*, *Cyberpunk*, dan *Light Mode*.
* **Custom Background Patterns**: Pilihan pola latar latar belakang animasi (*Dots*, *Grid*, *Scanlines*).
* **Layout Stabil**: Komponen UI terkunci (*fixed layout constraints*) untuk mencegah gangguan visual (*jumping/flickering*) saat animasi pemrosesan berlangsung.

---

## 🛠️ Arsitektur & Teknologi

* **Bahasa**: C++20
* **GUI Framework**: Qt6 (Widgets, Concurrent, PrintSupport)
* **Image Processing**: OpenCV C++
* **OCR Engine**: Tesseract OCR C++ API (`libtesseract`) & Leptonica (`liblept`)
* **Document Processing Pipeline**: Poppler CLI (`pdftohtml`, `pdftoppm`, `pdftotext`, `pdfunite`) & Pandoc
* **Build System**: CMake (Minimum v3.16)

---

## 🚀 Panduan Instalasi & Kompilasi (Linux / Ubuntu / Mint)

### 1. Install Dependensi Sistem
Jalankan perintah berikut di terminal untuk memasang compiler C++, Qt6, OpenCV, Tesseract, dan tools dokumen pendukung:

```bash
sudo apt update && sudo apt install -y \
    build-essential \
    cmake \
    qt6-base-dev \
    libopencv-dev \
    libtesseract-dev \
    tesseract-ocr-ind \
    tesseract-ocr-eng \
    libleptonica-dev \
    poppler-utils \
    pandoc

2. Clone Repositori
Bash

git clone [https://github.com/username/warki-toolbox.git](https://github.com/username/warki-toolbox.git)
cd warki-toolbox

3. Kompilasi Proyek
Bash

mkdir build && cd build
cmake ..
make -j$(nproc)

4. Jalankan Aplikasi
Bash

./WarkiToolbox

📂 Struktur Proyek
Plaintext

warki-toolbox/
├── src/
│   ├── main.cpp                 # Entry point aplikasi
│   ├── MainWindow.h/.cpp        # Jendela utama & navigasi sidebar
│   ├── DocConverterPage.h/.cpp # Modul PDF Converter & Combine PDF
│   ├── OcrEngine.h/.cpp         # Engine OCR Tesseract & Preprocessing OpenCV
│   ├── AiBgPage.h/.cpp          # Modul AI Hapus Background Foto
│   ├── BgEngine.h/.cpp          # Core engine pemrosesan latar belakang
│   ├── PdfEngine.h/.cpp         # Engine kompresi PDF
│   ├── ImageEngine.h/.cpp       # Engine resizer & kompresi gambar
│   └── SettingsPage.h/.cpp      # Pengaturan tema dan pola latar belakang
├── CMakeLists.txt               # Konfigurasi build CMake
├── .gitignore                   # Aturan pengabaian file Git
└── README.md                    # Dokumentasi proyek