QT       += core gui

TEMPLATE = lib
CONFIG += plugin

TARGET = ErodePlugin
DESTDIR = E:\Qt\Qt5.12.8\QtProjectFile\build-photoViewer-Desktop_Qt_5_12_8_MinGW_64_bit-Debug\debug\plugins

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += D:\OpenCV\opencv480\opencv480_cmake_build\install\include \
                D:\OpenCV\opencv480\opencv480_cmake_build\install\include\opencv \
                D:\OpenCV\opencv480\opencv480_cmake_build\install\include\opencv2
LIBS += D:\OpenCV\opencv480\opencv480_cmake_build\install\x64\mingw\bin\libopencv_*.dll

INCLUDEPATH += E:\Qt\Qt5.12.8\QtProjectFile\photoViewer

SOURCES += \
    erodeplugin.cpp

HEADERS += \
    erodeplugin.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
