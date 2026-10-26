#include "AiBgPage.h"
#include "PrintLayoutDialog.h"
#include <QMessageBox>
#include <QResizeEvent>

AiBgPage::AiBgPage(QWidget *parent) : QWidget(parent) {
    bgEngine = new BgEngine(this);
    setupUI();
}

AiBgPage::~AiBgPage() {}

void AiBgPage::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(15);

    QLabel *header = new QLabel("✨ AI Background Remover & Photo Color Changer", this);
    header->setObjectName("headerLabel");
    layout->addWidget(header);

    QFrame *frame = new QFrame(this);
    frame->setObjectName("cardFrame");
    QVBoxLayout *fLayout = new QVBoxLayout(frame);
    fLayout->setSpacing(15);

    // Box Pratinjau Foto Dinamis Memanfaatkan Ruang Kosong Maksimal
    imagePreviewLabel = new QLabel("Pilih foto pasfoto/produk untuk diproses...", frame);
    imagePreviewLabel->setAlignment(Qt::AlignCenter);
    imagePreviewLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    imagePreviewLabel->setStyleSheet("border: 2px dashed #313244; border-radius: 8px; background-color: #181825; color: #a6adc8;");
    fLayout->addWidget(imagePreviewLabel, 1); // Stretch factor 1 agar mengisi seluruh ruang atas

    QHBoxLayout *actionRow = new QHBoxLayout();
    btnBrowse = new QPushButton("🖼️ Pilih Foto", frame);
    btnBrowse->setStyleSheet("QPushButton { background-color: #89b4fa; color: #11111b; font-weight: bold; padding: 10px; border-radius: 6px; }");

    btnZoom = new QPushButton("🔍 Perbesar Pratinjau", frame);
    btnZoom->setStyleSheet("QPushButton { background-color: #45475a; color: #cdd6f4; font-weight: bold; padding: 10px; border-radius: 6px; }");
    btnZoom->setEnabled(false);

    btnSave = new QPushButton("💾 Simpan Foto", frame);
    btnSave->setStyleSheet("QPushButton { background-color: #a6e3a1; color: #11111b; font-weight: bold; padding: 10px; border-radius: 6px; }"
                           "QPushButton:disabled { background-color: #45475a; color: #6c7086; }");
    btnSave->setEnabled(false);

    btnPrintLayout = new QPushButton("🖨️ Layout & Cetak", frame);
    btnPrintLayout->setStyleSheet("QPushButton { background-color: #fab387; color: #11111b; font-weight: bold; padding: 10px; border-radius: 6px; }"
                                  "QPushButton:disabled { background-color: #45475a; color: #6c7086; }");
    btnPrintLayout->setEnabled(false);

    actionRow->addWidget(btnBrowse, 2);
    actionRow->addWidget(btnZoom, 1);
    actionRow->addWidget(btnSave, 1);
    actionRow->addWidget(btnPrintLayout, 1);
    fLayout->addLayout(actionRow);

    QLabel *presetLabel = new QLabel("Preset Warna Pasfoto Formal:", frame);
    presetLabel->setStyleSheet("font-weight: bold; margin-top: 5px; color: #cdd6f4;");
    fLayout->addWidget(presetLabel);

    QHBoxLayout *colorRow = new QHBoxLayout();
    btnRed = new QPushButton("🔴 Merah (#FF0000)", frame);
    btnBlue = new QPushButton("🔵 Biru (#0055FF)", frame);
    btnWhite = new QPushButton("⚪ Putih (#FFFFFF)", frame);
    btnCustomColor = new QPushButton("🎨 Custom Color", frame);

    QString btnStyle = "QPushButton { padding: 8px; font-weight: bold; border-radius: 6px; background-color: #313244; color: #cdd6f4; }"
                       "QPushButton:hover { background-color: #45475a; }";

    btnRed->setStyleSheet(btnStyle);
    btnBlue->setStyleSheet(btnStyle);
    btnWhite->setStyleSheet(btnStyle);
    btnCustomColor->setStyleSheet(btnStyle);

    btnRed->setEnabled(false);
    btnBlue->setEnabled(false);
    btnWhite->setEnabled(false);
    btnCustomColor->setEnabled(false);

    colorRow->addWidget(btnRed);
    colorRow->addWidget(btnBlue);
    colorRow->addWidget(btnWhite);
    colorRow->addWidget(btnCustomColor);
    fLayout->addLayout(colorRow);

    layout->addWidget(frame, 1);

    connect(btnBrowse, &QPushButton::clicked, this, &AiBgPage::browseImage);
    connect(btnZoom, &QPushButton::clicked, this, &AiBgPage::showZoomModal);
    connect(btnSave, &QPushButton::clicked, this, &AiBgPage::saveImage);
    connect(btnPrintLayout, &QPushButton::clicked, this, &AiBgPage::openPrintDialog);
    connect(btnRed, &QPushButton::clicked, [this]() { applyPresetColor(QColor(255, 0, 0)); });
    connect(btnBlue, &QPushButton::clicked, [this]() { applyPresetColor(QColor(0, 85, 255)); });
    connect(btnWhite, &QPushButton::clicked, [this]() { applyPresetColor(QColor(255, 255, 255)); });
    connect(btnCustomColor, &QPushButton::clicked, this, &AiBgPage::openColorPicker);
}

void AiBgPage::browseImage() {
    QString path = QFileDialog::getOpenFileName(this, "Pilih Foto", "", "Images (*.png *.jpg *.jpeg *.webp)");
    if (!path.isEmpty()) {
        currentImgPath = path;
        currentLoadedImg.load(path);
        processedImg = currentLoadedImg;

        updatePreview();

        btnRed->setEnabled(true);
        btnBlue->setEnabled(true);
        btnWhite->setEnabled(true);
        btnCustomColor->setEnabled(true);
        btnZoom->setEnabled(true);
        btnSave->setEnabled(true);
        btnPrintLayout->setEnabled(true);
    }
}

void AiBgPage::applyPresetColor(const QColor &color) {
    if (currentLoadedImg.isNull()) return;

    processedImg = bgEngine->replaceBackground(currentLoadedImg, color);
    updatePreview();
}

void AiBgPage::openColorPicker() {
    QColor chosenColor = QColorDialog::getColor(Qt::red, this, "Pilih Warna Latar");
    if (chosenColor.isValid()) {
        applyPresetColor(chosenColor);
    }
}

void AiBgPage::updatePreview() {
    if (processedImg.isNull()) return;
    
    // Menggunakan ukuran area label secara dinamis (dikurangi sedikit padding)
    QSize targetSize = imagePreviewLabel->size() - QSize(20, 20);
    if (targetSize.width() < 100 || targetSize.height() < 100) {
        targetSize = QSize(400, 400);
    }

    QPixmap p = QPixmap::fromImage(processedImg).scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    imagePreviewLabel->setPixmap(p);
}

void AiBgPage::showZoomModal() {
    if (processedImg.isNull()) return;

    QDialog zoomDialog(this);
    zoomDialog.setWindowTitle("🔍 Zoom Pratinjau Detail Foto");
    zoomDialog.resize(600, 600);

    QVBoxLayout *dLayout = new QVBoxLayout(&zoomDialog);
    QScrollArea *scrollArea = new QScrollArea(&zoomDialog);
    
    QLabel *fullLabel = new QLabel();
    fullLabel->setPixmap(QPixmap::fromImage(processedImg));
    fullLabel->setAlignment(Qt::AlignCenter);

    scrollArea->setWidget(fullLabel);
    scrollArea->setWidgetResizable(true);

    dLayout->addWidget(scrollArea);
    zoomDialog.exec();
}

void AiBgPage::saveImage() {
    if (processedImg.isNull()) return;

    QString savePath = QFileDialog::getSaveFileName(this, "Simpan Foto Hasil", "pasfoto_processed.png", "PNG Image (*.png);;JPG Image (*.jpg)");
    if (!savePath.isEmpty()) {
        if (processedImg.save(savePath)) {
            QMessageBox::information(this, "Berhasil", "✅ Foto berhasil disimpan ke perangkat!");
        } else {
            QMessageBox::warning(this, "Gagal", "❌ Gagal menyimpan foto!");
        }
    }
}

void AiBgPage::openPrintDialog() {
    if (processedImg.isNull()) return;
    PrintLayoutDialog printDlg(processedImg, this);
    printDlg.exec();
}