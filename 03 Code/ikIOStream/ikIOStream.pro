#-------------------------------------------------
#
# Project created by QtCreator 2013-12-23T20:30:38
#
#-------------------------------------------------

#QT       += core gui

TARGET = ikIOStream
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ikiostream.cpp \
    interface.cpp \
    database.cpp \
    sqlitewrapper.cpp \
    iimportandexport.cpp

HEADERS  += mainwindow.h \
    ikiostream.h \
    interface.h \
    database.h \
    sqlite3.h \
    pubfun.h \
    sqlitewrapper.h \
    iimportandexport.h

FORMS    += mainwindow.ui

LIBS += -L../bin -lsqlite3\
        -L/usr/local/lib -lsoci_core -lsoci_sqlite3

INCLUDEPATH += ../../../SqlSoci/soci/src/backends/sqlite3
INCLUDEPATH += ../../../SqlSoci/soci/src/core/

QMAKE_CXXFLAGS += -std=c++11
