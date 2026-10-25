#include "SplashScreen.h"

SplashScreen::SplashScreen(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(540, 360);

    QVBoxLayout *layout = new QVBoxLayout(this);
    
    QWidget *box = new QWidget(this);
    box->setStyleSheet("background-color: #0d0e15; border: 2px solid #89b4fa; border-radius: 20px;");
    QVBoxLayout *boxLayout = new QVBoxLayout(box);
    boxLayout->setContentsMargins(30, 35, 30, 35);

    // Icon Petir Biru
    logoLabel = new QLabel("⚡", box);
    logoLabel->setAlignment(Qt::AlignCenter);
    
    // Title App
    titleLabel = new QLabel("WARKI TOOLBOX", box);
    titleLabel->setAlignment(Qt::AlignCenter);
    
    // Tagline Developer Biak
    developerLabel = new QLabel("BY BIAK DEVELOPER", box);
    developerLabel->setAlignment(Qt::AlignCenter);
    developerLabel->setStyleSheet("font-size: 11px; font-weight: bold; color: #fab387; letter-spacing: 3px; background: transparent; border: none; margin-top: -5px;");

    // Subtitle
    subTitleLabel = new QLabel("High-Performance Offline Desktop Engine", box);
    subTitleLabel->setAlignment(Qt::AlignCenter);
    subTitleLabel->setStyleSheet("font-size: 12px; color: #a6adc8; background: transparent; border: none; margin-top: 10px;");

    boxLayout->addWidget(logoLabel);
    boxLayout->addWidget(titleLabel);
    boxLayout->addWidget(developerLabel);
    boxLayout->addWidget(subTitleLabel);

    layout->addWidget(box);

    // Timer Animasi Petir Realistis (Efek Kilat / Flicker)
    lightningTimer = new QTimer(this);
    connect(lightningTimer, &QTimer::timeout, this, &SplashScreen::animateLightning);
    lightningTimer->start(60);
}

SplashScreen::~SplashScreen() {}

void SplashScreen::animateLightning() {
    // Simulasi kilatan petir acak (flicker intensity)
    int isFlicker = QRandomGenerator::global()->bounded(0, 10);
    int blueGlow = (isFlicker > 7) ? 255 : QRandomGenerator::global()->bounded(160, 230);
    int fontSize = (isFlicker > 8) ? 76 : 72; // Efek denyut kejut

    QString colorStr = QString("rgb(137, 180, %1)").arg(blueGlow);
    
    logoLabel->setStyleSheet(QString("font-size: %1px; color: %2; background: transparent; border: none;").arg(fontSize).arg(colorStr));
    titleLabel->setStyleSheet(QString("font-size: 26px; font-weight: 900; color: %1; letter-spacing: 2px; background: transparent; border: none;").arg(colorStr));
}