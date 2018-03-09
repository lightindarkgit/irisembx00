#pragma once
#include <thread>
#include <memory>
#include "socketcom.h"
#include "autoMachine.h"
#include <mutex>

//客户端数据结构体
typedef struct __ClientData
{
	SOCKET Socket;                         //网络文件描述符（SOCKERT句柄）
	std::string Sip;                       //地址
	unsigned short Port;
	char RecvBuf[g_recvBuf];               //接收缓冲区
	AutoMachine AutoControl;               //自动机解析成员－客户端可以直接在类中使用---在客户端比较灵活，可以在类中直接使用
}ClientData, *PClientData;

typedef std::list<std::shared_ptr<ClientData>> ClientDataList;                   //SOCKET连接链表

class CNetServerAppDlg;
class NetServer
{
public:
	NetServer();
	NetServer(std::string ip,USHORT port);
	virtual ~NetServer();
public:
	int Init(std::string ip, USHORT port);
	virtual int Send(char *buf,int len);
	virtual void StartThread(void);
protected:
	virtual bool CreateRecvThread();
	virtual bool CreateSendThread();
	virtual bool CreateAcceptThread(void);
protected:
	std::thread _recvThread;            //创建接收线程
	CInitSock _initSocket;              //SOCKET库的引入和控制
	ClientDataList _sockClient;         //实际队列（有客户端数据）
	ClientDataList _sockHandle;         //创建客户端SOCKET队列---注意不可超过FD_SIZE=128
	SOCKET _listenSock;                 //监听SOCKET
	CNetServerAppDlg *_pTemp;
	std::mutex _mtx;
};

