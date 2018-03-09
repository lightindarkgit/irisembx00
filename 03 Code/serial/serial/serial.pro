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
    basictypes.cpp \
    asyncserial.cpp \
    syncserial.cpp \
    asyncserialbase.cpp \
    syncserialbase.cpp \
    serialframe.cpp \
    innercommon.cpp \
    serialbase.cpp \
    cserial.cpp

HEADERS += \
    basictypes.h \
    ../../common.h \
    asyncserial.h \
    syncserial.h \
    asyncserialbase.h \
    syncserialbase.h \
    serialbase.h \
    innercommon.h \
    serialframe.h \
    cserial.h
unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
