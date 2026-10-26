#ifndef PRINTLAYOUTDIALOG_H
#define PRINTLAYOUTDIALOG_H

#include <QDialog>
#include <QImage>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include <QPoint>
#include <QMouseEvent>

class PrintLayoutDialog : public QDialog {
    Q_OBJECT

public:
    explicit PrintLayoutDialog(const QImage &sourceImg, QWidget *parent = nullptr);
    ~PrintLayoutDialog();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void updateLayoutPreview();
    void printDocument();
    void resetOffset();

private:
    void setupUI();
    QImage generateSheetImage();

    QImage sourceImage;
    QLabel *previewLabel;
    QComboBox *comboPhotoSize;
    QComboBox *comboPaperSize;
    QSpinBox *spinColumns;
    QSpinBox *spinRows;
    QCheckBox *chkCropMarks;
    QPushButton *btnResetOffset;
    QPushButton *btnPrint;
    QPushButton *btnClose;

    // Koordinat Drag Manual
    bool isDragging;
    QPoint dragStartPos;
    int offsetX;
    int offsetY;

    struct PhotoSize {
        QString name;
        double widthMm;
        double heightMm;
    };

    struct PaperSize {
        QString name;
        double widthMm;
        double heightMm;
    };

    QList<PhotoSize> photoSizes;
    QList<PaperSize> paperSizes;
};

#endif // PRINTLAYOUTDIALOG_H