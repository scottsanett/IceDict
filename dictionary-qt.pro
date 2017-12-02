3#-------------------------------------------------
#
# Project created by QtCreator 2017-01-18T15:58:07
#
#-------------------------------------------------

QT       += core gui\
           network\
           widgets\
           webenginewidgets

# greaterThan(QT_MAJOR_VERSION, 5): QT += widgets


TARGET = IceDict
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++17
VERSION = 1.5
# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
RESOURCES = resource.qrc
ICON = IceDict.icns
CONFIG += resources_big
win32:RC_ICONS += IceDict.ico
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    TreeWidgetItem.cpp \
    MainWindow.cpp \
    PageDownloader.cpp \
    inflection.cpp \
    FindPane.cpp \
    StatusBar.cpp

HEADERS  += \
    TreeWidget.hpp \
    TreeWidgetItem.hpp \
    MainWindow.hpp \
    PageDownloader.hpp \
    inflection.hpp \
    macros.hpp \
    FindPane.hpp \
    StatusBar.hpp

FORMS += \
    mainwindow.ui
