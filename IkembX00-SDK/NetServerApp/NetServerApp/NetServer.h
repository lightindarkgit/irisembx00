#pragma once
#include <thread>
#include <memory>
#include "socketcom.h"
#include "autoMachine.h"
#include <mutex>

//�ͻ������ݽṹ��
typedef struct __ClientData
{
	SOCKET Socket;                         //�����ļ���������SOCKERT�����
	std::string Sip;                       //��ַ
	unsigned short Port;
	char RecvBuf[g_recvBuf];               //���ջ�����
	AutoMachine AutoControl;               //�Զ���������Ա���ͻ��˿���ֱ��������ʹ��---�ڿͻ��˱Ƚ�������������ֱ��ʹ��
}ClientData, *PClientData;

typedef std::list<std::shared_ptr<ClientData>> ClientDataList;                   //SOCKET��������

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
	std::thread _recvThread;            //���������߳�
	CInitSock _initSocket;              //SOCKET�������Ϳ���
	ClientDataList _sockClient;         //ʵ�ʶ��У��пͻ������ݣ�
	ClientDataList _sockHandle;         //�����ͻ���SOCKET����---ע�ⲻ�ɳ���FD_SIZE=128
	SOCKET _listenSock;                 //����SOCKET
	CNetServerAppDlg *_pTemp;
	std::mutex _mtx;
};

