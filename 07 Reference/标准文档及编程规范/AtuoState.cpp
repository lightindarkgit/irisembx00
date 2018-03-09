
/******************************************************************************************
** �ļ���:   AtuoState.cpp
���� ��Ҫ��:   CAtuoState
**  
** Copyright (c) �пƺ�����޹�˾
** ������:   fjf
** ��  ��:   2013-04-17
** �޸���:   
** ��  ��:
** ��  ��:   ����ͨ�����ݴ�����
 *���� ��Ҫģ��˵��: CAtuoState.h                               ����ͨ�����ݴ�����                    
**
** ��  ��:   1.0.0
** ��  ע:   
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

char g_comBuf[DATARECVBUF];      //ȫ�ֽ��ջ�����

CAtuoState::CAtuoState(void)
: _curState(0)
, _nextState(0)
{
	memset(g_comBuf,0,DATARECVBUF);//��ʼ��
}

CAtuoState::~CAtuoState(void)
{
}

/*****************************************************************************
*                         ��������
*  �� �� ����ParseData
*  ��    �ܣ����ղ�������������
*  ˵    ����
*  ��    ����buf:���绺��������
*            len:���յ����ݳ���
*  �� �� ֵ��
*  �� �� �ˣ�fjf
*  ����ʱ�䣺2009-09-18
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/

int CAtuoState::ParseData(char* buf, int len)
{
	char temp_buf[1024];
	memmove(temp_buf,buf,len);

	for (int num = 0;num < len;num++)
	{
		//ȷ��״̬����ǰ״̬���ж��Ƿ����һ֡
		_nextState =	RouteCircle(temp_buf[num],_curState);
		IsFullFrame();
		_curState = _nextState;
	}

	return 0;
}

/*****************************************************************************
*                        ֡���ͼ������
*  �� �� ����MakePicData
*  ��    �ܣ���װͼ��
*  ˵    ����
*  ��    ���� frameBuf;һ������
*             len:һ������
*  �� �� ֵ��
*  �� �� �ˣ�fjf
*  ����ʱ�䣺2009-10-16
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/

void CAtuoState::MakePicData(char* frameBuf,int len)
{
	//���ݰ������ͼ
	//ͼ������
	char * p_temp       = NULL;
	char pack_id[2]     = {0};
	static int s_pack_num = 0;


	short p_id = *((short*)pack_id);//����

}




