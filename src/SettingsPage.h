#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QFrame>

class SettingsPage : public QWidget {
    Q_OBJECT

public:
    explicit SettingsPage(QWidget *parent = nullptr);
    ~SettingsPage();

signals:
    void themeChanged(const QString &themeName);
    void wallpaperChanged(const QString &imagePath);

private slots:
    void applyTheme(int index);
    void selectWallpaper();
    void resetWallpaper();

private:
    void setupUI();

    QComboBox *comboTheme;
    QPushButton *btnBrowseWallpaper;
    QPushButton *btnResetWallpaper;
    QLabel *wallpaperPathLabel;
};

#endif // SETTINGSPAGE_H