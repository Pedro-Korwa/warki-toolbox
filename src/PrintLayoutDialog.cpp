#include "PrintLayoutDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>
#include <QMessageBox>
#include <QScrollArea>

PrintLayoutDialog::PrintLayoutDialog(const QImage &sourceImg, QWidget *parent) 
    : QDialog(parent), sourceImage(sourceImg), isDragging(false), offsetX(0), offsetY(0) {
    
    photoSizes = {
        {"2 x 3 cm (21.5 x 27.9 mm)", 21.5, 27.9},
        {"3 x 4 cm (27.9 x 38.1 mm)", 27.9, 38.1},
        {"4 x 6 cm (38.1 x 55.9 mm)", 38.1, 55.9},
        {"Passport (35 x 45 mm)", 35.0, 45.0}
    };

    paperSizes = {
        {"4R (102 x 152 mm / 4x6 in)", 102.0, 152.0},
        {"3R (89 x 127 mm / 3.5x5 in)", 89.0, 127.0},
        {"5R (127 x 178 mm / 5x7 in)", 127.0, 178.0},
        {"A4 (210 x 297 mm)", 210.0, 297.0}
    };

    setupUI();
    updateLayoutPreview();
}

PrintLayoutDialog::~PrintLayoutDialog() {}

void PrintLayoutDialog::setupUI() {
    setWindowTitle("🖨️ Cetak Pasfoto & Layout Interaktif");
    resize(960, 700);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(20);

    // Panel Kiri: Pratinjau Kertas
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setStyleSheet("QScrollArea { background-color: #1e1e2e; border: 2px solid #313244; border-radius: 8px; }");
    previewLabel = new QLabel(this);
    previewLabel->setAlignment(Qt::AlignCenter);
    scrollArea->setWidget(previewLabel);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea, 3);

    // Panel Kanan: Pengaturan Cetak
    QVBoxLayout *controlLayout = new QVBoxLayout();
    
    QGroupBox *groupSettings = new QGroupBox("Pengaturan Layout & Posisi", this);
    groupSettings->setStyleSheet(
        "QGroupBox { background-color: #2b2b3b; color: #89b4fa; font-weight: bold; border: 1px solid #45475a; border-radius: 8px; margin-top: 10px; padding-top: 20px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; color: #89b4fa; }"
        "QLabel { color: #ffffff !important; font-weight: bold; font-size: 13px; }"
        "QComboBox, QSpinBox { background-color: #1e1e2e; color: #ffffff; border: 1px solid #585b70; border-radius: 4px; padding: 6px; font-size: 13px; font-weight: bold; }"
        "QComboBox QAbstractItemView { background-color: #1e1e2e; color: #ffffff; selection-background-color: #45475a; }"
        "QCheckBox { color: #ffffff !important; font-weight: bold; font-size: 13px; }"
    );

    QVBoxLayout *gLayout = new QVBoxLayout(groupSettings);
    gLayout->setSpacing(12);

    gLayout->addWidget(new QLabel("Ukuran Pasfoto:"));
    comboPhotoSize = new QComboBox(this);
    for (const auto &ps : photoSizes) {
        comboPhotoSize->addItem(ps.name);
    }
    gLayout->addWidget(comboPhotoSize);

    gLayout->addWidget(new QLabel("Ukuran Kertas Foto:"));
    comboPaperSize = new QComboBox(this);
    for (const auto &pps : paperSizes) {
        comboPaperSize->addItem(pps.name);
    }
    gLayout->addWidget(comboPaperSize);

    QHBoxLayout *gridControl = new QHBoxLayout();
    spinColumns = new QSpinBox(this);
    spinColumns->setRange(1, 15);
    spinColumns->setValue(3);
    
    spinRows = new QSpinBox(this);
    spinRows->setRange(1, 15);
    spinRows->setValue(2);

    QVBoxLayout *colBox = new QVBoxLayout();
    colBox->addWidget(new QLabel("Jumlah Kolom:"));
    colBox->addWidget(spinColumns);

    QVBoxLayout *rowBox = new QVBoxLayout();
    rowBox->addWidget(new QLabel("Jumlah Baris:"));
    rowBox->addWidget(spinRows);

    gridControl->addLayout(colBox);
    gridControl->addLayout(rowBox);
    gLayout->addLayout(gridControl);

    chkCropMarks = new QCheckBox("Tampilkan Garis Potong (Gunting)", this);
    chkCropMarks->setChecked(true);
    gLayout->addWidget(chkCropMarks);

    // Keterangan Fitur Drag Manual
    QLabel *lblInfo = new QLabel("💡 Tips: Klik & tahan (drag) pada pratinjau foto untuk menggeser posisi foto secara manual.", this);
    lblInfo->setWordWrap(true);
    lblInfo->setStyleSheet("color: #a6adc8 !important; font-size: 11px; font-style: italic; margin-top: 5px;");
    gLayout->addWidget(lblInfo);

    btnResetOffset = new QPushButton("🔄 Reset Posisi Ke Kiri Atas", this);
    btnResetOffset->setStyleSheet("QPushButton { background-color: #45475a; color: #cdd6f4; font-weight: bold; padding: 6px; border-radius: 4px; }");
    gLayout->addWidget(btnResetOffset);

    controlLayout->addWidget(groupSettings);
    controlLayout->addStretch();

    btnPrint = new QPushButton("🖨️ Cetak Sekarang", this);
    btnPrint->setStyleSheet("QPushButton { background-color: #a6e3a1; color: #11111b; font-weight: bold; font-size: 14px; padding: 12px; border-radius: 6px; }");
    
    btnClose = new QPushButton("Batal / Tutup", this);
    btnClose->setStyleSheet("QPushButton { background-color: #313244; color: #cdd6f4; font-weight: bold; font-size: 13px; padding: 10px; border-radius: 6px; }");

    controlLayout->addWidget(btnPrint);
    controlLayout->addWidget(btnClose);

    mainLayout->addLayout(controlLayout, 2);

    connect(comboPhotoSize, QOverload<int>::of(&QComboBox::currentIndexChanged), [this]() { resetOffset(); });
    connect(comboPaperSize, QOverload<int>::of(&QComboBox::currentIndexChanged), [this]() { resetOffset(); });
    connect(spinColumns, QOverload<int>::of(&QSpinBox::valueChanged), this, &PrintLayoutDialog::updateLayoutPreview);
    connect(spinRows, QOverload<int>::of(&QSpinBox::valueChanged), this, &PrintLayoutDialog::updateLayoutPreview);
    connect(chkCropMarks, &QCheckBox::toggled, this, &PrintLayoutDialog::updateLayoutPreview);
    connect(btnResetOffset, &QPushButton::clicked, this, &PrintLayoutDialog::resetOffset);
    connect(btnPrint, &QPushButton::clicked, this, &PrintLayoutDialog::printDocument);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);
}

QImage PrintLayoutDialog::generateSheetImage() {
    int dpi = 300;
    double mmToInch = 25.4;

    int pIndex = comboPaperSize->currentIndex();
    double paperW_mm = paperSizes[pIndex].widthMm;
    double paperH_mm = paperSizes[pIndex].heightMm;

    int paperPixelWidth = static_cast<int>((paperW_mm / mmToInch) * dpi);
    int paperPixelHeight = static_cast<int>((paperH_mm / mmToInch) * dpi);

    QImage sheet(paperPixelWidth, paperPixelHeight, QImage::Format_ARGB32);
    sheet.fill(Qt::white);

    QPainter painter(&sheet);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::Antialiasing);

    int photoIndex = comboPhotoSize->currentIndex();
    double photoW_mm = photoSizes[photoIndex].widthMm;
    double photoH_mm = photoSizes[photoIndex].heightMm;

    int photoW_px = static_cast<int>((photoW_mm / mmToInch) * dpi);
    int photoH_px = static_cast<int>((photoH_mm / mmToInch) * dpi);

    int cols = spinColumns->value();
    int rows = spinRows->value();

    // Margin awal dari kiri atas kertas (Pojok Kiri Atas)
    int baseStartX = static_cast<int>((5.0 / mmToInch) * dpi); // Margin aman 5 mm dari kiri
    int baseStartY = static_cast<int>((5.0 / mmToInch) * dpi); // Margin aman 5 mm dari atas

    // Terapkan Offset Drag Manual
    int startX = baseStartX + offsetX;
    int startY = baseStartY + offsetY;

    int gapX = static_cast<int>((2.0 / mmToInch) * dpi); // Jarak antar foto 2 mm
    int gapY = static_cast<int>((2.0 / mmToInch) * dpi);

    QImage scaledPhoto = sourceImage.scaled(photoW_px, photoH_px, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    if (scaledPhoto.width() > photoW_px || scaledPhoto.height() > photoH_px) {
        int cropX = (scaledPhoto.width() - photoW_px) / 2;
        int cropY = (scaledPhoto.height() - photoH_px) / 2;
        scaledPhoto = scaledPhoto.copy(cropX, cropY, photoW_px, photoH_px);
    }

    QPen cropPen(QColor(150, 150, 150), 2, Qt::DashLine);

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            int x = startX + c * (photoW_px + gapX);
            int y = startY + r * (photoH_px + gapY);

            // Pengamanan agar foto tidak tergambar di luar kanvas kertas
            if (x < paperPixelWidth && y < paperPixelHeight) {
                painter.drawImage(x, y, scaledPhoto);

                if (chkCropMarks->isChecked()) {
                    painter.setPen(cropPen);
                    painter.drawRect(x, y, photoW_px, photoH_px);
                }
            }
        }
    }

    painter.end();
    return sheet;
}

void PrintLayoutDialog::updateLayoutPreview() {
    QImage sheet = generateSheetImage();
    QPixmap pix = QPixmap::fromImage(sheet).scaled(450, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    previewLabel->setPixmap(pix);
}

void PrintLayoutDialog::resetOffset() {
    offsetX = 0;
    offsetY = 0;
    updateLayoutPreview();
}

// Logika Drag & Drop Manual
void PrintLayoutDialog::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && previewLabel->underMouse()) {
        isDragging = true;
        dragStartPos = event->pos();
    }
}

void PrintLayoutDialog::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging) {
        QPoint delta = event->pos() - dragStartPos;
        offsetX += delta.x() * 2; // Skala sensitivitas gerakan
        offsetY += delta.y() * 2;
        dragStartPos = event->pos();
        updateLayoutPreview();
    }
}

void PrintLayoutDialog::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        isDragging = false;
    }
}

void PrintLayoutDialog::printDocument() {
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog printDialog(&printer, this);
    printDialog.setWindowTitle("Cetak Pasfoto");

    if (printDialog.exec() == QDialog::Accepted) {
        QImage sheet = generateSheetImage();
        QPainter painter(&printer);
        
        QRect rect = painter.viewport();
        QSize size = sheet.size();
        size.scale(rect.size(), Qt::KeepAspectRatio);
        
        painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
        painter.setWindow(sheet.rect());
        
        painter.drawImage(0, 0, sheet);
        painter.end();

        QMessageBox::information(this, "Berhasil", "✅ Pasfoto berhasil dikirim ke printer!");
    }
}