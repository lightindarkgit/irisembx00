/******************************************************************************************
** �ļ���:   NetClientPoll.h
���� ��Ҫ��:   NetClientPoll
**
** Copyright (c) �пƺ�����޹�˾
** ������:   fjf
** ��  ��:   2013-12-04
** �޸���:
** ��  ��:
** ��  ��:   ����ͨ�ſͻ����࣭����Pollģʽ
** ���� ��Ҫģ��˵����
**
** ��  ��:   1.0.0
** ��  ע:
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
	//�����̺߳���
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
