/*****************************************************************************
** 文 件 名：QIdentify.h
** 主 要 类：QIdentify
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：刘中昌
** 创建时间：20013-10-15
**
** 修 改 人：
** 修改时间：
** 描  述:   识别模块
** 主要模块说明: 识别类 ：识别过程中需要的状态、回调等信息
**
** 版  本:   1.0.0
** 备  注:  该类已经取消
**
*****************************************************************************/
#ifndef QIDENTIFY_H
#define QIDENTIFY_H
enum PersonType
{
    Admin,
    Other
};

class QIdentify
{
public:
    QIdentify(PersonType personType = Other);
    ~QIdentify();

    int ReAcquireIdenDevice();		// 获取识别设备
    int startIdentify();
    void stopIdentify();

private:
    // 函数调用处理
    int InitIdenModule ();			// 初始化识别模块
    void TerminateIdenModule ();		// 退出识别模块
    int AcquireIdenDevice();			// 获取识别设备
    int ReleaseIdenDevice();			// 释放识别设备
    int SetPerformDevice();			// 设置设备的识别参数
    int QueryDevice();				// 获取设备信息

private:
    PersonType _personType;
};

#endif // QIDENTIFY_H
