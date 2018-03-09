/*****************************************************************************
** 文 件 名：basictypes.h
** 主 要 类：
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：王磊
** 创建时间：20013-10-27
**
** 修 改 人：
** 修改时间：
** 描  述:   串口类所涉及的基本数据类型定义
** 主要模块说明: 端口， 奇偶校验方式，数据位，停止位和串口配置, 错误码定义
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#pragma once
#include "../../../common.h"


const int E_SERIAL_BASE = -20000;                //串口错误码基准
const int E_SERIAL_OK = 0;                      //返回成功
const int E_SERIAL_ERROR = E_SERIAL_BASE - 1;               //返回失败

const int E_SERIAL_INVALID_PTR = E_SERIAL_BASE - 2;         //无效句柄

const int E_SERIAL_OPEN = E_SERIAL_BASE - 3;                //打开串口
const int E_SERIAL_INVALID_PORT = E_SERIAL_BASE - 4;        //无效串口
const int E_SERIAL_NOT_DEV = E_SERIAL_BASE - 5;             //打开的不是终端设备

const int E_SERIAL_SET_FILE_STATUS = E_SERIAL_BASE - 6;     //设置文件状态

const int E_SERIAL_READ = E_SERIAL_BASE - 7;                //读串口出错
const int E_SERIAL_ALLOC_MEMORY = E_SERIAL_BASE - 8;        //内存分配失败

const int E_SERIAL_SET_FRAME_INFO = E_SERIAL_BASE - 9;      //设置帧信息
const int E_SERIAL_GET_FRAME_INFO = E_SERIAL_BASE - 10;      //获取帧信息
const int E_SERIAL_INVALID_FRAME_EXTRACTOR = E_SERIAL_BASE - 11;      //无效帧解析器

const int E_SERIAL_SET_SERIAL = E_SERIAL_BASE - 12;          //设置串口
const int E_SERIAL_GET_SERIAL = E_SERIAL_BASE - 13;          //获取串口设置
const int E_SERIAL_SET_PARITY = E_SERIAL_BASE - 14;          //设置串口校验方式
const int E_SERIAL_SET_BAUD = E_SERIAL_BASE - 15;            //设置串口波特率
const int E_SERIAL_SET_DATA_BITS = E_SERIAL_BASE - 16;       //设置串口数据位
const int E_SERIAL_SET_STOP_BITS = E_SERIAL_BASE - 17;       //设置串口停止位

const int E_SERIAL_INVALID_CALLBACK = E_SERIAL_BASE - 18;    //非法callback参数

const int E_SERIAL_NO_FRAME = E_SERIAL_BASE - 19;            //未接收到一个有效完整帧
const int E_SERIAL_NO_DATA = E_SERIAL_BASE - 20;             //没有接收到串口数据
const int E_SERIAL_CREATE_THREAD_FAILED = E_SERIAL_BASE - 21;    //线程创建失败
const int E_SERIAL_CANCEL = E_SERIAL_BASE - 22;    //忽略下发数据

//波特率定义
typedef int BaudRate;
//端口定义
enum Port{ttyS0 = 0, ttyS1, ttyS2, ttyUSB0, ttyUSB1, ttyUSB2};
//校验方式定义
enum ParityMode{noParity = 0, spaceParity, oddParity, evenParity};
//数据位定义
enum DataBits{fiveBits = 5, sixBits, sevenBits, eightBits};
//停止位定义
enum StopBits{zeroBit = 0, oneBit = 1, twoBits};

//串口配置定义
struct SerialConfig
{
    ParityMode SerialParityMode;  //校验方式
    BaudRate SerialBaudRate;      //波特率
    DataBits SerialDataBits;      //数据位
    StopBits SerialStopBits;      //停止位

    SerialConfig(IN ParityMode parityMode = noParity,
                 IN BaudRate baudRate = 9600,
                 IN DataBits dataBits = eightBits,
                 IN StopBits stopBits = oneBit);
};

////帧信息定义
//struct FrameInfo
//{
//    unsigned char FrameHeader;      //帧头码
//    unsigned char FrameTail;        //帧尾码
//    int FrameLen;                   //帧长

//    FrameInfo(IN const unsigned char frameHeader = 0xdd, IN const unsigned char frameTail = 0xed, IN const int frameLen = 9);
//};


/*****************************************************************************
*                        数据接收回调函数
*  函 数 名：RecvCallBack
*  功    能：数据接收回调函数
*  说    明：
*           pBuf, 帧缓冲区指针，指向成帧的串口数据
*  参    数：
*  返 回 值：
*  创 建 人：王磊
*  创建时间：2013-10-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
typedef void (*RecvCallback)(IN const unsigned char *pBuf, IN const int dataSize);
