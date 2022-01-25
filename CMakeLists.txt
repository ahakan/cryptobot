cmake_minimum_required(VERSION 3.5)

project(cryptobot VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS
        Widgets
        WebSockets
        Network
REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS
        Widgets
        WebSockets
        Network
REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

set(INC_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inc/)
set(SRC_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/src/)
set(STYLE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/style/)

set(HEADERS
        ${INC_DIRECTORY}/mainwindow.h
        ${INC_DIRECTORY}/dashboardwindow.h

        ${INC_DIRECTORY}/utilities.h
        ${INC_DIRECTORY}/socket.h
)

set(SOURCES
        ${SRC_DIRECTORY}/main.cpp

        ${SRC_DIRECTORY}/mainwindow.cpp
        ${SRC_DIRECTORY}/dashboardwindow.cpp

        ${SRC_DIRECTORY}/utilities.cpp
        ${SRC_DIRECTORY}/socket.cpp

        ${SRC_DIRECTORY}/mainwindow.ui
        ${SRC_DIRECTORY}/dashboardwindow.ui

        dashboard.qrc
)

set(PROJECT_SOURCES
        ${HEADERS}
        ${SOURCES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(cryptobot
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET cryptobot APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(cryptobot SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(cryptobot
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(cryptobot PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::WebSockets
        Qt${QT_VERSION_MAJOR}::Network

        /usr/lib/libssl.so.1.1
        /usr/lib/libcrypto.so.1.1
)

set_target_properties(cryptobot PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(cryptobot)
endif()
