#include "BgEngine.h"
#include <QPainter>
#include <QQueue>
#include <QPoint>

BgEngine::BgEngine(QObject *parent) : QObject(parent) {}

BgEngine::~BgEngine() {}

QImage BgEngine::removeBackground(const QImage &inputImg) {
    QImage result = inputImg.convertToFormat(QImage::Format_ARGB32);
    int width = result.width();
    int height = result.height();

    QImage alphaMask(width, height, QImage::Format_Grayscale8);
    alphaMask.fill(0); // 0 = Subjek, 255 = Background

    QRgb cornerPixel = result.pixel(0, 0);
    int bgR = qRed(cornerPixel);
    int bgG = qGreen(cornerPixel);
    int bgB = qBlue(cornerPixel);

    int threshold = 40; // Batas toleransi warna latar asli

    QQueue<QPoint> queue;
    QList<QPoint> seeds = { QPoint(0, 0), QPoint(width - 1, 0), QPoint(0, height - 1), QPoint(width - 1, height - 1) };

    for (const QPoint &seed : seeds) {
        queue.enqueue(seed);
        alphaMask.scanLine(seed.y())[seed.x()] = 255;
    }

    int dx[] = {-1, 1, 0, 0};
    int dy[] = {0, 0, -1, 1};

    while (!queue.isEmpty()) {
        QPoint p = queue.dequeue();

        for (int i = 0; i < 4; ++i) {
            int nx = p.x() + dx[i];
            int ny = p.y() + dy[i];

            if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                uchar *maskRow = alphaMask.scanLine(ny);
                if (maskRow[nx] == 0) {
                    QRgb px = result.pixel(nx, ny);
                    int diffR = qAbs(qRed(px) - bgR);
                    int diffG = qAbs(qGreen(px) - bgG);
                    int diffB = qAbs(qBlue(px) - bgB);

                    if (diffR < threshold && diffG < threshold && diffB < threshold) {
                        maskRow[nx] = 255;
                        queue.enqueue(QPoint(nx, ny));
                    }
                }
            }
        }
    }

    // Jadikan area latar belakang murni transparan (Alpha = 0)
    for (int y = 0; y < height; ++y) {
        QRgb *line = reinterpret_cast<QRgb*>(result.scanLine(y));
        const uchar *maskRow = alphaMask.constScanLine(y);
        for (int x = 0; x < width; ++x) {
            if (maskRow[x] == 255) {
                line[x] = qRgba(0, 0, 0, 0); // Transparan murni
            }
            // PENTING: Piksel subjek dibiarkan 100% utuh tanpa diubah warnanya!
        }
    }

    return result;
}

QImage BgEngine::replaceBackground(const QImage &inputImg, const QColor &bgColor) {
    // 1. Ambil subjek transparan yang bersih
    QImage transparentImg = removeBackground(inputImg);

    // 2. Buat kanvas baru dengan warna latar belakang pilihan (Merah/Biru/Custom)
    QImage finalImage(transparentImg.size(), QImage::Format_ARGB32);
    finalImage.fill(bgColor);

    // 3. Tempelkan subjek di atas warna latar baru tanpa modifikasi warna internal
    QPainter painter(&finalImage);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.drawImage(0, 0, transparentImg);
    painter.end();

    return finalImage;
}