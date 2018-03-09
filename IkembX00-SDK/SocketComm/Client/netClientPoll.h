/******************************************************************************************
** 文件名:   NetClientPoll.h
×× 主要类:   NetClientPoll
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2013-12-04
** 修改人:
** 日  期:
** 描  述:   网络通信客户端类－－－Poll模式
** ×× 主要模块说明：
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
#include "netClient.h"

class NetClientPoll:public NetClient
{
public:
	NetClientPoll();
	~NetClientPoll();
public:
	virtual int Send(char*buf,int len);
public:
	//定义线程函数
	bool StartThread();
	bool CreateSendThread();
	bool CreateRecvThread();
public:
        static void  RecvThreadPoll(void *pParam);
	static void SendThreadPoll(void *pParam);
protected:
	//virtual bool Connect(int socketFd, struct sockaddr*serverAddr, int addrLen);
private:

};
