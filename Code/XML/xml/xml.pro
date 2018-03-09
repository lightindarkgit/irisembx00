TEMPLATE = lib

CONFIG -= qt
CONFIG += console
#CONFIG += staticlib
TARGET = XmlAPI


LIBS +=-L/usr/local/lib/ -lxml2

SOURCES += \
    XmlDocument.cpp \
    XmlElement.cpp \
    NoneCopyable.cpp \
    XmlNode.cpp \
    XmlAttribute.cpp \
    XmlContent.cpp \
    XmlComment.cpp \
    XmlText.cpp \
    OPutBuffer.cpp \
    OStreamBuffer.cpp \
    IKXmlApi.cpp

HEADERS += \
    XmlDocument.h \
    XmlElement.h \
    NoneCopyable.h \
    XmlNode.h \
    XmlAttribute.h \
    XmlComment.h \
    XmlContent.h \
    IKXml.h \
    XmlText.h \
    OPutBuffer.h \
    OStreamBuffer.h \
    IKXmlApi.h

QMAKE_CXXFLAGS += -std=c++11 -fpermissive
