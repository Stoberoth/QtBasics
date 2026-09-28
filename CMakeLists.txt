cmake_minimum_required(VERSION 3.16)

project(Qt_basics VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Core Quick Qml)

# macro QT6 qui remplace les automoc ....
qt_standard_project_setup()

# création de la cible executable
qt_add_executable(appQt_basics src/main.cpp)

# compile les fichiers QML
qt_add_qml_module(appQt_basics
    URI Qt_basics
    VERSION 1.0
    QML_FILES qml/Main.qml
)

target_link_libraries(appQt_basics PRIVATE Qt6::Core Qt6::Quick Qt6::Qml)
