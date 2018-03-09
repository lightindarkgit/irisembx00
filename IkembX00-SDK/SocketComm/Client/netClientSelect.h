
/******************************************************************************************
** 文件名:   NetClientSelect.h
×× 主要类:   NetClientSelect
**  
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2013-11-22
** 修改人:   
** 日  期:
** 描  述:   网络通信客户端类－－－Select模式
** ×× 主要模块说明：                    
**
** 版  本:   1.0.0
** 备  注:   
**
 *****************************************************************************************/
#include "netClient.h"
#include <thread>

class NetClientSelect:public NetClient
{
public:
	NetClientSelect();
	~NetClientSelect();
public:
	int   Send(char*buf,int len);
	bool  CreateSendThread();
	bool  CreateSendThreadStd();
	bool  CreateRecvThread();
	bool  CreateRecvThreadStd();
	bool  StartThread();
public:
        static void * WorkThread(void *);
	static void  WorkThreadStd(NetClientSelect *);
	static void * SendThreadSelect(void *);
	static void  RecvThreadSelect(NetClientSelect*);
public:
	SendParam& GetSendBuf();
	SOCKET GetSocket();
protected:
	//bool Connect(int socketFd, struct sockaddr*serverAddr, int addrLen);
private:
	//接收和发送线程类
	std::thread _sendThread;
	std::thread _recvThread;
	SendParam _sendData;
};
