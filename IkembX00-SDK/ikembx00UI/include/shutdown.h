/*****************************************************************************
** 文 件 名：shutdown.h
** 主 要 类：Shutdown
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：L.Wang
** 创建时间：20013-10-18
**
** 修 改 人：
** 修改时间：
** 描  述:   关机操作界面
** 主要模块说明: 关机， 取消关机， 重启
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#ifndef SHUTDOWN_H
#define SHUTDOWN_H

#include <QDialog>
#include <QTimer>
#include <QMovie>
#include <QLabel>

//To Close Infrared, need notify relative threads
#include "ikevent.h"
#include "buffering.h"
#include "commononqt.h"



namespace Ui {
class Shutdown;
}


class Shutdown : public QDialog
{
    Q_OBJECT
    
public:
    explicit Shutdown(QWidget *parent = 0);
    ~Shutdown();


signals:
    void sigGoBack();

protected slots:
    void slotShutdown();
    void slotReboot();
    void slotManagePower();

    //屏蔽Esc键信号
    void reject();
private:
    void ShutdownImp();
    void RebootImp();
    void initWaitingUI();
    void dispWaitingUI(const QString& notice);
    void hideWaitingUI();

private:
    enum class PowerOpt
    {
        SHUTDOWN,
        REBOOT,
        NONE
    } po;
    Ui::Shutdown *ui;
    IKEventIns eIns;
    QTimer timeOutTimer;
    int timeOut;
    QMovie *waitingUIPlayer;
    QLabel *waitingUIDisper;
    QLabel *operInfoDisper;
    PowerManager pwMgr;
};

#endif // SHUTDOWN_H
