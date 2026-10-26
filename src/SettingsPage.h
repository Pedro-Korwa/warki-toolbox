#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
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
    void patternChanged(const QString &patternKey);

private slots:
    void applyTheme(int index);
    void applyPattern(int index);

private:
    void setupUI();

    QComboBox *comboTheme;
    QComboBox *comboPattern;
};

#endif // SETTINGSPAGE_H