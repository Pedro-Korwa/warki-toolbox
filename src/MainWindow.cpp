#include "MainWindow.h"
#include <QHeaderView>
#include <QThread>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), selectedFileSize(0), spinnerFrame(0) {
    pdfEngine = new PdfEngine(this);
    
    // Inisialisasi Timer Animasi Status
    spinnerTimer = new QTimer(this);
    connect(spinnerTimer, &QTimer::timeout, this, &MainWindow::updateSpinnerAnimation);

    setupUI();
}

MainWindow::~MainWindow() {}

QString MainWindow::formatFileSize(qint64 bytes) {
    if (bytes < 1024) return QString::number(bytes) + " Bytes";
    if (bytes < 1024 * 1024) return QString::number(bytes / 1024.0, 'f', 2) + " KB";
    return QString::number(bytes / (1024.0 * 1024.0), 'f', 2) + " MB";
}

void MainWindow::setupUI() {
    setWindowTitle("Warki Toolbox - Professional Desktop Utility");
    resize(980, 680);
    setStyleSheet("background-color: #1e1e2e; color: #cdd6f4; font-family: 'Segoe UI', sans-serif;");

    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Sidebar Navigasi
    QWidget *sidebar = new QWidget(this);
    sidebar->setFixedWidth(220);
    sidebar->setStyleSheet("background-color: #181825; border-right: 1px solid #313244;");

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(15, 25, 15, 25);
    sidebarLayout->setSpacing(10);

    QLabel *appTitle = new QLabel("⚡ WARKI", sidebar);
    appTitle->setStyleSheet("font-size: 20px; font-weight: bold; color: #89b4fa; padding-bottom: 15px;");
    sidebarLayout->addWidget(appTitle);

    QPushButton *btnNavPDF = new QPushButton("📄  Kompres PDF", sidebar);
    QPushButton *btnNavImg = new QPushButton("🖼️  Resize Gambar", sidebar);
    QPushButton *btnNavAI  = new QPushButton("✨  Hapus BG (AI)", sidebar);

    QString sideBtnStyle = "QPushButton { text-align: left; padding: 12px; border-radius: 8px; font-weight: bold; background: transparent; color: #a6adc8; }"
                           "QPushButton:hover { background-color: #313244; color: #cdd6f4; }"
                           "QPushButton:checked { background-color: #45475a; color: #89b4fa; }";

    btnNavPDF->setStyleSheet(sideBtnStyle);
    btnNavImg->setStyleSheet(sideBtnStyle);
    btnNavAI->setStyleSheet(sideBtnStyle);
    btnNavPDF->setCheckable(true);
    btnNavPDF->setChecked(true);

    sidebarLayout->addWidget(btnNavPDF);
    sidebarLayout->addWidget(btnNavImg);
    sidebarLayout->addWidget(btnNavAI);
    sidebarLayout->addStretch();

    stackedWidget = new QStackedWidget(this);
    stackedWidget->addWidget(createPDFPage());

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(stackedWidget, 1);

    setCentralWidget(centralWidget);
}

QWidget* MainWindow::createPDFPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(15);

    QLabel *header = new QLabel("📄 PDF Optimization & Task Manager", page);
    header->setStyleSheet("font-size: 22px; font-weight: bold; color: #89b4fa;");
    layout->addWidget(header);

    QFrame *controlFrame = new QFrame(page);
    controlFrame->setStyleSheet("QFrame { background-color: #181825; border-radius: 10px; padding: 15px; border: 1px solid #313244; }");
    QVBoxLayout *controlLayout = new QVBoxLayout(controlFrame);

    QHBoxLayout *fileRow = new QHBoxLayout();
    pdfFileLabel = new QLabel("Pilih file PDF untuk dikompres...", controlFrame);
    pdfFileLabel->setStyleSheet("background-color: #313244; padding: 10px; border-radius: 6px; border: none;");
    
    btnBrowsePDF = new QPushButton("Pilih PDF", controlFrame);
    btnBrowsePDF->setStyleSheet("QPushButton { background-color: #89b4fa; color: #11111b; font-weight: bold; padding: 10px 18px; border-radius: 6px; }"
                                "QPushButton:hover { background-color: #b4befe; }");

    fileRow->addWidget(pdfFileLabel, 1);
    fileRow->addWidget(btnBrowsePDF);
    controlLayout->addLayout(fileRow);

    QHBoxLayout *optRow = new QHBoxLayout();
    comboTargetSizePDF = new QComboBox(controlFrame);
    
    comboTargetSizePDF->addItem("🚀 Ultra Kompresi (Target 100 KB - 300 KB / 72 DPI)", static_cast<int>(TargetPreset::Target100_300KB));
    comboTargetSizePDF->addItem("⚡ Ekstrem (Target 300 KB - 500 KB / 96 DPI)", static_cast<int>(TargetPreset::Target300_500KB));
    comboTargetSizePDF->addItem("⚖️ Sedang / Standar Web (Target 1 MB - 2 MB / 150 DPI)", static_cast<int>(TargetPreset::Target1MB_2MB));
    comboTargetSizePDF->addItem("✨ Naikkan Kualitas / Upscale Cetak (300 DPI)", static_cast<int>(TargetPreset::UpscaleHQ));
    
    comboTargetSizePDF->setStyleSheet("QComboBox { background-color: #313244; color: #cdd6f4; padding: 8px; border-radius: 6px; border: none; }");

    btnStartPDF = new QPushButton("🚀 Mulai Kompresi", controlFrame);
    btnStartPDF->setStyleSheet("QPushButton { background-color: #a6e3a1; color: #11111b; font-weight: bold; padding: 10px 20px; border-radius: 6px; }"
                               "QPushButton:disabled { background-color: #45475a; color: #6c7086; }");
    btnStartPDF->setEnabled(false);

    optRow->addWidget(comboTargetSizePDF, 1);
    optRow->addWidget(btnStartPDF);
    controlLayout->addLayout(optRow);

    warningLabelPDF = new QLabel(controlFrame);
    warningLabelPDF->setWordWrap(true);
    warningLabelPDF->setStyleSheet("color: #f38ba8; font-size: 11px; padding: 5px; background: transparent; border: none;");
    warningLabelPDF->hide();
    controlLayout->addWidget(warningLabelPDF);

    progressBarPDF = new QProgressBar(controlFrame);
    progressBarPDF->setRange(0, 0);
    progressBarPDF->setTextVisible(false);
    progressBarPDF->setStyleSheet("QProgressBar { background-color: #313244; border-radius: 4px; height: 8px; }"
                                  "QProgressBar::chunk { background-color: #a6e3a1; }");
    progressBarPDF->hide();
    controlLayout->addWidget(progressBarPDF);

    statusLabelPDF = new QLabel("Status: Siap", controlFrame);
    statusLabelPDF->setStyleSheet("color: #a6adc8; font-size: 11px; border: none;");
    controlLayout->addWidget(statusLabelPDF);

    layout->addWidget(controlFrame);

    QLabel *historyHeader = new QLabel("📋 Task History / Hasil Kompresi", page);
    historyHeader->setStyleSheet("font-size: 15px; font-weight: bold; color: #bac2de; margin-top: 10px;");
    layout->addWidget(historyHeader);

    historyTable = new QTableWidget(0, 4, page);
    historyTable->setHorizontalHeaderLabels({"Nama File", "Ukuran Awal", "Ukuran Baru", "Aksi"});
    historyTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    historyTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    historyTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    historyTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    historyTable->setStyleSheet("QTableWidget { background-color: #181825; border-radius: 8px; border: 1px solid #313244; gridline-color: #313244; }"
                                "QHeaderView::section { background-color: #313244; color: #cdd6f4; padding: 8px; font-weight: bold; border: none; }");
    
    layout->addWidget(historyTable, 1);

    connect(btnBrowsePDF, &QPushButton::clicked, this, &MainWindow::browsePDF);
    connect(btnStartPDF, &QPushButton::clicked, this, &MainWindow::startPDFCompression);
    connect(comboTargetSizePDF, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::checkFileSizeWarning);

    // Handler Selesai Kompresi
    connect(pdfEngine, &PdfEngine::compressionFinished, [this](bool success, QString inPath, QString outPath) {
        spinnerTimer->stop(); // Hentikan Animasi
        progressBarPDF->hide();
        btnStartPDF->setEnabled(true);
        btnBrowsePDF->setEnabled(true);

        if (success) {
            QThread::msleep(100);
            QFileInfo inInfo(inPath);
            QFileInfo outInfo(outPath);
            outInfo.refresh();

            QString sizeIn = formatFileSize(inInfo.size());
            QString sizeOut = formatFileSize(outInfo.size());

            statusLabelPDF->setText("Status: ✅ Kompresi Selesai!");
            addHistoryRow(outInfo.fileName(), sizeIn, sizeOut, outPath);
        } else {
            statusLabelPDF->setText("Status: ❌ Gagal mengompres PDF!");
        }
    });

    return page;
}

// Slot Animasi Berputar
void MainWindow::updateSpinnerAnimation() {
    const QString frames[] = {"⏳", "⌛"};
    spinnerFrame = (spinnerFrame + 1) % 2;
    statusLabelPDF->setText(QString("Status: %1 Sedang mengompresi file PDF...").arg(frames[spinnerFrame]));
}

void MainWindow::browsePDF() {
    QString path = QFileDialog::getOpenFileName(this, "Pilih File PDF", "", "PDF Files (*.pdf)");
    if (!path.isEmpty()) {
        selectedPDFPath = path;
        QFileInfo info(path);
        selectedFileSize = info.size();
        pdfFileLabel->setText(info.fileName() + " (" + formatFileSize(selectedFileSize) + ")");
        btnStartPDF->setEnabled(true);
        statusLabelPDF->setText("Status: File siap dikompres.");

        checkFileSizeWarning();
    }
}

void MainWindow::checkFileSizeWarning() {
    double sizeInMB = selectedFileSize / (1024.0 * 1024.0);
    int currentPresetIndex = comboTargetSizePDF->currentIndex();

    if (sizeInMB > 10.0 && currentPresetIndex < 2) { 
        warningLabelPDF->setText(QString("⚠️ Peringatan: File ini cukup besar (%1 MB). Mengompresi file > 10 MB ke target kecil (100-500 KB) akan menurunkan kualitas gambar secara signifikan agar muat.").arg(sizeInMB, 0, 'f', 2));
        warningLabelPDF->show();
    } else {
        warningLabelPDF->hide();
    }
}

void MainWindow::startPDFCompression() {
    QString outputFile = QFileDialog::getSaveFileName(this, "Simpan Hasil", selectedPDFPath.section('.', 0, -2) + "_compressed.pdf", "PDF Files (*.pdf)");
    if (outputFile.isEmpty()) return;

    TargetPreset preset = static_cast<TargetPreset>(comboTargetSizePDF->currentData().toInt());

    progressBarPDF->show();
    btnStartPDF->setEnabled(false);
    btnBrowsePDF->setEnabled(false);

    // Jalankan Animasi Berputar Setiap 350 milidetik
    spinnerFrame = 0;
    spinnerTimer->start(350);

    pdfEngine->compressAsync(selectedPDFPath, outputFile, preset);
}

void MainWindow::addHistoryRow(const QString &fileName, const QString &origSize, const QString &newSize, const QString &outputPath) {
    int row = historyTable->rowCount();
    historyTable->insertRow(row);

    historyTable->setItem(row, 0, new QTableWidgetItem(fileName));
    historyTable->setItem(row, 1, new QTableWidgetItem(origSize));
    historyTable->setItem(row, 2, new QTableWidgetItem(newSize));

    QPushButton *btnOpenFolder = new QPushButton("📂 Buka Folder");
    btnOpenFolder->setStyleSheet("QPushButton { background-color: #313244; color: #89b4fa; border: 1px solid #45475a; border-radius: 4px; padding: 4px 8px; font-size: 11px; }"
                                 "QPushButton:hover { background-color: #45475a; color: #ffffff; }");

    connect(btnOpenFolder, &QPushButton::clicked, [outputPath]() {
        QFileInfo info(outputPath);
        QDesktopServices::openUrl(QUrl::fromLocalFile(info.absolutePath()));
    });

    historyTable->setCellWidget(row, 3, btnOpenFolder);
}