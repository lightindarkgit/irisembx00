
/******************************************************************************************
** 文件名:   AtuoState.cpp
×× 主要类:   CAtuoState
**  
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2013-04-17
** 修改人:   
** 日  期:
** 描  述:   串口通信数据处理类
 *×× 主要模块说明: CAtuoState.h                               串口通信数据处理类                    
**
** 版  本:   1.0.0
** 备  注:   
**
*****************************************************************************************/

#include "StdAfx.h"
#include "AtuoState.h"
#include "NetComm.h"
#include "NetClient.h"
#include "comm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

char g_comBuf[DATARECVBUF];      //全局接收缓冲区

CAtuoState::CAtuoState(void)
: _curState(0)
, _nextState(0)
{
	memset(g_comBuf,0,DATARECVBUF);//初始化
}

CAtuoState::~CAtuoState(void)
{
}

/*****************************************************************************
*                         分析函数
*  函 数 名：ParseData
*  功    能：接收并分析网络数据
*  说    明：
*  参    数：buf:网络缓冲区数据
*            len:接收到数据长度
*  返 回 值：
*  创 建 人：fjf
*  创建时间：2009-09-18
*  修 改 人：
*  修改时间：
*****************************************************************************/

int CAtuoState::ParseData(char* buf, int len)
{
	char temp_buf[1024];
	memmove(temp_buf,buf,len);

	for (int num = 0;num < len;num++)
	{
		//确定状态机当前状态，判断是否完成一帧
		_nextState =	RouteCircle(temp_buf[num],_curState);
		IsFullFrame();
		_curState = _nextState;
	}

	return 0;
}

/*****************************************************************************
*                        帧组成图像数据
*  函 数 名：MakePicData
*  功    能：组装图像
*  说    明：
*  参    数： frameBuf;一包数据
*             len:一包长度
*  返 回 值：
*  创 建 人：fjf
*  创建时间：2009-10-16
*  修 改 人：
*  修改时间：
*****************************************************************************/

void CAtuoState::MakePicData(char* frameBuf,int len)
{
	//根据包编号组图
	//图缓冲区
	char * p_temp       = NULL;
	char pack_id[2]     = {0};
	static int s_pack_num = 0;


	short p_id = *((short*)pack_id);//包号

}




