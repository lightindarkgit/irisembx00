/******************************************************************************************
** �ļ���:   stdWait.h
���� ��Ҫ��: StdWait
**
** Copyright (c) �пƺ�����޹�˾
** ������:   fjf
** ��  ��:   2013-12-16
** �޸���:
** ��  ��:
** ��  ��:   ��׼��״̬������װ��ͷ�ļ�
** ���� ��Ҫģ��˵����
**
** ��  ��:   1.0.0
** ��  ע:
**
******************************************************************************************/
#pragma once

#include <mutex>
#include <condition_variable>
#define FD_SETSIZE  128
class StdWait 
{
public:
	StdWait ();
	StdWait(StdWait&) = delete;                 //��ֹ�������캯��---��������ת�ƺ�������ֵ��������ô�죿
	StdWait& operator=(const StdWait&) = delete;//��ֹ��ֵ������Ϊ�˼�
	~StdWait ();
public:
	void NoticeOne();
	void NoticeAll();
	void Wait();
	void NoticeAllExit();
	void ResetState();
private:
	std::mutex _mutex;
	std::condition_variable _conVar;
	bool _flag = false;
};

