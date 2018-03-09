#-------------------------------------------------
# IKEMBX00项目创建
# 创建人  ： liu
# 创建时间：2013-10-14T19:13:14
#
#-------------------------------------------------

QT       += core gui phonon qt3support
QT       += sql

TARGET = ikembx00
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dialoglogin.cpp \
    qidentify.cpp \
    dialogabout.cpp \
    enroll.cpp \
    dialogenroll.cpp \
    dialogidentify.cpp \
    manager.cpp \
    suspendbar.cpp \
    irislogin.cpp \
    syssettings.cpp \
    personmanage.cpp \
    detailpersoninfo.cpp \
    datamanage.cpp \
    shutdown.cpp \
    application.cpp \
    messagebox.cpp \
    simplepersoninfo.cpp \
    departmanage.cpp \
    capturephoto.cpp \
    ../common.cpp \
    FeatureManageForSave.cpp \
    identify.cpp \
    network.cpp \
    serialevent.cpp \
    interaction.cpp \
    dbconnection.cpp



HEADERS  += mainwindow.h \
    dialoglogin.h \
    qidentify.h \
    dialogabout.h \
    enroll.h \
    dialogenroll.h \
    dialogidentify.h \
    manager.h \
    suspendbar.h \
    irislogin.h \
    syssettings.h \
    personmanage.h \
    detailpersoninfo.h \
    datamanage.h \
    shutdown.h\
    application.h\
    datamanage.h \
    messagebox.h \
    simplepersoninfo.h \
    departmanage.h \
    capturephoto.h \
    ..AlgApi/SRC/AlgApi.h\
    Common/interface.h\
    FeatureManageForDemo.h\
    identify.h\
    ../common.h \
    interaction.h \
    network.h\
    ../bin/serialmuxdemux.h \
    serialevent.h \
    dbconnection.h



FORMS    += mainwindow.ui \
    dialoglogin.ui \
    dialogabout.ui \
    enroll.ui \
    dialogenroll.ui \
    dialogidentify.ui \
    manager.ui \
    suspendbar.ui \
    irislogin.ui \
    syssettings.ui \
    personmanage.ui \
    detailpersoninfo.ui \
    datamanage.ui \
    shutdown.ui \
    messagebox.ui \
    simplepersoninfo.ui \
    departmanage.ui \
    capturephoto.ui

LIBS += -L../bin/ \
    -lAlgApi \
    -likir \
    -lBRCCommon \
    -lusbapi \
    -lusb \
    -lSerialMuxDemux \
    -lserial

RESOURCES += \
    setmsgboxbtn2cn.qrc \
    ikembx00.qrc \
    enroll.qrc


#QMAKE_CXXFLAGS += -std=gnu++0x -fpermissive
QMAKE_CXXFLAGS += -std=gnu++11 -fpermissive

OTHER_FILES +=

#QMAKE_CXXFLAGS += -std=c++0x
QMAKE_CXXFLAGS += -std=c++11


