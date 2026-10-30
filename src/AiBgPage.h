#ifndef AIBGPAGE_H
#define AIBGPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>
#include <QColorDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QScrollArea>
#include <QDialog>
#include <QResizeEvent>
#include <QTimer>
#include "BgEngine.h"

class AiBgPage : public QWidget {
    Q_OBJECT

public:
    explicit AiBgPage(QWidget *parent = nullptr);
    ~AiBgPage();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    BgEngine *bgEngine;
    
    QImage currentLoadedImg;   // Gambar asli dari user
    QImage transparentCache;   // Cache hasil potong AI RMBG-1.4
    QImage processedImg;       // Gambar hasil akhir dengan latar warna
    QString currentImgPath;

    QTimer *loadingTimer;
    int animationFrame = 0;

    QLabel *imagePreviewLabel;
    QPushButton *btnBrowse;
    QPushButton *btnZoom;
    QPushButton *btnSave;
    QPushButton *btnPrintLayout;
    QPushButton *btnRed;
    QPushButton *btnBlue;
    QPushButton *btnWhite;
    QPushButton *btnCustomColor;

    void setupUI();
    void browseImage();
    void applyPresetColor(const QColor &color);
    void openColorPicker();
    void updatePreview();
    void showZoomModal();
    void saveImage();
    void openPrintDialog();
    void renderColorOnCache(const QColor &color);
    void drawLightningLoading(const QString &statusText = "Memproses...");
};

#endif // AIBGPAGE_H