/*****************************************************************************
 ** 文 件 名：typeconvert.cpp
 ** 主 要 类：TypeConvert
 **
 ** Copyright (c) 中科虹霸有限公司
 **
 ** 创 建 人：liuzhch
 ** 创建时间：2014-05-17
 **
 ** 修 改 人：
 ** 修改时间：
 ** 描  述:   数据转换操作（数据库结构与网络传输结构之间的转换）
 ** 主要模块说明:
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 *****************************************************************************/
#include "typeconvert.h"
#include "../Common/Logger.h"
#include <QString>
#include "../Common/Exectime.h"

/*****************************************************************************
 *                      人员信息-数据库转网络
 *  函 数 名：ConvertPersonUp
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-5-22
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool TypeConvert::ConvertPersonUp(const Person &from, PersonBase &to)
{
	
	//TO DO
	memset(&to,0,sizeof(PersonBase));

	from.personID.Get(to.Pid);
	strncpy(to.PersonName, from.name.c_str(), min((size_t)g_nameLen, from.name.length()));
	to.PersonSex = (0 == from.sex.compare("男")) ? EnumSex::male : EnumSex::female;
	strncpy(to.IdCardNumber, from.IDcard.c_str(), min((size_t)g_idCardNoLen, from.IDcard.length()));
	to.PersonType = from.superID > 0 ? EnumPersonType::adminPerson : EnumPersonType::ordinaryPerson;
	strncpy(to.DepartName, from.departName.c_str(), min((size_t)g_nameLen, from.departName.length()));
	strncpy(to.PersonSn, from.workSn.c_str(), min((size_t)g_snLen, from.workSn.length()));
	strncpy(to.CardNumber, from.cardID.c_str(), min((size_t)g_cardNoLen, from.cardID.length()));
	strncpy(to.Memo, from.memo.c_str(), min((size_t)g_memoLen, from.memo.length()));

	return true;
}


/*****************************************************************************
 *                      简单人员信息-数据库转网络
 *  函 数 名：ConvertSimplePersonUp
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-06-03
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool TypeConvert::ConvertSimplePersonUp(const Person &from, PersonSimple &to)
{
	
	//TO DO
	memset(&to,0,sizeof(PersonSimple));
	from.personID.Get(to.Pid);
	strncpy(to.PersonName, from.name.c_str(), min((size_t)g_nameLen, from.name.length()));
	strncpy(to.PersonSn, from.workSn.c_str(), min((size_t)g_snLen, from.workSn.length()));
	to.HasFeature = from.hasIrisData>0?1:0;

	return true;
}
/*****************************************************************************
 *                      人员信息-网络转数据库
 *  函 数 名：ConvertPersonDown
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：返回token
 *  创 建 人：刘中昌
 *  创建时间：2014-5-22
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int TypeConvert::ConvertPersonDown(const PersonBase &from,Person &to)
{
    //TO DO
    to.personID.Set(from.Pid);
    to.name =  std::string(from.PersonName,g_nameLen);
    to.sex          = (EnumSex::male == from.PersonSex) ? std::string("男") : std::string("女");
    to.IDcard       = std::string(from.IdCardNumber,g_idCardNoLen);
    to.superID      = (EnumPersonType::adminPerson == from.PersonType) ? 1 : 0;
    to.workSn       = std::string(from.PersonSn,g_snLen);
    to.departName   = std::string(from.DepartName,g_nameLen);
    to.cardID       = std::string(from.CardNumber,g_cardNoLen);
    to.memo         = std::string(from.Memo,g_memoLen);

    return from.OpToken;
}
/*****************************************************************************
 *                      人员图像-数据库转网络
 *  函 数 名：ConvertPersonImageUp
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-5-22
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool TypeConvert::ConvertPersonImageUp(const PersonImage &from, PersonPhoto *to)
{
	
	//TO DO
	if(nullptr == to)
	{
		std::cout << "[ERROR@" << __FUNCTION__ << "]:"
			<< "bad pointer for cache person face image data!" << std::endl;
		LOG_ERROR("bad pointer for cache person face image data!");
		return false;
	}

	if(from.personImage.Size() <= 0)
	{
		std::cout << "[" << __FUNCTION__  << "@TypeConvert]"
			<< "invalid person face image for its size is negative!"
			<< std::endl;
		LOG_ERROR("invalid person face image for its size is negative!");
		return false;
	}

	memset(to,0,sizeof(PersonPhoto));

	from.imageID.Get(to->PhotoId);
	from.personID.Get(to->Pid);
	to->PhotoSize = from.personImage.Size();


	//from.personImage.Get(to->Photo, from.personImage.Size());
	strncpy(to->Memo, from.memo.c_str(), min((size_t)g_memoLen, from.memo.length()));
	size_t size = from.personImage.Size();
	from.personImage.Get((unsigned char*)to->Photo,size);
	if(size != to->PhotoSize)
	{
		LOG_ERROR("size != to->PhotoSize");
		return false;
	}

	return true;
}
/*****************************************************************************
 *                      人员图像-数据库转网络
 *  函 数 名：ConvertPersonImageUp
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-5-22
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool TypeConvert::ConvertPersonImageUp(const IPersonPhoto &from, PersonPhoto *to)
{
	
	//TO DO
	if(nullptr == to)
	{
		std::cout << "[ERROR@" << __FUNCTION__ << "]:" << "bad pointer for cache person face image data!" << std::endl;
		LOG_ERROR("bad pointer for cache person face image data!");
		return false;
	}
	uuid_unparse(from.PhotoId,to->PhotoId);
	uuid_unparse(from.Pid,to->Pid);
	to->PhotoSize = from.PhotoSize;
	to->OpToken = from.OpToken;
	//to->Memo = from.Memo;
	//strncpy(to->Memo, from.Memo, min((size_t)g_memoLen, 128);
	//&to->Photo[0]= from.Photo;


	//    from.imageID.Get(to->PhotoId);
	//    from.personID.Get(to->Pid);
	//    to->PhotoSize = from.personImage.Size();

	//    //from.personImage.Get(to->Photo, from.personImage.Size());
	//    strncpy(to->Memo, from.memo.c_str(), min((size_t)g_memoLen, from.memo.length()));


	return true;
}
/*****************************************************************************
 *                      人员图像-网络转数据库
 *  函 数 名：ConvertPersonImageDown
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：返回token
 *  创 建 人：刘中昌
 *  创建时间：2014-5-22
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int TypeConvert::ConvertPersonImageDown(const PersonPhoto *from, PersonImage &to)
{
	
	//TO DO
	to.imageID.Set(from->PhotoId);
	to.personID.Set(from->Pid);
	to.personImage.Set(from->Photo, from->PhotoSize);
	to.memo = from->Memo;

	return from->OpToken;
}

//上传
/*****************************************************************************
 *                      虹膜数据-数据库转网络
 *  函 数 名：ConvertIrisUp
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-5-22
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool TypeConvert::ConvertIrisUp(const IrisDataBase &from, UploadIrisData &to)
{
	
	//TO DO

	memset(&to,0,sizeof(UploadIrisData));
	from.irisDataID.Get(to.Fid);
	from.personID.Get(to.Pid);

	strncpy(to.DeviceSn, from.devSn.c_str(), min((size_t)g_snLen, from.devSn.length()));
	strncpy(to.EnrollTime, from.regTime.c_str(), min((size_t)g_timeLen, from.regTime.length()));
	to.EyeFlag = (EnumEye)from.eyeFlag;



	//Iris feature attributes
	to.IrisEnrollData.pupilRow = from.pupilRow;
	to.IrisEnrollData.pupilCol = from.pupilCol;
	to.IrisEnrollData.pupilRadius = from.pupilRadius;

	to.IrisEnrollData.irisRow = from.irisRow;
	to.IrisEnrollData.irisCol = from.irisCol;
	to.IrisEnrollData.irisRadius = from.irisRadius;

	to.IrisEnrollData.spoofValue = from.spoofValue;
	to.IrisEnrollData.focusScore = from.focusScore;
	to.IrisEnrollData.interlaceValue = from.interlaceValue;

	to.IrisEnrollData.percentVisible = from.percentVisible;
	to.IrisEnrollData.qualityLevel = from.qualityLevel;
	to.IrisEnrollData.qualityScore = from.qualityScore;


	from.irisCode.Get(to.IrisEnrollData.EnrollTemplate, min((size_t)g_enrollFeatureLen, from.irisCode.Size()));
	from.matchIrisCode.Get(to.IrisEnrollData.RecogTemplate, min((size_t)g_idenFeatureLen, from.matchIrisCode.Size()));

	size_t len = from.eyePic.Size();//.GetMaxLen();
	from.eyePic.Get((unsigned char *)&to.IrisEnrollData.IrisImage, len);
	if(len >0 && len != from.eyePic.Size())
	{
		LOG_ERROR("len: %d size: %d",len,from.eyePic.Size());
		return false;
	}
	return true;
}
/*****************************************************************************
 *                      虹膜数据-网络转数据库
 *  函 数 名：ConvertIrisDown
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-5-22
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool TypeConvert::ConvertIrisDown(const UploadIrisData &from, IrisDataBase &to)
{
	
	//TO DO
	to.irisDataID.Set(from.Fid);
	to.personID.Set(from.Pid);

	to.devSn = from.DeviceSn;
	to.regTime = from.EnrollTime;
	to.eyeFlag = (short)from.EyeFlag;


	to.pupilRow = from.IrisEnrollData.pupilRow;
	to.pupilCol = from.IrisEnrollData.pupilCol;
	to.pupilRadius = from.IrisEnrollData.pupilRadius;

	to.irisRow = from.IrisEnrollData.irisRow;
	to.irisCol = from.IrisEnrollData.irisCol;
	to.irisRadius = from.IrisEnrollData.irisRadius;

	to.spoofValue = from.IrisEnrollData.spoofValue;
	to.focusScore = from.IrisEnrollData.focusScore;
	to.interlaceValue = from.IrisEnrollData.interlaceValue;

	to.percentVisible = from.IrisEnrollData.percentVisible;
	to.qualityLevel = from.IrisEnrollData.qualityLevel;
	to.qualityScore = from.IrisEnrollData.qualityScore;

	to.irisCode.Set(from.IrisEnrollData.EnrollTemplate, g_enrollFeatureLen);
	to.matchIrisCode.Set(from.IrisEnrollData.RecogTemplate, g_idenFeatureLen);

	to.eyePic.Set((unsigned char *)&from.IrisEnrollData.IrisImage, sizeof(from.IrisEnrollData.IrisImage));

	return true;
}

//下发
/*****************************************************************************
 *                      虹膜数据-网络转数据库
 *  函 数 名：ConvertIrisDown
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值： 返回token
 *  创 建 人：刘中昌
 *  创建时间：2014-5-22
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int TypeConvert::ConvertIrisDown(const SyncIrisData& from, IrisDataBase& to)
{
	
	//TO DO
	to.irisDataID.Set(from.Fid);
	to.personID.Set(from.Pid);
	to.eyeFlag = (short)from.EyeFlag;
	to.irisCode.Set(from.FeatureData, g_enrollFeatureLen);

	return from.OpToken;
}

/*****************************************************************************
 *                      识别记录-数据库转网络
 *  函 数 名：ConvertRecLogUp
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-5-22
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool TypeConvert::ConvertRecLogUp(const PersonRecLog &from, RecogRecord &to)
{
	
	//TO DO
	memset(&to,0,sizeof(RecogRecord));
	strncpy(to.DeviceSn, from.devSN.c_str(), min((size_t)g_snLen, from.devSN.length()));
	strncpy(to.IdenTime,from.recogTime.c_str(),min((size_t)g_timeLen,from.recogTime.size()));
	from.personID.Get(to.Pid);
	from.irisDataID.Get(to.Fid);
	to.RIndex = from.recLogID;
	return true;
}
/*****************************************************************************
 *                      Uuid 转 std::string
 *  函 数 名：ConvertUuidToStr
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-5-26
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int TypeConvert::ConvertUuidToStr(const std::vector<Uuid> &from, std::vector<std::string> &to)
{
	
	for(Uuid var :from)
	{
		IkUuid ikuid;
		ikuid.Set(var.uuid);
		to.push_back(ikuid.Unparse());
	}
	return to.size();
}

/*****************************************************************************
 *                      std::string 转 Uuid
 *  函 数 名：ConvertUuidfromStr
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-5-26
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int TypeConvert::ConvertUuidfromStr(const std::vector<std::string> &from, std::vector<Uuid> &to)
{
	
	for(std::string var :from)
	{
		Uuid uid;
		uuid_parse(var.c_str(),uid.uuid);
		to.push_back(uid);
	}
	return to.size();
}

//bool TypeConvert::ConvertRecogRecordUp(const PersonRecLog& from,  RecogRecord& to)
//{
//    to.DeviceSn
//}

