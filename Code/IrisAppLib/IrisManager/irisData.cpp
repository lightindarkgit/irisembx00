/******************************************************************************************
 ** 文件名:   irisData.cpp
 ×× 主要类:   IrisData
 **
 ** Copyright (c) 中科虹霸有限公司
 ** 创建人:   fjf
 ** 日  期:   2013-01-06
 ** 修改人:
 ** 日  期:
 ** 描  述: 虹膜数据类
 ** ×× 主要模块说明：
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 *****************************************************************************************/

#include "irisData.h"
#include "../../Common/Exectime.h"
#include "../../Common/Logger.h"

//特征数组
unsigned char  g_Buf[1024 * 1024 * 220];      //一个特征是512字节，左右眼共开出200M内存:增加20M余量
ControlNode    g_People[1000 * 420 ];         //ControlNode是12个字节 共12＊（左：100000＋ 右100000）人:20人的余量,注意范围
//这里计算原来有误，对应节点应该是人数的两倍,原来反而是一半，所以大数据加载时内存溢出
//在桂林长海项目中发现的这个问题。

IrisData::IrisData()
{
	Exectime etm(__FUNCTION__);
	Init();
}

IrisData::~IrisData()
{
	Exectime etm(__FUNCTION__);

	// TODO Auto-generated destructor stub
}
/*****************************************************************************
 *                         初始化管理员数据参数
 *  函 数 名：InitSuperData
 *  功    能：对管理员的相关操作参数进行初始化
 *  说    明：
 *  参    数：无
 *  返 回 值：无
 *  创 建 人：fjf
 *  创建时间：2014-03-07
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IrisData::InitSuperData()
{
	Exectime etm(__FUNCTION__);
	//super data指针
	_pFeatureData = _pLFeatureData = _pRFeatureData = nullptr;
	_pLHead = _pRHead = nullptr;
	_superLCount = _superRCount = 0;
	_pPersonInfo = nullptr;

	//开辟管理员特征，固定
	//指定左眼特征指针头和当前位置指针
	_pLHead = _pLFeatureData = _pFeatureData =_pRFeatureData = new unsigned char[g_SuperCount * 512];
	_pRFeatureData += g_SuperCount  * 256;           //指定右眼的特征头和当前位置,内存分两块，左面为左眼，右面为右眼
	_pRHead = _pRFeatureData;

}
/*****************************************************************************
 *                         初始化普通人员操作参数
 *  函 数 名：InitData
 *  功    能：初始化人员操作的基本参数
 *  说    明：
 *  参    数：无
 *  返 回 值：无
 *  创 建 人：fjf
 *  创建时间：2014-03-07
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IrisData::InitData()
{
	Exectime etm(__FUNCTION__);
	//指定当前参数值
	_curPlace = 0;
	_curRPlace = 0;
	_countLeft = 0;
	_countRight = 0;
	_countLeft = 0;
	_countRight = 0;

	// 指定当前指针位置
	_pPosFeature  = _pLFeatureHead = g_Buf;
	_pPosCNode    = _pLCNodeHead = g_People;
	_pPosRFeature = _pRFeatureHead = g_Buf + 1024 * 1024 * 110;
	_pPosRCNode   = _pRCNodeHead = g_People + 1000 * 210;
}
/*****************************************************************************
 *                        初始化指针及分配内存
 *  函 数 名：Init
 *  功    能：初始化
 *  说    明：
 *  参    数：无
 *  返 回 值：无
 *  创 建 人：fjf
 *  创建时间：2014-03-07
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IrisData::Init()
{
	Exectime etm(__FUNCTION__);
	InitSuperData();
	InitData();
}
/*****************************************************************************
 *                         得到指定的特征数组
 *  函 数 名：GetIrisFeature
 *  功    能：获取相应的特征数据
 *  说    明：
 *  参    数：flag:特征标志
 *           pIrisData :人员性
 *           size:
 *  返 回 值：是否成功
 *  创 建 人：fjf
 *  创建时间：2014-01-14
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool IrisData::GetIrisFeature(IdentifyType flag,unsigned char *&pIrisData,int &size)
{
	Exectime etm(__FUNCTION__);
	bool bRet = true;
	//分别获取左右眼的特征
	if ((flag == IdentifyType::Left) && (_countLeft > 0))
	{
		//此处的锁移到显示的加锁和解锁函数中
		size = _countLeft;
		pIrisData = _pLFeatureHead;
	}
	else if ((flag == IdentifyType::Right) && (_countRight > 0))
	{
		size = _countRight;
		pIrisData = _pRFeatureHead;
	}
	else
	{
		//其它情况错误
		size = 0;
		bRet = false;
	}

	return bRet;
}
/*****************************************************************************
 *                         清空所有数据
 *  函 数 名：ReleaseFeature
 *  功    能：清空所有数据
 *  说    明：
 *  参    数：
 *  返 回 值：无
 *  创 建 人：fjf
 *  创建时间：2014-01-14
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IrisData::ReleaseFeature(void)
{
	Exectime etm(__FUNCTION__);
	//清除人员
	this->_peopleData.clear();
	//清除ID和索引映射
	this->_mapFeature.clear();

	//强制释放左右眼智能指针内存 fjf 2014-11-05
	for (int num = 0;num < this->_countLeft;num++)
	{
		this->_pLCNodeHead[num].p.reset();
	}

	for (int num = 0;num < this->_countRight;num++)
	{
		this->_pRCNodeHead[num].p.reset();
	}

	//清空左右眼数据
	memset(g_Buf,0,1024*1024*220);
	memset(g_People,0,420*1000);
	//初始化
	this->InitData();
}

/*****************************************************************************
 *                         从特征序列选择指定的人员数据
 *  函 数 名：SelectInfoFromFeature
 *  功    能：获取相应的数据
 *  说    明：
 *  参    数：index:数据的缓冲区
 *           name :人员性
 *  返 回 值：是否成功
 *  创 建 人：fjf
 *  创建时间：2014-01-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool IrisData::SelectInfoFromFeature(int index,PeopleInfo &name,IdentifyType type)
{
	Exectime etm(__FUNCTION__);
	bool bRet = false;
	//是否判断大于整体索引值的情况
	if (index < 0 )
	{
		return bRet;
	}

	ControlNode pTemp;
	if (IdentifyType::Left == type)
	{
		pTemp = _pLCNodeHead[index];
	}
	else
	{
		pTemp = _pRCNodeHead[index];
	}

	//目前仅返回人员姓名和ID
	name.Name = pTemp.p->Name;
	name.CardID = pTemp.p->CardID;
	name.WorkSn = pTemp.p->WorkSn;
	name.Depart = pTemp.p->Depart;
	memmove(name.Id,pTemp.p->PeopleID,16);
	memmove(name.FeatureID,pTemp.FeatureID,16);

	return bRet;
}

// 通过虹膜UUID获取人员
bool IrisData::GetPeopleInfoByIrisFeature(uuid_t irisuuid,PeopleInfo& personinfo)
{
	bool bret = false;

	// 先去左眼列表查找
	ControlNode* plastnodeL = _pLCNodeHead;
	while(plastnodeL != _pPosCNode)
	{
		if(0 == uuid_compare(plastnodeL->FeatureID,irisuuid))
		{
			//目前仅返回人员姓名和ID
			personinfo.Name = plastnodeL->p->Name;
			personinfo.CardID = plastnodeL->p->CardID;
			personinfo.WorkSn = plastnodeL->p->WorkSn;
			personinfo.Depart = plastnodeL->p->Depart;
			memmove(personinfo.Id,plastnodeL->p->PeopleID,16);
			memmove(personinfo.FeatureID,plastnodeL->FeatureID,16);

			bret = true;
			return bret;
		}
		plastnodeL ++;
	}
	
	// 再去右眼列表查找
	ControlNode* plastnodeR = _pRCNodeHead;
	while(plastnodeR != _pPosRCNode)
	{
		if(0 == uuid_compare(plastnodeR->FeatureID,irisuuid))
		{
			//目前仅返回人员姓名和ID
			personinfo.Name = plastnodeR->p->Name;
			personinfo.CardID = plastnodeR->p->CardID;
			personinfo.WorkSn = plastnodeR->p->WorkSn;
			personinfo.Depart = plastnodeR->p->Depart;
			memmove(personinfo.Id,plastnodeR->p->PeopleID,16);
			memmove(personinfo.FeatureID,plastnodeR->FeatureID,16);

			bret = true;
			return bret;
		}
		plastnodeR ++;
	}

	return bret;
}

/*****************************************************************************
 *                         将特征插入到内存中
 *  函 数 名：InsertFeatureData
 *  功    能：将数据插入到当前的内存数据中
 *  说    明：插入完成后需要重新建立相关映射和数据集关系
 *  参    数：featureID:数据的缓冲区
 *           pPnode:人员指针
 *  			flag:特征的类型
 *           pIrisData :人员特征值
 *  返 回 值：isOK
 *  创 建 人：fjf
 *  创建时间：2014-01-14
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool IrisData::InsertFeatureData(uuid_t featureID,std::shared_ptr<PeopleNode> &pPnode,IdentifyType flag,unsigned char * pIrisData)
{
	Exectime etm(__FUNCTION__);
	bool bRet = false;
	GUID gd;
	memmove(gd.ud,featureID,16);
	std::lock_guard<std::mutex> lk(this->_dataMutex);

    if(_pPosFeature >= g_Buf + 1024 * 1024 * 110
            || _pPosRFeature >= g_Buf + 1024 * 1024 * 220)
    {
        LOG_ERROR("****not enough Memary can be uesed****");
        return false;
    }

	//建立特征ID和特征序列数组的映射
	//left eye
	if (flag == IdentifyType::Left)
	{
		pPnode->LeftFeature.push_back(gd);                               //将左眼的特征ID加入到人员的左眼队列
		memmove(_pPosFeature,pIrisData,g_FeaturSize);                    //拷贝特征
		memmove(_pPosCNode->FeatureID,featureID,g_UuidLen);              //拷贝特征ID到映射结点
		_pPosCNode->p = pPnode;
		//建立特征ID和数组索引的映射
		_mapFeature[_uuidControl.StringFromUuid(featureID)] = _curPlace;

		//控制位置－始终在尾节点位置
		_curPlace++;                                                     //注意索引与下面特征和人员的数组的对应关系
		_pPosFeature += g_FeaturSize;
		_pPosCNode++;
		_countLeft++;                                                    //left eye特征计数增加
	}
	else   //right eye
	{
		pPnode->RightFeature.push_back(gd);                              //将右眼的特征ID加入到人员中的右眼队列
		memmove(_pPosRFeature,pIrisData,g_FeaturSize);                   //拷贝特征
		memmove(_pPosRCNode->FeatureID,featureID,g_UuidLen);             //拷贝特征ID到映射结点
		_pPosRCNode->p = pPnode;
		//建立特征ID和数组索引的映射
		_mapFeature[_uuidControl.StringFromUuid(featureID)] = _curRPlace;

		//控制位置
		_curRPlace++;                                                    //注意索引与下面特征和人员的数组的对应关系
		_pPosRFeature += g_FeaturSize;
		_pPosRCNode++;
		_countRight++;
	}


	return bRet;
}
/*****************************************************************************
 *                         控制节点的当前指针
 *  函 数 名：PeopleCount
 *  功    能：控制节点的当前指针---考虑有问题，一一对应，函数暂时废弃
 *  说    明：始终保持在尾部
 *  参    数：
 *  返 回 值：无
 *  创 建 人：fjf
 *  创建时间：2014-02-19
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IrisData::PeopleCount(void)
{
	Exectime etm(__FUNCTION__);
	_pPosCNode++;
}
/*****************************************************************************
 *                         查找特征映射表中的数据
 *  函 数 名：FindKeyFeature
 *  功    能：查找MAP中相应的数据
 *  说    明：取代MAP[].如果使用[]会在没有键值的情况下插入一个默认值，增加无用数据
 *  参    数：
 *  返 回 值：索引值
 *  创 建 人：fjf
 *  创建时间：2014-05-04
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int IrisData::FindKeyFeature(std::string sUuid)
{
	Exectime etm(__FUNCTION__);
	FeatureMap::iterator it = _mapFeature.find(sUuid);
	if (_mapFeature.end() != it)
	{
		return it->second;
	}

	return -1;
}
/*****************************************************************************
 *                         将人员插入到内存中
 *  函 数 名：InserPersonData
 *  功    能：将人员数据插入到当前的内存数据中
 *  说    明：插入完成后需要重新建立相关映射和数据集关系
 *  参    数：pTemp:数据的指针
 *  返 回 值：isOK
 *  创 建 人：fjf
 *  创建时间：2014-01-15
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool IrisData::InserPersonData(std::shared_ptr<PeopleNode> &pTemp)
{
	Exectime etm(__FUNCTION__);
	std::lock_guard<std::mutex> lk(this->_dataMutex);
	this->_peopleData.push_back(pTemp);

	return true;
}
/*****************************************************************************
 *                         删除人员
 *  函 数 名：DeletePeopleData
 *  功    能：将人员数据从链表中删除
 *  说    明：删除后需要重映射
 *  参    数：pTemp:数据的指针
 *  返 回 值：isOK
 *  创 建 人：fjf
 *  创建时间：2014-02-20
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IrisData::DeletePeopleData(std::shared_ptr<PeopleNode> &pTemp)
{
	Exectime etm(__FUNCTION__);
	std::lock_guard<std::mutex> lk(this->_dataMutex);
	this->_peopleData.remove(pTemp);
	return;
}
/*****************************************************************************
 *                         更新指定人员的特征
 *  函 数 名：UpdateFeatureData
 *  功    能：更新指定的人员特征
 *  说    明：更新是否使唤删除和插入？暂时先不使用更新
 *           同样也涉及到映射关系的重新建立
 *  参    数：featureID:更新人员的特征ID
 *           flag:特征类型
 *           pIrisData:特征数据
 *  返 回 值：是否成功
 *  创 建 人：fjf
 *  创建时间：2014-01-14
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int IrisData::UpdateFeatureData(uuid_t featureID,IdentifyType flag,unsigned char*pIrisData)
{
	Exectime etm(__FUNCTION__);
	int iRet = -1;
	return iRet;
}
/*****************************************************************************
 *                         从特征序列删除指定的人员数据
 *  函 数 名：DeleteFeatureData
 *  功    能：
 *  说    明：
 *  参    数：featureID:删除人员的特征ID
 *  返 回 值：是否成功
 *  创 建 人：fjf
 *  创建时间：2014-01-14
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int IrisData::DeleteFeatureData(uuid_t featureID,IdentifyType flag)
{
	Exectime etm(__FUNCTION__);
	//1、先找到ID对应的INDEX   2、由INDEX找到指向IrisToNode--省略，直接到3. 3、由IrisToNode中的指针找到相应的人员
	//4、逆向删除并释放内存 5、删除两级映射
	//注意：重新建立映射的时机和位置、人员映射
    int iRet = 0;

	//左右眼特征和控制节点变换
	if (flag == IdentifyType::Left)
	{
         ControlNode* plastnodeL = (_pPosCNode == _pLCNodeHead) ? _pPosCNode : (_pPosCNode - 1);

		//得到当前和最后一个，最后倒入到当前，并清空最后原来的内存空间
		std::string sKey = this->_uuidControl.StringFromUuid(featureID);
        std::string sMovKey = this->_uuidControl.StringFromUuid(plastnodeL->FeatureID);
		//处理人员映射
		//处理MAP[]使用FIND－－－2014-05-04
		int index = FindKeyFeature(sKey);                                                      //找到删除的位置索引
		//如果没找到
		if (0 > index)
		{
			return -1;
		}

		//处理一下删除的节点中的智能指针的内存 fjf 2014-11-05
		(_pLCNodeHead + index)->p.reset();//2014-11-05
		//移动最后一个到当前位置
        (_pLCNodeHead + index)->p = plastnodeL->p;
        uuid_copy((_pLCNodeHead + index)->FeatureID, plastnodeL->FeatureID);
        plastnodeL->p.reset();//清空最后一个

		//处理尾指针
		_pPosCNode--;                                                                         //尾指针前移

		//处理特征－－每512个字节对就一个index
       unsigned char* plastfeatureL = (_pPosFeature == _pLFeatureHead) ? _pPosFeature : (_pPosFeature - 512);
        memmove(_pLFeatureHead+ 512* index,plastfeatureL,512);
        memset(plastfeatureL,0,512);
		_pPosFeature -= 512;                                                                  //退后512个字节

		//左眼特征数量和控制节点人员数量同时减一
		_countLeft--;

		//重建映射 2014－11－06 处理当前位置处置不当的问题
		_curPlace--;                                                                          //当前位置指到移动后的位置
		_mapFeature.erase(sKey);                                                              //删除原来的第一级映射，即特征ID到特征索引映射
		_mapFeature[sMovKey] = index;                                                         //重建第一级映射
	}
	else
	{
		//处理右眼
        ControlNode* plastnodeR = (_pPosRCNode == _pRCNodeHead) ? _pPosRCNode : (_pPosRCNode - 1);
		//得到当前和最后一个，最后倒入到当前，并清空最后原来的内存空间
		std::string sKey = this->_uuidControl.StringFromUuid(featureID);
        std::string sMovKey = this->_uuidControl.StringFromUuid(plastnodeR->FeatureID);
		//处理MAP[]使用FIND－－－2014-05-04
		int index = FindKeyFeature(sKey);                                                      //找到删除的位置索引
		//如果没找到
		if (0 > index)
		{
			return -1;
		}

		//处理智能指针内存释放 2014－11－05 fjf
		(_pRCNodeHead + index)->p.reset();
		//移动最后一个到当前位置
        (_pRCNodeHead + index)->p = plastnodeR->p;
        memmove((_pRCNodeHead + index)->FeatureID,plastnodeR->FeatureID,g_UuidLen);
        plastnodeR->p.reset();//清空最后一个

		//处理尾指针和数量
		_pPosRCNode--;                                                                         //右眼尾指针前移

		//处理特征－－每512个字节对就一个index
        unsigned char* plastfeatureR = (_pPosRFeature == _pRFeatureHead) ? _pPosRFeature : (_pPosRFeature - 512);
        memmove(_pRFeatureHead+ 512* index,plastfeatureR,512);
        memset(plastfeatureR,0,512);
		_pPosRFeature -= 512;                                                                  //退后512个字节

		//左眼特征数量和控制节点人员数量同时减一
		_countRight--;

		//重建映射
		_curRPlace--;                                                                         //当前位置指到移动后的位置
		_mapFeature.erase(sKey);                                                              //删除原来的第一级映射，即特征ID到特征索引映射
		_mapFeature[sMovKey] = index;                                                         //重建第一级映射
	}

	return iRet;
}
/*****************************************************************************
 *                         获取指定人员的特征ID（1：1）
 *  函 数 名：SelectFeatureData
 *  功    能：指定某个人的全部的特征ID
 *  说    明：
 *  参    数：personID:人员的ID
 *           flag:特征类型
 *           pIrisData:特征数据
 *  返 回 值：特征数量
 *  创 建 人：fjf
 *  创建时间：2014-01-14
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int IrisData::SelectFeatureData(uuid_t personID,IdentifyType flag,unsigned char *&pIrisData)
{
	Exectime etm(__FUNCTION__);
	int iRet = -1;
	return iRet;
}
int IrisData::FeatureArrayAdd()
{
	Exectime etm(__FUNCTION__);
	int iRet = -1;
	return iRet;
}

int IrisData::FeatureArrayDelete()
{
	Exectime etm(__FUNCTION__);
	int iRet = -1;
	return iRet;
}
/*****************************************************************************
 *                         锁住指定人员特征缓冲区
 *  函 数 名：LockData
 *  功    能：获取专用锁，不允许再修改指定的特征和人员内存缓冲区
 *  说    明：
 *  参    数：
 *  返 回 值：暂时无用
 *  创 建 人：fjf
 *  创建时间：2014-02-19
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool IrisData::LockData()
{
	Exectime etm(__FUNCTION__);
	bool bRet = false;
	this->_dataMutex.lock();
	return bRet;
}
/*****************************************************************************
 *                         解锁指定人员特征缓冲区
 *  函 数 名：UnlockData
 *  功    能：释放专用锁，允许修改指定的特征和人员内存缓冲区－－－主要供网络通信时使用
 *  说    明：
 *  参    数：
 *  返 回 值：暂时无用
 *  创 建 人：fjf
 *  创建时间：2014-02-19
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool IrisData::UnlockData()
{
	Exectime etm(__FUNCTION__);
	bool bRet = false;
	this->_dataMutex.unlock();
	return bRet;
}
/*****************************************************************************
 *                         获得管理员特征数据
 *  函 数 名：GetSuperFeature
 *  功    能：把左右眼的特征数据分别传回到应用
 *  说    明：
 *  参    数：flag:左右眼
 *  			pIrisData:特征缓冲区
 *  			size:特征数量
 *  返 回 值：成功与否
 *  创 建 人：fjf
 *  创建时间：2014-02-24
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool IrisData::GetSuperFeature(IdentifyType flag,unsigned char*&pIrisData,int &size)
{
    Exectime etm(__FUNCTION__);
	bool bRet = true;

	if ((flag == IdentifyType::Left) && (_superLCount > 0))
	{
		pIrisData = _pLHead;
		size = _superLCount;
	}
	else if ((flag == IdentifyType::Right)&&(_superRCount > 0))
	{
		pIrisData = _pRHead ;
		size = _superRCount;
	}
	else
	{
		size = 0;
		bRet = false;
	}

	return bRet;
}
/*****************************************************************************
 *                         释放所有管理员数据
 *  函 数 名：ReleaseSuperData
 *  功    能：释放当前数据，但是不释放内存
 *  说    明：
 *  参    数：
 *  返 回 值：暂时无用
 *  创 建 人：fjf
 *  创建时间：2014-02-24
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool IrisData::ReleaseSuperData(void)
{
	Exectime etm(__FUNCTION__);
	bool bRet = false;

	memset(_pFeatureData,0,g_SuperCount*512);                //初始化缓冲区
	_pLFeatureData = _pLHead;                                //恢复左右头指针位置
	_pRFeatureData = _pRHead;
	_superRCount   = _superLCount = 0;                       //恢复数量

	return bRet;
}
/*****************************************************************************
 *                         释放所有管理员数据
 *  函 数 名：BuildSuperData
 *  功    能：建立内存模型
 *  说    明：
 *  参    数：
 *  返 回 值：暂时无用
 *  创 建 人：fjf
 *  创建时间：2014-02-25
 *  修 改 人：gdj
 *  修改时间：
 *****************************************************************************/
bool IrisData::BuildSuperData(IdentifyType flag,unsigned char *pIrisData,PeopleNode &pnData)
{
	Exectime etm(__FUNCTION__);

	//判断是否超员
	if ((_superLCount + _superRCount) >= g_SuperCount)
	{
         LOG_ERROR("管理员特征超限制 _superLCount + superRCount=%d",_superLCount+_superRCount);
        return false;
	}

	//左右眼分别组建
	if (IdentifyType::Left == flag)
	{
        //@dj
        if(_superLCount >= g_SuperHalf )
        {
            LOG_ERROR("管理员特征超限制 _superLCount=%d",_superLCount);

            return false;
        }
		//拷贝人员和左眼特征数据
		memmove(&_superCount[_superLCount],&pnData,sizeof(pnData));
		memmove(_pLFeatureData,pIrisData,g_FeaturSize);
		_pLFeatureData+=g_FeaturSize;
		_superLCount++;
	}
	else
	{
        //@dj
        if(_superRCount >= g_SuperHalf)
        {
            LOG_ERROR("管理员特征超限制_superRCount=%d",_superRCount);
            return false;
        }
		//拷贝人员和左眼特征数据
		memmove(&_superCount[g_SuperHalf +_superRCount],&pnData,sizeof(pnData));
        memmove(_pRFeatureData,pIrisData,g_FeaturSize); //申请了120*512空间，但首地址从60*512开始；dj
        _pRFeatureData += g_FeaturSize;
		_superRCount++;
	}

    return true;
}

