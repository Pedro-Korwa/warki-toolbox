#include "PdfEngine.h"

PdfEngine::PdfEngine(QObject *parent) : QObject(parent), process(nullptr) {}

void PdfEngine::compressAsync(const QString &inputFile, const QString &outputFile, TargetPreset preset) {
    if (process) {
        process->deleteLater();
    }
    process = new QProcess(this);

    QStringList arguments;
    arguments << "-sDEVICE=pdfwrite"
              << "-dCompatibilityLevel=1.4"
              << "-dNOPAUSE"
              << "-dQUIET"
              << "-dBATCH"
              << "-dCompressFonts=true"
              << "-dSubsetFonts=true"
              << "-dEmbedAllFonts=true";

    if (preset == TargetPreset::Target100_300KB) {
        arguments << "-dPDFSETTINGS=/screen"
                  << "-dDownsampleColorImages=true"
                  << "-dColorImageDownsampleType=/Bicubic"
                  << "-dColorImageResolution=72"
                  << "-dGrayImageDownsampleType=/Bicubic"
                  << "-dGrayImageResolution=72";
    } else if (preset == TargetPreset::Target300_500KB) {
        arguments << "-dPDFSETTINGS=/screen"
                  << "-dDownsampleColorImages=true"
                  << "-dColorImageDownsampleType=/Bicubic"
                  << "-dColorImageResolution=96"
                  << "-dGrayImageDownsampleType=/Bicubic"
                  << "-dGrayImageResolution=96";
    } else if (preset == TargetPreset::Target1MB_2MB) {
        arguments << "-dPDFSETTINGS=/ebook"
                  << "-dDownsampleColorImages=true"
                  << "-dColorImageResolution=150"
                  << "-dGrayImageResolution=150";
    } else if (preset == TargetPreset::UpscaleHQ) {
        // Mode Naikkan Kualitas / HQ (300 DPI & High Quality Rendering)
        arguments << "-dPDFSETTINGS=/printer"
                  << "-dColorImageResolution=300"
                  << "-dGrayImageResolution=300";
    }

    arguments << "-sOutputFile=" + outputFile << inputFile;

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this, inputFile, outputFile](int exitCode, QProcess::ExitStatus exitStatus) {
                bool success = (exitCode == 0 && exitStatus == QProcess::NormalExit);
                emit compressionFinished(success, inputFile, outputFile);
            });

    process->start("gs", arguments);
}