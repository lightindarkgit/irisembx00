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
#include <uuid/uuid.h>
#include <memory>
#include <stdio.h>
#include <string.h>
//#include "featureCommon.h"

/////////////////////////////////////////////////////////
//下面两人个结构体和联合体用来处理数组到结构体的变化
typedef struct __UUID
{
    unsigned long Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
} UUIDD;
typedef union __GUID
{
	uuid_t ud;
	UUIDD gd;
}GUID;


class UuidControl
{
public:
	UuidControl();
	virtual ~UuidControl();
public:
	std::string GetUUIDString();
	void GetUUID(uuid_t ud);
	std::string UUIDToPrintString(uuid_t ud);
	std::string UUIDToString(uuid_t ud);
	bool StringToUUIDCharArray(const std::string &strUUID,uuid_t  ud);
private:
	// GUID gd;
};


