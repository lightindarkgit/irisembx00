/******************************************************************************************
 ** 文件名:   IrisManager.cpp
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

#include "irisManager.h"
#include "../../Common/Exectime.h"
#include "../../Common/Logger.h"

IrisManager::IrisManager():_isChange(false)
{
	Exectime etm(__FUNCTION__);

}
IrisManager::~IrisManager()
{
	Exectime etm(__FUNCTION__);

}
/*****************************************************************************
 *                         初始化加载人员及特征数据
 *  函 数 名：Init
 *  功    能：初始化
 *  说    明：
 *  参    数：无
 *  返 回 值：是否成功
 *  创 建 人：fjf
 *  创建时间：2014-03-07
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IrisManager::Init(void)
{
	Exectime etm(__FUNCTION__);
	//加载超级用户和一般用户的特征及数据
    this->LoadSuperData();
	this->LoadData();
}
/*****************************************************************************
 *                         获得特征数据
 *  函 数 名：GetIrisFeature
 *  功    能：获取相应的数据
 *  说    明：
 *  参    数：flag:特征标记
 *           pIrisData :特征缓冲区
 *           size:特征数量
 *  返 回 值：是否成功
 *  创 建 人：fjf
 *  创建时间：2014-01-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool IrisManager::GetIrisFeature(IdentifyType flag,unsigned char *&pIrisData,int& size)
{
	Exectime etm(__FUNCTION__);
	bool bRet = false;
	bRet =	this->_irisData.GetIrisFeature(flag,pIrisData,size);
	return bRet;
}

/*****************************************************************************
 *                         释放特征数据的锁操作
 *  函 数 名：ReleaseFeature
 *  功    能：获取相应的数据
 *  说    明：
 *  参    数：
 *  返 回 值：无
 *  创 建 人：fjf
 *  创建时间：2014-01-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/

void IrisManager::ReleaseFeature(void)
{
	Exectime etm(__FUNCTION__);
	std::lock_guard<std::mutex> lk(this->_irisData._dataMutex);
	this->_mapPeople.clear();                                    //清除人员映射
	this->_irisData.ReleaseFeature();
}
/*****************************************************************************
 *                         查找指定映射表中的数据
 *  函 数 名：FindKeyPeople
 *  功    能：查找MAP中相应的数据
 *  说    明：取代MAP[].如果使用[]会在没有键值的情况下插入一个默认值，增加无用数据
 *  参    数：
 *  返 回 值：指针
 *  创 建 人：fjf
 *  创建时间：2014-05-04
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
std::shared_ptr<PeopleNode> IrisManager::FindKeyPeople(std::string sKey)
{
	Exectime etm(__FUNCTION__);
	PeopleMap::iterator it = _mapPeople.find(sKey);
	if (_mapPeople.end() != it)
	{
		return it->second;
	}

	return NULL;
}
/*****************************************************************************
 *                         批量添加人员特征
 *  函 数 名：SetIrisFeature
 *  功    能：批量的增加人员的特征到内存缓冲
 *  说    明：
 *  参    数：id:人员信息指针（人员数据）
 *  			flag:特征类型
 *  			pIrisData:特征缓冲区
 *  			size:特征数量
 *  返 回 值：是否成功
 *  创 建 人：fjf
 *  创建时间：2014-01-14
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool IrisManager::SetIrisFeature(InfoData &id,IdentifyType flag,unsigned char* pIrisData,int size)
{
	Exectime etm(__FUNCTION__);
	//数据库接口写好后，FOR循环移到loaddata函数中，Add函数也调用这个函数来操作
	//for(int len = 0;len < size;len++)
	//{
	//创建人员数据---注意判断一人多个特征
	//std::string sKey = _uuid.UUIDToString(id[len].PeopleData.id);
	std::string sKey = _uuid.StringFromUuid(id.PeopleData.Id);
	if (NULL == FindKeyPeople(sKey))
	{
		std::shared_ptr<PeopleNode> pTemp = std::make_shared<PeopleNode>();
		pTemp->Name = id.PeopleData.Name;//id[len].PeopleData.name;
		pTemp->CardID = id.PeopleData.CardID;
		pTemp->WorkSn = id.PeopleData.WorkSn;
		pTemp->Depart = id.PeopleData.Depart;
		memmove(pTemp->PeopleID,id.PeopleData.Id,g_UuidLen);
		_irisData.InserPersonData(pTemp);//add ---remove


		//创建人员ID到人员信息映射MAP
		this->_mapPeople[sKey] = pTemp;
		//_irisData.InsertFeatureData(id[len].ud,pTemp,flag,pIrisData+len*512);
		_irisData.InsertFeatureData(id.ud,pTemp,flag,pIrisData);

	}
	else
	{
		//此处可替换_mapPeople[sKey],但前面进行了判断，所以此处不会出现误插入
		_irisData.InsertFeatureData(id.ud,_mapPeople[sKey],flag,pIrisData);
	}
	//}

	return true;
}
/*****************************************************************************
 *                         批量添加人员特征
 *  函 数 名：GetStringFromChar
 *  功    能：从数组形式转到字符串形式
 *  说    明：
 *  参    数：unc:uuid_t类型的数组
 *  返 回 值：std::string类型的字符串
 *  创 建 人：fjf
 *  创建时间：2014-01-14
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
std::string IrisManager::GetStringFromChar(unsigned char* unc)
{
	Exectime etm(__FUNCTION__);
	//char buf[g_UuidLen + 1];
	//memset(buf,0,g_UuidLen + 1);
	//memmove(buf,unc,g_UuidLen);
	//std::string s(buf);

	return _uuid.UUIDToString(unc);
}
/*****************************************************************************
 *                         获得指定特征数据的人员信息
 *  函 数 名：SelectInfoFromFeature
 *  功    能：获取相应的数据
 *  说    明：
 *  参    数：index:特征索引
 *           name :人员信息
 *  返 回 值：是否成功
 *  创 建 人：fjf
 *  创建时间：2014-01-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool IrisManager::SelectInfoFromFeature(int index,PeopleInfo &name,IdentifyType type)
{
	Exectime etm(__FUNCTION__);
	return this->_irisData.SelectInfoFromFeature(index,name,type);
}

bool IrisManager::GetPeopleInfoByIrisFeature(uuid_t irisuuid,PeopleInfo& personinfo)
{
	Exectime etm(__FUNCTION__);
	return this->_irisData.GetPeopleInfoByIrisFeature(irisuuid,personinfo);
}

/*****************************************************************************
 *                         获得人员特征
 *  函 数 名：SelectFeatureData
 *  功    能：获得指定人员的特征
 *  说    明：
 *  参    数：ud:人员ID
 *  			flag:特征类型
 *  			pIrisData:特征缓冲区指针（特征数据）
 *  返 回 值：无
 *  创 建 人：fjf
 *  创建时间：2014-01-14
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int IrisManager::SelectFeatureData(uuid_t ud,IdentifyType flag,unsigned char *&pIrisData)
{
	Exectime etm(__FUNCTION__);
	return this->_irisData.SelectFeatureData(ud,flag,pIrisData);
}

int IrisManager::DeletePersonAndFeatureData(InfoData& info)
{
    Exectime etm(__FUNCTION__);
    int iRet = -1;
    std::string sKey = _uuid.StringFromUuid(info.PeopleData.Id);
    std::shared_ptr<PeopleNode> p = FindKeyPeople(sKey);

    //判断人员信息
    if (NULL == p )
    {
        LOG_INFO("DeleteFeatureData: NULL == p,key: [%s], ret: %d",sKey.c_str(),iRet);
        return iRet;
    }

    //锁住当前动作
    {
        std::lock_guard<std::mutex> lk(this->_irisData._dataMutex);

        //删除人员特征－－分左右眼
        for (GUID featureID:p->LeftFeature)
        {
            //删除后仍然需要重组和重新映射
            iRet = this->_irisData.DeleteFeatureData(featureID.ud,IdentifyType::Left);
            if(iRet != 0)
            {
                LOG_ERROR("DeleteFeatureData Left failed. %d",iRet)
            }
        }

        for (GUID featureID:p->RightFeature)
        {
            //删除后仍然需要重组和重新映射
            LOG_INFO("test-------step1");
            iRet = this->_irisData.DeleteFeatureData(featureID.ud,IdentifyType::Right);
            LOG_INFO("test--------step2");
            if(iRet != 0)
            {
                LOG_ERROR("DeleteFeatureData Right failed. %d",iRet);
            }
        }

        //删除人员及映射
        p->LeftFeature.clear();
        p->RightFeature.clear();
    }

    // TODO
    _irisData.DeletePeopleData(p);

    _mapPeople.erase(sKey);

    return iRet;
}

/*****************************************************************************
 *                         删除指定人员特征
 *  函 数 名：DeleteFeatureData
 *  功    能：在内存中删除相关人员的内存特征及相关数据
 *  说    明：
 *  参    数：ud:删除相关的人员的人员ID
 *  			flag:人员特征的类型
 *  返 回 值：返回删除情况
 *  创 建 人：fjf
 *  创建时间：2014-01-14
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int IrisManager::DeleteFeatureData(InfoData& info,IdentifyType flag)
{
	Exectime etm(__FUNCTION__);
	int iRet = -1;
    std::string sKey = _uuid.StringFromUuid(info.PeopleData.Id);
	std::shared_ptr<PeopleNode> p = FindKeyPeople(sKey);

	//判断人员信息
	if (NULL == p )
	{
        LOG_INFO("DeleteFeatureData: NULL == p,key: [%s], ret: %d",sKey.c_str(),iRet);
		return iRet;
	}

	//锁住当前动作
    {
        std::lock_guard<std::mutex> lk(this->_irisData._dataMutex);

        //删除人员特征－－分左右眼
        std::list<GUID>::iterator it = p->LeftFeature.begin();
        while (it != p->LeftFeature.end())
        {
            //删除后仍然需要重组和重新映射
            if(uuid_compare(info.ud,(*it).ud) == 0)
            {
                iRet = this->_irisData.DeleteFeatureData((*it).ud,IdentifyType::Left);
                if(iRet != 0)
                {
                    LOG_ERROR("DeleteFeatureData Left failed. %d",iRet)
                }
                else
                {
                    p->LeftFeature.erase(it);
                }
                break;
            }
            it ++;
        }

        it = p->RightFeature.begin();
         while (it != p->RightFeature.end())
        {
            //删除后仍然需要重组和重新映射
            if(uuid_compare(info.ud,(*it).ud) == 0)
            {
                iRet = this->_irisData.DeleteFeatureData((*it).ud,IdentifyType::Right);
                if(iRet != 0)
                {
                    LOG_ERROR("DeleteFeatureData Right failed. %d",iRet);
                }
                else
                {
                     p->RightFeature.erase(it);
                }
                break;
            }
            it ++;
        }
    }

    // 如果这个人没有虹膜就可以删除映射了
    if(p->LeftFeature.size() == 0 && p->RightFeature.size() == 0 )
    {
        _irisData.DeletePeopleData(p);

        _mapPeople.erase(sKey);
    }

	return iRet;
}
/*****************************************************************************
 *                         添加相应的特征到内存数组
 *  函 数 名：AddFeatureData
 *  功    能：添加相应的数据到内存中的缓冲区，并建立映射，以送入算法进行匹配
 *  说    明：新增的数据不用进行重新映射，只把新的映射添加即可。
 *  参    数：ud:特征ID还是人员ID，最后再与接口定
 *  			flag:特征类型
 *  			pIrisData:特征数据
 *  返 回 值：增加人员特征到内存
 *  创 建 人：fjf
 *  创建时间：2014-01-14
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int IrisManager::AddFeatureData(InfoData& info)
{
	Exectime etm(__FUNCTION__);
	//得动态增加人员信息
	//2014-06-10增加：对人员和特征的加载是否一次加载左右眼，处理InfoData这个数据结构为数组，可以直接插入批量数据
	//std::shared_ptr<PeopleNode> pTemp = std::make_shared<PeopleNode>();
	//pTemp->Name = info.PeopleData.Name;
	//pTemp->Sex = info.PeopleData.Sex;
	//pTemp->WorkSn = info.PeopleData.WorkSn;
	//人员处理未增加
	//是否调用SetIrisFeature
	//return this->_irisData.InsertFeatureData(info.ud,pTemp,flag,pIrisData);
	int iRet = -1;
	//for ()  //批量增加
	if( this->SetIrisFeature(info,info.TypeControl,info.FeatureData,0))
	{
		iRet = 0;
	}

	return iRet;
}
/*****************************************************************************
 *                         更新内存中的特征相关数据
 *  函 数 名：UpdateFeatureData
 *  功    能：更新特征的相关内存中的数据和映射
 *  说    明：这个函数暂时不使用：采用删除加插入的方式代替
 *  参    数：
 *  返 回 值：成功为更新数量，错误为－1
 *  创 建 人：fjf
 *  创建时间：2014-01-14
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int IrisManager::UpdateFeatureData(InfoData& info,IdentifyType flag,unsigned char*pIrisData,int size)
{
	Exectime etm(__FUNCTION__);
	return this->_irisData.UpdateFeatureData(info.ud,flag,pIrisData);
}

int IrisManager::UpdatePersonInfo(InfoData& info)
{
    int nret = -1;

    std::string sKey = _uuid.StringFromUuid(info.PeopleData.Id);
    std::shared_ptr<PeopleNode> pTemp = FindKeyPeople(sKey);

    //判断人员信息
    if (NULL == pTemp )
    {
        LOG_INFO("UpdatePersonInfo: NULL == pTemp,key: [%s], ret: %d",sKey.c_str(),nret);
        return nret;
    }

    pTemp->Name = info.PeopleData.Name;//id[len].PeopleData.name;
    pTemp->CardID = info.PeopleData.CardID;
    pTemp->WorkSn = info.PeopleData.WorkSn;
    pTemp->Depart = info.PeopleData.Depart;

    return  nret;
}

/*****************************************************************************
 *                         对特征数据进行处理——包括增、删、改、查
 *  函 数 名：FeatureDataDoWith
 *  功    能：根据情况来控制特征数据的合并等动作
 *  说    明：
 *  参    数：flag:特征标记
 *           pIrisData :特征缓冲区
 *           size:特征数量
 *  返 回 值：是否成功
 *  创 建 人：fjf
 *  创建时间：2014-01-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/

bool IrisManager::FeatureDataDoWith()
{
	Exectime etm(__FUNCTION__);
	bool bRet = false;
	if (_isChange)
	{
		for(InfoData id:_dataBuf)
		{
			//自己处理一下人员信息？
			//this->_irisData.InsertFeatureData(id.ud,id.TypeControl,id.FeatureData);
		}

		//写数据库的行为暂时不知道有没有。现在只支持在脱机同步下自动存储数据库

		this->_dataBuf.clear();
	}
	return bRet;
}
/*****************************************************************************
 *                         对特征数据进行加锁
 *  函 数 名：LockData
 *  功    能：在数据并发时，用锁保证安全
 *  说    明：
 *  参    数：无
 *  返 回 值：是否成功
 *  创 建 人：fjf
 *  创建时间：2014-02-17
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IrisManager::LockData(void)
{
	Exectime etm(__FUNCTION__);
	_irisData.LockData();
}
/*****************************************************************************
 *                         对特征数据进行争锁
 *  函 数 名：UnlockData
 *  功    能：在数据并发后，对数据操作进行解锁，防止死锁
 *  说    明：
 *  参    数：无
 *  返 回 值：是否成功
 *  创 建 人：fjf
 *  创建时间：2014-02-17
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IrisManager::UnlockData(void)
{
	Exectime etm(__FUNCTION__);
	_irisData.UnlockData();
}

/*****************************************************************************
 *                         从数据库加载数据
 *  函 数 名：LoadData
 *  功    能：从数据库中读写数据
 *  说    明：
 *  参    数：无
 *  返 回 值：是否成功
 *  创 建 人：fjf
 *  创建时间：2014-02-17
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/

bool IrisManager::LoadData(void)
{
	Exectime etm(__FUNCTION__);
	//通过读写数据库调用_IrisData函数，增加特征数据
	bool bRet = false;
	DbTable dt;
	_vpData.clear();
	int count = dt.GetPersonIrisDataNum();
	for (int num = 0;num < count;)
	{
		//一千人取一次//2015-01-28 何姐测试发现1000耗时太长，改成40000
		int len = dt.GetPersonIrisData(_vpData,num);

		//保存数据并建立映射关系
		for (int size = 0;size < len;size++)
		{
			InfoData id;
			id.PeopleData.Name = _vpData[size].PersonName;
			_vpData[size].PersonID.Get(id.PeopleData.Id);
			id.PeopleData.Sex = _vpData[size].Sex;
			_vpData[size].IrisData.Get(id.FeatureData,512);
			id.TypeControl = (IdentifyType)_vpData[size].EyeFlag;
			_vpData[size].IrisID.Get(id.ud);

			//增加卡号、工号、部门   2014-06-18
			id.PeopleData.CardID = _vpData[size].CardID;
			id.PeopleData.WorkSn = _vpData[size].WorkSn;
			id.PeopleData.Depart = _vpData[size].DepartName;

			//使用原函数
			SetIrisFeature(id,id.TypeControl,id.FeatureData,0);
		}
		_vpData.clear();
		num += 40000;
	}



	return bRet;
}
/*****************************************************************************
 *                         从数据库加载数据－－－管理员
 *  函 数 名：LoadSuperData
 *  功    能：从数据库中读写管理员特征数据
 *  说    明：
 *  参    数：无
 *  返 回 值：是否成功
 *  创 建 人：fjf
 *  创建时间：2014-02-24
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool IrisManager::LoadSuperData(void)
{
	Exectime etm(__FUNCTION__);
	bool bRet = false;
	//调用生成管理员特征队列
	DbTable dt;
	std::vector<ViewPersonIris> vp;
	if (0 == dt.GetSuperIrisData(vp))
	{
		return bRet;
	}

	//处理管理员数据加载到内存中
	for(ViewPersonIris& p: vp)
	{
		unsigned char featureBuf[512];
		PeopleNode pn;
		pn.Name = p.PersonName;
		p.PersonID.Get(pn.PeopleID);
		pn.Sex = p.Sex;
		p.IrisData.Get(featureBuf,512);

		//增加卡号、工号和部门  2014-06-18
		pn.CardID = p.CardID;
		pn.WorkSn = p.WorkSn;
		pn.Depart = p.DepartName;
		//暂时不处理工号
		_irisData.BuildSuperData(IdentifyType(p.EyeFlag),featureBuf,pn);
	}

	bRet = true;

	return bRet;
}

/*****************************************************************************
 *                         获得特征数据－－－管理员
 *  函 数 名：GetSuperFeaure
 *  功    能：取得当前管理员特征数据
 *  说    明：
 *  参    数：flag: 眼睛类型
 *  			pIrisData:特征数据
 *  			size:特征大小个数
 *  返 回 值：是否成功
 *  创 建 人：fjf
 *  创建时间：2014-02-24
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/

bool IrisManager::GetSuperFeature(IdentifyType flag,unsigned char*&pIrisData,int &size)
{
	Exectime etm(__FUNCTION__);
	return _irisData.GetSuperFeature(flag,pIrisData,size);
}

/*****************************************************************************
 *                         释放特征数据－－－管理员
 *  函 数 名：ReleaseSuperData
 *  功    能：管理员特征数据清空
 *  说    明：
 *  参    数：
 *  返 回 值：是否成功
 *  创 建 人：fjf
 *  创建时间：2014-02-24
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/

bool IrisManager::ReleaseSuperData(void)
{
	Exectime etm(__FUNCTION__);
	return _irisData.ReleaseSuperData();
}

