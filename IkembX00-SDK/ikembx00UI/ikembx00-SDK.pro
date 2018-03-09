#-------------------------------------------------
# IKEMBX00项目创建
# 创建人  ： liu
# 创建时间：2013-10-14T19:13:14
#
#-------------------------------------------------

QT       += core gui qt3support
QT       += sql
QT       += network
CONFIG += qdbus
CONFIG += debug

TARGET = ikembx00
TEMPLATE = app

HEADERS += \
    include/wakescreen.h \
    include/version.h \
    include/typeconvert.h \
    include/syssettings.h \
    include/sysnetinterface.h \
    include/suspendbar.h \
    include/simutouchevent.h \
    include/shutdown.h \
    include/qidentify.h \
    include/personmanage.h \
    include/network.h \
    include/manager.h \
    include/mainwindow.h \
    include/keyboardinput.h \
    include/interaction.h \
    include/ikxmlhelper.h \
    include/IKUSBSDK.h \
    include/iksocket.h \
    include/iknetwork.h \
    include/ikmessagebox.h \
    include/ikevent.h \
    include/hbweather.h \
    include/hb_weather_struct.h \
    include/FeatureManageForDemo.h \
    include/enroll.h \
    include/dialogselectlogin.h \
    include/dialogresetpw.h \
    include/dialoglogin.h \
    include/dialogidentifyforcard.h \
    include/dialogidentify.h \
    include/dialogfeatureexist.h \
    include/dialogenroll.h \
    include/dialogabout.h \
    include/detailpersoninfo.h \
    include/departmanage.h \
    include/dboperationinterface.h \
    include/dbconnection.h \
    include/datamanage.h \
    include/daemon.h \
    include/config.h \
    include/commononqt.h \
    include/capturephoto.h \
    include/buffering.h \
    include/application.h \
    include/alarm.h \
    include/addalarm.h \
    Common/interface.h \
    Common/AudioPlay.h \
    ../Common/SyncLock.h \
    ../Common/Logger.h \
    ../Common/Headers.h \
    ../Common/Global.h \
    include/drawtracgif.h \
    ../Common/Exectime.h \
    include/ikconfig.h

SOURCES += \
    src/syssettings.cpp \
    src/sysnetinterface.cpp \
    src/suspendbar.cpp \
    src/simutouchevent.cpp \
    src/shutdown.cpp \
    src/qidentify.cpp \
    src/personmanage.cpp \
    src/network.cpp \
    src/manager.cpp \
    src/mainwindow.cpp \
    src/main.cpp \
    src/interaction.cpp \
    src/ikxmlhelper.cpp \
    src/iksocket.cpp \
    src/iknetwork.cpp \
    src/ikmessagebox.cpp \
    src/ikevent.cpp \
    src/hbweather.cpp \
    src/FeatureManageForSave.cpp \
    src/enroll.cpp \
    src/dialogselectlogin.cpp \
    src/dialogresetpw.cpp \
    src/dialoglogin.cpp \
    src/dialogidentifyforcard.cpp \
    src/dialogidentify.cpp \
    src/dialogfeatureexist.cpp \
    src/dialogenroll.cpp \
    src/dialogabout.cpp \
    src/detailpersoninfo.cpp \
    src/departmanage.cpp \
    src/dboperationinterface.cpp \
    src/dbconnection.cpp \
    src/datamanage.cpp \
    src/daemon.cpp \
    src/config.cpp \
    src/commononqt.cpp \
    src/capturephoto.cpp \
    src/buffering.cpp \
    src/application.cpp \
    src/alarm.cpp \
    src/addalarm.cpp \
    Common/AudioPlay.cpp \ 
    ../common.cpp \
    src/typeconvert.cpp \
    ../Common/SyncLock.cpp \
    ../Common/Global.cpp \
    src/wakescreen.cpp \
    src/drawtracgif.cpp \
    ../Common/Exectime.cpp \
    ../Common/Logger.cpp \
    src/ikconfig.cpp


FORMS += \
    frm/syssettings.ui \
    frm/suspendbar.ui \
    frm/shutdown.ui \
    frm/personmanage.ui \
    frm/manager.ui \
    frm/mainwindow.ui \
    frm/irislogin.ui \
    frm/ikmessagebox.ui \
    frm/enroll.ui \
    frm/dialogselectlogin.ui \
    frm/dialogresetpw.ui \
    frm/dialoglogin.ui \
    frm/dialogidentify.ui \
    frm/dialogfeatureexist.ui \
    frm/dialogenroll.ui \
    frm/dialogabout.ui \
    frm/detailpersoninfo.ui \
    frm/departmanage.ui \
    frm/datamanage.ui \
    frm/capturephoto.ui \
    frm/buffering.ui \
    frm/addalarm.ui


LIBS += -L/usr/lib/i386-linux-gnu/ -luuid \
    -L../lib/ -ldatabase \
    -lIKxmlAPI \
    -lIKUSBSDK \        #sdk
    -lusbapi \          #sdk
    -lIKAlgAPI \        #sdk
    -lIKAlgoIR \        #sdk
    -lIKAlgoFR \        #sdk
    -lFaceCamera \
    -lsocketcomm \
    -likIOStream \
    -lSerialMuxDemux \
    -lInPut \
    -lIrisManager \
    -lSDL_mixer \
    -lSDL \
    -ljpeg \
    -llog4cpp \
    -L/usr/lib/postgresql -lpq  \
    -L/usr/local/lib -lsoci_core -lsoci_postgresql -ldl \
    #-ljson_linux-gcc-4.8_libmt

#    -lQtMultimediaKit



INCLUDEPATH +=/usr/include \
              /usr/include/postgresql \
              /usr/include/sqlite3 \
              /usr/include/QtMultimediaKit \
              /usr/include/QtMobility \
	      /usr/include/SDL \
              /usr/local/include/soci \
             ../databaseLib/IKDatabaseLib/include\
             ../CodecNetData \
             ../Common \
             ../CFaceCamera \
             ./include

RESOURCES += \
    setmsgboxbtn2cn.qrc \
    ikembx00.qrc \


#QMAKE_CXXFLAGS += -std=gnu++0x -fpermissive
QMAKE_CXXFLAGS += -std=gnu++11 -fpermissive

OTHER_FILES += \
    ikembx00.qss

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



