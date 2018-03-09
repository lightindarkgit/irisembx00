/*****************************************************************************
 ** 文 件 名：asyncserial.cpp
 ** 主 要 类：AsyncSerial
 **
 ** Copyright (c) 中科虹霸有限公司
 **
 ** 创 建 人：王磊
 ** 创建时间：20013-10-18
 **
 ** 修 改 人：
 ** 修改时间：
 ** 描  述:   异步串口接口类实现
 ** 主要模块说明: 异步串口类
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 *****************************************************************************/
#include <stdlib.h>
#include <string.h>

#include "../include/basictypes.h"
#include "../include/asyncserialbase.h"
#include "../include/asyncserial.h"
#include "../include/innercommon.h"
#include "../../../Common/Exectime.h"

//引入命名空间
using namespace std;

//同步串口实现
AsyncSerial::AsyncSerial():
	_asyncSerialBase(nullptr)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	_asyncSerialBase = new AsyncSerialBase();
}


AsyncSerial::~AsyncSerial()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	delete _asyncSerialBase;
}

///*****************************************************************************
//*                        异步串口初始化
//*  函 数 名：Init
//*  功    能：初始化异步串口
//*  说    明：
//*  参    数：
//*           frameInfo, 帧信息，如帧头码，帧尾码和帧长
//*           callback， 异步串口接收回调函数
//*  返 回 值：
//*           成功返回E_SERIAL_OK，否则返回对应错误码
//*  创 建 人：王磊
//*  创建时间：2013-11-25
//*  修 改 人：
//*  修改时间：
//*****************************************************************************/
//int AsyncSerial::Init(IN const FrameInfo &frameInfo, IN RecvCallback callback)
//{
//    //TO DO
//    if(nullptr != _asyncSerialBase)
//    {
//        delete _asyncSerialBase;
//        _asyncSerialBase = nullptr;
//    }

//    _asyncSerialBase = new AsyncSerialBase();
//    if(nullptr == _asyncSerialBase)
//    {
//        return E_SERIAL_INVALID_PORT;
//    }

//    return _asyncSerialBase->Init(frameInfo, callback);
//}


/*****************************************************************************
 *                        打开串口
 *  函 数 名：OpenSerial
 *  功    能：根据指定的参数打开串口
 *  说    明：
 *  参    数：
 *           port，串口端口号, 取值范围：ttyS0, ttyS1, ttyS2, ttyUSB0, ttyUSB1, ttyUSB2；
 *  返 回 值：
 *           成功打开串口返回E_SERIAL_OK，否则返回对应错误码
 *  创 建 人：王磊
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int AsyncSerial::OpenSerial(IN const Port port, IN RecvCallback callback)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(nullptr == _asyncSerialBase)
	{
		return E_SERIAL_INVALID_PORT;
	}

	return OpenSerial((int)port, callback);
}

/*****************************************************************************
 *                        打开串口
 *  函 数 名：OpenPort
 *  功    能：根据指定的参数打开串口
 *  说    明：在成功打开串口之后，还会启动串口数据接收线程；
 *  参    数：
 *           port，串口端口字符串，
 *           callback，串口异步接收回调函数
 *  返 回 值：
 *           成功打开串口返回E_SERIAL_OK，否则返回对应错误码
 *  创 建 人：王磊
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int AsyncSerial::OpenSerial(IN const char *port, IN RecvCallback callback)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(nullptr == port || nullptr == _asyncSerialBase)
	{
		return E_SERIAL_INVALID_PORT;
	}
	printf("Serial to open: %s\n", port);
	_asyncSerialBase->Init(callback);

	string portName(ToLowerStr(port));    //将字符串转换为小写，以便于比较判断
	printf("After Convert Serial to open: %s\n", portName.c_str());
	if(!portName.compare("ttys0"))
	{
		return _asyncSerialBase->OpenSerial("/dev/ttyS0");
	}
	else if(!portName.compare("ttys1"))
	{
		return _asyncSerialBase->OpenSerial("/dev/ttyS1");
	}
	else if(!portName.compare("ttys2"))
	{
		return _asyncSerialBase->OpenSerial("/dev/ttyS2");
	}
	else if(!portName.compare("ttys3"))
	{
		return _asyncSerialBase->OpenSerial("/dev/ttyS3");
	}
	else if(!portName.compare("ttys4"))
	{
		return _asyncSerialBase->OpenSerial("/dev/ttyS4");
	}
	else if(!portName.compare("ttys5"))
	{
		return _asyncSerialBase->OpenSerial("/dev/ttyS5");
	}
	else if(!portName.compare("ttys6"))
	{
		return _asyncSerialBase->OpenSerial("/dev/ttyS6");
	}
	else if(!portName.compare("ttys7"))
	{
		return _asyncSerialBase->OpenSerial("/dev/ttyS7");
	}
	else if(!portName.compare("ttyusb0"))
	{
		return _asyncSerialBase->OpenSerial("/dev/ttyUSB0");
	}
	else if(!portName.compare("ttyusb1"))
	{
		return _asyncSerialBase->OpenSerial("/dev/ttyUSB1");
	}
	else if(!portName.compare("ttyusb2"))
	{
		return _asyncSerialBase->OpenSerial("/dev/ttyUSB2");
	}
	else if(!portName.compare("ttyusb3"))
	{
		return _asyncSerialBase->OpenSerial("/dev/ttyUSB3");
	}
	else if(!portName.compare("ttyusb4"))
	{
		return _asyncSerialBase->OpenSerial("/dev/ttyUSB4");
	}
	else if(!portName.compare("ttyusb5"))
	{
		return _asyncSerialBase->OpenSerial("/dev/ttyUSB5");
	}
	else if(!portName.compare("ttyusb6"))
	{
		return _asyncSerialBase->OpenSerial("/dev/ttyUSB6");
	}
	else if(!portName.compare("ttyusb7"))
	{
		return _asyncSerialBase->OpenSerial("/dev/ttyUSB7");
	}

	return E_SERIAL_INVALID_PORT;
}

/*****************************************************************************
 *                        打开串口
 *  函 数 名：OpenPort
 *  功    能：根据指定的参数打开串口
 *  说    明：在成功打开串口之后，还会启动串口数据接收线程；
 *  参    数：
 *           port，串口端口号
 *  返 回 值：
 *           成功打开串口返回E_SERIAL_OK，否则返回对应错误码
 *  创 建 人：王磊
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int AsyncSerial::OpenSerial(IN const int port, IN RecvCallback callback)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(port < 0 || nullptr == _asyncSerialBase)
	{
		return E_SERIAL_INVALID_PORT;
	}

	_asyncSerialBase->Init(callback);

	//判断并打开指定串口
	switch(port)
	{
		case ttyS0:
			return _asyncSerialBase->OpenSerial("/dev/ttyS0");
		case ttyS1:
			return _asyncSerialBase->OpenSerial("/dev/ttyS1");
		case ttyS2:
			return _asyncSerialBase->OpenSerial("/dev/ttyS2");
		case ttyUSB0:
			return _asyncSerialBase->OpenSerial("/dev/ttyUSB0");
		case ttyUSB1:
			return _asyncSerialBase->OpenSerial("/dev/ttyUSB1");
		case ttyUSB2:
			return _asyncSerialBase->OpenSerial("/dev/ttyUSB2");
		default:
			break;
	}

	return E_SERIAL_INVALID_PORT;
}

/*****************************************************************************
 *                        关闭串口
 *  函 数 名：CloseSerial
 *  功    能：关闭串口
 *  说    明：
 *  参    数：
 *  返 回 值：void
 *  创 建 人：王磊
 *  创建时间：2013-11-25
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void AsyncSerial::CloseSerial()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(nullptr == _asyncSerialBase || !IsOpen())
	{
		return;
	}

	//先停止接收，然后关闭串口
	int rtnVal = E_SERIAL_ERROR;
	rtnVal = _asyncSerialBase->StopRecv();
	if(E_SERIAL_OK != rtnVal)
	{
		//关闭接收线程失败，无须担心的是在当前条件下不会出现
		return;
	}

	return _asyncSerialBase->CloseSerial();
}

/*****************************************************************************
 *                        检查串口是否打开
 *  函 数 名：IsOpen
 *  功    能：检查串口是否打开
 *  说    明：
 *  参    数：
 *  返 回 值：
 *           已打开返回true， 否则返回false
 *  创 建 人：王磊
 *  创建时间：2013-11-25
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool AsyncSerial::IsOpen()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(nullptr == _asyncSerialBase)
	{
		return false;
	}

	return _asyncSerialBase->IsOpen();
}

/*****************************************************************************
 *                        获取串口串口端口
 *  函 数 名：GetPort
 *  功    能：获取串口端口
 *  说    明：
 *  参    数：无
 *  返 回 值：成功则返回当前打开的串口端口设备描述符，否则返回对应错误码
 *  创 建 人：王磊
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int AsyncSerial::GetSerialFd()
{
	Exectime etm(__FUNCTION__);
	if(nullptr != _asyncSerialBase)
	{
		return _asyncSerialBase->GetSerialFd();
	}

	return E_SERIAL_INVALID_PORT;
}

/*****************************************************************************
 *                        获取串口配置
 *  函 数 名：GetConfig
 *  功    能：获取串口配置信息
 *  说    明：
 *  参    数：
 *           serialConfig， 通过该参数输出当前串口配置
 *  返 回 值：
 *           成功获取配置返回true，否则返回false
 *  创 建 人：王磊
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int AsyncSerial::GetConfig(OUT SerialConfig &serialConfig)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(nullptr == _asyncSerialBase)
	{
		return E_SERIAL_INVALID_PORT;
	}

	return _asyncSerialBase->GetConfig(serialConfig);
}

/*****************************************************************************
 *                        设置串口
 *  函 数 名：SetConfig
 *  功    能：设置串口配置参数
 *  说    明：
 *  参    数：
 *           serialConfig， 串口配置参数
 *  返 回 值：
 *           成功设置配置返回E_SERIAL_OK， 否则返回对应错误码
 *  创 建 人：王磊
 *  创建时间：2013-11-25
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int AsyncSerial::SetConfig(IN const SerialConfig &serialConfig)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(nullptr == _asyncSerialBase)
	{
		return E_SERIAL_INVALID_PORT;
	}

	return _asyncSerialBase->SetConfig(serialConfig);
}

/*****************************************************************************
 *                        接收串口数据
 *  函 数 名：Recv
 *  功    能：接收串口数据
 *  说    明：
 *  参    数：
 *           pBuf, 串口数据接收缓冲区
 *           sizeToRecv，待发送的字节数
 *  返 回 值：
 *           成功接收到的字节数
 *  创 建 人：王磊
 *  创建时间：2013-11-25
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int AsyncSerial::Send(IN const unsigned char *pBuf, IN const int sizeToSend)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(nullptr == pBuf)
	{
		return E_SERIAL_INVALID_PTR;
	}

	if(nullptr == _asyncSerialBase)
	{
		return E_SERIAL_INVALID_PORT;
	}

	int totalSend = 0;      //总计发送的字节数
	unsigned char frameBuf[frameBufSize];     //临时帧缓冲区

	for(int currentSend, sendRes; totalSend < sizeToSend;)
	{
		//计算本次循环要发送的数据字节大小
		if(sizeToSend - totalSend >= FRAME_DATA_SIZE)
		{
			currentSend = FRAME_DATA_SIZE;
		}
		else
		{
			currentSend = sizeToSend % FRAME_DATA_SIZE;
		}


		_asyncSerialBase->FormFrame(frameBuf, pBuf, currentSend);     //将原始发送数据封装成帧
		sendRes = _asyncSerialBase->Send(frameBuf, frameBufSize);     //发送帧到串口

		if(sendRes < 0)
		{
			break;
		}

		pBuf += FRAME_DATA_SIZE;
		totalSend += currentSend;
	}

	return  totalSend;
}

/*****************************************************************************
 *                        启动接收串口数据线程
 *  函 数 名：StartRecv
 *  功    能：启动读串口线程，接收串口数据
 *  说    明：
 *  参    数：
 *  返 回 值：
 *           成功，返回E_SERIAL_OK
 *  创 建 人：王磊
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int AsyncSerial::StartRecv()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(nullptr == _asyncSerialBase)
	{
		return E_SERIAL_INVALID_PORT;
	}

	return _asyncSerialBase->StartRecv();
}

/*****************************************************************************
 *                        停止接收串口数据
 *  函 数 名：StopRecv
 *  功    能：停止接收串口数据
 *  说    明：
 *  参    数：
 *  返 回 值：
 *           成功，返回E_SERIAL_OK
 *  创 建 人：王磊
 *  创建时间：2013-11-25
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int AsyncSerial::StopRecv()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(nullptr == _asyncSerialBase)
	{
		return E_SERIAL_INVALID_PORT;
	}

	return _asyncSerialBase->StopRecv();
}

