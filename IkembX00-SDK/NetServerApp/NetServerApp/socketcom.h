/******************************************************************************************
**文件名:   common.h
×× 主要类:   (无)
**  
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2013-11-06
** 修改人:   
** 日  期:
** 描  述:   网络通信使用的标准头文件
** ×× 主要模块说明：                    
**
** 版  本:   1.0.0
** 备  注:   
**
******************************************************************************************/
#pragma once
#include <functional>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <string>
#include <exception>
#include "stdWait.h"
#include <winsock2.h>

#pragma comment(lib, "Ws2_32.lib")	// 链接到WS2_32.lib//说明：CE及PPC用前者，WS2_32.lib用于PC

#define __CPP_11_LIB__                        //定义C++11编译环境

const int g_recvBuf   = 1024;                  //缓冲区的大小
const int g_sendBuf   = 1024;
const int g_limitFd   = 65535;                 //描述符的数量
const int g_port      = 9999;                  //端口号
const int g_timeOut   = 20;                    //非阻塞连接超时时间
const int g_timeBeat  = 1000 * 10;             //测试心跳所以少一些，真正为其6~12倍

//自定义消息发送数量
#define  WM_COUNT_MESSAGE WM_USER+10         

//数据发送结构体
typedef struct __SendDataBuf
{
	int Len;						//数据长度
	char SendBuf[g_sendBuf];        //数据缓冲区
}SendDataBuf,*PSendDataBuf;


typedef std::function<void(char*buf,int len)> DelegateRecv;     //标准库的function变量声明,用于网络数据的传送
typedef std::list<SendDataBuf> SendParam ;						//数据发送存储区


//网络通信出始化类
class CInitSock
{
public:
	CInitSock(BYTE minorVer = 2, BYTE majorVer = 2)
	{
		// 初始化WS2_32.dll
		WSADATA wsaData;
		WORD sockVersion = MAKEWORD(minorVer, majorVer);
		if (::WSAStartup(sockVersion, &wsaData) != 0)
		{
			exit(0);
		}
	}
	~CInitSock()
	{
		::WSACleanup();
	}
};



