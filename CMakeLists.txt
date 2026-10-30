cmake_minimum_required(VERSION 3.16)
project(WarkiToolbox VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 1. Cari Package Utama (Qt6, OpenCV, Tesseract, Leptonica)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets PrintSupport Concurrent)
find_package(OpenCV REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(TESSERACT REQUIRED tesseract)
pkg_check_modules(LEPTONICA REQUIRED lept)

# 2. Daftarkan Seluruh File Source dan Header C++
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
    src/OcrEngine.cpp
    src/OcrEngine.h
    src/DocConverterPage.cpp
    src/DocConverterPage.h
)

# 3. Include Directories
target_include_directories(WarkiToolbox PRIVATE 
    ${OpenCV_INCLUDE_DIRS} 
    ${TESSERACT_INCLUDE_DIRS} 
    ${LEPTONICA_INCLUDE_DIRS}
)

# 4. Link Libraries
target_link_libraries(WarkiToolbox PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::PrintSupport
    Qt6::Concurrent
    ${OpenCV_LIBS}
    ${TESSERACT_LIBRARIES}
    ${LEPTONICA_LIBRARIES}
)