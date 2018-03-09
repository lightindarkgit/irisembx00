/*****************************************************************************
** 文 件 名：syssettings.h
** 主 要 类：SysSettings
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：王磊
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
#include <set>

namespace Ui {
class SysSettings;
}

class SysSettings : public QDialog
{
    Q_OBJECT
    
public:
    explicit SysSettings(QWidget *parent = 0);
    ~SysSettings();


protected:
    void init();
    bool initNetwork();
    bool checkIPs();
    bool checkIPValid(QString& err, const QString &ip, const QString ipInfo);

signals:
    void sigGoBack();

protected slots:
    //保存设置
    void slotSave();
    //定时器，刷新时间显示
    void slotTimerOut();
    //调整时间
    void slotAdd();
    void slotMinus();
    //应用时间设置
    bool slotApplyTimeSettings();

    void slotIPChanged();
    void slotNetMaskChanged();
    void slotGateChanged();
    void slotDNSChanged();
    void slotViceDNSChanged();

private:
    Ui::SysSettings *ui;
    QTimer timer;
    std::set<std::string> interfaceNames;
    bool isSettingsChanged;
};

#endif // SYSSETTINGS_H
