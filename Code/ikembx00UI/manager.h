/*****************************************************************************
** 文 件 名： Manager.h
** 主 要 类： Manager
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：刘中昌
** 创建时间：20013-10-24
**
** 修 改 人：
** 修改时间：
** 描  述:    管理员操作菜单
** 主要模块说明: 管理员操作菜单
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#ifndef MANAGER_H
#define MANAGER_H

#include <QWidget>
#include <QTimer>
#include <string>
#include <map>

namespace Ui {
class Manager;
}

class Manager : public QWidget
{
    Q_OBJECT
    
public:
    explicit Manager(QWidget *parent = 0);
    ~Manager();
signals:
    void sigIdentify();
    void sigSysterm();
    void sigPerson();
    void sigDataManger();
    void sigShutdown();
    void sigHelp();


//Added by：Ｗang.L @2014-09-30
private slots:
    void slotPowerManager();
    void slotUpdateDateTime();

private:
    Ui::Manager *ui;
    QTimer *_timer;
    std::map<std::string, std::string> _weekDay;
};

#endif // MANAGER_H
