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


#include <stdlib.h>
#include <stdio.h>

#define BUF_SIZE  1024                  //缓冲区的大小
#define FD_LIMIT  65535                 //描述符的数量

//错误宏函数
#define ERR_EXIT(m) \
    do { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while (0)

//SOCKET重定义
typedef int SOCKET;
//客户端数据结构体
typedef struct ClientData
{
	SOCKET _socket;               //网络文件描述符（SOCKERT句柄）
	sockaddr_in _addr;            //地址
	char* _recvBuf;               //接收缓冲区
	char* _sendBuf;               //发送缓冲区
    //AutoMachine _autoControl;   //自动机解析成员－客户端可以直接在类中使用
};

