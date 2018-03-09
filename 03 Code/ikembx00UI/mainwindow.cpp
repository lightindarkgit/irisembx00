/*****************************************************************************
** 文 件 名：MainWindow.cpp
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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialoglogin.h"
#include "qundostack.h"
#include "dialogabout.h"
#include "QAction"
#include "QMouseEvent"

#include "personmanage.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //隐藏标题栏
    setWindowFlags(Qt::CustomizeWindowHint);

    /**********定时器*****************/
    //新建定时器
    _timerForSuspendBar = new QTimer();
    //关联定时器计满信号和相应的槽函数
    connect(_timerForSuspendBar,SIGNAL(timeout()),this,SLOT(hide_suspendbar()));
    //定时器开始计时，其中1000表示1000ms即1秒
    _timerForSuspendBar->start(1000 * 30);

    //隐藏工具栏
    //ui->mainToolBar->hide();

    //设置管理员登陆
    _suspendbar = new SuspendBar(this);
    _suspendbar->hide();
    connect(_suspendbar,SIGNAL(sigAdminLogin()),this,SLOT(on_login_for_admin_action_triggered()));
    connect(_suspendbar,SIGNAL(sigIrisLogin()),this,SLOT(go_irislogin_action()));
    connect(_suspendbar,SIGNAL(sigIdent()),this,SLOT(go_identify_action()));

    //初始化成员变量
     _workStatus = Other;
     _loginShowTag = false;

    //添加识别界面
    _ident = new DialogIdentify();
    _layoutIdentify = new QHBoxLayout();
    _layoutIdentify->addWidget(_ident);
    _layoutIdentify->setMargin(0);
    ui->pageIdentify->setLayout(_layoutIdentify);
    connect(_ident,SIGNAL(sigLoginSucceed(bool)),this,SLOT(login_result(bool)));

    //系统管理界面
    _systermSet = new SysSettings();
    _layoutSystermSet = new QHBoxLayout();
    _layoutSystermSet->setMargin(0);
    _layoutSystermSet->addWidget(_systermSet);
    ui->pageSysterm->setLayout(_layoutSystermSet);
    connect(_systermSet,SIGNAL(sigGoBack()),this,SLOT(go_manager_action()));


    //关闭界面
    _shutdown = new Shutdown();
    _layoutShutdown = new QHBoxLayout();
    _layoutShutdown->addWidget(_shutdown);
    _layoutShutdown->setMargin(0);
    ui->pageShutDown->setLayout(_layoutShutdown);
    connect(_shutdown,SIGNAL(sigGoBack()),this,SLOT(go_manager_action()));

    //数据管理界面
    _dataManage = new DataManage();
    _layoutDataManage = new QHBoxLayout();
    _layoutDataManage->addWidget(_dataManage);
    _layoutDataManage->setMargin(0);
    ui->pageDataManager->setLayout(_layoutDataManage);
    connect(_dataManage,SIGNAL(sigGoBack()),this,SLOT(go_manager_action()));

    //添加人员管理界面
    _personManage = new PersonManage();
    _layoutPerson = new QHBoxLayout();
    _layoutPerson->setMargin(0);
    _layoutPerson->addWidget(_personManage);
    ui->pagePerson->setLayout(_layoutPerson);
    connect(_personManage,SIGNAL(sigGoBack()),this,SLOT(go_manager_action()));

    //添加管理界面
     _manager = new Manager();
     _layoutManager = new QHBoxLayout();
     _layoutManager->addWidget(_manager);
     ui->pageManager->setLayout(_layoutManager);
     connect(_manager,SIGNAL(sigDataManger()),this,SLOT(go_data_action()));
     connect(_manager,SIGNAL(sigIdentify()),this,SLOT(go_identify_action()));
     connect(_manager,SIGNAL(sigHelp()),this,SLOT(go_help_action()));
     connect(_manager,SIGNAL(sigPerson()),this,SLOT(go_person_action()));
     connect(_manager,SIGNAL(sigShutdown()),this,SLOT(go_shutdown_action()));
     connect(_manager,SIGNAL(sigSysterm()),this,SLOT(go_systerm_action()));

     //添加管理员虹膜登陆界面
////     _irisLogin = new IrisLogin();
 //    _layoutIrisLogin = new QHBoxLayout();
////     _layoutIrisLogin->addWidget(_irisLogin);
//     _layoutIrisLogin->addWidget(_ident);
//     _layoutIrisLogin->setMargin(0);
//     ui->pageIrisLogin->setLayout(_layoutIrisLogin);
//     connect(_ident,SIGNAL(sigGoBack()),this,SLOT(go_identify_action()));

     //帮助界面
     _dialogAbout = new DialogAbout();
     _layoutDialogAbout = new QHBoxLayout();
     _layoutDialogAbout->addWidget(_dialogAbout);
     _layoutDialogAbout->setMargin(0);
     ui->pageHelp->setLayout(_layoutDialogAbout);
     connect(_dialogAbout,SIGNAL(sigGoBack()),this,SLOT(go_manager_action()));

    //显示识别界面
    go_identify_action();



}

MainWindow::~MainWindow()
{  

    delete ui;

    delete _ident;
    delete _layoutIdentify;

    delete _suspendbar;

    delete _manager;
    delete _layoutManager;

    delete  _personManage;
    delete _layoutPerson;

    delete _dialogAbout;
    delete _layoutDialogAbout;

    delete _dataManage;
    delete _layoutDataManage;

    delete _shutdown;
    delete _layoutShutdown;

    delete _systermSet;
    delete _layoutSystermSet;

}
/*****************************************************************************
*                        单击界面显示管理员登陆
*  函 数 名：mousePressEvent
*  功    能：单击界面显示管理员登陆
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-10-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
void MainWindow::mousePressEvent(QMouseEvent *)
{
    if(_suspendbar->isHidden() && ((_currentPage == IdentityPage && !_loginShowTag ) || _currentPage == IrisLoginPage))
    {
        _suspendbar->show();
        _timerForSuspendBar->stop();
        _timerForSuspendBar->start(1000 *30);
    }
    else
    {
        _timerForSuspendBar->stop();
        _suspendbar->hide();
    }
}


/*****************************************************************************
*                        上班打卡
*  函 数 名：on_start_work_action_triggered
*  功    能：上班打卡状态
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-10-16
*  修 改 人：
*  修改时间：
*****************************************************************************/
void MainWindow::on_start_work_action_triggered()
{
}


/*****************************************************************************
*                        管理员用户名登陆
*  函 数 名：on_login_for_admin_action_triggered
*  功    能：管理员用户名登陆
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-10-16
*  修 改 人：
*  修改时间：
*****************************************************************************/
void MainWindow::on_login_for_admin_action_triggered()
{
    _loginShowTag = true;
    _timerForSuspendBar->stop();
    _suspendbar->hide();
    DialogLogin *login = new DialogLogin(this);

    //设在对话框居中显示--默认情况有偏移
    login->setGeometry((this->width()- login->width())/2,(this->height()-login->height())/2,
                       login->width(),login->height());

    connect(login,SIGNAL(sigResult(bool)),this,SLOT(login_result(bool)));
    login->exec();
    _loginShowTag =false;
}

/*****************************************************************************
*                        管理员登陆是否成功
*  函 数 名：login_result
*  功    能：管理员登陆是否成功
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-10-16
*  修 改 人：
*  修改时间：
*****************************************************************************/
void MainWindow::login_result(bool isSucceed)
{
    if(isSucceed)
    {
        //管理界面
        go_manager_action();
        _ident->StopIdent();
    }
    else
    {
        go_identify_action();
    }
}


/*****************************************************************************
*                        弹出关于界面
*  函 数 名：on_about_action_triggered
*  功    能：弹出关于界面
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-10-16
*  修 改 人：
*  修改时间：
*****************************************************************************/
void MainWindow::on_about_action_triggered()
{
    DialogAbout about(this);
    about.show();
    about.setFocus();
}

/*****************************************************************************
*                       正常识别界面
*  函 数 名：go_identify_action
*  功    能：正常识别界面
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-10-16
*  修 改 人：
*  修改时间：
*****************************************************************************/
void MainWindow::go_identify_action()
{
    _timerForSuspendBar->stop();
    _suspendbar->hide();

    //跳传到识别界面
    changeInterface(ui->pageIdentify);
    _ident->StopIdent();
    _ident->StartAttendIdent();

    _currentPage = IdentityPage;
}

/*****************************************************************************
*                        虹膜识别登录界面
*  函 数 名：login_for_iris_action_triggered
*  功    能：虹膜识别登录界面
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-10-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
void MainWindow::go_irislogin_action()
{
    _suspendbar->hide();

    //跳入到管理员登陆识别界面
    changeInterface(ui->pageIdentify);
    _ident->StopIdent();
    _ident->StartLoginIdent();

    _currentPage = IrisLoginPage;
}

/*****************************************************************************
*                        跳转管理菜单
*  函 数 名：go_manager_action
*  功    能： 跳转管理菜单
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-10-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
void MainWindow::go_manager_action()
{
    changeInterface(ui->pageManager);
    _currentPage = ManagerPage;
}
/*****************************************************************************
*                        跳转人员管理界面
*  函 数 名：go_person_action
*  功    能：跳转人员管理界面
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-10-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
void MainWindow::go_person_action()
{
    changeInterface(ui->pagePerson);
    _currentPage = PersonPage;
}

/*****************************************************************************
*                        跳转数据管理界面
*  函 数 名：go_data_action
*  功    能：跳转数据管理界面
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-10-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
void MainWindow::go_data_action()
{
    changeInterface(ui->pageDataManager);
    _currentPage = DataManagerPage;
}

/*****************************************************************************
*                        跳转系统配置界面
*  函 数 名：go_systerm_action
*  功    能：跳转系统配置界面
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-10-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
void MainWindow::go_systerm_action()
{
    changeInterface(ui->pageSysterm);
    _currentPage = SystermPage;
}

/*****************************************************************************
*                        跳转关闭系统界面
*  函 数 名：go_shutdown_action
*  功    能：跳转关闭系统界面
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-10-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
void MainWindow::go_shutdown_action()
{
    changeInterface(ui->pageShutDown);
    _currentPage = ShutdownPage;
}

/*****************************************************************************
*                        长时间为关闭SuspendBar 关闭SuspendBar对话框
*  函 数 名：go_help_action
*  功    能：长时间为关闭SuspendBar 关闭SuspendBar对话框 并停止计时，跳转到识别界面
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-10-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
void MainWindow::hide_suspendbar()
{
    _suspendbar->hide();
    _suspendbar->hideBtnIdent();
    _timerForSuspendBar->stop();
    go_identify_action();
}

/*****************************************************************************
*                        帮助界面
*  函 数 名：go_help_action
*  功    能：跳转帮助界面
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-10-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
void MainWindow::go_help_action()
{
    changeInterface(ui->pageHelp);
    _currentPage = HelpPage;
}


/*****************************************************************************
*                        切换不同界面函数
*  函 数 名：changeInterface
*  功    能：切换不同界面函数
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-10-16
*  修 改 人：
*  修改时间：
*****************************************************************************/
void MainWindow::changeInterface(QWidget *page)
{
    ui->stackedWidget->setCurrentWidget(page);
}

/*****************************************************************************
*                        通过Action切换界面
*  函 数 名：MakePicData
*  功    能：组装图像
*  说    明：
*  参    数： index :StackWidget页面索引
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-10-16
*  修 改 人：
*  修改时间：
*****************************************************************************/
void MainWindow::onStackWidgetCurrentChanged(int index)
{
    QAction *actions[] = {ui->actionPageIdentify, 
                          ui->actionIrisLogin,
                          ui->actionManager,
                          ui->actionPerson,
                          ui->actionSysterm,
                          ui->actionShutDown,
                          ui->actionHelp
    };
    QAction **actionEnd = actions + sizeof(actions)/sizeof(actions[0]);
    for (QAction **it = actions; it < actionEnd; ++it)
        (*it)->setChecked(it - actions == index);
}
