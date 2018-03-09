/******************************************************************************************
** 文件名:   CAtuoState.h
×× 主要类:   CAtuoState
**  
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2013-04-17
** 修改人:   
** 日  期:
** 描  述:   后台服务通信操作类
 *×× 主要模块说明: CAtuoState.h                               自动机类处理类                     
**
** 版  本:   1.0.0
** 备  注:   
**
*****************************************************************************************/

#pragma once

#include "initsock.h"

#define MAX_SUM   1000                               //最大和不能超过1000
const int g_constMinSum = 200                        //最小和不能小于200----------说明：除非必须，使用全局常量替代宏               

class CNetComm;//前向声明

class CAtuoState
{
public:
	CAtuoState(void);
	virtual ~CAtuoState(void);
public:
	int ParseData(char* buf, int len);	              //数据分析函数
	void MakePicData(char* frameBuf,int len);           //组装图像
	BYTE GetCheckSum(char* buf, int len);
	int ParseData(void*owner,char *buf,int len,bool);  //重载分析函数
protected:
	int _sumData;                                         //和数据s
private:
	int _sumPack;                                         //图像包的数量
	int _packageLen;                                      //每包长度
	int _picNum;                                          //图片大小
	char _bufFrame[256];                                  //GPRS网关通信帧大小：目前最大为128，扩大一倍供扩展
	static bool s_isGprs;

public:
	int CheckValue;                                       //校验和的值 ---公共变量大写驼峰                               
};
