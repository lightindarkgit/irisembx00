/*****************************************************************************
** 文 件 名：serial.h
** 主 要 类：Serial
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：王磊
** 创建时间：20013-10-18
**
** 修 改 人：
** 修改时间：
** 描  述:   串口接口类实现
** 主要模块说明: 串口类实现
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <string>

#include "basictypes.h"
#include "syncserial.h"
#include "syncserialbase.h"
#include "innercommon.h"

using namespace std;


SyncSerial::SyncSerial():
    _syncSerialBase(NULL)
{
    //TO DO
    _syncSerialBase = new SyncSerialBase();
}

SyncSerial::~SyncSerial()
{
    delete _syncSerialBase;
}

///*****************************************************************************
//*                        串口类初始化_asyncSerialBase = NULL;
//*  函 数 名：Init
//*  功    能：初始化串口类的数据成员，以及帧缓冲区内存分配
//*  说    明：
//*  参    数：
//*  返 回 值：成功返回true,失败返回false
//*  创 建 人：王磊
//*  创建时间：2013-11-08
//*  修 改 人：
//*  修改时间：
//*****************************************************************************/
//int SyncSerial::Init(IN FrameInfo &frameInfo)
//{
//    if(NULL != _syncSerialBase)
//    {
//        delete _syncSerialBase;
//        _syncSerialBase = NULL;
//    }

//    _syncSerialBase = new SyncSerialBase();

//    return _syncSerialBase->Init(frameInfo);
//}

/*****************************************************************************
*                        打开串口
*  函 数 名：OpenPort
*  功    能：根据指定的参数打开串口
*  说    明：
*  参    数：
*           port，串口端口字符串，
*           callback，串口异步接收回调函数
*  返 回 值：
*          成功打开串口返回E_SERIAL_OK，否则返回对应错误码
*  创 建 人：王磊
*  创建时间：2013-11-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
int SyncSerial::OpenSerial(IN const char *port)
{
    if(NULL == port || NULL ==_syncSerialBase)
    {
        return E_SERIAL_INVALID_PORT;
    }


    string portName(ToLowerStr(port));    //将字符串转换为小写，以便于比较判断
//    int cmpRes = portName.compare("ttys0");
//    if(0 == cmpRes)
//    {
//        return cmpRes;
//    }

    if(!portName.compare("ttys0"))
    {
        return _syncSerialBase->OpenSerial("/dev/ttyS0");
    }
    else if(!portName.compare("ttys1"))
    {
        return _syncSerialBase->OpenSerial("/dev/ttyS1");
    }
    else if(!portName.compare("ttys2"))
    {
        return _syncSerialBase->OpenSerial("/dev/ttyS2");
    }
    else if(!portName.compare("ttyusb0"))
    {
        return _syncSerialBase->OpenSerial("/dev/ttyUSB0");
    }
    else if(! portName.compare("ttyusb1"))
    {
        return _syncSerialBase->OpenSerial("/dev/ttyUSB1");
    }
    else if(!portName.compare("ttyusb2"))
    {
        return _syncSerialBase->OpenSerial("/dev/ttyUSB2");
    }
    else
    {
        //错误处理
        return E_SERIAL_INVALID_PORT;
    }
}

/*****************************************************************************
*                        打开串口
*  函 数 名：OpenPort
*  功    能：根据指定的参数打开串口
*  说    明：
*  参    数：
*           port, 串口端口，枚举类型，取值范围：ttyS0, ttyS1, ttyS2, ttyUSB0, ttyUSB1, ttyUSB2；
*           callback，串口异步接收回调函数；
*  返 回 值：
*           成功打开串口返回E_SERIAL_OK，否则返回对应错误码
*  创 建 人：王磊
*  创建时间：2013-11-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
int SyncSerial::OpenSerial(IN const Port port)
{
    //return OpenSerial(*(dynamic_cast<const int *>(&port)));
    return OpenSerial((int)port);
}

/*****************************************************************************
*                        打开串口
*  函 数 名：OpenPort
*  功    能：根据指定的参数打开串口
*  说    明：
*  参    数：
*           port，串口端口序号；
*           callback，串口异步接收回调函数；
*  返 回 值：
*           成功打开串口返回E_SERIAL_OK，否则返回对应错误码
*  创 建 人：王磊
*  创建时间：2013-11-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
int SyncSerial::OpenSerial(IN const int port)
{
    if(port < 0 || NULL ==	_syncSerialBase)
    {
        return E_SERIAL_INVALID_PORT;
    }

    //判断并打开指定串口
    switch(port)
    {
    case ttyS0:
        return _syncSerialBase->OpenSerial("/dev/ttyS0");
    case ttyS1:
        return _syncSerialBase->OpenSerial("/dev/ttyS1");
    case ttyS2:
        return _syncSerialBase->OpenSerial("/dev/ttyS2");
    case ttyUSB0:
        return _syncSerialBase->OpenSerial("/dev/ttyUSB0");
    case ttyUSB1:
        return _syncSerialBase->OpenSerial("/dev/ttyUSB1");
    case ttyUSB2:
        return _syncSerialBase->OpenSerial("/dev/ttyUSB2");
    default:
        return E_SERIAL_INVALID_PORT;
    }
}

/*****************************************************************************
*                        关闭串口
*  函 数 名：ClosePort
*  功    能：关闭串口
*  说    明：
*  参    数：无
*  返 回 值：void
*  创 建 人：王磊
*  创建时间：2013-11-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
void SyncSerial::CloseSerial()
{
    if(NULL != _syncSerialBase && IsOpen())
    {
        _syncSerialBase->CloseSerial();
    }
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
int SyncSerial::GetSerialFd()
{
    if(NULL != _syncSerialBase)
    {
        return _syncSerialBase->GetSerialFd();
    }

    return E_SERIAL_INVALID_PORT;
}

/*****************************************************************************
*                        获取串口状态
*  函 数 名：IsOpen
*  功    能：获取串口当前状态
*  说    明：
*  参    数：
*  返 回 值：
*           打开，返回true， 否则返回false；
*  创 建 人：王磊
*  创建时间：2013-11-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool SyncSerial::IsOpen()
{
    if(NULL != _syncSerialBase)
    {
        return _syncSerialBase->IsOpen();
    }

    return false;
}

/*****************************************************************************
*                        获取串口配置
*  函 数 名：GetConfig
*  功    能：获取串口配置信息
*  说    明：
*  参    数：
*           serialConfig， 通过该参数输出当前串口配置
*  返 回 值：
*           成功打开串口返回E_SERIAL_OK，否则返回对应错误码
*  创 建 人：王磊
*  创建时间：2013-11-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
int SyncSerial::GetConfig(OUT SerialConfig &serialConfig)
{
    if(NULL != _syncSerialBase)
    {
        return _syncSerialBase->GetConfig(serialConfig);
    }

    return E_SERIAL_INVALID_PORT;
}

/*****************************************************************************
*                        设置串口
*  函 数 名：SetConfig
*  功    能：设置串口配置参数
*  说    明：
*  参    数：
*           serialConfig， 串口配置参数
*  返 回 值：
*           成功打开串口返回E_SERIAL_OK，否则返回对应错误码
*  创 建 人：王磊
*  创建时间：2013-11-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
int SyncSerial::SetConfig(IN const SerialConfig &serialConfig)
{
    if(NULL != _syncSerialBase)
    {
        return _syncSerialBase->SetConfig(serialConfig);
    }

    return E_SERIAL_INVALID_PORT;
}


///*****************************************************************************
//*                        获取帧信息
//*  函 数 名：GetFrameInfo
//*  功    能：获取帧信息，包括帧头,帧尾和帧长
//*  说    明：
//*  参    数：
//*           frameInfo，帧信息输出参数
//*  返 回 值：
//*           成功打开串口返回E_SERIAL_OK，否则返回对应错误码
//*  创 建 人：王磊
//*  创建时间：2013-11-12
//*  修 改 人：
//*  修改时间：
//*****************************************************************************/
//int SyncSerial::GetFrameInfo(OUT FrameInfo& frameInfo)
//{
//    if(NULL != _syncSerialBase)
//    {
//        return _syncSerialBase->GetFrameInfo(frameInfo);
//    }
//
//    return E_SERIAL_INVALID_PORT;
//}

///*****************************************************************************
//*                        设置帧信息
//*  函 数 名：SetFrameInfo
//*  功    能：设置帧信息，如帧头，帧尾和帧长
//*  说    明：
//*  参    数：
//*           frameInfo， 帧信息输入参数
//*  返 回 值：
//*           成功打开串口返回E_SERIAL_OK，否则返回对应错误码
//*  创 建 人：王磊
//*  创建时间：2013-11-12
//*  修 改 人：
//*  修改时间：
//*****************************************************************************/
//int SyncSerial::SetFrameInfo(IN const FrameInfo &frameInfo)
//{
//    if(NULL != _syncSerialBase)
//    {
//        return _syncSerialBase->SetFrameInfo(frameInfo);
//    }
//
//    return E_SERIAL_INVALID_PORT;
//}


/*****************************************************************************
*                        发送数据
*  函 数 名：Send
*  功    能：发送数据到串口
*  说    明：
*  参    数：
*           pBuf，待发送数据缓冲区，缓冲待发送的数据
*           sizeToSend，待发送数据的字节长度
*  返 回 值：
*           如果发送成功则返回发送到串口的字节数，否则，返回对应的错误码
*  创 建 人：王磊
*  创建时间：2013-11-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
int SyncSerial::Send(IN const unsigned char *pBuf, IN const int sizeToSend)
{
    if(NULL == pBuf)
    {
        return E_SERIAL_INVALID_PTR;
    }

    if(NULL == _syncSerialBase)
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

        _syncSerialBase->FormFrame(frameBuf, pBuf, currentSend);     //将原始发送数据封装成帧
        sendRes = _syncSerialBase->Send(frameBuf, frameBufSize);     //发送帧到串口

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
*                        接收串口数据
*  函 数 名：Recv
*  功    能：接收串口数据
*  说    明：
*  参    数：
*           pBuf,串口数据接收缓冲区
*           bufSize， 接收缓冲区大小
*  返 回 值：
*           接收成功，返回接收到的字节数，否则返回对应的错误码
*  创 建 人：王磊
*  创建时间：2013-11-08
*  修 改 人：
*  修改时间：
*****************************************************************************/
int SyncSerial::Recv(OUT unsigned char *pBuf, IN const int bufSize)
{
    if(NULL == _syncSerialBase)
    {
        return E_SERIAL_INVALID_PORT;
    }

    int rtnVal = _syncSerialBase->GetFrame(pBuf, bufSize);
    return E_SERIAL_OK == rtnVal ? FRAME_DATA_SIZE : rtnVal;
}
