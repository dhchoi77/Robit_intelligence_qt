cmake_minimum_required(VERSION 3.16)
project(Serial LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 버전을 못박지 않음 (설치된 Qt6 버전이 뭐든 자동으로 찾게)
find_package(Qt6 REQUIRED COMPONENTS Widgets SerialPort)

add_executable(Serial
    main.cpp
    mainwindow.h
    mainwindow.cpp
)

target_link_libraries(Serial PRIVATE
    Qt6::Widgets
    Qt6::SerialPort
)