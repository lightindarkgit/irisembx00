/******************************************************************************************
** 文件名:   stdWait.h
×× 主要类: StdWait
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2013-12-16
** 修改人:
** 日  期:
** 描  述:   标准库状态条件封装类头文件
** ×× 主要模块说明：
**
** 版  本:   1.0.0
** 备  注:
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
	StdWait(StdWait&) = delete;                 //禁止拷贝构造函数---？？？但转移函数对右值的引用怎么办？
	StdWait& operator=(const StdWait&) = delete;//禁止赋值操作是为了简化
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

