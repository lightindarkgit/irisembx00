/******************************************************************************************
** 文件名:   featureCommon.h
×× 主要类:
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2014-01-09
** 修改人:
** 日  期:
** 描  述: 特征管理通用头文件
** ×× 主要模块说明：
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
/* 说明：
 *     1、建立三层NODE数据结构：最上层为人员信息节点，中间为控制节点，最下为与顺序的特征数组索引一一映射的虹膜UUID节点。
 *     2、建立两级映射：人员ID到人员信息；人员特征ID到人员特征索引（或者说特征地址指针）
 *     3、多个人员的特征ID存放在人员信息的NODE节点中。然后在删除时，利用人员ID映射到人员详细信息，再取得左右眼特征ID，
 *        再利用特征ID的映射取得特征地址指针。然后逐级删除。
 *     4、建立ControlNode最下层节点与特征数组的一一对应内存关系。修改：2014－02－27 删除了IRISTONODE这一级的映射
 *     5、当识别动作时，使用识别成功的特征索引，使用在4中建立的一一映射的最下层节点ControlNode中的指针逐级向上查找人员信息。
 *     6、算法要求内存必须是平坦的。
 *
 ***********************************************************************************************************/
#pragma once
#include <list>
#include <map>
#include <uuid/uuid.h>
#include <memory>
#include <string.h>
#include <ctime>

#define __CPP_11_LIB__

const int g_SuperCount =  120;                           //超级管理员的数量
const int g_SuperHalf  = g_SuperCount/2;                             //数量的一半
const int g_FeaturSize = 512;                            //特征长度
enum class ControlType:short {Add,Del,Update};           //枚举类，内存中特征处理的类型
enum class IdentifyType:short{All,Left,Right,Either};    //枚举：左眼、右眼、全部、任意眼
const int g_UuidLen = 16;
//对外开放的人员结构
typedef struct  __PersonInfo
{
    uuid_t  Id;
    uuid_t  FeatureID;
    std::string CardID;       //卡号  此两个变量是在二期新增 2014-04-15
    std::string WorkSn;       //工号
    std::string Name;

    std::string Depart;
    std::string Sex;
}PeopleInfo;

typedef struct __InfoData
{
	uuid_t ud;   //特征ID
	IdentifyType  TypeControl;
	unsigned char FeatureData[512];
	PeopleInfo PeopleData;
}InfoData,*PInfoData;
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

typedef struct __UuidData
{
	__UuidData(){}
	//处理拷贝构造函数
	__UuidData(__UuidData&udd)
	{
		memmove(ud,udd.ud,g_UuidLen);
	}
	~__UuidData(){}
    //重载三个运算符
	bool operator < (const __UuidData&udd)
	{
		if (memcmp(ud,udd.ud,16))
		{
			return true;
		}

		return false;
	}
	bool operator == (const __UuidData&udd)
	{
		if (memcmp(ud,udd.ud,16))
		{
			return true;
		}

		return false;
	}

	__UuidData& operator = (const __UuidData&udd)
	{
		memmove(ud,udd.ud,16);
		return *this;
	}

	uuid_t ud;
}UUID;

//人员信息
typedef struct __PeopleNode
{
	uuid_t PeopleID;                   //人员ID
	std::string Name;                  //最大64位长度
	std::string Sex;
	std::string WorkSn;
	//2014-06-18 增加卡号、部门
	std::string CardID;
	std::string Depart;

	std::list<GUID> LeftFeature;       //右眼特征ID数组
	std::list<GUID> RightFeature;      //左眼特征ID数组
}PeopleNode,*PPeopleNode;

typedef std::map<std::string,std::shared_ptr<PeopleNode>> PeopleMap;                  //利用人员ID可以迅速查找到人员信息，并且找到其中的特征ID
typedef std::map<std::string,int>  FeatureMap;                                        //利用特征ID迅速查找到特征大数组中的索引（或者说特征的地址指针）

//中间控制数据结构体:目的是解决多个特征对应一个人员时不重复的加载人员空间---现在暂时只要ControlNode，不使用IrisToNode
typedef struct __ControlNode
{
	std::shared_ptr<PeopleNode> p;
	//std::string uuidStr;                //特征的字符串形式
	uuid_t FeatureID;                   //特征ID
}ControlNode,*PControlNode;

//其下两个部分一一对应
typedef struct __IrisToNode
{
	uuid_t FeatureID;
	ControlNode * PNode;
}IrisToNode,*PIrisToNode;

//
//特征数组--迁移到IrisData.cpp中
//unsigned char * buf[1024 * 1024 * 200];  //一个特征是512字节，左右眼共开出200M内存


