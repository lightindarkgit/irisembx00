/******************************************************************************************
** 文件名:   IrisManager.h
×× 主要类:   IrisManager
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2013-12-30
** 修改人:
** 日  期:
** 描  述: 虹膜特征管理类
** ×× 主要模块说明：
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/

#pragma once

#include "irisData.h"
#include <list>
#include "dbTable.h"

class IrisManager
{
public:
	IrisManager();
	~IrisManager();
public:
	//the first
	bool GetIrisFeature(IdentifyType flag,unsigned char*&pIrisData,int &size);                         //获取虹膜特征，分左右眼获取
	bool SetIrisFeature(InfoData &id,IdentifyType flag,unsigned char *pIrisData,int size);             //注册特征写入并保存
	bool SelectInfoFromFeature(int index,PeopleInfo& name,IdentifyType type);                          //根据特征序号获得人员信息
	bool GetPeopleInfoByIrisFeature(uuid_t irisuuid,PeopleInfo& personinfo);
	bool GetAlgorithm(void);
public:
	//特征的管理操作，增、删、改、查
	int AddFeatureData(InfoData& info/*,IdentifyType flag,unsigned char *pIrisData*/);
	int UpdateFeatureData(InfoData& info,IdentifyType flag,unsigned char *pIrisData,int size);
    int UpdatePersonInfo(InfoData& info);
    int DeletePersonAndFeatureData(InfoData& info);
    int DeleteFeatureData(InfoData& info,IdentifyType flag);
	//select中的特征指针和数量都是回传给外部使用的，由外部分配和释放
	int SelectFeatureData(uuid_t ud,IdentifyType flag,unsigned char *&pIrisData);                //查询指定特征
public:
	// Add by lius 为大军提供,通过人员姓名获取特征数量
    int	GetPersonFeatureCount(std::string personname,int& featureleftcnt,int& featurerightcnt);

public:
	//处理特征的合并、删除和修改
	bool FeatureDataDoWith();
	std::string GetStringFromChar(unsigned char* unc);                                          //UUID从数组到字符串
public:
	bool LoadData(void);                //显示加载特征及相关数据
	void ReleaseFeature(void);          //通知特征内存解锁----显示释放所有数据
	void LockData(void);                //显示的锁和解锁函数
	void UnlockData(void);
	void Init(void);
public:
	//数据库的操作管理----2014-2-13数据库的管理从数据层转移到管理层
	int ReadFeature();
	int UpdateFeature();
	int DeleteFeature();
	int SelectFeaure();
public:
	//替代MAP[]操作函数
	std::shared_ptr<PeopleNode> FindKeyPeople(std::string sKey);
	std::shared_ptr<PeopleNode> FindPeopleByName(std::string personname);
public:
	//管理员特征的处理
	bool GetSuperFeature(IdentifyType flag,unsigned char*&pIrisData,int &size);
    bool LoadSuperData(void);
    bool ReleaseSuperData(void);
private:
	IrisData _irisData;
	UuidControl _uuid;
	std::list<InfoData> _dataBuf;                   //实时更新的数据进入到特征时的缓冲
	bool _isChange;                                 //是否有数据可以处理
	//处理两级映射
	PeopleMap _mapPeople;                           //人员ID和人员信息映射
	FeatureMap _mapFeature;                         //人员特征ID和特征ID在内存中的索引映射表
private:
	std::vector<ViewPersonIris> _vpData;            //从数据库加载的人员特征数据
};
