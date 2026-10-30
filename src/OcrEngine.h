#ifndef OCRENGINE_H
#define OCRENGINE_H

#include <QObject>
#include <QImage>
#include <QString>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

enum class ExportFormat {
    TXT,
    DOCX,
    XLSX,
    PDF
};

class OcrEngine : public QObject {
    Q_OBJECT

public:
    explicit OcrEngine(QObject *parent = nullptr);
    ~OcrEngine();

    // OCR Ekstrak Teks dari Gambar ke Format HTML (Rich Text Office Ready)
    QString processImageToHtml(const QImage &inputImg);

    // Simpan Hasil ke File .docx / .xlsx / .pdf / .txt Sungguhan
    bool exportToFile(const QString &htmlContent, const QImage &sourceImg, const QString &outputPath, ExportFormat format);

private:
    tesseract::TessBaseAPI *tessApi;
    QImage preprocessForOcr(const QImage &inputImg);
};

#endif // OCRENGINE_H