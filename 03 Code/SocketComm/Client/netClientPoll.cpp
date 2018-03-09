
/******************************************************************************************
** 文件名:   NetClientPoll.cpp
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
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "netClientPoll.h"
#include <assert.h>

NetClientPoll::NetClientPoll()
{
}

NetClientPoll::~NetClientPoll()
{
}
/*****************************************************************************
*                        接收线程函数
*  函 数 名：RecvThreadPoll
*  功    能：接收网络通信数据函数
*  说    明：
*  参    数：
*
*  返 回 值：
*  创 建 人：fjf
*  创建时间：2013-12-04
*  修 改 人：
*  修改时间：
*****************************************************************************/
void NetClientPoll::RecvThreadPoll(void *pParam)
{
	//初步的变量声明
	NetClientPoll *pTemp = static_cast<NetClientPoll*>(pParam);
	pollfd pfd;

	//初始化句柄及事件

	pfd.fd = pTemp->_socket;
	pfd.events = POLLIN | POLLRDHUP;
	pfd.revents = 0;

	int pipefd[2];
	int ret = pipe(pipefd);
	assert(-1 != ret);

	//循环操作
	while (true)
	{
		//-1为永远阻塞，0为立刻返回
		int ret = poll(&pfd,2,-1);

		if (ret < 0)
		{
			ERR_MSG("poll failure\n");
			break;
		}

		if (pfd.revents & POLLRDHUP)
		{
			ERR_MSG("server close!");
			break;
		}
		else if (pfd.revents &POLLIN)
		{
			recv(pfd.fd, pTemp->_bufRecv, sizeof(_bufRecv)-1, 0);
		}
	}

	//关闭网络通信文件描述符（句柄）
	close(pTemp->_socket);
}
/*****************************************************************************
*                        发送线程函数
*  函 数 名：SendThreadPoll
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
void NetClientPoll::SendThreadPoll(void *pParam)
{

}
/*****************************************************************************
*                        发送函数
*  函 数 名：Send
*  功    能：发送网络通信数据函数
*  说    明：
*  参    数：
*
*  返 回 值：
*  创 建 人：fjf
*  创建时间：2013-12-20
*  修 改 人：
*  修改时间：
*****************************************************************************/
int NetClientPoll::Send(char *buf, int len)
{
	return 0;
}
/*****************************************************************************
*                        创建接收线程函数
*  函 数 名：CreateRecvThread
*  功    能：创建接收线程函数
*  说    明：
*  参    数：
*
*  返 回 值：
*  创 建 人：fjf
*  创建时间：2013-12-20
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool NetClientPoll::CreateRecvThread()
{
	return false;
}
/*****************************************************************************
*                        创建发送线程函数
*  函 数 名：CreateSendThread
*  功    能：创建发送线程函数
*  说    明：
*  参    数：
*
*  返 回 值：
*  创 建 人：fjf
*  创建时间：2013-12-20
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool NetClientPoll::CreateSendThread()
{
	return false;
}
/*****************************************************************************
*                        启动工作线程函数
*  函 数 名：StartThread
*  功    能：启动接收和发送工作线程函数
*  说    明：
*  参    数：
*
*  返 回 值：
*  创 建 人：fjf
*  创建时间：2013-12-20
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool NetClientPoll::StartThread()
{
	return true;
}
