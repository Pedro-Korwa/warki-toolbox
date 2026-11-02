#include <QApplication>
#include <QStyleFactory>
#include <QTimer>
#include <QMessageBox>
#include "MainWindow.h"
#include "SplashScreen.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Paksa Qt menggunakan style Fusion agar stylesheet kustom bekerja 100% di semua OS
    app.setStyle(QStyleFactory::create("Fusion"));

    try {
        // Alokasi memori heap via pointer untuk SplashScreen dan MainWindow
        SplashScreen *splash = new SplashScreen();
        splash->show();

        MainWindow *window = new MainWindow();

        QTimer::singleShot(2500, [splash, window]() {
            splash->close();
            splash->deleteLater();
            window->show();
        });

        return app.exec();
    } catch (const std::exception &e) {
        QMessageBox::critical(nullptr, "Fatal Error", e.what());
        return -1;
    } catch (...) {
        QMessageBox::critical(nullptr, "Fatal Error", "An unknown fatal error occurred on startup.");
        return -1;
    }
}