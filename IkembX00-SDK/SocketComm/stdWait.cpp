/******************************************************************************************
 ** 文件名:   stdWait.cpp
 ×× 主要类: StdWait
 **
 ** Copyright (c) 中科虹霸有限公司
 ** 创建人:   fjf
 ** 日  期:   2013-12-16
 ** 修改人:
 ** 日  期:
 ** 描  述:   标准库状态条件封装类文件
 ** ×× 主要模块说明：
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 ******************************************************************************************/
#include "stdWait.h"
#include "../Common/Exectime.h"

StdWait::StdWait()
{
	Exectime etm(__FUNCTION__);
}

StdWait ::~StdWait()
{
	Exectime etm(__FUNCTION__);
}
/*****************************************************************************
 *                        唤醒线程函数
 *  函 数 名：NoticeAll
 *  功    能：唤醒所有等待此条件的线程
 *  说    明：
 *  参    数：无
 *  返 回 值：无
 *  创 建 人：fjf
 *  创建时间：2013-12-17
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void StdWait::NoticeAll()
{
	Exectime etm(__FUNCTION__);
	std::unique_lock<std::mutex> uniqueLocker(_mutex);
	this->_flag = true;
	this->_conVar.notify_all();
}
/*****************************************************************************
 *                        唤醒线程函数
 *  函 数 名：NoticeOne
 *  功    能：唤醒等待况态条件的一个线程
 *  说    明：
 *  参    数：无
 *  返 回 值：无
 *  创 建 人：fjf
 *  创建时间：2013-12-17
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void StdWait::NoticeOne()
{
	Exectime etm(__FUNCTION__);
	std::unique_lock<std::mutex> uniqueLocker(_mutex);

	//锁住后对标志变量重新赋值，防止假唤醒操作
	this->_flag = true;
	this->_conVar.notify_one();
}
/*****************************************************************************
 *                        线程退出通知函数
 *  函 数 名：NoticeAllExit
 *  功    能：通知所有等待函数此线程退出
 *  说    明：
 *  参    数：无
 *  返 回 值：无
 *  创 建 人：fjf
 *  创建时间：2013-12-17
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void StdWait::NoticeAllExit()
{
	Exectime etm(__FUNCTION__);
	//	std::unique_lock<std::mutex> uniqueLocker(_mutex);
	//	std::notify_all_at_thread_exit(this->_conVar,std::move(uniqueLocker));
	//	this->_flag = true;
}
/*****************************************************************************
 *                        线程等待函数
 *  函 数 名：Wait
 *  功    能：封装的C++11的线程等待函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：fjf
 *  创建时间：2013-12-17
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void StdWait::Wait()
{
	Exectime etm(__FUNCTION__);
	std::unique_lock<std::mutex> uniqueLocker(_mutex);

	//防止假唤醒，但会影响效率
	while (!this->_flag)
	{
		this->_conVar.wait(uniqueLocker);
	}
}
/*****************************************************************************
 *                        线程重置函数
 *  函 数 名：ResetState
 *  功    能：线程标志位重置函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：fjf
 *  创建时间：2013-12-17
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void StdWait::ResetState()
{
	Exectime etm(__FUNCTION__);
	//不在Wait成功能控制此标志，目的是防止对假唤醒判断的冲击
	std::unique_lock<std::mutex> uniqueLocker(_mutex);
	this->_flag = false;
}

