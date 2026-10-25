#ifndef PDFCOMPRESSORDIALOG_H
#define PDFCOMPRESSORDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QProcess>
#include <QFileInfo>

class PdfCompressorDialog : public QDialog {
    Q_OBJECT

public:
    explicit PdfCompressorDialog(QWidget *parent = nullptr);
    ~PdfCompressorDialog();

private slots:
    void browseFile();
    void startCompression();

private:
    void setupUI();
    void compressPDF(const QString &inputFile, const QString &outputFile, int targetOption);
    QString formatFileSize(qint64 bytes);

    QLabel *titleLabel;
    QLabel *fileLabel;
    QPushButton *btnBrowse;
    
    QLabel *targetLabel;
    QComboBox *comboTargetSize;
    
    QPushButton *btnStart;
    QProgressBar *progressBar;
    QLabel *statusLabel;

    QString selectedFilePath;
};

#endif // PDFCOMPRESSORDIALOG_H