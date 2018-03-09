/*****************************************************************************
** 文 件 名：syssettings.h
** 主 要 类：SysSettings
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：L.Wang
** 创建时间：20013-10-18
**
** 修 改 人：
** 修改时间：
** 描  述:   系统设置
** 主要模块说明: IP配置管理， 系统时间配置
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#ifndef SYSSETTINGS_H
#define SYSSETTINGS_H

#include <QDialog>
#include <QTimer>
#include <QDateTime>
#include <QModelIndex>

#include <set>
#include "network.h"
#include "buffering.h"
#include "commononqt.h"


class IkSocket;
class SysStatusSignal;

namespace Ui {
class SysSettings;
}

class SysSettings : public QDialog
{
    Q_OBJECT
    
public:
    explicit SysSettings(QWidget *parent = 0);
    ~SysSettings();

    void refresh();
private:
    //初始化UI
    bool initUI();

    //初始化系统时间设置
    bool initSysTime();
    //初始化网络设置
    bool initNetwork();
    //初始化管理员设置
    bool initAdminManage();
    //初始化识别设置
    bool initIdenSettings();
    //初始化其他设置
    bool initOthers();
    //初始化闹铃设置
    bool initAlarm();

    //校验IP
    bool checkIPs();
    bool checkIPValid(QString& err, const QString &ip, const QString ipInfo);

    //检查密码有效性
    bool checkPwdsEmpty();
    bool checkNewPwds();
    bool checkNewPwdNotSameToOld();
    bool checkOldPwd();
    bool updateNewPwd();

    //获取系统音量
    bool getVolume(int& curVol);
    //显示系统音量
    void showVolume();
    //设置系统音量
    bool changeVolume(int ratio);

    //判断配置文件是否存在
    bool isCfgFileExist();

    //加载服务端网络设置
    const ServerSettings loadServerSettings();
    bool saveServerSettings(/*const ServerSettings& serverSettings*/);

    //加载设备网络设置
    QString loadDeviceIP();
    bool saveDeviceIP(const QString& serverIP);

    //清空数据表中的用户数据
    bool truncateTables();
    //删除本地用户数据文件，如识别的人脸图像文件和虹膜图像文件
    bool deleteUserLocalFiles();
    //恢复配置文件到出厂状态
    // isOnLine :false单机版，true联网版
    bool restorConfigures(bool isOnline);

    //删除指定目录及其文件和子目录
    bool rmDir(const QString& path);

    static void* threadAsyncTime(void* ins);

    bool isCityExist(QString &cityName);

signals:
    //返回键
    void sigGoBack();
    void sigServerCfgUpdated(bool);
    void sigStopWaitingStauts();
    void sigStartWaitingStatus();
    void sigReboot();
    void sigUpdateCity(bool isFromNet,QString cityName);


protected slots:
    //保存网络配置
    void slotSaveNetworkSettings();
    //保存管理员信息
    void slotSaveAdminInfo();
    //保存识别设置
    void slotSaveIdenSettings();
    //保存其他设置
    void slotSaveOtherSettings();





    //定时器，刷新时间显示
    void slotTimerOut();

    //向上调整时间
    void slotAdd();
    //向下调整时间
    void slotMinus();
    //应用时间设置
    bool slotApplyTimeSettings();
    //同步时间
    void slotAsyncTime();
    //同步服务器时间结果
    void slotAsyncTimeOK();
    //同步服务器时间超时
    void slotAsyncTimeTimeout();
    //等待同步时间动画
    void slotAsyncTimeWaiting();

    //设置门铃响铃时间
    bool slotMinusBellTime();
    bool slotAddBellTime();

    //设置系统休眠时间
    void slotMinusSleepTime();
    void slotAddSleepTime();

    //设置语音播报音量大小
    void slotTurnUpVolume();
    void slotTurnDownVolume();
    void slotChangeVolume();


    //闹铃设置
    void slotAddNewAlarm();
    void slotEditAlarm(QModelIndex);

    //IP地址发生变化
    void slotIPChanged();
    void slotIdenSettingsChanged();
    void slotOtherSettingsChanged();
    //子网掩码发生变化
    void slotNetMaskChanged();
    //网关发生变化
    void slotGateChanged();

    //服务器IP地址发生变化
    void slotServerIPChanged();
    //服务器port已编辑
    void slotSrvPortChanged();

    //恢复出厂设置
    void slotFactorySet();

    //重启设备
    void slotReboot();


    //屏蔽Esc键信号
    void reject();



private slots:
    void on_isSetCity_clicked(bool checked);


    void on_cbxOutToWiegand_clicked(bool checked);

private:
    Ui::SysSettings *ui;
    QTimer timer;
    std::set<std::string> interfaceNames;
    bool isSettingsChanged;
    bool isIdenSettingsChanged;
    bool isAdminInfoChanged;
    bool isOtherSettingsChanged;
    bool isDevNetInfoChanged;
    bool isServerCfgChanged;
    bool isWigan26Checked;
    bool isWigan34Checked;

    QString primLanguage;
    QString cfgFile;
    int volumeVal;


    const int alarmLimits = 8;
    QStringList alarmTags;
    IkSocket* socket;

    QTimer asyTimeTimer;
    SysStatusSignal* sigIns;
    //Buffering waitStatus;
    bool needReboot;
    PowerManager pwMgr;
    bool m_isFromNet;
};

#endif // SYSSETTINGS_H
