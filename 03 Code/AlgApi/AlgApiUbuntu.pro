#-------------------------------------------------
#
# Project created by QtCreator 2013-12-24T11:11:28
#
#-------------------------------------------------

QT       += core

QT       += gui

TARGET = AlgApiUbuntu
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    SRC/ApiGetFlag.cpp \
    SRC/AlgApi.cpp \
    SRC/ApiBase.cpp \
    SRC/CameraBase.cpp \
    CameraDemo.cpp

HEADERS += \
    SRC/IK_IR_DLL.h \
    SRC/BRCCommon.h \
    SRC/ApiGetFlag.h \
    SRC/ApiBase.h \
    SRC/AlgApi.h \
    SRC/CameraBase.h \
    CameraDemo.h

#LIBS += -L/home/fpc/bin \
LIBS += -L$$PWD/../bin \
    -likir \
    -lBRCCommon \
    -lusbapi \
    -lusb \
    -lSerialMuxDemux \
    -lserial

QMAKE_CXXFLAGS += -std=c++0x -fpermissive
