/******************************************************************************************
 ** 文件名:   ikevent.cpp
 ×× 主要类:   (无)
 **
 ** Copyright (c) 中科虹霸有限公司
 ** 创建人:   Wang.L
 ** 日  期:   2015-01-26
 ** 修改人:
 ** 日  期:
 ** 描  述:   全局事件类定义，用于发送和接收event通知和处理event事件
 ** ×× 主要模块说明：
 **    串口数据传输：Serialvent事件
 **    识别事件： IKEventIns事件
 ** 版  本:   1.0.0
 ** 备  注:
 **
 ******************************************************************************************/
#include "ikevent.h"
#include "../Common/Exectime.h"

SerialEvent::SerialEvent()
{
	
}


void SerialEvent::onGetICCardNum(QString cardNum)
{
	
	emit GetICCrad(cardNum);
}




//IKEvent implement section
IKEvent::IKEvent()
{
	
	//TODO
}


void IKEvent::CloseInfrared()
{
	
	emit sigCloseInfrared();
}


void IKEvent::ReplyInfraredClosed()
{
	
	emit sigInfraredClosed();
}



//Init Gloab Event instance
IKEvent* IKEventIns::e = nullptr;

//Implements of IKEventIns
IKEventIns::IKEventIns()
{
	
	if(nullptr == e)
	{
		e = new IKEvent();
	}
}


IKEventIns::~IKEventIns()
{
	
	if(e)
	{
		delete e;
	}
}


IKEvent* IKEventIns::GetIKEvent()
{
	
	return e;
}

