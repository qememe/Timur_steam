cmake_minimum_required(VERSION 3.16)

project(Timur_steam VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network WebEngineWidgets)

add_executable(Timur_steam
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/GameManager.cpp
    src/GameManager.h
    src/AuthManager.cpp
    src/AuthManager.h
    src/models/Game.h
    resources.qrc
)

target_link_libraries(Timur_steam PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::WebEngineWidgets
)
