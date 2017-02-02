#-------------------------------------------------
#
# Project created by QtCreator 2017-01-18T15:58:07
#
#-------------------------------------------------

QT       += core gui\
           network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = dictionary-qt
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++1y
# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
RESOURCES = resource.qrc
CONFIG += resources_big
win32:RC_ICONS += alphabet/icon.ico
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    pagedownloader.cpp

HEADERS  += mainwindow.h \
    pagedownloader.h

FORMS += \
    mainwindow.ui

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
