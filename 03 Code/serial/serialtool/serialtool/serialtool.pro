#-------------------------------------------------
#
# Project created by QtCreator 2013-12-06T20:56:46
#
#-------------------------------------------------

QT       += core gui

TARGET = serialtool
TEMPLATE = app

LIBS += -L../../serial-build-desktop-Qt_4_8_1___PATH________ -lserial -pthread

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
