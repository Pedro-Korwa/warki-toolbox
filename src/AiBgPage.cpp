#include "AiBgPage.h"
#include "PrintLayoutDialog.h"
#include <QMessageBox>
#include <QResizeEvent>
#include <QPainter>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>

AiBgPage::AiBgPage(QWidget *parent) : QWidget(parent), loadingTimer(nullptr) {
    bgEngine = new BgEngine(this);
    setupUI();
}

AiBgPage::~AiBgPage() {
    if (loadingTimer) {
        loadingTimer->stop();
        delete loadingTimer;
    }
}

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

    imagePreviewLabel = new QLabel("Pilih foto pasfoto/produk untuk diproses...", frame);
    imagePreviewLabel->setAlignment(Qt::AlignCenter);
    
    // KUNCI UTAMA FIX: Mencegah QLabel memaksa ukuran Layout membengkak
    imagePreviewLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imagePreviewLabel->setMinimumSize(200, 200);
    imagePreviewLabel->setStyleSheet("border: 2px dashed #313244; border-radius: 8px; background-color: #181825; color: #a6adc8;");
    
    fLayout->addWidget(imagePreviewLabel, 1);

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
        
        transparentCache = QImage();
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

void AiBgPage::drawLightningLoading(const QString &statusText) {
    int w = imagePreviewLabel->width();
    int h = imagePreviewLabel->height();
    if (w < 100 || h < 100) { w = 300; h = 300; }

    QPixmap pixmap(w, h);
    pixmap.fill(QColor("#181825"));

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    int cx = w / 2;
    int cy = h / 2 - 15;

    int glowAlpha = 40 + (animationFrame * 15) % 80;
    QRadialGradient glow(cx, cy, 40);
    glow.setColorAt(0, QColor(137, 180, 250, glowAlpha));
    glow.setColorAt(1, QColor(24, 24, 37, 0));
    painter.setBrush(glow);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(cx - 40, cy - 40, 80, 80);

    QPolygonF lightning;
    lightning << QPointF(cx + 3, cy - 20)
              << QPointF(cx - 10, cy + 2)
              << QPointF(cx - 2, cy + 2)
              << QPointF(cx - 5, cy + 20)
              << QPointF(cx + 10, cy - 2)
              << QPointF(cx + 2, cy - 2);

    QColor mainNeon = (animationFrame % 2 == 0) ? QColor("#89b4fa") : QColor("#cba6f7");

    painter.setBrush(QColor("#11111b"));
    painter.drawPolygon(lightning.translated(2, 2));

    painter.setBrush(mainNeon);
    painter.drawPolygon(lightning);

    painter.setPen(QColor("#a6adc8"));
    QFont font("SansSerif", 9, QFont::Normal);
    painter.setFont(font);

    QRect textRect(0, cy + 30, w, 20);
    painter.drawText(textRect, Qt::AlignCenter, statusText);

    painter.end();
    
    imagePreviewLabel->setPixmap(pixmap);
}

void AiBgPage::applyPresetColor(const QColor &color) {
    if (currentLoadedImg.isNull()) return;

    if (transparentCache.isNull()) {
        this->setCursor(Qt::WaitCursor);

        btnBrowse->setEnabled(false);
        btnRed->setEnabled(false);
        btnBlue->setEnabled(false);
        btnWhite->setEnabled(false);
        btnCustomColor->setEnabled(false);

        animationFrame = 0;
        if (!loadingTimer) {
            loadingTimer = new QTimer(this);
        }
        
        loadingTimer->disconnect();
        connect(loadingTimer, &QTimer::timeout, this, [this]() {
            animationFrame++;
            drawLightningLoading("Memproses AI Background Remover...");
        });
        
        drawLightningLoading("Memproses AI Background Remover...");
        loadingTimer->start(80);

        QFutureWatcher<QImage> *watcher = new QFutureWatcher<QImage>(this);
        connect(watcher, &QFutureWatcher<QImage>::finished, this, [this, watcher, color]() {
            if (loadingTimer) {
                loadingTimer->stop();
                loadingTimer->disconnect();
            }

            transparentCache = watcher->result();
            this->setCursor(Qt::ArrowCursor);

            btnBrowse->setEnabled(true);
            btnRed->setEnabled(true);
            btnBlue->setEnabled(true);
            btnWhite->setEnabled(true);
            btnCustomColor->setEnabled(true);

            renderColorOnCache(color);
            watcher->deleteLater();
        });

        QFuture<QImage> future = QtConcurrent::run([this]() {
            return bgEngine->removeBackground(currentLoadedImg);
        });
        watcher->setFuture(future);

    } else {
        renderColorOnCache(color);
    }
}

void AiBgPage::renderColorOnCache(const QColor &color) {
    if (transparentCache.isNull()) return;

    QImage finalImg(transparentCache.size(), QImage::Format_ARGB32);
    finalImg.fill(color);

    QPainter painter(&finalImg);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawImage(0, 0, transparentCache);
    painter.end();

    processedImg = finalImg;
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
    
    // Hitung ukuran maksimum area label saat ini
    QSize labelSize = imagePreviewLabel->size();
    if (labelSize.width() < 50 || labelSize.height() < 50) return;

    // Scale pixmap agar muat persis di dalam bingkai tanpa mendorong layout keluar
    QPixmap p = QPixmap::fromImage(processedImg).scaled(
        labelSize, 
        Qt::KeepAspectRatio, 
        Qt::SmoothTransformation
    );
    
    imagePreviewLabel->setPixmap(p);
}

void AiBgPage::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    updatePreview();
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