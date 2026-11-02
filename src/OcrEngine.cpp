#include "OcrEngine.h"
#include <QFile>
#include <QTextStream>
#include <QTextDocument>
#include <QPrinter>
#include <QRegularExpression>
#include <QDebug>

OcrEngine::OcrEngine(QObject *parent) : QObject(parent) {
#ifdef ENABLE_OCR
    tessApi = new tesseract::TessBaseAPI();
    
    // Inisialisasi Tesseract dengan Bahasa Indonesia dan Inggris
    if (tessApi->Init(NULL, "ind+eng", tesseract::OEM_LSTM_ONLY)) {
        tessApi->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);
    }

    // Tentukan parameter penting untuk akurasi teks & dokumen
    tessApi->SetVariable("user_defined_dpi", "300");
    tessApi->SetVariable("preserve_interword_spaces", "1");
#endif
}

OcrEngine::~OcrEngine() {
#ifdef ENABLE_OCR
    if (tessApi) {
        tessApi->End();
        delete tessApi;
    }
#endif
}

QImage OcrEngine::preprocessForOcr(const QImage &inputImg) {
    if (inputImg.isNull()) return QImage();

    // 1. Ubah ke Grayscale8 menggunakan fungsi bawaan Qt (Tanpa OpenCV)
    QImage gray = inputImg.convertToFormat(QImage::Format_Grayscale8);

    // 2. Upscale Gambar jika tinggi kurang dari 2000px untuk meningkatkan presisi OCR
    if (gray.height() < 2000 && gray.height() > 0) {
        double scale = 2000.0 / gray.height();
        int newWidth = static_cast<int>(gray.width() * scale);
        gray = gray.scaled(newWidth, 2000, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    return gray;
}

QString OcrEngine::processImageToHtml(const QImage &inputImg) {
    if (inputImg.isNull()) return "";

#ifdef ENABLE_OCR
    if (!tessApi) return "";

    // Preprocessing gambar menggunakan Qt Native
    QImage cleanImg = preprocessForOcr(inputImg);

    // Set PSM ke Auto Page Segmentation
    tessApi->SetPageSegMode(tesseract::PSM_AUTO);
    tessApi->SetImage(cleanImg.bits(), cleanImg.width(), cleanImg.height(), 1, cleanImg.bytesPerLine());

    // Ambil hasil berbasis hOCR (Simpan koordinat & layout)
    char *hocrText = tessApi->GetHOCRText(0);
    if (!hocrText) return "";

    QString hocrStr = QString::fromUtf8(hocrText);
    delete[] hocrText;

    // Bungkus dengan style CSS Microsoft Word / Office 2021
    QString styledHtml = "<html><head><style>"
                         "body { font-family: 'Calibri', 'Segoe UI', Arial, sans-serif; font-size: 11pt; color: #11111b; line-height: 1.4; background-color: #ffffff; padding: 25px; }"
                         ".ocr_page { width: 100%; }"
                         ".ocr_line { display: block; margin-bottom: 5px; }"
                         ".ocrx_word { margin-right: 3px; }"
                         "table { border-collapse: collapse; width: 100%; margin-top: 10px; margin-bottom: 10px; }"
                         "td, th { border: 1px solid #000000; padding: 6px; text-align: left; font-size: 10pt; }"
                         "</style></head><body>" + hocrStr + "</body></html>";

    return styledHtml;
#else
    qWarning() << "Tesseract & OpenCV OCR are disabled in this build configuration.";
    return QString("<html><body><p>Fitur OCR dinonaktifkan pada versi build ini.</p></body></html>");
#endif
}

bool OcrEngine::exportToFile(const QString &htmlContent, const QImage &sourceImg, const QString &outputPath, ExportFormat format) {
    Q_UNUSED(sourceImg);
    if (htmlContent.isEmpty()) return false;

    if (format == ExportFormat::PDF) {
        QTextDocument doc;
        doc.setHtml(htmlContent);

        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(outputPath);
        printer.setPageSize(QPageSize(QPageSize::A4));
        printer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);

        doc.print(&printer);
        return true;

    } else if (format == ExportFormat::DOCX) {
        QFile file(outputPath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

        QTextStream out(&file);
        out << "<html xmlns:o='urn:schemas-microsoft-com:office:office' "
            << "xmlns:w='urn:schemas-microsoft-com:office:word' "
            << "xmlns='http://www.w3.org/TR/REC-html40'>\n"
            << "<head><meta charset='utf-8'><title>Document</title></head>\n"
            << "<body>\n"
            << htmlContent
            << "\n</body></html>";

        file.close();
        return true;

    } else if (format == ExportFormat::XLSX) {
        QFile file(outputPath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

        QTextStream out(&file);
        out << "<html><head><meta charset='utf-8'></head><body>\n"
            << htmlContent
            << "\n</body></html>";

        file.close();
        return true;

    } else { // TXT
        QFile file(outputPath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;

        QTextDocument doc;
        doc.setHtml(htmlContent);

        QTextStream out(&file);
        out << doc.toPlainText();
        file.close();
        return true;
    }
}