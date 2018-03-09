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
#include "../Common/Exectime.h"
#include <QMouseEvent>

Application::Application(int &argc, char **argv):
	QApplication(argc,argv),
	_mainWind(0)
{
    Exectime etm(__FUNCTION__);

}


void Application::SetWindowInstance(MainWindow* wid)
{
     Exectime etm(__FUNCTION__);
	
     _mainWind = wid;
}

void Application::ShowMessage()
{
	
	//int abc =90;
	//QMessageBoxDefine(QMessageBox::NoIcon,tr("失败"), tr("用户名或密码错误！请确认！"),QMessageBox::Yes);
	//QMessageBox::information（"超时", "Title", "Content", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes）；
}

//bool Application::notify(QObject *obj, QEvent *e)
//{
//     Exectime etm(__FUNCTION__);
	

//    if(/*e->type()== QEvent::MouseMove ||*/ e->type()== QEvent::MouseButtonPress)
//	{
//		if(_mainWind)
//		{
//            _mainWind->Alive();
//		}
//	}
//    //if(e->type()== QEvent::MouseButtonPress)

//	//do something
//	return QApplication::notify(obj,e);
//}

//bool Application::qwsEventFilter(QWSEvent *event)

