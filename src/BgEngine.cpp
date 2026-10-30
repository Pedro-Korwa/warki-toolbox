#include "BgEngine.h"
#include <QPainter>
#include <QFile>
#include <QDir>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

BgEngine::BgEngine(QObject *parent) : QObject(parent) {}
BgEngine::~BgEngine() {}

QImage BgEngine::removeBackground(const QImage &inputImg) {
    if (inputImg.isNull()) return inputImg;

    // Cari lokasi model rmbg-1.4.onnx di beberapa kemungkinan path
    QString modelPath = "";
    QStringList possiblePaths = {
        "models/rmbg-1.4.onnx",
        "../models/rmbg-1.4.onnx",
        QDir::currentPath() + "/models/rmbg-1.4.onnx",
        QDir::currentPath() + "/../models/rmbg-1.4.onnx"
    };

    for (const QString &path : possiblePaths) {
        if (QFile::exists(path)) {
            modelPath = path;
            break;
        }
    }

    if (modelPath.isEmpty()) {
        return inputImg; // Fallback jika file model tidak ada
    }

    try {
        cv::dnn::Net net = cv::dnn::readNetFromONNX(modelPath.toStdString());
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);

        QImage qImg = inputImg.convertToFormat(QImage::Format_RGBA8888);
        cv::Mat matImg(qImg.height(), qImg.width(), CV_8UC4, (void*)qImg.bits(), qImg.bytesPerLine());

        cv::Mat rgbMat;
        cv::cvtColor(matImg, rgbMat, cv::COLOR_RGBA2RGB);

        cv::Mat blob = cv::dnn::blobFromImage(rgbMat, 1.0 / 255.0, cv::Size(1024, 1024), 
                                              cv::Scalar(0, 0, 0), true, false);

        net.setInput(blob);
        cv::Mat output = net.forward();

        int dims[] = {1024, 1024};
        cv::Mat mask1024(2, dims, CV_32F, output.ptr<float>());

        cv::Mat maskNorm;
        mask1024.convertTo(maskNorm, CV_8U, 255.0);

        cv::Mat finalMask;
        cv::resize(maskNorm, finalMask, cv::Size(qImg.width(), qImg.height()), 0, 0, cv::INTER_LINEAR);

        QImage resultImg = qImg.copy();
        for (int y = 0; y < resultImg.height(); ++y) {
            uchar *line = resultImg.scanLine(y);
            const uchar *maskLine = finalMask.ptr<uchar>(y);
            for (int x = 0; x < resultImg.width(); ++x) {
                line[x * 4 + 3] = maskLine[x];
            }
        }

        return resultImg;

    } catch (const cv::Exception &e) {
        return inputImg;
    }
}

QImage BgEngine::replaceBackground(const QImage &inputImg, const QColor &bgColor) {
    QImage transparentImg = removeBackground(inputImg);
    
    QImage finalImage(transparentImg.size(), QImage::Format_ARGB32);
    finalImage.fill(bgColor);

    QPainter painter(&finalImage);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawImage(0, 0, transparentImg);
    painter.end();

    return finalImage;
}