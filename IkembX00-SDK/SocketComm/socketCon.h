/******************************************************************************
*  文 件 名：socketCon.cpp
*  主 要 类：socketCon
*  说    明: 网络通信的实现类
*  创 建 人：fjf
*  创建时间：20013-11-05
*  修 改 人：
*  修改时间：
*****************************************************************************/
#pragma once
#include "socketInterface.h"

class socketCon:public INetSocket
{
public:
    socketCon();
    virtual ~socketCon();
public:
    virtual int DoWithData(unsigned char* buf);
};

