#-------------------------------------------------
#
# Project created by QtCreator 2014-01-03T10:50:15
#
#-------------------------------------------------

QT       += core gui

TARGET = testmuxdemux
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        event.cpp

HEADERS  += mainwindow.h \
            serialmuxdemux.h\
            event.h \
    ../SerialMuxDemux/serialmuxdemux.h

LIBS+=-L../../lib/ -lSerialMuxDemux


FORMS    += mainwindow.ui

QMAKE_CXXFLAGS += -std=c++11 -fpermissive
