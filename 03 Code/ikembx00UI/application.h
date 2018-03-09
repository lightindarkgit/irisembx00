/*****************************************************************************
** 文 件 名： Application.h
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
//#endif // APPLICATION_H
#ifndef APPLICATION_H
#define APPLICATION_H
#include <QApplication>
#include <QDebug>
#include "QWSEvent"
#include "QTimer"

class Application : public QApplication
{
public:
    Application(int & argc, char ** argv);
    bool notify(QObject *, QEvent *);
    //bool qwsEventFilter(QWSEvent * event);
private slots:
    void ShowMessage();
private :
    QTimer *timer;
};
#endif // APPLICATION_H
