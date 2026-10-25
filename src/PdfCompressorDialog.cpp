#include "PdfCompressorDialog.h"
#include <QMessageBox>
#include <QThread>

PdfCompressorDialog::PdfCompressorDialog(QWidget *parent) : QDialog(parent) {
    setupUI();
}

PdfCompressorDialog::~PdfCompressorDialog() {}

// Helper untuk format ukuran file riil (Bytes ke KB/MB)
QString PdfCompressorDialog::formatFileSize(qint64 bytes) {
    if (bytes < 1024) {
        return QString::number(bytes) + " Bytes";
    } else if (bytes < 1024 * 1024) {
        return QString::number(bytes / 1024.0, 'f', 2) + " KB";
    } else {
        return QString::number(bytes / (1024.0 * 1024.0), 'f', 2) + " MB";
    }
}

void PdfCompressorDialog::setupUI() {
    setWindowTitle("PDF Optimization & Compression Tool");
    resize(550, 400);
    setStyleSheet("background-color: #1e1e2e; color: #cdd6f4; font-family: 'Segoe UI', sans-serif;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(15);

    // Header Title
    titleLabel = new QLabel("📄 Optimasi & Kompresi File PDF", this);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #89b4fa;");
    layout->addWidget(titleLabel);

    // Section 1: Select File
    QHBoxLayout *fileLayout = new QHBoxLayout();
    fileLabel = new QLabel("Belum ada file PDF yang dipilih...", this);
    fileLabel->setStyleSheet("background-color: #313244; padding: 12px; border-radius: 8px; border: 1px solid #45475a;");
    
    btnBrowse = new QPushButton("Pilih PDF", this);
    btnBrowse->setStyleSheet("QPushButton { background-color: #89b4fa; color: #11111b; font-weight: bold; padding: 12px 20px; border-radius: 8px; }"
                             "QPushButton:hover { background-color: #b4befe; }");
    
    fileLayout->addWidget(fileLabel, 1);
    fileLayout->addWidget(btnBrowse);
    layout->addLayout(fileLayout);

    // Section 2: Target Size Spesifik Dropdown
    QHBoxLayout *targetLayout = new QHBoxLayout();
    targetLabel = new QLabel("Target Ukuran & Kualitas:", this);
    targetLabel->setStyleSheet("font-size: 13px; color: #bac2de;");

    comboTargetSize = new QComboBox(this);
    comboTargetSize->addItem("🚀 Ultra Kompresi (Target 100 KB - 300 KB / 72 DPI)", 1);
    comboTargetSize->addItem("⚡ Ekstrem (Target 300 KB - 500 KB / 96 DPI)", 2);
    comboTargetSize->addItem("⚖️ Sedang / Standar Web (Target < 1 MB - 2 MB / 150 DPI)", 3);
    comboTargetSize->addItem("🔍 Kualitas Tinggi / Cetak (Target > 2 MB / 300 DPI)", 4);
    
    comboTargetSize->setStyleSheet("QComboBox { background-color: #313244; color: #cdd6f4; padding: 8px; border-radius: 6px; border: 1px solid #45475a; }"
                                    "QComboBox QAbstractItemView { background-color: #1e1e2e; color: #cdd6f4; selection-background-color: #45475a; }");

    targetLayout->addWidget(targetLabel);
    targetLayout->addWidget(comboTargetSize, 1);
    layout->addLayout(targetLayout);

    // Section 3: Animated Progress Bar & Status
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 0); // Indeterminate animated loading
    progressBar->setTextVisible(false);
    progressBar->setStyleSheet("QProgressBar { background-color: #313244; border-radius: 6px; height: 12px; }"
                               "QProgressBar::chunk { background-color: #a6e3a1; border-radius: 6px; }");
    progressBar->hide();
    layout->addWidget(progressBar);

    statusLabel = new QLabel("Pilih file PDF dan klik 'Mulai Kompresi'", this);
    statusLabel->setStyleSheet("color: #a6adc8; font-size: 12px;");
    statusLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(statusLabel);

    // Section 4: Action Button
    btnStart = new QPushButton("🚀 Mulai Kompresi", this);
    btnStart->setStyleSheet("QPushButton { background-color: #a6e3a1; color: #11111b; font-size: 15px; font-weight: bold; padding: 14px; border-radius: 8px; }"
                            "QPushButton:hover { background-color: #94e2d5; }"
                            "QPushButton:disabled { background-color: #45475a; color: #6c7086; }");
    btnStart->setEnabled(false);
    layout->addWidget(btnStart);

    // Signals
    connect(btnBrowse, &QPushButton::clicked, this, &PdfCompressorDialog::browseFile);
    connect(btnStart, &QPushButton::clicked, this, &PdfCompressorDialog::startCompression);
}

void PdfCompressorDialog::browseFile() {
    QString path = QFileDialog::getOpenFileName(this, "Pilih File PDF", "", "PDF Files (*.pdf)");
    if (!path.isEmpty()) {
        selectedFilePath = path;
        QFileInfo info(path);
        fileLabel->setText(info.fileName() + " (" + formatFileSize(info.size()) + ")");
        btnStart->setEnabled(true);
        statusLabel->setText("File siap dikompres.");
    }
}

void PdfCompressorDialog::startCompression() {
    QString outputFile = QFileDialog::getSaveFileName(this, "Simpan Hasil Kompresi", selectedFilePath.section('.', 0, -2) + "_compressed.pdf", "PDF Files (*.pdf)");
    if (outputFile.isEmpty()) return;

    int targetOption = comboTargetSize->currentData().toInt();

    progressBar->show();
    btnStart->setEnabled(false);
    btnBrowse->setEnabled(false);
    statusLabel->setText("⏳ Mengompres file PDF...");

    compressPDF(selectedFilePath, outputFile, targetOption);
}

void PdfCompressorDialog::compressPDF(const QString &inputFile, const QString &outputFile, int targetOption) {
    QProcess *process = new QProcess(this);
    QString program = "gs";
    
    QStringList arguments;
    arguments << "-sDEVICE=pdfwrite"
              << "-dCompatibilityLevel=1.4"
              << "-dNOPAUSE"
              << "-dQUIET"
              << "-dBATCH";

    // Konfigurasi DPI dan kompresi spesifik berdasarkan target pilihan user
    if (targetOption == 1) { // Ultra (100KB - 300KB)
        arguments << "-dPDFSETTINGS=/screen"
                  << "-dColorImageResolution=72"
                  << "-dGrayImageResolution=72"
                  << "-dMonoImageResolution=72";
    } else if (targetOption == 2) { // Ekstrem (300KB - 500KB)
        arguments << "-dPDFSETTINGS=/screen"
                  << "-dColorImageResolution=96"
                  << "-dGrayImageResolution=96"
                  << "-dMonoImageResolution=96";
    } else if (targetOption == 3) { // Sedang (< 1MB - 2MB)
        arguments << "-dPDFSETTINGS=/ebook"
                  << "-dColorImageResolution=150"
                  << "-dGrayImageResolution=150";
    } else { // Kualitas Tinggi
        arguments << "-dPDFSETTINGS=/printer";
    }

    arguments << "-sOutputFile=" + outputFile << inputFile;

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this, process, inputFile, outputFile](int exitCode, QProcess::ExitStatus exitStatus) {
                progressBar->hide();
                btnStart->setEnabled(true);
                btnBrowse->setEnabled(true);

                if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
                    // Jeda singkat & refresh info file agar ukuran riil dari OS terbaca akurat
                    QThread::msleep(100); 
                    
                    QFileInfo inInfo(inputFile);
                    QFileInfo outInfo(outputFile);
                    outInfo.refresh(); // Refresh buffer metadata file dari OS

                    QString sizeIn = formatFileSize(inInfo.size());
                    QString sizeOut = formatFileSize(outInfo.size());

                    statusLabel->setText(QString("🎉 Berhasil! %1 ➔ %2").arg(sizeIn, sizeOut));
                    
                    QMessageBox::information(this, "Sukses", 
                        QString("File berhasil dikompres!\n\nUkuran Awal: %1\nUkuran Baru: %2")
                        .arg(sizeIn, sizeOut));
                } else {
                    statusLabel->setText("❌ Gagal mengompres PDF!");
                    QMessageBox::critical(this, "Error", "Proses kompresi PDF gagal.");
                }
                process->deleteLater();
            });

    process->start(program, arguments);
}