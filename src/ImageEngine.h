#ifndef IMAGEENGINE_H
#define IMAGEENGINE_H

#include <QObject>
#include <QImage>
#include <QString>

class ImageEngine : public QObject {
    Q_OBJECT

public:
    explicit ImageEngine(QObject *parent = nullptr);
    bool resizeAndCompress(const QString &inputPath, const QString &outputPath, int targetWidth, int targetHeight, int quality);
};

#endif // IMAGEENGINE_H