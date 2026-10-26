#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QComboBox>
#include <QProgressBar>
#include <QTableWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QDesktopServices>
#include <QUrl>
#include <QTimer>
#include <QSpinBox>

#include "PdfEngine.h"
#include "ImageEngine.h"
#include "SettingsPage.h"
#include "AiBgPage.h" // Include header AiBgPage

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void browsePDF();
    void startPDFCompression();
    void checkFileSizeWarning();
    void updateSpinnerAnimation();
    void browseImage();
    void startImageProcess();
    
    void changeTheme(const QString &themeKey);
    void setCustomPattern(const QString &patternKey);

private:
    void setupUI();
    QWidget* createPDFPage();
    QWidget* createImagePage();
    
    QString formatFileSize(qint64 bytes);
    void addHistoryRow(QTableWidget *table, const QString &fileName, const QString &origSize, const QString &newSize, const QString &outputPath);

    QStackedWidget *stackedWidget;
    SettingsPage *settingsPage;
    AiBgPage *aiBgPage; // Member variable AiBgPage
    
    // PDF Controls
    QLabel *pdfFileLabel;
    QPushButton *btnBrowsePDF;
    QComboBox *comboTargetSizePDF;
    QLabel *warningLabelPDF;
    QPushButton *btnStartPDF;
    QProgressBar *progressBarPDF;
    QLabel *statusLabelPDF;
    QTableWidget *historyTablePDF;

    // Image Controls
    QLabel *imgFileLabel;
    QPushButton *btnBrowseImg;
    QSpinBox *spinWidth;
    QSpinBox *spinHeight;
    QSpinBox *spinQuality;
    QPushButton *btnStartImg;
    QLabel *statusLabelImg;
    QTableWidget *historyTableImg;

    PdfEngine *pdfEngine;
    ImageEngine *imageEngine;
    
    QString selectedPDFPath;
    qint64 selectedFileSize;
    QString selectedImagePath;

    QTimer *spinnerTimer;
    int spinnerFrame;
    QString currentTheme;
};

#endif // MAINWINDOW_H