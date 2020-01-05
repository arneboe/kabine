cmake_minimum_required(VERSION 3.0)

find_package(Qt5 REQUIRED COMPONENTS Gui Qml)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_subdirectory(src)
