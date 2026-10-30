#include "OcrEngine.h"
#include <QFile>
#include <QTextStream>
#include <QTextDocument>
#include <QPrinter>
#include <QRegularExpression>
#include <opencv2/opencv.hpp>

OcrEngine::OcrEngine(QObject *parent) : QObject(parent) {
    tessApi = new tesseract::TessBaseAPI();
    
    // Inisialisasi Tesseract dengan Bahasa Indonesia dan Inggris
    if (tessApi->Init(NULL, "ind+eng", tesseract::OEM_LSTM_ONLY)) {
        tessApi->Init(NULL, "eng", tesseract::OEM_LSTM_ONLY);
    }

    // Tentukan parameter penting untuk akurasi teks & dokumen
    tessApi->SetVariable("user_defined_dpi", "300");
    tessApi->SetVariable("preserve_interword_spaces", "1");
}

OcrEngine::~OcrEngine() {
    if (tessApi) {
        tessApi->End();
        delete tessApi;
    }
}

QImage OcrEngine::preprocessForOcr(const QImage &inputImg) {
    QImage qImg = inputImg.convertToFormat(QImage::Format_RGBA8888);
    cv::Mat src(qImg.height(), qImg.width(), CV_8UC4, (void*)qImg.bits(), qImg.bytesPerLine());

    cv::Mat gray, resized, thresh;

    // 1. Ubah ke Grayscale
    cv::cvtColor(src, gray, cv::COLOR_RGBA2GRAY);

    // 2. Upscale Gambar jika tinggi kurang dari 2000px (Kunci Utama Tesseract Presisi)
    if (gray.rows < 2000) {
        double scale = 2000.0 / gray.rows;
        cv::resize(gray, resized, cv::Size(), scale, scale, cv::INTER_CUBIC);
    } else {
        resized = gray.clone();
    }

    // 3. Denoise & Binarisasi Otsu Automatic Thresholding
    cv::Mat blur;
    cv::GaussianBlur(resized, blur, cv::Size(3, 3), 0);
    cv::threshold(blur, thresh, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);

    // Convert balik ke QImage Grayscale
    QImage result(thresh.data, thresh.cols, thresh.rows, thresh.step, QImage::Format_Grayscale8);
    return result.copy();
}

QString OcrEngine::processImageToHtml(const QImage &inputImg) {
    if (inputImg.isNull() || !tessApi) return "";

    // Preprocessing tingkat tinggi via OpenCV
    QImage cleanImg = preprocessForOcr(inputImg);

    // Set PSM ke Auto Page Segmentation (Sangat ampuh untuk surat resmi bertabel)
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
}

bool OcrEngine::exportToFile(const QString &htmlContent, const QImage &sourceImg, const QString &outputPath, ExportFormat format) {
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