#include "MainWindow.h"
#include <QApplication>
#include <QHeaderView>
#include <QListView>
#include <QThread>
#include <QPainter>
#include <QPaintEvent>
#include "DocConverterPage.h"

class PatternStackedWidget : public QStackedWidget {
public:
    explicit PatternStackedWidget(QWidget *parent = nullptr) : QStackedWidget(parent), pattern("none"), isDark(true) {}

    void setPattern(const QString &p, bool dark) {
        pattern = p;
        isDark = dark;
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        QStackedWidget::paintEvent(event);

        if (pattern == "none") return;

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, false);

        QColor color = isDark ? QColor(137, 180, 250, 40) : QColor(76, 79, 105, 30);
        painter.setPen(color);

        if (pattern == "dots") {
            int step = 20;
            for (int x = 0; x < width(); x += step) {
                for (int y = 0; y < height(); y += step) {
                    painter.drawPoint(x, y);
                }
            }
        } else if (pattern == "grid") {
            int step = 24;
            for (int x = 0; x < width(); x += step) {
                painter.drawLine(x, 0, x, height());
            }
            for (int y = 0; y < height(); y += step) {
                painter.drawLine(0, y, width(), y);
            }
        } else if (pattern == "scanlines") {
            int step = 4;
            for (int y = 0; y < height(); y += step) {
                painter.drawLine(0, y, width(), y);
            }
        }
    }

private:
    QString pattern;
    bool isDark;
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), selectedFileSize(0), spinnerFrame(0), currentTheme("dark") {
    pdfEngine = new PdfEngine(this);
    imageEngine = new ImageEngine(this);
    
    spinnerTimer = new QTimer(this);
    connect(spinnerTimer, &QTimer::timeout, this, &MainWindow::updateSpinnerAnimation);

// Di dalam private class MainWindow:
DocConverterPage *docConverterPage;

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
    resize(1020, 700);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Sidebar
    QWidget *sidebar = new QWidget(this);
    sidebar->setObjectName("sidebarWidget");
    sidebar->setFixedWidth(220);

    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
    sidebarLayout->setContentsMargins(15, 25, 15, 25);
    sidebarLayout->setSpacing(5);

    QLabel *appTitle = new QLabel("⚡ WARKI", sidebar);
    appTitle->setObjectName("appTitleLabel");
    
    QLabel *devLabel = new QLabel("BY BIAK DEVELOPER", sidebar);
    devLabel->setObjectName("devTagLabel");

    sidebarLayout->addWidget(appTitle);
    sidebarLayout->addWidget(devLabel);

    QPushButton *btnNavPDF      = new QPushButton("📄  Kompres PDF", sidebar);
    QPushButton *btnNavImg      = new QPushButton("🖼️  Resize Gambar", sidebar);
    QPushButton *btnNavAI       = new QPushButton("✨  Hapus BG", sidebar);
    QPushButton *btnNavDoc      = new QPushButton("📄  OCR & Konversi", sidebar);
    QPushButton *btnNavSettings = new QPushButton("⚙️  Pengaturan", sidebar);

    btnNavPDF->setObjectName("navButton");
    btnNavImg->setObjectName("navButton");
    btnNavAI->setObjectName("navButton");
    btnNavDoc->setObjectName("navButton");
    btnNavSettings->setObjectName("navButton");

    btnNavPDF->setCheckable(true);
    btnNavImg->setCheckable(true);
    btnNavAI->setCheckable(true);
    btnNavDoc->setCheckable(true);
    btnNavSettings->setCheckable(true);
    btnNavPDF->setChecked(true);

    sidebarLayout->addWidget(btnNavPDF);
    sidebarLayout->addWidget(btnNavImg);
    sidebarLayout->addWidget(btnNavAI);
    sidebarLayout->addWidget(btnNavDoc);
    sidebarLayout->addStretch();
    sidebarLayout->addWidget(btnNavSettings);

    settingsPage = new SettingsPage(this);
    aiBgPage = new AiBgPage(this); 
    docConverterPage = new DocConverterPage(this); // Inisialisasi Halaman OCR & Konversi Dokumen

    PatternStackedWidget *customStacked = new PatternStackedWidget(this);
    stackedWidget = customStacked;
    stackedWidget->addWidget(createPDFPage());    // Index 0
    stackedWidget->addWidget(createImagePage());  // Index 1
    stackedWidget->addWidget(aiBgPage);           // Index 2
    stackedWidget->addWidget(docConverterPage);   // Index 3
    stackedWidget->addWidget(settingsPage);      // Index 4

    connect(settingsPage, &SettingsPage::themeChanged, this, &MainWindow::changeTheme);
    connect(settingsPage, &SettingsPage::patternChanged, this, &MainWindow::setCustomPattern);

    auto clearChecks = [btnNavPDF, btnNavImg, btnNavAI, btnNavDoc, btnNavSettings]() {
        btnNavPDF->setChecked(false);
        btnNavImg->setChecked(false);
        btnNavAI->setChecked(false);
        btnNavDoc->setChecked(false);
        btnNavSettings->setChecked(false);
    };

    connect(btnNavPDF, &QPushButton::clicked, [this, clearChecks, btnNavPDF]() { clearChecks(); btnNavPDF->setChecked(true); stackedWidget->setCurrentIndex(0); });
    connect(btnNavImg, &QPushButton::clicked, [this, clearChecks, btnNavImg]() { clearChecks(); btnNavImg->setChecked(true); stackedWidget->setCurrentIndex(1); });
    connect(btnNavAI, &QPushButton::clicked, [this, clearChecks, btnNavAI]() { clearChecks(); btnNavAI->setChecked(true); stackedWidget->setCurrentIndex(2); });
    connect(btnNavDoc, &QPushButton::clicked, [this, clearChecks, btnNavDoc]() { clearChecks(); btnNavDoc->setChecked(true); stackedWidget->setCurrentIndex(3); });
    connect(btnNavSettings, &QPushButton::clicked, [this, clearChecks, btnNavSettings]() { clearChecks(); btnNavSettings->setChecked(true); stackedWidget->setCurrentIndex(4); });

    mainLayout->addWidget(sidebar);
    mainLayout->addWidget(stackedWidget, 1);

    setCentralWidget(centralWidget);
    changeTheme("dark");
}

void MainWindow::changeTheme(const QString &themeKey) {
    currentTheme = themeKey;
    if (themeKey == "light") {
        qApp->setStyleSheet(
            "QMainWindow, QWidget#centralWidget { background-color: #eff1f5; color: #4c4f69; font-family: 'Segoe UI', sans-serif; }"
            "QWidget#sidebarWidget { background-color: #e6e9ef; border-right: 1px solid #ccd0da; }"
            "QLabel#appTitleLabel { font-size: 20px; font-weight: bold; color: #1e66f5; }"
            "QLabel#devTagLabel { font-size: 9px; font-weight: bold; color: #fe640b; letter-spacing: 2px; padding-bottom: 15px; }"
            "QPushButton#navButton { text-align: left; padding: 12px; border-radius: 8px; font-weight: bold; background: transparent; color: #5c5f77; border: none; }"
            "QPushButton#navButton:hover { background-color: #dce0e8; color: #4c4f69; }"
            "QPushButton#navButton:checked { background-color: #ccd0da; color: #1e66f5; }"
            "QLabel#headerLabel { font-size: 22px; font-weight: bold; color: #1e66f5; }"
            "QLabel#subHeaderLabel { font-size: 15px; font-weight: bold; color: #5c5f77; margin-top: 10px; }"
            "QFrame#cardFrame { background-color: #e6e9ef; border-radius: 10px; border: 1px solid #ccd0da; }"
            "QLabel#filePathLabel { background-color: #dce0e8; color: #4c4f69; padding: 10px; border-radius: 6px; border: none; }"
            "QLabel { color: #4c4f69; border: none; background: transparent; }"
            "QComboBox { background-color: #dce0e8; color: #4c4f69; padding: 8px; border-radius: 6px; border: 1px solid #ccd0da; }"
            "QComboBox QAbstractItemView, QListView { background-color: #eff1f5; color: #4c4f69; border: 1px solid #ccd0da; outline: 0; padding: 4px; }"
            "QComboBox QAbstractItemView::item { background-color: #eff1f5; color: #4c4f69; padding: 8px; min-height: 25px; }"
            "QComboBox QAbstractItemView::item:selected { background-color: #bcc0cc; color: #1e66f5; border-radius: 4px; }"
            "QTableWidget { background-color: #e6e9ef; color: #4c4f69; border-radius: 8px; border: 1px solid #ccd0da; gridline-color: #ccd0da; }"
            "QTableCornerButton::section { background-color: #dce0e8; border: none; }"
            "QHeaderView::section { background-color: #dce0e8; color: #4c4f69; padding: 8px; font-weight: bold; border: none; }"
            "QSpinBox { background-color: #dce0e8; color: #4c4f69; padding: 8px; border-radius: 6px; border: none; }"
            "QTextEdit { background-color: #dce0e8; color: #4c4f69; border: 1px solid #ccd0da; border-radius: 8px; padding: 8px; }"
        );
    } else if (themeKey == "cyber") {
        qApp->setStyleSheet(
            "QMainWindow, QWidget#centralWidget { background-color: #030a16; color: #00f0ff; font-family: 'Segoe UI', sans-serif; }"
            "QWidget#sidebarWidget { background-color: #051124; border-right: 1px solid #00f0ff; }"
            "QLabel#appTitleLabel { font-size: 20px; font-weight: bold; color: #00f0ff; }"
            "QLabel#devTagLabel { font-size: 9px; font-weight: bold; color: #ff007f; letter-spacing: 2px; padding-bottom: 15px; }"
            "QPushButton#navButton { text-align: left; padding: 12px; border-radius: 8px; font-weight: bold; background: transparent; color: #70aeff; border: none; }"
            "QPushButton#navButton:hover { background-color: #0c213e; color: #00f0ff; }"
            "QPushButton#navButton:checked { background-color: #0c213e; color: #00f0ff; border: 1px solid #00f0ff; }"
            "QLabel#headerLabel { font-size: 22px; font-weight: bold; color: #00f0ff; }"
            "QLabel#subHeaderLabel { font-size: 15px; font-weight: bold; color: #70aeff; margin-top: 10px; }"
            "QFrame#cardFrame { background-color: #08162b; border-radius: 10px; border: 1px solid #00f0ff; }"
            "QLabel#filePathLabel { background-color: #0c213e; color: #00f0ff; padding: 10px; border-radius: 6px; border: none; }"
            "QLabel { color: #00f0ff; border: none; background: transparent; }"
            "QComboBox { background-color: #0c213e; color: #00f0ff; padding: 8px; border-radius: 6px; border: 1px solid #00f0ff; }"
            "QComboBox QAbstractItemView, QListView { background-color: #08162b; color: #00f0ff; border: 1px solid #00f0ff; outline: 0; padding: 4px; }"
            "QComboBox QAbstractItemView::item { background-color: #08162b; color: #00f0ff; padding: 8px; min-height: 25px; }"
            "QComboBox QAbstractItemView::item:selected { background-color: #0c213e; color: #ffffff; border-radius: 4px; }"
            "QTableWidget { background-color: #08162b; color: #00f0ff; border-radius: 8px; border: 1px solid #00f0ff; gridline-color: #0c213e; }"
            "QTableCornerButton::section { background-color: #0c213e; border: none; }"
            "QHeaderView::section { background-color: #0c213e; color: #00f0ff; padding: 8px; font-weight: bold; border: none; }"
            "QSpinBox { background-color: #0c213e; color: #00f0ff; padding: 8px; border-radius: 6px; border: none; }"
            "QTextEdit { background-color: #0c213e; color: #00f0ff; border: 1px solid #00f0ff; border-radius: 8px; padding: 8px; }"
        );
    } else { // Dark Catppuccin (Default)
        qApp->setStyleSheet(
            "QMainWindow, QWidget#centralWidget { background-color: #1e1e2e; color: #cdd6f4; font-family: 'Segoe UI', sans-serif; }"
            "QWidget#sidebarWidget { background-color: #181825; border-right: 1px solid #313244; }"
            "QLabel#appTitleLabel { font-size: 20px; font-weight: bold; color: #89b4fa; }"
            "QLabel#devTagLabel { font-size: 9px; font-weight: bold; color: #fab387; letter-spacing: 2px; padding-bottom: 15px; }"
            "QPushButton#navButton { text-align: left; padding: 12px; border-radius: 8px; font-weight: bold; background: transparent; color: #a6adc8; border: none; }"
            "QPushButton#navButton:hover { background-color: #313244; color: #cdd6f4; }"
            "QPushButton#navButton:checked { background-color: #45475a; color: #89b4fa; }"
            "QLabel#headerLabel { font-size: 22px; font-weight: bold; color: #89b4fa; }"
            "QLabel#subHeaderLabel { font-size: 15px; font-weight: bold; color: #bac2de; margin-top: 10px; }"
            "QFrame#cardFrame { background-color: #181825; border-radius: 10px; border: 1px solid #313244; }"
            "QLabel#filePathLabel { background-color: #313244; color: #cdd6f4; padding: 10px; border-radius: 6px; border: none; }"
            "QLabel { color: #cdd6f4; border: none; background: transparent; }"
            "QComboBox { background-color: #313244; color: #cdd6f4; padding: 8px; border-radius: 6px; border: none; }"
            "QComboBox QAbstractItemView, QListView { background-color: #181825; color: #cdd6f4; border: 1px solid #313244; outline: 0; padding: 4px; }"
            "QComboBox QAbstractItemView::item { background-color: #181825; color: #cdd6f4; padding: 8px; min-height: 25px; }"
            "QComboBox QAbstractItemView::item:selected { background-color: #45475a; color: #ffffff; border-radius: 4px; }"
            "QTableWidget { background-color: #181825; color: #cdd6f4; border-radius: 8px; border: 1px solid #313244; gridline-color: #313244; }"
            "QTableCornerButton::section { background-color: #313244; border: none; }"
            "QHeaderView::section { background-color: #313244; color: #cdd6f4; padding: 8px; font-weight: bold; border: none; }"
            "QSpinBox { background-color: #313244; color: #cdd6f4; padding: 8px; border-radius: 6px; border: none; }"
            "QTextEdit { background-color: #1e1e2e; color: #cdd6f4; border: 1px solid #313244; border-radius: 8px; padding: 8px; }"
        );
    }
}

void MainWindow::setCustomPattern(const QString &patternKey) {
    PatternStackedWidget *customStacked = static_cast<PatternStackedWidget*>(stackedWidget);
    if (customStacked) {
        customStacked->setPattern(patternKey, currentTheme != "light");
    }
}

QWidget* MainWindow::createPDFPage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(15);

    QLabel *header = new QLabel("📄 PDF Optimization & Task Manager", page);
    header->setObjectName("headerLabel");
    layout->addWidget(header);

    QFrame *controlFrame = new QFrame(page);
    controlFrame->setObjectName("cardFrame");
    QVBoxLayout *controlLayout = new QVBoxLayout(controlFrame);

    QHBoxLayout *fileRow = new QHBoxLayout();
    pdfFileLabel = new QLabel("Pilih file PDF untuk dikompres...", controlFrame);
    pdfFileLabel->setObjectName("filePathLabel");
    
    btnBrowsePDF = new QPushButton("Pilih PDF", controlFrame);
    btnBrowsePDF->setStyleSheet("QPushButton { background-color: #89b4fa; color: #11111b; font-weight: bold; padding: 10px 18px; border-radius: 6px; }"
                                "QPushButton:hover { background-color: #b4befe; }");

    fileRow->addWidget(pdfFileLabel, 1);
    fileRow->addWidget(btnBrowsePDF);
    controlLayout->addLayout(fileRow);

    QHBoxLayout *optRow = new QHBoxLayout();
    comboTargetSizePDF = new QComboBox(controlFrame);
    comboTargetSizePDF->setView(new QListView(comboTargetSizePDF));
    comboTargetSizePDF->addItem("🚀 Ultra Kompresi (Target 100 KB - 300 KB / 72 DPI)", static_cast<int>(TargetPreset::Target100_300KB));
    comboTargetSizePDF->addItem("⚡ Ekstrem (Target 300 KB - 500 KB / 96 DPI)", static_cast<int>(TargetPreset::Target300_500KB));
    comboTargetSizePDF->addItem("⚖️ Sedang / Standar Web (Target 1 MB - 2 MB / 150 DPI)", static_cast<int>(TargetPreset::Target1MB_2MB));
    comboTargetSizePDF->addItem("✨ Naikkan Kualitas / Upscale Cetak (300 DPI)", static_cast<int>(TargetPreset::UpscaleHQ));

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
    statusLabelPDF->setStyleSheet("font-size: 11px; border: none;");
    controlLayout->addWidget(statusLabelPDF);

    layout->addWidget(controlFrame);

    QLabel *historyHeader = new QLabel("📋 Task History / Hasil Kompresi", page);
    historyHeader->setObjectName("subHeaderLabel");
    layout->addWidget(historyHeader);

    historyTablePDF = new QTableWidget(0, 4, page);
    historyTablePDF->verticalHeader()->setVisible(false);
    historyTablePDF->setHorizontalHeaderLabels({"Nama File", "Ukuran Awal", "Ukuran Baru", "Aksi"});
    historyTablePDF->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    historyTablePDF->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    historyTablePDF->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    historyTablePDF->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    
    layout->addWidget(historyTablePDF, 1);

    connect(btnBrowsePDF, &QPushButton::clicked, this, &MainWindow::browsePDF);
    connect(btnStartPDF, &QPushButton::clicked, this, &MainWindow::startPDFCompression);
    connect(comboTargetSizePDF, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::checkFileSizeWarning);

    connect(pdfEngine, &PdfEngine::compressionFinished, [this](bool success, QString inPath, QString outPath) {
        spinnerTimer->stop();
        progressBarPDF->hide();
        btnStartPDF->setEnabled(true);
        btnBrowsePDF->setEnabled(true);

        if (success) {
            QThread::msleep(100);
            QFileInfo inInfo(inPath); QFileInfo outInfo(outPath); outInfo.refresh();
            statusLabelPDF->setText("Status: ✅ Kompresi Selesai!");
            addHistoryRow(historyTablePDF, outInfo.fileName(), formatFileSize(inInfo.size()), formatFileSize(outInfo.size()), outPath);
        } else {
            statusLabelPDF->setText("Status: ❌ Gagal mengompres PDF!");
        }
    });

    return page;
}

QWidget* MainWindow::createImagePage() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(15);

    QLabel *header = new QLabel("🖼️ Image Resizer & Quality Compressor", page);
    header->setObjectName("headerLabel");
    layout->addWidget(header);

    QFrame *controlFrame = new QFrame(page);
    controlFrame->setObjectName("cardFrame");
    QVBoxLayout *controlLayout = new QVBoxLayout(controlFrame);

    QHBoxLayout *fileRow = new QHBoxLayout();
    imgFileLabel = new QLabel("Pilih gambar (JPG, PNG, WebP)...", controlFrame);
    imgFileLabel->setObjectName("filePathLabel");
    
    btnBrowseImg = new QPushButton("Pilih Gambar", controlFrame);
    btnBrowseImg->setStyleSheet("QPushButton { background-color: #89b4fa; color: #11111b; font-weight: bold; padding: 10px 18px; border-radius: 6px; }");

    fileRow->addWidget(imgFileLabel, 1);
    fileRow->addWidget(btnBrowseImg);
    controlLayout->addLayout(fileRow);

    QHBoxLayout *dimensRow = new QHBoxLayout();
    
    spinWidth = new QSpinBox(controlFrame); spinWidth->setRange(0, 8000); spinWidth->setPrefix("Lebar: "); spinWidth->setSuffix(" px");
    spinHeight = new QSpinBox(controlFrame); spinHeight->setRange(0, 8000); spinHeight->setPrefix("Tinggi: "); spinHeight->setSuffix(" px");
    spinQuality = new QSpinBox(controlFrame); spinQuality->setRange(10, 100); spinQuality->setValue(80); spinQuality->setPrefix("Kualitas: "); spinQuality->setSuffix(" %");

    btnStartImg = new QPushButton("🚀 Proses Gambar", controlFrame);
    btnStartImg->setStyleSheet("QPushButton { background-color: #a6e3a1; color: #11111b; font-weight: bold; padding: 10px 20px; border-radius: 6px; }"
                               "QPushButton:disabled { background-color: #45475a; color: #6c7086; }");
    btnStartImg->setEnabled(false);

    dimensRow->addWidget(spinWidth);
    dimensRow->addWidget(spinHeight);
    dimensRow->addWidget(spinQuality);
    dimensRow->addWidget(btnStartImg);
    controlLayout->addLayout(dimensRow);

    statusLabelImg = new QLabel("Status: Siap", controlFrame);
    statusLabelImg->setStyleSheet("font-size: 11px; border: none;");
    controlLayout->addWidget(statusLabelImg);

    layout->addWidget(controlFrame);

    QLabel *historyHeader = new QLabel("📋 Task History / Hasil Gambar", page);
    historyHeader->setObjectName("subHeaderLabel");
    layout->addWidget(historyHeader);

    historyTableImg = new QTableWidget(0, 4, page);
    historyTableImg->verticalHeader()->setVisible(false);
    historyTableImg->setHorizontalHeaderLabels({"Nama File", "Ukuran Awal", "Ukuran Baru", "Aksi"});
    historyTableImg->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    historyTableImg->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    historyTableImg->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    historyTableImg->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    
    layout->addWidget(historyTableImg, 1);

    connect(btnBrowseImg, &QPushButton::clicked, this, &MainWindow::browseImage);
    connect(btnStartImg, &QPushButton::clicked, this, &MainWindow::startImageProcess);

    return page;
}

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
        warningLabelPDF->setText(QString("⚠️ Peringatan: File ini cukup besar (%1 MB). Mengompresi file > 10 MB ke target kecil (100-500 KB) akan menurunkan kualitas gambar secara signifikan.").arg(sizeInMB, 0, 'f', 2));
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

    spinnerFrame = 0;
    spinnerTimer->start(350);
    pdfEngine->compressAsync(selectedPDFPath, outputFile, preset);
}

void MainWindow::browseImage() {
    QString path = QFileDialog::getOpenFileName(this, "Pilih Gambar", "", "Image Files (*.png *.jpg *.jpeg *.webp)");
    if (!path.isEmpty()) {
        selectedImagePath = path;
        QImage img(path);
        QFileInfo info(path);
        
        imgFileLabel->setText(info.fileName() + " (" + formatFileSize(info.size()) + ")");
        spinWidth->setValue(img.width());
        spinHeight->setValue(img.height());
        
        btnStartImg->setEnabled(true);
        statusLabelImg->setText("Status: Gambar siap diproses.");
    }
}

void MainWindow::startImageProcess() {
    QString outputFile = QFileDialog::getSaveFileName(this, "Simpan Gambar", selectedImagePath.section('.', 0, -2) + "_resized.png", "Images (*.png *.jpg *.webp)");
    if (outputFile.isEmpty()) return;

    statusLabelImg->setText("Status: ⏳ Memproses gambar...");
    bool ok = imageEngine->resizeAndCompress(selectedImagePath, outputFile, spinWidth->value(), spinHeight->value(), spinQuality->value());

    if (ok) {
        QFileInfo inInfo(selectedImagePath); QFileInfo outInfo(outputFile);
        statusLabelImg->setText("Status: ✅ Gambar Berhasil Diproses!");
        addHistoryRow(historyTableImg, outInfo.fileName(), formatFileSize(inInfo.size()), formatFileSize(outInfo.size()), outputFile);
    } else {
        statusLabelImg->setText("Status: ❌ Gagal memproses gambar!");
    }
}

void MainWindow::addHistoryRow(QTableWidget *table, const QString &fileName, const QString &origSize, const QString &newSize, const QString &outputPath) {
    int row = table->rowCount();
    table->insertRow(row);

    table->setItem(row, 0, new QTableWidgetItem(fileName));
    table->setItem(row, 1, new QTableWidgetItem(origSize));
    table->setItem(row, 2, new QTableWidgetItem(newSize));

    QPushButton *btnOpenFolder = new QPushButton("📂 Buka Folder");
    btnOpenFolder->setStyleSheet("QPushButton { background-color: #89b4fa; color: #11111b; border: none; border-radius: 4px; padding: 4px 8px; font-size: 11px; font-weight: bold; }"
                                 "QPushButton:hover { background-color: #b4befe; }");

    connect(btnOpenFolder, &QPushButton::clicked, [outputPath]() {
        QFileInfo info(outputPath);
        QDesktopServices::openUrl(QUrl::fromLocalFile(info.absolutePath()));
    });

    table->setCellWidget(row, 3, btnOpenFolder);
}