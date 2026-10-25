cmake_minimum_required(VERSION 3.16)
project(WarkiToolbox VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(WarkiToolbox
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/PdfEngine.h
    src/PdfEngine.cpp
)

target_link_libraries(WarkiToolbox PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)