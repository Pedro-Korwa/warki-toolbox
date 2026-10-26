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
    header->setObjectName("headerLabel");
    layout->addWidget(header);

    // Box 1: Tema & Pola Latar Belakang
    QFrame *themeFrame = new QFrame(this);
    themeFrame->setObjectName("cardFrame");
    QVBoxLayout *themeLayout = new QVBoxLayout(themeFrame);

    QLabel *themeTitle = new QLabel("🎨 Tema & Pola Wallpaper Latar Belakang", themeFrame);
    themeTitle->setStyleSheet("font-size: 15px; font-weight: bold;");
    themeLayout->addWidget(themeTitle);

    // Dropdown Tema
    QHBoxLayout *themeRow = new QHBoxLayout();
    QLabel *labelTheme = new QLabel("Pilihan Tema Warna:", themeFrame);
    
    comboTheme = new QComboBox(themeFrame);
    comboTheme->setView(new QListView(comboTheme));
    comboTheme->addItem("🌙 Dark Catppuccin (Bawaan)", "dark");
    comboTheme->addItem("☀️ Light Crisp", "light");
    comboTheme->addItem("⚡ Kasuari Cyber Neon", "cyber");

    themeRow->addWidget(labelTheme);
    themeRow->addWidget(comboTheme, 1);
    themeLayout->addLayout(themeRow);

    // Dropdown Pola Latar
    QHBoxLayout *patternRow = new QHBoxLayout();
    QLabel *labelPattern = new QLabel("Pola Wallpaper (Fast Render):", themeFrame);

    comboPattern = new QComboBox(themeFrame);
    comboPattern->setView(new QListView(comboPattern));
    comboPattern->addItem("🚫 Tanpa Pola (Solid Clean)", "none");
    comboPattern->addItem("🌌 Pola Titik-Titik (Matrix Dots)", "dots");
    comboPattern->addItem("📐 Pola Garis Kisi (Modern Grid)", "grid");
    comboPattern->addItem("🖥️ Pola Garis Scan (Retro Lines)", "scanlines");

    patternRow->addWidget(labelPattern);
    patternRow->addWidget(comboPattern, 1);
    themeLayout->addLayout(patternRow);

    layout->addWidget(themeFrame);

    // Box 2: Tentang Aplikasi & GitHub
    QFrame *aboutFrame = new QFrame(this);
    aboutFrame->setObjectName("cardFrame");
    QVBoxLayout *aboutLayout = new QVBoxLayout(aboutFrame);

    QLabel *aboutTitle = new QLabel("Tentang Warki Toolbox", aboutFrame);
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
    connect(comboPattern, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SettingsPage::applyPattern);
    
    connect(btnGithub, &QPushButton::clicked, []() {
        QDesktopServices::openUrl(QUrl("https://github.com/Pedro-Korwa/warki-toolbox"));
    });
}

void SettingsPage::applyTheme(int index) {
    QString themeKey = comboTheme->itemData(index).toString();
    emit themeChanged(themeKey);
}

void SettingsPage::applyPattern(int index) {
    QString patternKey = comboPattern->itemData(index).toString();
    emit patternChanged(patternKey);
}