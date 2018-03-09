/******************************************************************************************
** 文件名:   NetClient.cpp
×× 主要类:   NetClient
**  
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2013-11-05
** 修改人:   
** 日  期:
** 描  述:   网络通信客户端基类
** ×× 主要模块说明：                    
**
** 版  本:   1.0.0
** 备  注:   
**
 *****************************************************************************************/
#pragma once
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "../socketcom.h"
#include <functional>
#include "../socketBase.h"
#include <list>
#include <mutex>
#include <condition_variable>
#include "../stdWait.h"
#include "../queueThread.h"

class NetClient:public SocketBase
{
public:
	typedef std::function<void(char*buf,int len)> DelegateRecv;     //标准库的function变量声明
public:
	NetClient();
	virtual ~NetClient();
public:
	bool Init(const ClientData &cd,DelegateRecv cbr);
	int RecvTransfer( char* buf);
	virtual int Send( char * buf,int len);
	virtual bool Close();

	bool ControlBlock(bool isBlock);
	bool NonBlockConnect();
	void NoticeOne();
protected:
	virtual bool Connect(int socketFd,struct sockaddr*serverAddr,int addrLen);
	bool SetSocket();
protected:
	SOCKET _socket;                   //网络句柄
	DelegateRecv _dr;
	char _bufRecv[g_recvBuf];
public:
	//线程信号通知机制
	StdWait ConWait;
	QueueThread<SendDataBuf> QueueData;
	bool _isQuit = true;
private:
	ClientData _cd;
};
