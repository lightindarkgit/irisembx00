/*****************************************************************************
** 文 件 名：DialogLogin.cpp
** 主 要 类：DialogLogin
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：刘中昌
** 创建时间：20013-10-15
**
** 修 改 人：
** 修改时间：
** 描  述:   登陆对话框
** 主要模块说明: 管理员登陆验证
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include "dialoglogin.h"
#include "ui_dialoglogin.h"
#include "qmessagebox.h"
#include "messagebox.h"

DialogLogin::DialogLogin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogLogin)
{
    ui->setupUi(this);

    this->setBackgroundColor(QColor(220,220,220));
    connect(ui->btnCancel,SIGNAL(clicked()),this,SLOT(Cancel()));
    ui->widgetTitle->setBackgroundColor(QColor(71,71,71));

    //隐藏标题栏
    setWindowFlags(Qt::CustomizeWindowHint);

    //新建定时器
    _timer = new QTimer(this);
    //关联定时器计满信号和相应的槽函数
    connect(_timer,SIGNAL(timeout()),this,SLOT(close()));
    //定时器开始计时，其中1000表示1000ms即1秒
    _timer->start(1000 * 30);
}

DialogLogin::~DialogLogin()
{
    delete ui;
    delete _timer;
}

/*****************************************************************************
*                        判断用户登陆
*  函 数 名：judgeUser
*  功    能：判断是否为管理员登陆
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2013-10-16
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogLogin::judgeUser()
{
    if(true/*this->ui->txtUserName->text().toStdString() == "admin"
            && this->ui->pwdLogin->text().toStdString() =="123456"*/)
    {
//        QMessageBoxDefine(QMessageBox::NoIcon,tr("成功"), tr("登陆成功！"),QMessageBox::Yes,this);
//        MessageBox *msgBox = new MessageBox();
//        msgBox->information("dsfa","asdfadsifjoaijfa dsfjia",QMessageBox::Yes|QMessageBox::Cancel);
//        msgBox->exec();
        // myMessageBox-
        //  QPushButton *okbtn=new QPushButton(QObject::tr("确定"));

        //QPushButton *cancelbtn=new QPushButton(QObject::tr("取消"));
        // QMessageBox *mymsgbox=new QMessageBox;
        // mymsgbox->addButton(okbtn,QMessageBox::AcceptRole);

        // mymsgbox->addButton(cancelbtn,QMessageBox::RejectRole);
        //mymsgbox->text()=tr("登陆成功！");
        // /mymsgbox->setWindowTitle("成功");
        //// mymsgbox->setText("登陆成功！");
        //mymsgbox->setDetailedText("登陆成功！");
        //mymsgbox->show();
        //mymsgbox->information(NULL,tr("成功"), tr("登陆成功！")，okbtn);

        //QMessageBoxDefine::information(NULL,tr("成功"), tr("登陆成功！"), QMessageBox::Yes, QMessageBox::Yes);
        //QMessageBox::information(NULL, "Title", "Content", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        //QMessageBox::information(NULL,tr("成功"), tr("登陆成功！"), QMessageBox::Yes, QMessageBox::Yes);
        emit sigResult(true);
        this->close();
    }
    else
    {
        QMessageBox::information(NULL,tr("失败"), tr("用户名或密码错误！请确认！"), QMessageBox::Yes, QMessageBox::Yes);
        //QMessageBoxDefine::information(NULL,tr("失败"), tr("用户名或密码错误！请确认！"), QMessageBox::Yes, QMessageBox::Yes);
        //QMessageBoxDefine(QMessageBox::NoIcon,tr("失败"), tr("用户名或密码错误！请确认！"),QMessageBox::Yes,this);
        this->ui->pwdLogin->text().clear();
        this->ui->txtUserName->text().clear();
    }
        //QMessageBox::information（"登陆成功！", "Title", "Content", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes）；

    //sigResult(false);
    //return false;
   // emit sigResult(true);
}

void DialogLogin::Cancel()
{
    //_timer->stop();
    sigResult(false);
}

void DialogLogin::showEvent(QShowEvent *)
{
    this->activateWindow();
    ui->txtUserName->setFocus();
}

// void DialogLogin::moveEvent(QMoveEvent *mouseEvent)
// {
////     if(mouseEvent->type()= QEvent::MouseMove)
////     {
//         _timer->stop();
//         _timer->start(1000*3);
//   //  }
// }

//  bool	DialogLogin::event ( QEvent * event )
//  {
//      if(event->type()== QEvent::MouseMove)
//      {
//          _timer->stop();
//          _timer->start(1000*3);
//      }
//      return true;
//  }
