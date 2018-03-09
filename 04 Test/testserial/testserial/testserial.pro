TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += main.cpp

INCLUDEPATH +=  /opt/gtest1.6/include /opt/gtest1.6/include/gtest /opt/gtest1.6/include/gtest/internal

LIBS += -L/opt/gtest1.6 -lgtest -pthread -L../../../03 Code/serial/serial-build-desktop-Qt_4_8_1___PATH________/libserial -lserial

HEADERS += \
    /opt/gtest1.6/include/gtest/gtest.h

