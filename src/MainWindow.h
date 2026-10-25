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
#include "PdfEngine.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void browsePDF();
    void startPDFCompression();
    void checkFileSizeWarning();
    void updateSpinnerAnimation(); // Slot animasi berputar

private:
    void setupUI();
    QWidget* createPDFPage();
    QString formatFileSize(qint64 bytes);
    void addHistoryRow(const QString &fileName, const QString &origSize, const QString &newSize, const QString &outputPath);

    QStackedWidget *stackedWidget;
    QLabel *pdfFileLabel;
    QPushButton *btnBrowsePDF;
    QComboBox *comboTargetSizePDF;
    QLabel *warningLabelPDF;
    QPushButton *btnStartPDF;
    QProgressBar *progressBarPDF;
    QLabel *statusLabelPDF;
    QTableWidget *historyTable;

    PdfEngine *pdfEngine;
    QString selectedPDFPath;
    qint64 selectedFileSize;

    // Timer & Variabel Animasi Spinner
    QTimer *spinnerTimer;
    int spinnerFrame;
};

#endif // MAINWINDOW_H