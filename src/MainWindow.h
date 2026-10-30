#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QProgressBar>
#include <QTableWidget>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QTimer>

#include "PdfEngine.h"
#include "ImageEngine.h"
#include "SettingsPage.h"
#include "AiBgPage.h"
#include "DocConverterPage.h" // Include header baru

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void browsePDF();
    void startPDFCompression();
    void browseImage();
    void startImageProcess();
    void checkFileSizeWarning();
    void updateSpinnerAnimation();
    void changeTheme(const QString &themeKey);
    void setCustomPattern(const QString &patternKey);

private:
    PdfEngine *pdfEngine;
    ImageEngine *imageEngine;
    SettingsPage *settingsPage;
    AiBgPage *aiBgPage;
    DocConverterPage *docConverterPage; // Deklarasi variabel baru di sini

    QStackedWidget *stackedWidget;

    // PDF Page Widgets
    QLabel *pdfFileLabel;
    QComboBox *comboTargetSizePDF;
    QPushButton *btnBrowsePDF;
    QPushButton *btnStartPDF;
    QLabel *warningLabelPDF;
    QProgressBar *progressBarPDF;
    QLabel *statusLabelPDF;
    QTableWidget *historyTablePDF;

    // Image Page Widgets
    QLabel *imgFileLabel;
    QSpinBox *spinWidth;
    QSpinBox *spinHeight;
    QSpinBox *spinQuality;
    QPushButton *btnBrowseImg;
    QPushButton *btnStartImg;
    QLabel *statusLabelImg;
    QTableWidget *historyTableImg;

    // State Variables
    QString selectedPDFPath;
    qint64 selectedFileSize;
    QString selectedImagePath;
    QTimer *spinnerTimer;
    int spinnerFrame;
    QString currentTheme;

    void setupUI();
    QWidget* createPDFPage();
    QWidget* createImagePage();
    void addHistoryRow(QTableWidget *table, const QString &fileName, const QString &origSize, const QString &newSize, const QString &outputPath);
    QString formatFileSize(qint64 bytes);
};

#endif // MAINWINDOW_H