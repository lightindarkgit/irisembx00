#-------------------------------------------------
# IKEMBX00项目创建
# 创建人  ： liu
# 创建时间：2013-10-14T19:13:14
#
#-------------------------------------------------

QT       += core gui phonon qt3support
QT       += sql
QT       += network

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
    departmanage.cpp \
    capturephoto.cpp \
    FeatureManageForSave.cpp \
    identify.cpp \
    network.cpp \
    interaction.cpp \
    dialogfeatureexist.cpp \
    buffering.cpp \
    iknetwork.cpp \
    iksocket.cpp \
    ../common.cpp \
    dialogidentifyforcard.cpp \
    addalarm.cpp \
    alarm.cpp \
    typeconvert.cpp \
    dboperationinterface.cpp \
    dialogselectlogin.cpp \
    sysnetinterface.cpp \
    config.cpp \
    simutouchevent.cpp \
    daemon.cpp \
    wakescreen.cpp \
    ikxmlhelper.cpp \
    ikevent.cpp \
    ikmessagebox.cpp \
    commononqt.cpp \
    hbweather.cpp \
    ../Common/SyncLock.cpp \
    ../Common/Global.cpp \
    dialogresetpw.cpp \
    ../Common/Exectime.cpp \
    mylabel.cpp



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
    departmanage.h \
    capturephoto.h \
    AlgApi.h\
    Common/interface.h\
    FeatureManageForDemo.h\
    identify.h\
    ../common.h \
    interaction.h \
    network.h\
    ../SerialMuxDemux/SerialMuxDemux/include/serialmuxdemux.h \
    keyboardinput.h \
    dialogfeatureexist.h \
    dbdata.h \
    buffering.h \
    iknetwork.h \
    buffering.h \
    iksocket.h \
    dialogidentifyforcard.h \
    ikmessagebox.h \
    addalarm.h \
    alarm.h \
    typeconvert.h \
    dboperationinterface.h \
    ../USBAPI/blockingqueue.h \
    ../USBAPI/simplelock.h \
    dialogselectlogin.h \
    sysnetinterface.h \
    config.h \
    simutouchevent.h \
    daemon.h \
    wakescreen.h \
    ikxmlhelper.h \
    ikevent.h \
    commononqt.h \
    json/writer.h \
    json/version.h \
    json/value.h \
    json/reader.h \
    json/json.h \
    json/forwards.h \
    json/features.h \
    json/config.h \
    json/autolink.h \
    json/assertions.h \
    FunCallBack.h \
    hbweather.h \
    hb_weather_struct.h \
    Logger.h \
    SyncLock.h \
    Global.h \
    ../Common/Headers.h \
    version.h \
    dialogresetpw.h \
    ../Common/Exectime.h \
    mylabel.h



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
    departmanage.ui \
    capturephoto.ui \
    dialogfeatureexist.ui \
    buffering.ui \
    ikmessagebox.ui \
    addalarm.ui \
    dialogselectlogin.ui \
    dialogresetpw.ui

LIBS += -L../lib/ -ldatabase \
    -lIKxmlAPI \
    -lAlgApi \
    -lsocketcomm \
    -likir \
    -likir_id \
    -lBRCCommon \
    -lusbapi \
    -likIOStream \
#    -lusb \
    -lSerialMuxDemux \
#    -lserial \
    -lInPut \
    -lIrisManager \
    -llogger \
    -L/usr/lib/postgresql -lpq  \
    -L/usr/local/lib -lsoci_core -lsoci_postgresql -ldl \
    -ljson_linux-gcc-4.8_libmt
#    -L/usr/local/HuitingTTS/libs -lhttts \
#    -L/usr/lib/i386-linux-gnu -lpulse -lpulsecommon-1.1 -lpulse-simple



INCLUDEPATH +=/usr/include \
              /usr/include/postgresql \
              /usr/include/sqlite3 \
              /usr/local/include/soci \
             ../databaseLib/IKDatabaseLib/include\
             ../CodecNetData \
             ../AlgApi_eclipse \
             ../Common \
#             /usr/local/HuitingTTS/includes

RESOURCES += \
    setmsgboxbtn2cn.qrc \
    ikembx00.qrc


#QMAKE_CXXFLAGS += -std=gnu++0x -fpermissive
QMAKE_CXXFLAGS += -std=gnu++11 -fpermissive

OTHER_FILES +=

#QMAKE_CXXFLAGS += -std=c++0x
QMAKE_CXXFLAGS += -std=c++11

######IGNORING UNUSED PARAMETER ####
#### Added By: Wang.L @ 20150204
QMAKE_CXXFLAGS +=  -Wno-unused-parameter
QMAKE_CXXFLAGS +=  -Wno-unused-variable


############################
## added at 20140328 by yqhe
## 测试多语言功能
## 工具->外部->Qt语言家->更新翻译,将生成翻译文件
## ikembx00_zh_CN和ikembx00_en.ts
## 这两个文件可在QT翻译家程序（QT Linguist）中使用，添加对应的在不同语言环境下文字
## 修改完成后，lupdate，生成QT理解的二进制文件*.qm，即可在启动时设置不同语言
##
TRANSLATIONS+=ikembx00_zh_CN.ts \
              ikembx00_en.ts

## 为使提取出的中文字符串不是乱码，需要加上下面的语句
CODECFORTR = UTF-8
############################
