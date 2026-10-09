cmake_minimum_required(VERSION 3.16)

project(Qt_basics VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Core Quick Qml Concurrent Network)

# macro QT6 qui remplace les automoc ....
qt_standard_project_setup()

# création de la cible executable
qt_add_executable(appQt_basics include/ImportWorker.h
                              src/ImportWorker.cpp src/main.cpp )

target_include_directories(appQt_basics PRIVATE include)

# compile les fichiers QML
qt_add_qml_module(appQt_basics
    URI Qt_basics
    VERSION 1.0
    QML_FILES qml/Main.qml qml/TaskDelegate.qml
    SOURCES include/DashboardController.h src/DashboardController.cpp
    include/TaskListModel.h src/TaskListModel.cpp
    include/TaskFilterProxyModel.h src/TaskFilterProxyModel.cpp
    include/HabitGauge.h src/HabitGauge.cpp
)


target_link_libraries(appQt_basics PRIVATE Qt6::Core
                      Qt6::Quick Qt6::Qml Qt6::Concurrent Qt6::Network)
