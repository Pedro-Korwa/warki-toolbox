#include "SettingsPage.h"
#include <QListView>

SettingsPage::SettingsPage(QWidget *parent) : QWidget(parent) {
    setupUI();
}

SettingsPage::~SettingsPage() {}

void SettingsPage::setupUI() {
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(20);

    QLabel *header = new QLabel("⚙️ Pengaturan & Kustomisasi Tampilan", this);
    header->setStyleSheet("font-size: 22px; font-weight: bold; color: #89b4fa;");
    layout->addWidget(header);

    // Box 1: Theme & Wallpaper
    QFrame *themeFrame = new QFrame(this);
    themeFrame->setObjectName("cardFrame");
    QVBoxLayout *themeLayout = new QVBoxLayout(themeFrame);

    QLabel *themeTitle = new QLabel("🎨 Tema & Latar Belakang Aplikasi", themeFrame);
    themeTitle->setStyleSheet("font-size: 15px; font-weight: bold;");
    themeLayout->addWidget(themeTitle);

    QHBoxLayout *themeRow = new QHBoxLayout();
    QLabel *labelTheme = new QLabel("Pilihan Tema:", themeFrame);
    
    comboTheme = new QComboBox(themeFrame);
    comboTheme->setView(new QListView(comboTheme)); // Wajib agar popup view mengikuti stylesheet 100%
    comboTheme->addItem("🌙 Dark Catppuccin (Bawaan)", "dark");
    comboTheme->addItem("☀️ Light Crisp", "light");
    comboTheme->addItem("⚡ Kasuari Cyber Neon", "cyber");

    themeRow->addWidget(labelTheme);
    themeRow->addWidget(comboTheme, 1);
    themeLayout->addLayout(themeRow);

    QHBoxLayout *wallRow = new QHBoxLayout();
    wallpaperPathLabel = new QLabel("Tidak ada wallpaper kustom (Standar)", themeFrame);
    wallpaperPathLabel->setStyleSheet("font-size: 11px;");

    btnBrowseWallpaper = new QPushButton("🖼️ Pilih Wallpaper", themeFrame);
    btnBrowseWallpaper->setStyleSheet("QPushButton { background-color: #89b4fa; color: #11111b; font-weight: bold; padding: 8px 14px; border-radius: 6px; }");

    btnResetWallpaper = new QPushButton("🔄 Reset", themeFrame);
    btnResetWallpaper->setStyleSheet("QPushButton { background-color: #f38ba8; color: #11111b; font-weight: bold; padding: 8px 14px; border-radius: 6px; }");

    wallRow->addWidget(wallpaperPathLabel, 1);
    wallRow->addWidget(btnBrowseWallpaper);
    wallRow->addWidget(btnResetWallpaper);
    themeLayout->addLayout(wallRow);

    layout->addWidget(themeFrame);

    // Box 2: About & GitHub
    QFrame *aboutFrame = new QFrame(this);
    aboutFrame->setObjectName("cardFrame");
    QVBoxLayout *aboutLayout = new QVBoxLayout(aboutFrame);

    QLabel *aboutTitle = new QLabel("ℹ️ Tentang Warki Toolbox", aboutFrame);
    aboutTitle->setStyleSheet("font-size: 15px; font-weight: bold;");

    QLabel *appInfo = new QLabel(
        "<b>Warki Toolbox v1.0</b><br>"
        "Dikembangkan oleh: <b>BIAK DEVELOPER</b><br><br>"
        "Warki Toolbox adalah aplikasi utilitas desktop open-source berperforma tinggi yang berjalan 100% secara offline. "
        "Dirancang menggunakan C++20 dan Qt 6 untuk kebutuhan optimasi dokumen PDF, image resizing, serta pemrosesan AI.", 
        aboutFrame
    );
    appInfo->setWordWrap(true);
    appInfo->setStyleSheet("font-size: 12px; line-height: 1.4;");

    QPushButton *btnGithub = new QPushButton("🌐 Kunjungi Repository GitHub", aboutFrame);
    btnGithub->setStyleSheet("QPushButton { background-color: #a6e3a1; color: #11111b; font-weight: bold; padding: 10px; border-radius: 6px; margin-top: 10px; }"
                             "QPushButton:hover { background-color: #94e2d5; }");

    aboutLayout->addWidget(aboutTitle);
    aboutLayout->addWidget(appInfo);
    aboutLayout->addWidget(btnGithub);

    layout->addWidget(aboutFrame);
    layout->addStretch();

    connect(comboTheme, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SettingsPage::applyTheme);
    connect(btnBrowseWallpaper, &QPushButton::clicked, this, &SettingsPage::selectWallpaper);
    connect(btnResetWallpaper, &QPushButton::clicked, this, &SettingsPage::resetWallpaper);
    
    connect(btnGithub, &QPushButton::clicked, []() {
        QDesktopServices::openUrl(QUrl("https://github.com/Pedro-Korwa/warki-toolbox"));
    });
}

void SettingsPage::applyTheme(int index) {
    QString themeKey = comboTheme->itemData(index).toString();
    emit themeChanged(themeKey);
}

void SettingsPage::selectWallpaper() {
    QString imagePath = QFileDialog::getOpenFileName(this, "Pilih Gambar Wallpaper", "", "Image Files (*.png *.jpg *.jpeg *.webp)");
    if (!imagePath.isEmpty()) {
        wallpaperPathLabel->setText("Wallpaper: " + QFileInfo(imagePath).fileName());
        emit wallpaperChanged(imagePath);
    }
}

void SettingsPage::resetWallpaper() {
    wallpaperPathLabel->setText("Tidak ada wallpaper kustom (Standar)");
    emit wallpaperChanged("");
}