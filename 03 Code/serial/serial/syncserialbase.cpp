/*****************************************************************************
** 文 件 名：syncserialbase.cpp
** 主 要 类：SyncSerialBase
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：王磊
** 创建时间：20013-10-18
**
** 修 改 人：
** 修改时间：
** 描  述:   串口类定义
** 主要模块说明: 串口类定义， 串口接收数据回调函数声明
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include <stdio.h>        //标准输入输出定义
#include <stdlib.h>       //标准函数库定义
#include <unistd.h>       //Unix标准函数定义
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>        //文件控制定义
#include <termios.h>      //POSIX中断控制定义
#include <errno.h>        //错误号定义
#include <string.h>
#include <pthread.h>

#include "innercommon.h"
#include "basictypes.h"
#include "syncserialbase.h"      //串口类定义头文件
#include "serialframe.h"         //帧处理头文件

//引入标准命名空间
using namespace std;


//同步串口类实现
SyncSerialBase::SyncSerialBase():
    _serialFrame(NULL)
{
    //TO DO
    _serialFrame = new SerialFrame();   //创建帧解析器
}

SyncSerialBase::~SyncSerialBase()
{
    CloseSerial();

    if(NULL != _serialFrame)
    {
        delete _serialFrame;   //销毁帧提取器
    }
}

///*****************************************************************************
//*                        串口类初始化
//*  函 数 名：Init
//*  功    能：初始化串口类的数据成员，以及帧缓冲区内存分配
//*  说    明：
//*  参    数：
//*           frameInfo, 帧信息，如帧头码，帧尾码和帧长
//*  返 回 值：
//*           成功返回E_SERIAL_OK，否则返回对应错误码
//*  创 建 人：王磊
//*  创建时间：2013-11-08
//*  修 改 人：
//*  修改时间：
//*****************************************************************************/
//int SyncSerialBase::Init(IN const FrameInfo& frameInfo)
//{
//    //设置帧信息
//    if(E_SERIAL_OK != _frameExtractor->SetFrameInfo(frameInfo))
//    {
//        return E_SERIAL_ERROR;
//    }

//    //申请帧缓冲区内存
//    _frameBuf = new unsigned char[frameInfo.FrameLen];
//    if(NULL == _frameBuf)
//    {
//        return E_SERIAL_ALLOC_MEMORY;
//    }
//
//
//    return E_SERIAL_OK;
//}

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
int SyncSerialBase::OpenSerial(IN const char *port)
{
    if(NULL == port)
    {
        return E_SERIAL_INVALID_PTR;
    }

    //打开串口
    return Open(port);
}

/*****************************************************************************
*                        获取一帧串口数据
*  函 数 名：GetFrame
*  功    能：获取一帧串口数据
*  说    明：如果在超时之前获取到一帧数据，则返回E_SERIAL_OK, 否则返回对应的错误码
*  参    数：
*           pBuf，帧缓冲区
*           bufSize， 接收缓冲区大小
*  返 回 值：
*           成功返回E_SERIAL_OK，否则返回对应错误码
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
int SyncSerialBase::GetFrame(OUT unsigned char *pBuf, IN const int bufSize)
{
    //TO DO
    int rtnVal = E_SERIAL_ERROR;

    //如果当前还有未解析的数据，则先从已收到的数据中进行解析
    if(E_SERIAL_OK == _serialFrame->GetFrame(pBuf))
    {
        return E_SERIAL_OK;
    }

    //如果已收到的数据不足一帧，或不存在尚未解析的数据，则先从串口接收数据然后再进行解析
//    do
//    {
        rtnVal = Recv(_recvBuf, bufSize, BLOCK_MODE);
//    }while(rtnVal <= 0);


    _serialFrame->WriteData(_recvBuf, rtnVal);
    if(E_SERIAL_OK != _serialFrame->GetFrame(pBuf))
    {
        return E_SERIAL_NO_FRAME;
    }

    return E_SERIAL_OK;
}

/*****************************************************************************
*                        获取一帧串口数据
*  函 数 名：GetFrame
*  功    能：获取一帧串口数据
*  说    明：如果在超时之前获取到一帧数据，则返回E_SERIAL_OK, 否则返回对应的错误码
*  参    数：
*           pBuf，帧缓冲区
*           timeoutSecs, 超时时间，单位：秒
*  返 回 值：
*           成功返回E_SERIAL_OK，否则返回对应错误码
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
int SyncSerialBase::FormFrame(unsigned char *pFrameBuf, const unsigned char *prawData, IN const int dataSize)
{
    //TO DO
    if(NULL == _serialFrame)
    {
        return E_SERIAL_INVALID_FRAME_EXTRACTOR;
    }

   return _serialFrame->FormFrame(pFrameBuf,prawData, dataSize);
}
