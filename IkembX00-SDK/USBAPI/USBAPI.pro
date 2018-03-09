#-------------------------------------------------
#
# Project created by QtCreator 2013-11-14T14:29:28
#
#-------------------------------------------------

QT       += core gui

TARGET = USBAPI
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    ikusbapibase.cpp \
    iusbimage.cpp \
    dealwithusbtransferbuf.cpp \
    simplelock.cpp \
    imagemanger.cpp \
    asyncusbapi.cpp \
    syncusbapi.cpp \

HEADERS  += mainwindow.h \
    ikusbapibase.h \
    libusb.h \
    iusbimage.h \
    dealwithusbtransferbuf.h \
    ../common.h \
    simplelock.h \
    imagemanger.h \
    usbcommon.h \
    asyncusbapi.h \
    syncusbapi.h \
    blockingqueue.h

FORMS    += mainwindow.ui

#LIBS += -L/home/fjf/libusb/ -lusb

unix:!macx:!symbian: LIBS += -L$$PWD/ -lusb

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/


unix:!macx:!symbian: PRE_TARGETDEPS += $$PWD/libusb.a

QMAKE_CXXFLAGS += -std=gnu++11

QMAKE_CXXFLAGS += -std=c++11

