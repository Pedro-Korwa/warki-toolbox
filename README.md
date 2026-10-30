# ⚡ Warki Toolbox

**Warki Toolbox** is a high-performance, lightweight, and **100% offline** desktop utility suite designed for fast, accurate, and privacy-focused document processing, image manipulation, and PDF optimization.

Built natively with **C++20** and the **Qt6** framework, Warki Toolbox provides a professional-grade local alternative (*Nitro Pro / Adobe Acrobat style*) without cloud dependencies, telemetry tracking, or heavy runtime overhead.

---

## 🌟 Key Features

### 1. 📄 PDF Suite & Document Converter
* **Multi-Format Conversion**: Convert PDF files to **Word (`.docx`)**, **Excel (`.xlsx`)**, **PowerPoint (`.pptx`)**, **Images (`.png`, `.jpg`)**, and **Plain Text (`.txt`)**.
* **Combine & Merge PDF**: Merge multiple PDF files into a single document instantly with flexible file re-ordering (move up/down).
* **High-Speed & Lightweight**: Powered by asynchronous C++ native processing using `QtConcurrent` multi-threading and Poppler tools without LibreOffice GUI memory overhead.
* **Progress Tracking & Safe Operations**: Features dynamic progress bars, status indicators, and an interactive **Cancel** button to safely interrupt tasks at any time.

### 2. 📷 Photo to Document & Smart OCR
* **Image Preprocessing (OpenCV)**: Enhances document scans, book photos, or receipts using *Otsu Binarization* and *Automatic Contrast Adjustment* for clear text extraction.
* **Tesseract OCR Integration**: High-accuracy text extraction using *hOCR layout analysis* supporting both Indonesian (`ind`) and English (`eng`).
* **Rich Text / Office Preview**: Directly view and edit extracted content in a Microsoft Word / Office 2021 style preview pane before saving.

### 3. ✨ Background Removal (`RMBG-1.4`)
* **AI Photo Background Removal**: Isolate photo subjects accurately using a local AI engine without needing an internet connection.
* **Color Presets & Transparency**: Export with transparent backgrounds (PNG) or apply standard ID photo backdrop colors (Red, Blue, White) instantly.

### 4. 🖼️ Image Resizer & Quality Compressor
* **Custom Dimensions**: Resize image width and height (`.jpg`, `.png`, `.webp`) in exact pixels.
* **Quality Compression**: Adjust compression levels to save storage space without sacrificing visual fidelity.

### 5. 📄 PDF Compressor
* **Target Size Presets**: Preset targets ranging from ultra-compression (*100 KB - 300 KB*) to web-standard for official form submission requirements.

---

## 🎨 User Interface (UI & Themes)

Designed with modern, eye-pleasing UI principles for long working hours:
* **Built-in Themes**: *Dark Catppuccin*, *Cyberpunk*, and *Light Mode*.
* **Custom Background Patterns**: Interactive pattern overlays (*Dots*, *Grid*, *Scanlines*).
* **Stable Layout Constraints**: Fixed UI element heights prevent flickering or dynamic layout shifting during background task execution.

---

## 🛠️ Architecture & Tech Stack

* **Language**: C++20
* **GUI Framework**: Qt6 (Widgets, Concurrent, PrintSupport)
* **Image Processing**: OpenCV C++
* **OCR Engine**: Tesseract OCR C++ API (`libtesseract`) & Leptonica (`liblept`)
* **Document Processing Pipeline**: Poppler CLI (`pdftohtml`, `pdftoppm`, `pdftotext`, `pdfunite`) & Pandoc
* **Build System**: CMake (Minimum v3.16)

---

## 🚀 Installation & Build Guide (Linux / Ubuntu / Mint)

### 1. Install System Dependencies
Run the following command in your terminal to install the C++ compiler, Qt6, OpenCV, Tesseract, and document utility packages:

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

2. Clone Repository
Bash

git clone [https://github.com/Pedro-Korwa/warki-toolbox.git](https://github.com/Pedro-Korwa/warki-toolbox.git)
cd warki-toolbox

3. Build Project
Bash

mkdir build && cd build
cmake ..
make -j$(nproc)

4. Run Application
Bash

./WarkiToolbox

📂 Project Structure
Plaintext

warki-toolbox/
├── src/
│   ├── main.cpp                 # Application entry point
│   ├── MainWindow.h/.cpp        # Main window & sidebar navigation
│   ├── DocConverterPage.h/.cpp # PDF Converter & Combine PDF page
│   ├── OcrEngine.h/.cpp         # Tesseract OCR & OpenCV Preprocessing engine
│   ├── AiBgPage.h/.cpp          # AI Background Removal UI page
│   ├── BgEngine.h/.cpp          # Core background removal engine
│   ├── PdfEngine.h/.cpp         # PDF Compression engine
│   ├── ImageEngine.h/.cpp       # Image resizer & quality compression engine
│   └── SettingsPage.h/.cpp      # Theme & background pattern settings
├── CMakeLists.txt               # CMake build configuration
├── .gitignore                   # Git ignore rules
└── README.md                    # Project documentation

