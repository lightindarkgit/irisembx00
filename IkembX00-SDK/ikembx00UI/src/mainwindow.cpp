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
#include "iksocket.h"
#include "iknetwork.h"
#include "../XML/xml/IKXmlApi.h"
#include "commononqt.h"

#include "IKUSBSDK.h"
#include "../Common/Exectime.h"
#include <QDebug>
#include "../Common/interface.h"
extern  bool        g_binited;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
     Exectime etm(__FUNCTION__);
    ui->setupUi(this);

    //隐藏标题栏
    setWindowFlags(Qt::CustomizeWindowHint);
    setGeometry(QRect(0, 0, 1024, 600));

    _timerGoBackIdent = new QTimer(this);
    connect(_timerGoBackIdent,SIGNAL(timeout()),this,SLOT(GoBackIdent()));

    //初始化成员变量
    _workStatus = Other;
    _loginShowTag = false;

    //添加识别界面
    _ident = new DialogIdentify(this);
    _layoutIdentify = new QHBoxLayout();
    _layoutIdentify->addWidget(_ident);
    _layoutIdentify->setMargin(0);
    ui->pageIdentify->setLayout(_layoutIdentify);
    connect(_ident,SIGNAL(sigLoginSucceed(bool)),this,SLOT(login_result(bool)));


    //系统管理界面
    _systermSet = new SysSettings(this);
    _layoutSystermSet = new QHBoxLayout();
    _layoutSystermSet->setMargin(0);
    _layoutSystermSet->addWidget(_systermSet);
    ui->pageSysterm->setLayout(_layoutSystermSet);
    connect(_systermSet,SIGNAL(sigGoBack()),this,SLOT(go_manager_action()));
    connect(_systermSet,SIGNAL(sigServerCfgUpdated(bool)),this,SLOT(connectNetWork()));
    connect(_systermSet,SIGNAL(sigUpdateCity(bool,QString)),_ident,SLOT(slotUpdateCityName(bool,QString)));

//    //关闭界面
    _shutdown = new Shutdown(this);
    _layoutShutdown = new QHBoxLayout();
    _layoutShutdown->addWidget(_shutdown);
    _layoutShutdown->setMargin(0);
    ui->pageShutDown->setLayout(_layoutShutdown);
    connect(_shutdown,SIGNAL(sigGoBack()),this,SLOT(go_manager_action()));

    //数据管理界面
    _dataManage = new DataManage(this);
    _layoutDataManage = new QHBoxLayout();
    _layoutDataManage->addWidget(_dataManage);
    _layoutDataManage->setMargin(0);

    ui->pageDataManager->setLayout(_layoutDataManage);
    connect(_dataManage,SIGNAL(sigGoBack()),this,SLOT(go_manager_action()));

    //添加人员管理界面
    _personManage = new PersonManage(this);
    _layoutPerson = new QHBoxLayout();
    _layoutPerson->setMargin(0);
    _layoutPerson->addWidget(_personManage);
    ui->pagePerson->setLayout(_layoutPerson);
    connect(_personManage,SIGNAL(sigGoBack()),this,SLOT(go_manager_action()));
    //dj for sdk  personMaage 作为中介将摄像头数据发送给enroll模块
    connect(_ident,SIGNAL(sigShowEnrollCamera(IKEnrIdenStatus )),_personManage,SLOT(slotShowCameraImage(IKEnrIdenStatus)),Qt::DirectConnection);
    _personManage->installEventFilter(this); //@dj


    //添加管理界面
    _manager = new Manager(this);
    _layoutManager = new QHBoxLayout();
    _layoutManager->addWidget(_manager);
    _layoutManager->setMargin(0);
    ui->pageManager->setLayout(_layoutManager);

    connect(_manager,SIGNAL(sigDataManger()),this,SLOT(go_data_action()));
    connect(_manager,SIGNAL(sigIdentify()),this,SLOT(go_identify_action()));
    connect(_manager,SIGNAL(sigHelp()),this,SLOT(go_help_action()));
    connect(_manager,SIGNAL(sigPerson()),this,SLOT(go_person_action()));
    connect(_manager,SIGNAL(sigShutdown()),this,SLOT(go_shutdown_action()));
    connect(_manager,SIGNAL(sigSysterm()),this,SLOT(go_systerm_action()));


    //帮助界面
    _dialogAbout = new DialogAbout(this);
    _layoutDialogAbout = new QHBoxLayout();
    _layoutDialogAbout->addWidget(_dialogAbout);
    _layoutDialogAbout->setMargin(0);
    ui->pageHelp->setLayout(_layoutDialogAbout);
    connect(_dialogAbout,SIGNAL(sigGoBack()),this,SLOT(go_manager_action()));


    //显示识别界面
    changeInterface(ui->pageIdentify);
    _currentPage = IdentityPage;
   // _ident->StartIdent();
    go_identify_action();

    //    socket通信
    //    ClientData cd;
    //    strcpy(cd.Addr,ip.toStdString().c_str());
    //    IkNetWork netWork;
    //    _scoket = ISocketAPI::CreateInstance(std::bind(&IkNetWork::DealwithNetWork,&netWork,std::placeholders::_1,std::placeholders::_2),cd);
    _socket = nullptr;
    //if(!WorkMode::IsSingleWorkMode())
    //{
    connectNetWork();

    this->setMouseTracking(true);
    this->setAttribute(Qt::WA_AcceptTouchEvents);
    //}



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
    delete _timerGoBackIdent;


}

void MainWindow::connectNetWork()
{  
     Exectime etm(__FUNCTION__);
    if(WorkMode::IsSingleWorkMode())
    {
        return;
    }
    _socket = IkSocket::GetIkSocketInstance();
    if(_socket)
    {
        connect(_socket,SIGNAL(sigIrisUpdate()),this,SLOT(ReLoadIris()));
    }
    //_socket->Test();
}

void MainWindow::ReLoadIris()
{
     Exectime etm(__FUNCTION__);
    if(_currentPage == IdentityPage)
    {
        _ident->SetReLoadIris(true); //caotao 20161108, according to the statement of lius, update frome socket is no need.
    }
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
     Exectime etm(__FUNCTION__);

     if(_currentPage == ManagerPage||_currentPage == PersonPage)
     {
         _timerGoBackIdent->start(_goBackIdent);
     }
     _ident->WakeInteraction();
     LOG_DEBUG("============mousepressEvent==============");

}

void MainWindow::Alive()
{
     Exectime etm(__FUNCTION__);

     if(_currentPage == ManagerPage||_currentPage == PersonPage)
     {
         _timerGoBackIdent->start(_goBackIdent);
     }
    _ident->WakeInteraction();
}

void MainWindow::GoBackIdent()
{
     Exectime etm(__FUNCTION__);
    if(_currentPage != IdentityPage)
    {
        if(_currentPage ==PersonPage)
        {
            //如果正在注册则退出注册界面并清理资源
            _personManage->setInactive();
        }

        _timerGoBackIdent->stop();
        go_identify_action();

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
     Exectime etm(__FUNCTION__);
    if(isSucceed)
    {
        //管理界面
        go_manager_action();
        _ident->quitIdent();
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
     Exectime etm(__FUNCTION__);
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
     Exectime etm(__FUNCTION__);
    //跳传到识别界面
    changeInterface(ui->pageIdentify);
    _currentPage = IdentityPage;

    _ident->StartIdent();


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
     Exectime etm(__FUNCTION__);
    changeInterface(ui->pageManager);
    _currentPage = ManagerPage;
    _timerGoBackIdent->start(_goBackIdent);
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
     Exectime etm(__FUNCTION__);
    changeInterface(ui->pagePerson);
    _personManage->refresh();
    _currentPage = PersonPage;
    //_timerGoBackIdent->stop();
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
     Exectime etm(__FUNCTION__);
    changeInterface(ui->pageDataManager);
    _dataManage->refresh();
    _currentPage = DataManagerPage;
    _timerGoBackIdent->stop();
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
     Exectime etm(__FUNCTION__);
    changeInterface(ui->pageSysterm);
    _systermSet->refresh();
    _currentPage = SystermPage;
    _timerGoBackIdent->stop();
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
     Exectime etm(__FUNCTION__);
    changeInterface(ui->pageShutDown);
    _currentPage = ShutdownPage;
    _timerGoBackIdent->stop();
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
//    _suspendbar->hide();
//    _suspendbar->hideBtnIdent();
//    _timerForSuspendBar->stop();
//    if(_currentPage == IrisLoginPage)
//    {
//        // 虹膜识别定时退出到考勤识别 如果当前界面为其他界面忽略
//        go_identify_action();
//    }
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
     Exectime etm(__FUNCTION__);
    changeInterface(ui->pageHelp);
    _currentPage = HelpPage;
    _timerGoBackIdent->stop();
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
     Exectime etm(__FUNCTION__);
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
     Exectime etm(__FUNCTION__);
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

bool MainWindow::eventFilter(QObject *obj, QEvent *e)
{
    if(_currentPage == ManagerPage||_currentPage == PersonPage)
    {
        switch(e->type())
        {
        case QEvent::MouseButtonPress:
        case QEvent::KeyRelease:
        case QEvent::Show:
        case QEvent::Hide:
        case QEvent::ToolTip:
        case QEvent::WindowActivate:
        case QEvent::WindowDeactivate:
        case QEvent::LayoutRequest:
            _timerGoBackIdent->start(_goBackIdent);
            LOG_INFO("--------------------------eventID=%d-----------------------",e->type());
            break;

        }

    }
    return QWidget::eventFilter(obj,e);


}

/*****************************************************************************
*                        防止系统关闭屏幕
*  函 数 名：SlotAntiScreenSaver
*  功    能：组装图像
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：Wang.L
*  创建时间：2015-10-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
void MainWindow::SlotAntiScreenSaver()
{
     Exectime etm(__FUNCTION__);
    QKeyEvent mokeKeyPressed(QEvent::KeyPress, Qt::Key_F1, Qt::NoModifier);
    QCoreApplication::sendEvent(this, &mokeKeyPressed);


    std::cout << "[MainWindow.SlotAntiScreenSaver]mock key triggered at "
              << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();
}
