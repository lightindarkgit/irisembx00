
/******************************************************************************************
** 文件名:  socketInterfaceAPI.cpp
×× 主要类:  ISocketAPI
**  
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2013-11-20
** 修改人:   
** 日  期:
** 描  述:   网络通信接口类
** ×× 主要模块说明：                    
**
** 版  本:   1.0.0
** 备  注:   
**
*****************************************************************************************/
#include "socketAPI.h"
#include "socketInterfaceAPI.h"


ISocketAPI::ISocketAPI()
{
}
ISocketAPI::~ISocketAPI()
{
}

/*****************************************************************************
*                         创建网络通信实例函数
*  函 数 名：CreateInstance
*  功    能：创建网络通信实例，供外部使用
*  说    明：是否需要创建一个单实例？
*  参    数：cbr:提供的函数变量
*            cd :网络初始化参数
*  返 回 值：
*  创 建 人：fjf
*  创建时间：2013-11-21
*  修 改 人：
*  修改时间：
*****************************************************************************/

ISocketAPI* ISocketAPI::CreateInstance(DelegateRecv cbr, const ClientData& cd) throw()
{
    return new SocketAPI(cd,cbr);
}
