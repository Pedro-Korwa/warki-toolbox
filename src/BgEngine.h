#ifndef BGENGINE_H
#define BGENGINE_H

#include <QObject>
#include <QImage>
#include <QColor>
#include <QString>

class BgEngine : public QObject {
    Q_OBJECT

public:
    explicit BgEngine(QObject *parent = nullptr);
    ~BgEngine();

    QImage replaceBackground(const QImage &inputImg, const QColor &bgColor);
    QImage removeBackground(const QImage &inputImg);
};

#endif // BGENGINE_H