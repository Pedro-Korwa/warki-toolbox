#include "ImageEngine.h"

ImageEngine::ImageEngine(QObject *parent) : QObject(parent) {}

bool ImageEngine::resizeAndCompress(const QString &inputPath, const QString &outputPath, int targetWidth, int targetHeight, int quality) {
    QImage img(inputPath);
    if (img.isNull()) return false;

    QImage resizedImg;
    if (targetWidth > 0 && targetHeight > 0) {
        resizedImg = img.scaled(targetWidth, targetHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    } else {
        resizedImg = img;
    }

    return resizedImg.save(outputPath, nullptr, quality);
}