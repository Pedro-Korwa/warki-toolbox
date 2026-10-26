#ifndef AIBGPAGE_H
#define AIBGPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QColorDialog>
#include <QFrame>
#include <QDialog>
#include <QScrollArea>
#include "BgEngine.h"

class AiBgPage : public QWidget {
    Q_OBJECT

public:
    explicit AiBgPage(QWidget *parent = nullptr);
    ~AiBgPage();

private slots:
    void browseImage();
    void applyPresetColor(const QColor &color);
    void openColorPicker();
    void saveImage();
    void showZoomModal();
    void openPrintDialog();

private:
    void setupUI();
    void updatePreview();

    QLabel *imagePreviewLabel;
    QPushButton *btnBrowse;
    QPushButton *btnRed;
    QPushButton *btnBlue;
    QPushButton *btnWhite;
    QPushButton *btnCustomColor;
    QPushButton *btnZoom;
    QPushButton *btnSave;
    QPushButton *btnPrintLayout;

    BgEngine *bgEngine;
    QImage currentLoadedImg;
    QImage processedImg;
    QString currentImgPath;
};

#endif // AIBGPAGE_H