#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>
#include <QRandomGenerator>

class SplashScreen : public QWidget {
    Q_OBJECT

public:
    explicit SplashScreen(QWidget *parent = nullptr);
    ~SplashScreen();

private slots:
    void animateLightning();

private:
    QLabel *logoLabel;
    QLabel *titleLabel;
    QLabel *developerLabel;
    QLabel *subTitleLabel;
    QTimer *lightningTimer;
};

#endif // SPLASHSCREEN_H