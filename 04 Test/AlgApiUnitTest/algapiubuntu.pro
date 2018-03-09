#-------------------------------------------------
#
# Project created by QtCreator 2013-11-15T16:05:37
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = algapiubuntu
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    VirtualReadImg.cpp \
    UnitTestAlgApi.cpp \
    ApiGetFlag.cpp \
    ApiBase.cpp \
    AlgApi.cpp

HEADERS += \
    VirtualReadImg.h \
    VirtualReadImg.h \
    UnitTestAlgApi.h \
    usbcommon.h \
    BRCCommon.h \
    common.h \
    IK_IR_DLL.h \
    ApiGetFlag.h \
    ApiBase.h \
    AlgApi.h

unix:!macx:!symbian: LIBS += -L$$PWD/ -likir \
                            -lBRCCommon \
                            -lusbapi \
                            -lusb

INCLUDEPATH += $$PWD/
DEPENDPATH += $$PWD/

INCLUDEPATH += /opt/gtest1.6/include

#INCLUDEPATH += $(GTEST_DIR)/include \
#    $(GTEST_DIR)/include/gtest

LIBS += -L/opt/gtest1.6 \
    -lgtest

#LIBS += -L$(GTEST_DIR) \
#    -lgtest

#gcc:CXXFLAGS += -std=c++0x
QMAKE_CXXFLAGS += -std=c++0x -fpermissive

message($(GTEST_DIR))
