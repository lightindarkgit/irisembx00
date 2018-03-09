#-------------------------------------------------
#
# Project created by QtCreator 2013-12-23T10:18:45
#
#-------------------------------------------------

QT       += core gui

TARGET = libusbTest
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    usbtest.cpp

HEADERS  += mainwindow.h \
    syncubsapi.h \
    usbtest.h

FORMS    += \
    mainwindow.ui

OTHER_FILES += \
    ayncusbapi

LIBS += -L/home/fjf/test -lusbapi

LIBS += -L/home/fjf/libusbTest -lusb

LIBS += -L/opt/gtest1.6 -lgtest -pthread

INCLUDEPATH += $(GTEST_DIR)/include

QMAKE_CXXFLAGS += -std=gnu++0x
