#pragma once
#include "NetServer.h"

class ServerSelect :
	public NetServer
{
public:
	ServerSelect();
	virtual ~ServerSelect();
public:	
	virtual void StartThread(void);
	void SetReadSocket();
	std::shared_ptr<ClientData> GetClientHandle(const SOCKET &s);

	//处理连接客户端队列函数
	bool DeleteClientHandle(const SOCKET &s);
	bool AddClientHandle(std::shared_ptr<ClientData> &pcd);
	bool CreateClientHandle(void);

	//管理原始客户端队列函数
	std::shared_ptr<ClientData> GetHandle(void);
	bool PutHandle(std::shared_ptr<ClientData>& p);
protected:
	virtual int Send(char *buf, int len);
	virtual bool CreateRecvThread(void);
	virtual bool CreateSendThread(void);
	virtual bool CreateAcceptThread(void);
private:
	int _countSocket;
	std::thread _acceptThread;              //接收线程
	fd_set _fd;                             //控制监测的收发句柄
};

