#-------------------------------------------------
#
# Project created by QtCreator 2013-12-23T20:30:38
#
#-------------------------------------------------

#QT       += core gui

TARGET = ikIOStream
TEMPLATE = app
#TEMPLATE = lib

SOURCES += main.cpp\
        mainwindow.cpp \
    interface.cpp \
    iimportandexport.cpp \
    ikiostream.cpp

HEADERS  += mainwindow.h \
    interface.h \
    iimportandexport.h \
    ikiostream.h


FORMS    += mainwindow.ui

LIBS += -lsqlite3 \
        -L/usr/local/lib -lsoci_core -lsoci_sqlite3 \
        -L/usr/lib/postgresql -lpq  \
        -lsoci_postgresql -ldl \
        -L../lib -ldatabase \
        -luuid

INCLUDEPATH +=/usr/include \
              /usr/include/postgresql \
              /usr/include/sqlite3 \
              /usr/local/include/soci \
             ../databaseLib/IKDatabaseLib/include\


QMAKE_CXXFLAGS += -std=c++11
