#-------------------------------------------------
#
# Project created by QtCreator 2014-01-07T18:47:32
#
#-------------------------------------------------

QT       -= core gui

TARGET = SerialMuxDemux
TEMPLATE = lib
#CONFIG += staticlib

DEFINES += SERIALMUXDEMUX_LIBRARY

SOURCES += ../source/serialmuxdemux.cpp

HEADERS += ../include/serialmuxdemux.h\
        SerialMuxDemux_global.h \
    ../../serial/serial/basictypes.h \
    ../../serial/serial/asyncserial.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE2FD1390
    TARGET.CAPABILITY =
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = SerialMuxDemux.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

LIBS += -L../../serial/serial-build-desktop-Qt_4_8_1___PATH________ -lserial

QMAKE_CXXFLAGS += -std=c++11 -fpermissive

#QT       -= core
#QT       -= gui

#TARGET = SerialMuxDemux
#TEMPLATE = lib
#CONFIG += staticlib

##DEFINES += SERIALMUXDEMUX_LIBRARY

#SOURCES += serialmuxdemux.cpp

#HEADERS += serialmuxdemux.h\
#        SerialMuxDemux_global.h \
#    ../../serial/serial/basictypes.h \
#    ../../serial/serial/asyncserial.h


#LIBS += -L../../serial/serial-build-desktop-Qt_4_8_1___PATH________ -lserial

#QMAKE_CXXFLAGS += -std=c++11 -fpermissive
