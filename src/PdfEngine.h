#ifndef PDFENGINE_H
#define PDFENGINE_H

#include <QObject>
#include <QString>
#include <QProcess>

enum class TargetPreset {
    Target100_300KB,  // 72 DPI (Ultra)
    Target300_500KB,  // 96 DPI (Ekstrem)
    Target1MB_2MB,    // 150 DPI (Medium/Standard)
    UpscaleHQ         // 300 DPI (Naikkan Kualitas / HQ)
};

class PdfEngine : public QObject {
    Q_OBJECT

public:
    explicit PdfEngine(QObject *parent = nullptr);
    void compressAsync(const QString &inputFile, const QString &outputFile, TargetPreset preset);

signals:
    void compressionFinished(bool success, QString inputFile, QString outputFile);

private:
    QProcess *process;
};

#endif // PDFENGINE_H