#include "DocConverterPage.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QFileInfo>
#include <QDir>
#include <QTemporaryDir>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>

DocConverterPage::DocConverterPage(QWidget *parent) 
    : QWidget(parent), currentProcess(nullptr), animTimer(nullptr), animFrame(0), cancelRequested(false) {
    
    animTimer = new QTimer(this);
    connect(animTimer, &QTimer::timeout, this, &DocConverterPage::updateLoadingAnimation);

    setupUI();
}

DocConverterPage::~DocConverterPage() {
    if (currentProcess) {
        currentProcess->kill();
        delete currentProcess;
    }
}

void DocConverterPage::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(15);

    QLabel *header = new QLabel("📄 PDF Suite & Document Converter", this);
    header->setObjectName("headerLabel");
    mainLayout->addWidget(header);

    tabWidget = new QTabWidget(this);
    tabWidget->setStyleSheet(
        "QTabWidget::pane { border: 1px solid #313244; background-color: #181825; border-radius: 8px; }"
        "QTabBar::tab { background-color: #1e1e2e; color: #a6adc8; padding: 10px 20px; font-weight: bold; border-top-left-radius: 6px; border-top-right-radius: 6px; margin-right: 4px; }"
        "QTabBar::tab:selected { background-color: #313244; color: #89b4fa; border-bottom: 2px solid #89b4fa; }"
    );

    tabWidget->addTab(createConvertTab(), "🔄 Konversi PDF ke Word / Excel / PPT / Gambar");
    tabWidget->addTab(createMergeTab(), "🧩 Combine & Merge PDF");

    mainLayout->addWidget(tabWidget, 1);
}

QWidget* DocConverterPage::createConvertTab() {
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(12);

    QHBoxLayout *topRow = new QHBoxLayout();
    btnAddPdfConvert = new QPushButton("➕ Tambah File PDF", tab);
    btnAddPdfConvert->setStyleSheet("QPushButton { background-color: #89b4fa; color: #11111b; font-weight: bold; padding: 10px 15px; border-radius: 6px; }");

    btnClearPdfConvert = new QPushButton("🗑️ Bersihkan Daftar", tab);
    btnClearPdfConvert->setStyleSheet("QPushButton { background-color: #45475a; color: #cdd6f4; font-weight: bold; padding: 10px 15px; border-radius: 6px; }");

    topRow->addWidget(btnAddPdfConvert);
    topRow->addWidget(btnClearPdfConvert);
    topRow->addStretch();
    layout->addLayout(topRow);

    tableConvertFiles = new QTableWidget(0, 3, tab);
    tableConvertFiles->verticalHeader()->setVisible(false);
    tableConvertFiles->setHorizontalHeaderLabels({"Nama File", "Ukuran File", "Path"});
    tableConvertFiles->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tableConvertFiles->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    tableConvertFiles->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    tableConvertFiles->setStyleSheet("QTableWidget { background-color: #1e1e2e; border: 1px solid #313244; border-radius: 6px; gridline-color: #313244; color: #cdd6f4; }");
    layout->addWidget(tableConvertFiles, 1);

    QHBoxLayout *actionRow = new QHBoxLayout();
    QLabel *lblTarget = new QLabel("Format Tujuan:", tab);
    lblTarget->setStyleSheet("font-weight: bold; color: #cdd6f4;");

    comboTargetFormat = new QComboBox(tab);
    comboTargetFormat->addItems({
        "Word Document (.docx)",
        "Excel Spreadsheet (.xlsx)",
        "PowerPoint Presentation (.pptx)",
        "Gambar PNG (.png)",
        "Gambar JPEG (.jpg)",
        "Teks Polos (.txt)"
    });
    comboTargetFormat->setStyleSheet("QComboBox { background-color: #313244; color: #cdd6f4; padding: 8px; border-radius: 6px; font-weight: bold; }");

    btnStartConvert = new QPushButton("🚀 Mulai Konversi", tab);
    btnStartConvert->setStyleSheet("QPushButton { background-color: #a6e3a1; color: #11111b; font-weight: bold; padding: 10px 20px; border-radius: 6px; }"
                                   "QPushButton:disabled { background-color: #45475a; color: #6c7086; }");
    btnStartConvert->setEnabled(false);

    btnCancelConvert = new QPushButton("⛔ Batal", tab);
    btnCancelConvert->setStyleSheet("QPushButton { background-color: #f38ba8; color: #11111b; font-weight: bold; padding: 10px 15px; border-radius: 6px; }");
    btnCancelConvert->setEnabled(false);

    actionRow->addWidget(lblTarget);
    actionRow->addWidget(comboTargetFormat, 1);
    actionRow->addWidget(btnStartConvert);
    actionRow->addWidget(btnCancelConvert);
    layout->addLayout(actionRow);

    progressConvert = new QProgressBar(tab);
    progressConvert->setRange(0, 100);
    progressConvert->setValue(0);
    progressConvert->setTextVisible(false);
    progressConvert->setFixedHeight(12);
    progressConvert->setStyleSheet("QProgressBar { background-color: #313244; border-radius: 6px; border: none; } "
                                  "QProgressBar::chunk { background-color: #89b4fa; border-radius: 6px; }");
    layout->addWidget(progressConvert);

    statusConvertLabel = new QLabel("Status: Siap konversi", tab);
    statusConvertLabel->setMinimumHeight(24);
    statusConvertLabel->setStyleSheet("color: #a6adc8; font-size: 11px; font-weight: bold;");
    layout->addWidget(statusConvertLabel);

    connect(btnAddPdfConvert, &QPushButton::clicked, this, &DocConverterPage::addPdfToConvertTable);
    connect(btnClearPdfConvert, &QPushButton::clicked, [this]() {
        tableConvertFiles->setRowCount(0);
        btnStartConvert->setEnabled(false);
    });
    connect(btnStartConvert, &QPushButton::clicked, this, &DocConverterPage::processPdfConversion);
    connect(btnCancelConvert, &QPushButton::clicked, this, &DocConverterPage::cancelOperation);

    return tab;
}

QWidget* DocConverterPage::createMergeTab() {
    QWidget *tab = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(12);

    QHBoxLayout *topRow = new QHBoxLayout();
    btnAddPdfMerge = new QPushButton("➕ Tambah PDF", tab);
    btnAddPdfMerge->setStyleSheet("QPushButton { background-color: #89b4fa; color: #11111b; font-weight: bold; padding: 10px 15px; border-radius: 6px; }");

    btnMoveUp = new QPushButton("⬆️ Naik", tab);
    btnMoveDown = new QPushButton("⬇️ Turun", tab);
    btnRemoveMerge = new QPushButton("❌ Hapus File", tab);

    QString btnSecStyle = "QPushButton { background-color: #313244; color: #cdd6f4; font-weight: bold; padding: 10px 15px; border-radius: 6px; }";
    btnMoveUp->setStyleSheet(btnSecStyle);
    btnMoveDown->setStyleSheet(btnSecStyle);
    btnRemoveMerge->setStyleSheet(btnSecStyle);

    topRow->addWidget(btnAddPdfMerge);
    topRow->addWidget(btnMoveUp);
    topRow->addWidget(btnMoveDown);
    topRow->addWidget(btnRemoveMerge);
    topRow->addStretch();
    layout->addLayout(topRow);

    tableMergeFiles = new QTableWidget(0, 2, tab);
    tableMergeFiles->verticalHeader()->setVisible(false);
    tableMergeFiles->setHorizontalHeaderLabels({"Urutan & Nama File PDF", "Path Lokasi"});
    tableMergeFiles->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tableMergeFiles->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    tableMergeFiles->setStyleSheet("QTableWidget { background-color: #1e1e2e; border: 1px solid #313244; border-radius: 6px; gridline-color: #313244; color: #cdd6f4; }");
    layout->addWidget(tableMergeFiles, 1);

    QHBoxLayout *mergeBtnRow = new QHBoxLayout();
    btnStartMerge = new QPushButton("🔗 Combine / Gabungkan Semua PDF", tab);
    btnStartMerge->setStyleSheet("QPushButton { background-color: #fab387; color: #11111b; font-weight: bold; padding: 12px; border-radius: 6px; font-size: 13px; }"
                                 "QPushButton:disabled { background-color: #45475a; color: #6c7086; }");
    btnStartMerge->setEnabled(false);

    btnCancelMerge = new QPushButton("⛔ Batal", tab);
    btnCancelMerge->setStyleSheet("QPushButton { background-color: #f38ba8; color: #11111b; font-weight: bold; padding: 12px 20px; border-radius: 6px; }");
    btnCancelMerge->setEnabled(false);

    mergeBtnRow->addWidget(btnStartMerge, 1);
    mergeBtnRow->addWidget(btnCancelMerge);
    layout->addLayout(mergeBtnRow);

    progressMerge = new QProgressBar(tab);
    progressMerge->setRange(0, 100);
    progressMerge->setValue(0);
    progressMerge->setTextVisible(false);
    progressMerge->setFixedHeight(12);
    progressMerge->setStyleSheet("QProgressBar { background-color: #313244; border-radius: 6px; border: none; } "
                               "QProgressBar::chunk { background-color: #fab387; border-radius: 6px; }");
    layout->addWidget(progressMerge);

    statusMergeLabel = new QLabel("Status: Siap gabungkan PDF", tab);
    statusMergeLabel->setMinimumHeight(24);
    statusMergeLabel->setStyleSheet("color: #a6adc8; font-size: 11px; font-weight: bold;");
    layout->addWidget(statusMergeLabel);

    connect(btnAddPdfMerge, &QPushButton::clicked, this, &DocConverterPage::addPdfToMergeTable);
    connect(btnMoveUp, &QPushButton::clicked, [this]() { moveSelectedItem(-1); });
    connect(btnMoveDown, &QPushButton::clicked, [this]() { moveSelectedItem(1); });
    connect(btnRemoveMerge, &QPushButton::clicked, [this]() {
        int row = tableMergeFiles->currentRow();
        if (row >= 0) {
            tableMergeFiles->removeRow(row);
            btnStartMerge->setEnabled(tableMergeFiles->rowCount() > 1);
        }
    });
    connect(btnStartMerge, &QPushButton::clicked, this, &DocConverterPage::processPdfMerge);
    connect(btnCancelMerge, &QPushButton::clicked, this, &DocConverterPage::cancelOperation);

    return tab;
}

void DocConverterPage::updateLoadingAnimation() {
    const QString icons[] = {"⚡", "⏳", "✨", "⌛"};
    animFrame = (animFrame + 1) % 4;

    if (animTimer && animTimer->isActive()) {
        if (progressConvert->maximum() == 0) {
            statusConvertLabel->setText(QString("Status: %1 Memproses konversi file secara kilat... Mohon tunggu...").arg(icons[animFrame]));
        }
        if (progressMerge->maximum() == 0) {
            statusMergeLabel->setText(QString("Status: %1 Menggabungkan struktur PDF... Mohon tunggu...").arg(icons[animFrame]));
        }
    }
}

void DocConverterPage::addPdfToConvertTable() {
    QStringList files = QFileDialog::getOpenFileNames(this, "Pilih File PDF", "", "PDF Files (*.pdf)");
    for (const QString &path : files) {
        QFileInfo info(path);
        int row = tableConvertFiles->rowCount();
        tableConvertFiles->insertRow(row);
        tableConvertFiles->setItem(row, 0, new QTableWidgetItem(info.fileName()));
        tableConvertFiles->setItem(row, 1, new QTableWidgetItem(QString::number(info.size() / 1024.0 / 1024.0, 'f', 2) + " MB"));
        tableConvertFiles->setItem(row, 2, new QTableWidgetItem(path));
    }
    if (tableConvertFiles->rowCount() > 0) {
        btnStartConvert->setEnabled(true);
    }
}

void DocConverterPage::addPdfToMergeTable() {
    QStringList files = QFileDialog::getOpenFileNames(this, "Pilih File PDF untuk Digabungkan", "", "PDF Files (*.pdf)");
    for (const QString &path : files) {
        QFileInfo info(path);
        int row = tableMergeFiles->rowCount();
        tableMergeFiles->insertRow(row);
        tableMergeFiles->setItem(row, 0, new QTableWidgetItem(QString("%1. %2").arg(row + 1).arg(info.fileName())));
        tableMergeFiles->setItem(row, 1, new QTableWidgetItem(path));
    }
    if (tableMergeFiles->rowCount() > 1) {
        btnStartMerge->setEnabled(true);
    }
}

void DocConverterPage::moveSelectedItem(int direction) {
    int row = tableMergeFiles->currentRow();
    if (row < 0) return;
    int targetRow = row + direction;
    if (targetRow < 0 || targetRow >= tableMergeFiles->rowCount()) return;

    QString fileText = tableMergeFiles->item(row, 0)->text();
    QString pathText = tableMergeFiles->item(row, 1)->text();

    tableMergeFiles->removeRow(row);
    tableMergeFiles->insertRow(targetRow);
    tableMergeFiles->setItem(targetRow, 0, new QTableWidgetItem(fileText));
    tableMergeFiles->setItem(targetRow, 1, new QTableWidgetItem(pathText));
    tableMergeFiles->selectRow(targetRow);
}

void DocConverterPage::cancelOperation() {
    cancelRequested = true;
    if (currentProcess) {
        currentProcess->kill();
    }
}

void DocConverterPage::processPdfConversion() {
    int totalFiles = tableConvertFiles->rowCount();
    if (totalFiles == 0) return;

    int fmtIndex = comboTargetFormat->currentIndex();
    QString outputDir = QFileDialog::getExistingDirectory(this, "Pilih Folder Output");
    if (outputDir.isEmpty()) return;

    cancelRequested = false;
    
    progressConvert->setRange(0, 0);
    btnStartConvert->setEnabled(false);
    btnCancelConvert->setEnabled(true);

    animFrame = 0;
    animTimer->start(200);

    QFutureWatcher<bool> *watcher = new QFutureWatcher<bool>(this);
    connect(watcher, &QFutureWatcher<bool>::finished, this, [this, watcher, outputDir]() {
        animTimer->stop();
        progressConvert->setRange(0, 100);
        progressConvert->setValue(100);

        btnStartConvert->setEnabled(true);
        btnCancelConvert->setEnabled(false);

        if (cancelRequested) {
            statusConvertLabel->setText("Status: ⛔ Konversi dibatalkan oleh pengguna.");
            QMessageBox::information(this, "Batal", "Operasi konversi dibatalkan.");
        } else if (watcher->result()) {
            statusConvertLabel->setText("Status: ✅ Konversi 100% Selesai! Disimpan di: " + outputDir);
            QMessageBox::information(this, "Berhasil", "✅ Seluruh file PDF berhasil dikonversi secara kilat!");
        } else {
            statusConvertLabel->setText("Status: ❌ Gagal melakukan konversi!");
            QMessageBox::warning(this, "Gagal", "❌ Terjadi kesalahan saat konversi file.");
        }
        watcher->deleteLater();
    });

    QFuture<bool> future = QtConcurrent::run([this, totalFiles, outputDir, fmtIndex]() {
        for (int i = 0; i < totalFiles; ++i) {
            if (cancelRequested) return false;

            QString inputPath = tableConvertFiles->item(i, 2)->text();
            QFileInfo info(inputPath);
            QString baseName = info.completeBaseName();

            currentProcess = new QProcess();

            if (fmtIndex == 0) { // PDF to Word (.docx)
                QTemporaryDir tempDir;
                if (tempDir.isValid()) {
                    QString htmlPath = tempDir.path() + "/" + baseName + ".html";
                    QString outDocxPath = outputDir + "/" + baseName + ".docx";

                    QStringList htmlArgs;
                    htmlArgs << "-s" << "-noframes" << inputPath << htmlPath;
                    currentProcess->start("pdftohtml", htmlArgs);
                    currentProcess->waitForFinished(-1);

                    delete currentProcess;
                    currentProcess = new QProcess();

                    QStringList docxArgs;
                    docxArgs << htmlPath << "-o" << outDocxPath;
                    currentProcess->start("pandoc", docxArgs);
                    currentProcess->waitForFinished(-1);
                }

            } else if (fmtIndex == 1) { // PDF to Excel (.xlsx)
                QTemporaryDir tempDir;
                if (tempDir.isValid()) {
                    QString htmlPath = tempDir.path() + "/" + baseName + ".html";
                    QString outXlsxPath = outputDir + "/" + baseName + ".xlsx";

                    QStringList htmlArgs;
                    htmlArgs << "-s" << "-noframes" << inputPath << htmlPath;
                    currentProcess->start("pdftohtml", htmlArgs);
                    currentProcess->waitForFinished(-1);

                    delete currentProcess;
                    currentProcess = new QProcess();

                    QStringList xlsxArgs;
                    xlsxArgs << htmlPath << "-o" << outXlsxPath;
                    currentProcess->start("pandoc", xlsxArgs);
                    currentProcess->waitForFinished(-1);
                }

            } else if (fmtIndex == 2) { // PDF to PowerPoint (.pptx)
                QTemporaryDir tempDir;
                if (tempDir.isValid()) {
                    QString htmlPath = tempDir.path() + "/" + baseName + ".html";
                    QString outPptxPath = outputDir + "/" + baseName + ".pptx";

                    QStringList htmlArgs;
                    htmlArgs << "-s" << "-noframes" << inputPath << htmlPath;
                    currentProcess->start("pdftohtml", htmlArgs);
                    currentProcess->waitForFinished(-1);

                    delete currentProcess;
                    currentProcess = new QProcess();

                    QStringList pptxArgs;
                    pptxArgs << htmlPath << "-o" << outPptxPath;
                    currentProcess->start("pandoc", pptxArgs);
                    currentProcess->waitForFinished(-1);
                }

            } else if (fmtIndex == 3) { // PDF to PNG Images
                QString outPathPrefix = outputDir + "/" + baseName + "_page";
                QStringList args;
                args << "-png" << "-r" << "300" << inputPath << outPathPrefix;
                currentProcess->start("pdftoppm", args);
                currentProcess->waitForFinished(-1);

            } else if (fmtIndex == 4) { // PDF to JPEG Images
                QString outPathPrefix = outputDir + "/" + baseName + "_page";
                QStringList args;
                args << "-jpeg" << "-r" << "300" << inputPath << outPathPrefix;
                currentProcess->start("pdftoppm", args);
                currentProcess->waitForFinished(-1);

            } else { // PDF to Text
                QString outPath = outputDir + "/" + baseName + ".txt";
                QStringList args;
                args << inputPath << outPath;
                currentProcess->start("pdftotext", args);
                currentProcess->waitForFinished(-1);
            }

            delete currentProcess;
            currentProcess = nullptr;
        }
        return true;
    });

    watcher->setFuture(future);
}

void DocConverterPage::processPdfMerge() {
    int totalFiles = tableMergeFiles->rowCount();
    if (totalFiles < 2) return;

    QString savePath = QFileDialog::getSaveFileName(this, "Simpan File PDF Gabungan", "pdf_combined.pdf", "PDF Document (*.pdf)");
    if (savePath.isEmpty()) return;

    cancelRequested = false;
    
    progressMerge->setRange(0, 0);
    btnStartMerge->setEnabled(false);
    btnCancelMerge->setEnabled(true);

    animFrame = 0;
    animTimer->start(200);

    QStringList inputFiles;
    for (int i = 0; i < totalFiles; ++i) {
        inputFiles << tableMergeFiles->item(i, 1)->text();
    }

    QFutureWatcher<bool> *watcher = new QFutureWatcher<bool>(this);
    connect(watcher, &QFutureWatcher<bool>::finished, this, [this, watcher, savePath]() {
        animTimer->stop();
        progressMerge->setRange(0, 100);
        progressMerge->setValue(100);

        btnStartMerge->setEnabled(true);
        btnCancelMerge->setEnabled(false);

        if (cancelRequested) {
            statusMergeLabel->setText("Status: ⛔ Penggabungan dibatalkan.");
            QMessageBox::information(this, "Batal", "Operasi penggabungan dibatalkan.");
        } else if (watcher->result()) {
            statusMergeLabel->setText("Status: ✅ Berhasil menggabungkan PDF!");
            QMessageBox::information(this, "Berhasil", "✅ File PDF berhasil digabungkan!");
        } else {
            statusMergeLabel->setText("Status: ❌ Gagal menggabungkan PDF!");
            QMessageBox::warning(this, "Gagal", "❌ Gagal menggabungkan file PDF.");
        }
        watcher->deleteLater();
    });

    QFuture<bool> future = QtConcurrent::run([this, inputFiles, savePath]() {
        currentProcess = new QProcess();
        QStringList args = inputFiles;
        args << savePath;

        currentProcess->start("pdfunite", args);
        currentProcess->waitForFinished(-1);

        bool ok = (currentProcess->exitCode() == 0);

        delete currentProcess;
        currentProcess = nullptr;

        return ok;
    });

    watcher->setFuture(future);
}