cmake_minimum_required(VERSION 3.16)
project(WarkiToolbox VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 1. Cari Package Qt 6 (Termasuk PrintSupport)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets PrintSupport)

# 2. Definisikan Target Executable
add_executable(WarkiToolbox
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/PdfEngine.cpp
    src/PdfEngine.h
    src/ImageEngine.cpp
    src/ImageEngine.h
    src/SplashScreen.cpp
    src/SplashScreen.h
    src/SettingsPage.cpp
    src/SettingsPage.h
    src/BgEngine.cpp
    src/BgEngine.h
    src/AiBgPage.cpp
    src/AiBgPage.h
    src/PrintLayoutDialog.cpp
    src/PrintLayoutDialog.h
)

# 3. Hubungkan Libraries
target_link_libraries(WarkiToolbox PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::PrintSupport
)