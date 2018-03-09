/******************************************************************************************
** 文件名:   common.h
×× 主要类:   (无)
**  
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2013-11-06
** 修改人:   
** 日  期:
** 描  述:   网络通信使用的标准头文件
** ×× 主要模块说明：                    
**
** 版  本:   1.0.0
** 备  注:   
**
******************************************************************************************/
#pragma once
#include <functional>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <list>
#include <exception>
#include <pthread.h>
#include <semaphore.h>

#define __CPP_11_LIB__                        //定义C++11编译环境

const int g_recvBuf = 1024;                  //缓冲区的大小
const int g_sendBuf = 1024;
const int g_limitFd = 65535;                 //描述符的数量
const int g_port    = 9999;                  //端口号
const int g_timeOut = 20;                    //非阻塞连接超时时间

//数据发送结构体
typedef struct __SendDataBuf
{
	int Len;						//数据长度
	char SendBuf[g_sendBuf];        //数据缓冲区
}SendDataBuf,*PSendDataBuf;

typedef std::function<void(char*buf,int len)> DelegateRecv;     //标准库的function变量声明,用于网络数据的传送
typedef std::list<SendDataBuf> SendParam ;						//数据发送存储区


//错误宏函数
//错误宏函数
#define ERR_EXIT(m) \
do {\
	perror(m); \
	exit(EXIT_FAILURE); \
} while (0)

#define ERR_MSG(m) \
    do { \
        perror(m); \
    } while (0)

//SOCKET重定义
typedef int SOCKET;
//客户端数据结构体
typedef struct __ClientData
{
	SOCKET Socket;                         //网络文件描述符（SOCKERT句柄）
	char Addr[20];                        //地址
	char RecvBuf[g_recvBuf];               //接收缓冲区
    //AutoMachine _autoControl;            //自动机解析成员－客户端可以直接在类中使用---在客户端比较灵活，可以在类中直接使用
}ClientData,*PClientData;

//线程同步机制包装类
//信号类
class Sem
{
public:
	Sem()
	{
		if (sem_init(&_sem, 0, 0) != 0)
		{
			throw std::exception();
		}
	}
	~Sem()
	{
		sem_destroy(&_sem);
	}
	bool Wait()
	{
		return sem_wait(&_sem) == 0;
	}
	bool Post()
	{
		return sem_post(&_sem) == 0;
	}

private:
	sem_t _sem;
};
//锁类
class Locker
{
public:
	Locker()
	{
		if (pthread_mutex_init(&_mutex, NULL) != 0)
		{
			throw std::exception();
		}
	}
	~Locker()
	{
		pthread_mutex_destroy(&_mutex);
	}
	bool Lock()
	{
		return pthread_mutex_lock(&_mutex) == 0;
	}
	bool Unlock()
	{
		return pthread_mutex_unlock(&_mutex) == 0;
	}

private:
	pthread_mutex_t _mutex;
};

class Cond
{
public:
	Cond()
	{
		if (pthread_mutex_init(&_mutex, NULL) != 0)
		{
			throw std::exception();
		}
		if (pthread_cond_init(&_cond, NULL) != 0)
		{
			pthread_mutex_destroy(&_mutex);
			throw std::exception();
		}
	}
	~Cond()
	{
		pthread_mutex_destroy(&_mutex);
		pthread_cond_destroy(&_cond);
	}
	//处理信号监听
	bool Wait()
	{
		int ret = 0;
		pthread_mutex_lock(&_mutex);
		while (!_flag)
		{
			ret = pthread_cond_wait(&_cond, &_mutex);
		}
		
		pthread_mutex_unlock(&_mutex);

		return ret == 0;
	}
	//处理信号发送
	bool Signal()
	{
		pthread_mutex_lock(&_mutex);
		bool ret = pthread_cond_signal(&_cond);
		_flag = true;
		pthread_mutex_unlock(&_mutex);

		return ret;
	}

private:
	pthread_mutex_t _mutex;
	pthread_cond_t _cond;
	bool _flag = false;            //监听的标志位，用来处理假唤醒
};



