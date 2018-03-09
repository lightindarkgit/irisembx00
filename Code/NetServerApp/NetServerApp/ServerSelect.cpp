#include "stdafx.h"
#include "ServerSelect.h"
#include "EncodedNetData.h"
#include "../../Common/Exectime.h"

ServerSelect::ServerSelect()
{
	Exectime etm(__FUNCTION__);
	FD_ZERO(&_fd);
}


ServerSelect::~ServerSelect()
{
	Exectime etm(__FUNCTION__);
}

int ServerSelect::Send(char *buf, int len)
{
	Exectime etm(__FUNCTION__);
	int iRet = -1;
	std::lock_guard<std::mutex> lckData(_mtx);
	for (std::shared_ptr<ClientData> &pcd : _sockClient)
	{
		//最后应用再增加判断方法---包括在结构体内增加判断的标记
		if (pcd->Sip != "")
		{
			int iRet = send(pcd->Socket, buf, len, 0);
			if (SOCKET_ERROR == iRet)
			{
				//return WSAGetLastError();
			}
		}
	}
	return iRet;
}

CWnd* _pWnd;
bool ServerSelect::CreateRecvThread()
{
	Exectime etm(__FUNCTION__);
	bool bRet = true;
	try
	{
		_pWnd = AfxGetMainWnd();
		this->_recvThread = std::thread([&](int& count, std::string s)mutable throw()->bool{
				//处理select服务端
				SOCKET temp;
				FD_SET(_listenSock, &_fd);
				while (true)
				{
				//每次更新FD_SET
				fd_set fdRead = _fd;
				int iRet = select(0, &fdRead, NULL, NULL, NULL);

				//处理接收数据
				if (iRet > 0)
				{
				//这是用加锁的方法还是选择的方法===直接使用链表得加锁，粒度太大
				for (int num = 0; num < _fd.fd_count; num++)
				{
				if (FD_ISSET(_fd.fd_array[num], &fdRead))
				{
				if (_listenSock == _fd.fd_array[num])
				{
				SOCKADDR_IN sinClient;
				int lenClient = sizeof(sinClient);
				temp = accept(_listenSock, (SOCKADDR*)&sinClient, &lenClient);
				if (SOCKET_ERROR == temp)
				{
					continue;
				}

				//处理远端客户连接并保存入链表供处理和使用
				std::shared_ptr<ClientData> pcd = GetHandle();
				pcd->Socket = temp;
				pcd->Port = ntohs(sinClient.sin_port);
				pcd->Sip = inet_ntoa(sinClient.sin_addr);
				AddClientHandle(pcd);
				}
				else
				{
					std::shared_ptr<ClientData> pcd = GetClientHandle(_fd.fd_array[num]);
					int count = recv(pcd->Socket, pcd->RecvBuf, g_recvBuf, 0);

					//其下代码为测试使用
					if (count > 4)
					{
						PReKeepAliveCmd prkac = (PReKeepAliveCmd)pcd->RecvBuf;
						if (ACK_KEEP_ALIVE == prkac->RequestCode)
						{

							_pWnd->PostMessage(WM_COUNT_MESSAGE, NULL, NULL);
						}
					}
					//pcd->AutoControl.ParseData(pcd->RecvBuf,count);//注释掉解析处理
				}

				}
				}

				}
				}

		return bRet;

		}, std::ref(_countSocket), "start thread");

		this->_recvThread.detach();
	}
	catch (...)
	{
		bRet = false;
	}


	return bRet;
}

bool ServerSelect::CreateSendThread()
{
	Exectime etm(__FUNCTION__);
	bool bRet = false;
	return bRet;
}
/*****************************************************************************
 *                       重置监听句柄
 *  函 数 名：SetReadSocket
 *  功    能：重新对内核处理过的句柄进行恢复
 *  说    明：
 *  参    数：无
 *  返 回 值：无
 *  创 建 人：fjf
 *  创建时间：2014-03-13
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void ServerSelect::SetReadSocket()
{
	Exectime etm(__FUNCTION__);
	FD_ZERO(&_fd);
	FD_SET(_listenSock, &_fd);  //每次重置监听
	for (std::shared_ptr<ClientData>&pcd : _sockClient)
	{
		FD_SET(pcd->Socket, &_fd);
	}
}

bool ServerSelect::CreateAcceptThread()
{
	Exectime etm(__FUNCTION__);
	bool bRet = true;
	this->_acceptThread = std::thread([&]()mutable throw()->bool{
			//处理接收队列
			SOCKET temp;
			while (true)
			{
			//处理监听
			SOCKADDR_IN sinClient;
			int lenClient = sizeof(sinClient);
			temp = accept(_listenSock, (SOCKADDR*)&sinClient, &lenClient);

			//处理远端客户连接并保存入链表供处理和使用
			std::shared_ptr<ClientData> pcd = GetHandle();
			pcd->Socket = temp;
			pcd->Port = ntohs(sinClient.sin_port);
			pcd->Sip = inet_ntoa(sinClient.sin_addr);
			AddClientHandle(pcd);
			SetReadSocket();
			}
			return bRet;
			});
	return bRet;
}

std::shared_ptr<ClientData> ServerSelect::GetClientHandle(const SOCKET &s)
{
	Exectime etm(__FUNCTION__);
	std::lock_guard<std::mutex> lckData(_mtx);
	for (std::shared_ptr<ClientData> &pcd : _sockClient)
	{
		if (s == pcd->Socket)
		{
			return pcd;
		}
	}
	return nullptr;
}
std::shared_ptr<ClientData> ServerSelect::GetHandle()
{
	Exectime etm(__FUNCTION__);
	if (_sockHandle.size() < 1)
	{
		CreateClientHandle();
	}

	std::lock_guard<std::mutex> lckData(_mtx);
	std::shared_ptr<ClientData> pTemp = _sockHandle.front();
	_sockHandle.pop_front();
	return pTemp;
}
bool ServerSelect::PutHandle(std::shared_ptr<ClientData>& p)
{
	Exectime etm(__FUNCTION__);
	bool bRet = true;
	//std::lock_guard<std::mutex> lckData(_mtx);//其上层使用已经加锁
	_sockHandle.push_back(p);
	return bRet;
}
bool ServerSelect::DeleteClientHandle(const SOCKET& s)
{
	Exectime etm(__FUNCTION__);
	bool bRet = false;
	std::lock_guard<std::mutex> lckData(_mtx);
	for (std::shared_ptr<ClientData> &pcd : _sockClient)
	{
		if (s == pcd->Socket)
		{
			pcd->Port = 0;
			pcd->Socket = 0;
			pcd->Sip = "";
			memset(&pcd->AutoControl, 0, sizeof(AutoMachine));

			PutHandle(pcd);           //回收智能指针
			_sockClient.remove(pcd);  //删除活动队列中的客户端
			SetReadSocket();          //重置监听队列
			break;
		}
	}
	return bRet;
}
bool ServerSelect::AddClientHandle(std::shared_ptr<ClientData> &pcd)
{
	Exectime etm(__FUNCTION__);
	bool bRet = true;
	std::lock_guard<std::mutex> lckData(_mtx);
	_sockClient.push_back(pcd);
	SetReadSocket();

	return bRet;
}
bool ServerSelect::CreateClientHandle()
{
	Exectime etm(__FUNCTION__);
	bool bRet = true;
	std::lock_guard<std::mutex> lckData(_mtx);
	for (int count = 0; count < 6; count++)
	{
		std::shared_ptr<ClientData> pTemp = std::make_shared<ClientData>();
		if (nullptr == pTemp)
		{
			bRet = false;
		}
		_sockHandle.push_back(pTemp);
	}

	return bRet;
}
void ServerSelect::StartThread(void)
{
	Exectime etm(__FUNCTION__);
	//this->CreateAcceptThread();
	this->CreateRecvThread();
	this->CreateSendThread();
}

