/******************************************************************************************
** 文件名:   UuidControl.h
×× 主要类:   UuidControl
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2014-01-19
** 修改人:
** 日  期:
** 描  述: uuid类
** ×× 主要模块说明：
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/

#pragma once
#include <string>
#include "featureCommon.h"

class UuidControl
{
public:
	UuidControl();
	virtual ~UuidControl();
public:
	std::string GetUUIDString();
	void GetUUID(uuid_t ud);
	std::string UUIDToString(uuid_t ud);
    bool StringToUUIDCharArray(const std::string &sGuid,GUID & guid);

	//UUID自带的转字符串和反向解析
	void UuidUnparse(const uuid_t,char * ud);       //转成字符串
    int  UuidParse(const char * udin,uuid_t& ud);    //字符串转回数组
	std::string StringFromUuid(uuid_t ud);
    int UuidFromString(const std::string &sGuid,uuid_t& ud);
	void CopyUUID(uuid_t dstUd,const uuid_t srcUd);
	int CompareUUID(const uuid_t ud1,const uuid_t ud2);
private:
    GUID gd;
};


