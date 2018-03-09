/*****************************************************************************
** 文 件 名： Application.cpp
** 主 要 类： Application
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：刘中昌
** 创建时间：20013-10-24
**
** 修 改 人：
** 修改时间：
** 描  述:   应用屏保测试
** 主要模块说明: 应用屏保测试
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/

#include "application.h"
#include "time.h"
#include "qmessagebox.h"

Application::Application(int &argc, char **argv):
    QApplication(argc,argv)
{

    /**********定时器*****************/
    //新建定时器
    timer = new QTimer();
    //关联定时器计满信号和相应的槽函数
    connect(timer,SIGNAL(timeout()),this,SLOT(ShowMessage()));
    //定时器开始计时，其中1000表示1000ms即1秒
    timer->start(1000*5);
}
void Application::ShowMessage()
{
    int abc =90;
    //QMessageBoxDefine(QMessageBox::NoIcon,tr("失败"), tr("用户名或密码错误！请确认！"),QMessageBox::Yes);
    //QMessageBox::information（"超时", "Title", "Content", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes）；
}

bool Application::notify(QObject *obj, QEvent *e)
{
    if(e->type()== QEvent::MouseMove)
    {
        timer->stop();
        timer->start(1000*5);
    }

    //do something
    return QApplication::notify(obj,e);
}

//bool Application::qwsEventFilter(QWSEvent *event)
//bool Application::qwsEventFilter(QWSEvent * event)
//{
//    //do something
//    return QApplication::qwsEventFilter(event);
//}
