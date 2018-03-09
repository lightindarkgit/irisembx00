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

	//�������ӿͻ��˶��к���
	bool DeleteClientHandle(const SOCKET &s);
	bool AddClientHandle(std::shared_ptr<ClientData> &pcd);
	bool CreateClientHandle(void);

	//����ԭʼ�ͻ��˶��к���
	std::shared_ptr<ClientData> GetHandle(void);
	bool PutHandle(std::shared_ptr<ClientData>& p);
protected:
	virtual int Send(char *buf, int len);
	virtual bool CreateRecvThread(void);
	virtual bool CreateSendThread(void);
	virtual bool CreateAcceptThread(void);
private:
	int _countSocket;
	std::thread _acceptThread;              //�����߳�
	fd_set _fd;                             //���Ƽ����շ����
};

