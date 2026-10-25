#include <QApplication>
#include <QStyleFactory>
#include <QTimer>
#include "MainWindow.h"
#include "SplashScreen.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Paksa Qt menggunakan style Fusion agar stylesheet kustom bekerja 100% di semua OS
    app.setStyle(QStyleFactory::create("Fusion"));

    SplashScreen splash;
    splash.show();

    MainWindow window;

    QTimer::singleShot(2500, [&]() {
        splash.close();
        window.show();
    });

    return app.exec();
}