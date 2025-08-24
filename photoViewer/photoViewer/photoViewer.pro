QT       += core gui

TEMPLATE = app

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


#INCLUDEPATH+=D:\edgeDownload\OpenCV-MinGW-Build-OpenCV-4.5.5-x64\OpenCV-MinGW-Build-OpenCV-4.5.5-x64\include \
 #            D:\edgeDownload\OpenCV-MinGW-Build-OpenCV-4.5.5-x64\OpenCV-MinGW-Build-OpenCV-4.5.5-x64\include\opencv \
 #            D:\edgeDownload\OpenCV-MinGW-Build-OpenCV-4.5.5-x64\OpenCV-MinGW-Build-OpenCV-4.5.5-x64\include\opencv2
# LIBS+=D:\edgeDownload\OpenCV-MinGW-Build-OpenCV-4.5.5-x64\OpenCV-MinGW-Build-OpenCV-4.5.5-x64\x64\mingw\bin\libopencv_*.dll

INCLUDEPATH += D:\OpenCV\opencv480\opencv480_cmake_build\install\include \
                D:\OpenCV\opencv480\opencv480_cmake_build\install\include\opencv \
                D:\OpenCV\opencv480\opencv480_cmake_build\install\include\opencv2

LIBS += D:\OpenCV\opencv480\opencv480_cmake_build\install\x64\mingw\bin\libopencv_*.dll

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    editor_plugin_interface.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
