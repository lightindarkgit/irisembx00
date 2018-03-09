#-------------------------------------------------
#
# Project created by QtCreator 2013-11-11T18:52:31
#
#-------------------------------------------------

QT       -= core gui

TARGET = serial
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    source/syncserialbase.cpp \
    source/syncserial.cpp \
    source/serialframe.cpp \
    source/serialbase.cpp \
    source/innercommon.cpp \
    source/cserial.cpp \
    source/basictypes.cpp \
    source/basicserial.cpp \
    source/asyncserialbase.cpp \
    source/asyncserial.cpp

HEADERS += \
    ../../common.h \
    include/syncserialbase.h \
    include/syncserial.h \
    include/serialframe.h \
    include/serialbase.h \
    include/innercommon.h \
    include/cserial.h \
    include/basictypes.h \
    include/basicserial.h \
    include/asyncserialbase.h \
    include/asyncserial.h
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}


QMAKE_CXXFLAGS += -std=c++11 -fpermissive
