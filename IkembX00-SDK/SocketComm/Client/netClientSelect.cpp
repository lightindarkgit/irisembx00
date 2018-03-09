
/******************************************************************************************
 ** 文件名:   NetClientSelect.cpp
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

#include <thread>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <errno.h>
#include <stdio.h>
#include <string>
#include "netClientSelect.h"
#include <iostream>
#include "../../Common/Logger.h"
#include "../../Common/Exectime.h"


NetClientSelect::NetClientSelect()
{
	Exectime etm(__FUNCTION__);
}
NetClientSelect::~NetClientSelect()
{
	Exectime etm(__FUNCTION__);
	if (_recvThread.get_id() != std::thread::id())
	{
		_recvThread.join();
	}
}

/*****************************************************************************
 *                        网络连接函数
 *  函 数 名：Connect
 *  功    能：连接服务器函数
 *  说    明：
 *  参    数：
 *           
 *  返 回 值：
 *  创 建 人：fjf
 *  创建时间：2013-11-29
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/

//bool NetClientSelect::Connect(int socketFd, struct sockaddr*serverAddr, int addrLen)
//{
//	return NetClient::Connect(socketFd,serverAddr,addrLen);
//}

/*****************************************************************************
 *                        发送函数
 *  函 数 名：Send
 *  功    能：网络通信发送函数
 *  说    明：
 *  参    数：
 *           
 *  返 回 值：
 *  创 建 人：fjf
 *  创建时间：2013-12-04
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int NetClientSelect::Send(char *buf,int len)
{
	Exectime etm(__FUNCTION__);
	int ndex = 0;

	while(ndex < len)
	{
		errno = 0;
        LOG_INFO("this->_socket=%p,buf=%p,ndex=%d,len-ndex = %d",this->_socket,buf,ndex,len - ndex);
        int cnt = send(this->_socket,buf + ndex,len - ndex,MSG_NOSIGNAL);
        LOG_INFO("netclientSelect send over");

		//处理安全的发送
        //if (cnt <= 0)
        if (cnt < 0)

		{
            if(errno == EINTR )
				continue;
			LOG_ERROR("socket send error:   %d, fd: %d, len: %d",errno,this->_socket,len - ndex);
			this->_socket = -1;
			return -1;
		}
		else
		{
			ndex += cnt;
		}
	}

	return ndex;
}

/*****************************************************************************
 *                        发送线程创建函数
 *  函 数 名：CreateSendThread
 *  功    能：发送线程创建函数
 *  说    明：
 *  参    数：
 *           
 *  返 回 值：
 *  创 建 人：fjf
 *  创建时间：2013-12-04
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/


bool NetClientSelect::CreateSendThread()
{
	Exectime etm(__FUNCTION__);
	pthread_t id;
	int result = pthread_create(&id, NULL, SendThreadSelect, static_cast<void*>(this));
	if (0 != result)
	{
		//错误和异常的控制
		//char *msg = strerror(errno);
		LOG_ERROR("create sned thread failure! code: %d, %s",errno,strerror(errno));

		return false;
	}

	//分离失败
	if (pthread_detach(id))
	{
		LOG_ERROR("detach failure! code: %d, %s",errno,strerror(errno));
		return false;
	}

	return true;
}
SendParam &NetClientSelect::GetSendBuf()
{
	Exectime etm(__FUNCTION__);
	return _sendData; 
}
SOCKET NetClientSelect::GetSocket()
{
	Exectime etm(__FUNCTION__);
	return _socket;
}
/*****************************************************************************
 *                        网络连接函数
 *  函 数 名：CreateSendThreadStd
 *  功    能：创建发送线程标准库函数
 *  说    明：
 *  参    数：
 *           
 *  返 回 值：
 *  创 建 人：fjf
 *  创建时间：2013-12-04
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/

bool NetClientSelect::CreateSendThreadStd()
{
	Exectime etm(__FUNCTION__);
	try
	{
		//处理标准库多线程的lambda线程函数
		_sendThread = std::thread([&]{
				while (true)
				{   
				ConWait.Wait();
				SendDataBuf sdb;
				if (QueueData.try_pop(sdb))
				{
                send(GetSocket(), sdb.SendBuf, sdb.Len, MSG_NOSIGNAL);
				}
				}
				});
		_sendThread.detach();
	}
	catch (std::exception const &e)
	{
		return false;
	}

	return true;

}

/*****************************************************************************
 *                        线程发送函数
 *  函 数 名：SendThread
 *  功    能：发送线程函数
 *  说    明：
 *  参    数：
 *           
 *  返 回 值：
 *  创 建 人：fjf
 *  创建时间：2013-12-04
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/

void* NetClientSelect::SendThreadSelect(void*pParam)
{
	Exectime etm(__FUNCTION__);
	//此处使用封装的信号和锁类以及信号量类
	while (true)
	{
		NetClientSelect *pTemp = (NetClientSelect*)pParam;
		pTemp->ConWait.Wait();

		if (pTemp->_sendData.size() > 0)
		{
			//使用C++11
			for (SendDataBuf &sdb :pTemp->GetSendBuf())
			{
                send(pTemp->_socket,sdb.SendBuf,sdb.Len,MSG_NOSIGNAL);
			}

		}
	}

	return (void*)0;
}
/*****************************************************************************
 *                        线程控制函数
 *  函 数 名：StartThread
 *  功    能：创建并启动网络通信线程函数
 *  说    明：
 *  参    数：
 *           
 *  返 回 值：
 *  创 建 人：fjf
 *  创建时间：2013-11-29
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/

bool NetClientSelect::StartThread()
{
	Exectime etm(__FUNCTION__);
	bool bRet = true;
	//是否使用C＋＋11线程库
	//2014－06－04 注释掉发送线程的原因是在断开网络连接反复重连的情况下，如果一直连接不上，会产生大量的垃圾线程引起
	//整个进程崩溃
	//这个问题其实在INIT函数里进行了返回值的判断控制，但没有提交
	//真正使用时：在close函数接口里除了关闭SOCKET外，还要提供一个NOTICEONE函数，（在发送前判断队列是否为空，空则退出）
	//让发送线程退出
#ifndef __CPP_11_LIB__
	//this->CreateSendThread();
	this->CreateRecvThread();
#else
	this->CreateRecvThreadStd();
	//this->CreateSendThreadStd();
#endif

	return bRet;
}

/*****************************************************************************
 *                        创建标准库线程函数
 *  函 数 名：CreateRecvThreadStd
 *  功    能：创建C＋＋11标准库接收线程函数
 *  说    明：
 *  参    数：
 *           
 *  返 回 值：
 *  创 建 人：fjf
 *  创建时间：2013-12-04
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/


bool NetClientSelect::CreateRecvThreadStd()
{
	Exectime etm(__FUNCTION__);
	_recvThread = std::thread(WorkThreadStd,this);
	//_recvThread.detach();//2014-06-09采用join方式替代分离方式

	return true;
}

/*****************************************************************************
 *                        创建线程接收函数函数
 *  函 数 名：CreateRecvThread
 *  功    能：创建线程接收函数
 *  说    明：
 *  参    数：
 *           
 *  返 回 值：
 *  创 建 人：fjf
 *  创建时间：2013-12-04
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/


bool NetClientSelect::CreateRecvThread()
{
	Exectime etm(__FUNCTION__);
	pthread_t id;
	int result = pthread_create(&id, NULL, WorkThread, this);
	if (0 != result)
	{
		//错误和异常的控制
		//char *msg = strerror(errno);
		LOG_ERROR("create recv thread failure! code: %d, %s",errno,strerror(errno));

		return false;
	}

	//分离失败
	if (pthread_detach(id))
	{
		LOG_ERROR("detach failure! code: %d, %s",errno,strerror(errno));
		return false;
	}

	return true;
}

/*****************************************************************************
 *                        LINUX线程函数
 *  函 数 名：WorkThread
 *  功    能：LINUX线程使用的函数
 *  说    明：
 *  参    数：
 *           
 *  返 回 值：
 *  创 建 人：fjf
 *  创建时间：2013-11-22
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/

void *NetClientSelect::WorkThread(void *pParam)
{
	Exectime etm(__FUNCTION__);
	NetClientSelect* pTemp = static_cast<NetClientSelect*>(pParam);
	RecvThreadSelect(pTemp);

	return nullptr;
}

/*****************************************************************************
 *                        标准库提供的线程函数
 *  函 数 名：WorkThreadStd
 *  功    能：使用标准库Thread需要的线程函数
 *  说    明：
 *  参    数：
 *           
 *  返 回 值：
 *  创 建 人：fjf
 *  创建时间：2013-11-22
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/

void NetClientSelect::WorkThreadStd(NetClientSelect *ncs)
{
	Exectime etm(__FUNCTION__);
	RecvThreadSelect(ncs);
}

/*****************************************************************************
 *                        公共的线程函数
 *  函 数 名：ThreadSelect
 *  功    能：使用标准库和LINUX线程库使用的公共的线程函数
 *  说    明：
 *  参    数：
 *           
 *  返 回 值：
 *  创 建 人：fjf
 *  创建时间：2013-12-03
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/


void NetClientSelect::RecvThreadSelect(NetClientSelect *ncs)
{
	Exectime etm(__FUNCTION__);
	//处理监控查询的句柄集合
	fd_set socketSet;
	FD_ZERO(&socketSet);

	int ready;
	int maxFd = ncs->_socket;

	//等待退出信号
	while (ncs->_isQuit)
	{
		FD_SET(ncs->_socket,&socketSet);
		struct timeval timeout;
		timeout.tv_sec = 2;   //设置两秒
		timeout.tv_usec = 0;
		//timeval中的两个成员均为0则立刻返回，直接给一个NULL，则永远阻塞,直到某个描述符就绪
		ready = select(maxFd + 1,&socketSet,NULL,NULL,&timeout);
		//std::cout<<"select 2 second circle..."<<std::endl;

		//错误
		if (-1 == ready)
		{
			if(errno == EINTR || errno == EAGAIN)
			{
				continue;
			}

			//优雅的关闭SOCKET
			shutdown(ncs->_socket,1);
			close(ncs->_socket);
			ncs->_socket = -1;
			LOG_ERROR("select operate error,return -1. code: %d",errno);
			break;
		}

		//超时未发现就绪文件描述符
		if (0 == ready)
		{
			LOG_INFO("select operate return 0. code: %d",errno);
			continue;
		}

		if (FD_ISSET(ncs->_socket,&socketSet))
		{
            int ret = recv(ncs->_socket, ncs->_bufRecv, g_recvBuf, MSG_NOSIGNAL);

			//此处需增加对异常的处理及不同错误的处理情况
			if (0 < ret)
			{
				//处理得到的数据
				ncs->_dr(ncs->_bufRecv, ret);
			}
			else if (0 == ret)
			{
				//优雅的关闭SOCKET
				shutdown(ncs->_socket,1);
				close(ncs->_socket);
				ncs->_socket = -1;
				//ERR_MSG("scoket closed");
				LOG_INFO("select operate scoket closed. code: %d",errno);
				break;
			}
			else if (-1 == ret)
			{
				shutdown(ncs->_socket,1);
				close(ncs->_socket);
				ncs->_socket = -1;
				//ERR_MSG("recv error");
				LOG_ERROR("select operate recv error. code: %d",errno);
				break;
			}
		}
	}

	//退出后关闭网络通信句柄
	LOG_INFO("select operate close and exit thread...");
	close(ncs->_socket);
	ncs->_socket = -1;

}


