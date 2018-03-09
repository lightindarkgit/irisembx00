/*****************************************************************************
** 文 件 名：MainWindow.h
** 主 要 类：MainWindow
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：刘中昌
** 创建时间：20013-10-13
**
** 修 改 人：
** 修改时间：
** 描  述:   主窗体
** 主要模块说明: 标题栏、工具栏，模块切换
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dialogidentify.h"
#include "QHBoxLayout"
#include "QToolBar"
#include "QToolButton"
#include "suspendbar.h"
#include "irislogin.h"
#include "manager.h"
#include "dialogabout.h"
#include "shutdown.h"
#include "syssettings.h"
#include "datamanage.h"
#include "QTimer"

class MngForm;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    enum WorkStatus { CloclIn = 0, ClockOut, Other};
    enum PageStatus {IdentityPage,ManagerPage,IrisLoginPage,PersonPage,SystermPage,ShutdownPage,DataManagerPage,HelpPage,OtherPage};
private slots:

    //上班打卡
    void on_start_work_action_triggered();

    // 管理员用户名登陆
    void on_login_for_admin_action_triggered();

    //关于对话框
    void on_about_action_triggered();

    //管理员虹膜登陆
    void go_irislogin_action();

    //识别界面
    void go_identify_action();
\
    //管理界面
    void go_manager_action();

    //数据管理界面
    void go_data_action();

    //人员管理界面
    void go_person_action();

    //系统管理界面
    void go_systerm_action();

    //关机界面
    void go_shutdown_action();

    //帮助界面
    void go_help_action();

    //登陆是否成功
    void login_result(bool isSucceed);
    //隐藏登陆按钮栏
    void hide_suspendbar();
private :
    void mousePressEvent(QMouseEvent *);
    void changeInterface(QWidget *page);
    void onStackWidgetCurrentChanged(int index);
private:
    Ui::MainWindow *ui;

    DialogIdentify *_ident;
    QHBoxLayout *_layoutIdentify;

    WorkStatus _workStatus;
    SuspendBar  *_suspendbar;

    Manager    *_manager;
    QHBoxLayout *_layoutManager;

    QWidget *_personManage;
    QHBoxLayout *_layoutPerson;

    DialogAbout *_dialogAbout;
    QHBoxLayout *_layoutDialogAbout;

    Shutdown    *_shutdown;
    QHBoxLayout *_layoutShutdown;

    SysSettings  *_systermSet;
    QHBoxLayout *_layoutSystermSet;

    DataManage  *_dataManage;
    QHBoxLayout *_layoutDataManage;

    PageStatus _currentPage;

    QTimer *_timerForSuspendBar;

    bool    _loginShowTag;
};

#endif // MAINWINDOW_H
