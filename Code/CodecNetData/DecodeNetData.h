/*
 * DecodeNetData.h
 * 网络数据解码
 *
 *  Created on: 2014年3月18日
 *      Author: yqhe
 */

#pragma once

#include "CodecUtil.h"

// 解码网络数据
class DecodeNetData
{
private:
	DecodeNetData()
    {
    };

	~DecodeNetData()
    {
    };

private:
    /*******************************************************************
    *          解码无数据的网络数据
    * 版权所有： 北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称： WithoutDataCmd
    * 功    能：解码无数据的网络数据
    * 说    明：
    * 参    数：
    * 			输入参数
    * 			buf—— 需要解码的网络数据
    * 			输出参数
    *           ctrlCmd —— 解码结果
    * 返 回 值：true —— 解码成功；false —— 解码失败
    * 创 建 人：yqhe
    * 创建日期：2014/3/18 09：23
    *******************************************************************/
	static bool WithoutDataCmd(const char *buf, ControlCmd &ctrlCmd)
	{
        return DecodeData::WithoutDataCmd(buf, ctrlCmd);
	}

    /*******************************************************************
    *          解码有数据的网络数据
    * 版权所有： 北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称： ArrayDataCmd
    * 功    能：解码有数据的网络数据
    * 说    明：
    * 参    数：
    * 			输入参数
    * 			pNetData—— 需要解码的网络数据
    *           输出参数
    *           vod     —— 已解码的数据
    * 返 回 值：true —— 解码成功；false —— 解码失败
    * 创 建 人：yqhe
    * 创建日期：2014/3/18 09：05
    *******************************************************************/
    template<typename DataT, typename NetT>
	static bool ArrayDataCmd(
			const NetT *pNetData,
			std::vector<DataT> &vod,
			bool needCheck=true)
	{
    	return DecodeData::ArrayDataCmd(pNetData, vod, needCheck);
	}

    /*******************************************************************
    *          解码有数据的网络数据
    * 版权所有： 北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称： ArrayDataCmd
    * 功    能：解码有数据的网络数据
    * 说    明：
    * 参    数：
    * 			输入参数
    * 			pNetData—— 需要解码的网络数据
    *           输出参数
    *           vod     —— 已解码的数据
    * 返 回 值：true —— 解码成功；false —— 解码失败
    * 创 建 人：yqhe
    * 创建日期：2014/3/18 09：05
    *******************************************************************/
    template<typename DataT>
	static bool BlockDataCmd(const char *buf, DataT &readData, bool needCheck=true)
	{
    	return DecodeData::BlockDataCmd(buf, readData, needCheck);
	}

    /*******************************************************************
    *          解码人员照片网络数据
    * 版权所有： 北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称： PhotoDataCmd
    * 功    能：解码人员照片网络数据
    * 说    明：因照片数据较为特殊，所以单独写一个函数
    * 参    数：
    * 			输入参数
    * 			buf —— 需要解码的网络数据
    *           输出参数
    *           readData —— 已解码的数据
    * 返 回 值：true —— 解码成功；false —— 解码失败
    * 创 建 人：yqhe
    * 创建日期：2014/5/30 13：44
    *******************************************************************/
    static bool PhotoDataCmd(const char* buf, PersonPhoto &readData, bool needCheck = true)
    {
        return DecodeData::PhotoDataCmd(buf, readData, needCheck);
    }

    /*******************************************************************
    *          获得照片网络数据中照片的长度
    * 版权所有： 北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称： GetPhotoSize
    * 功    能：获得照片网络数据中照片的长度
    * 说    明：便于在解码之前，知道要分配多长的内存
    * 参    数：
    * 			输入参数
    * 			pNetData—— 需要解码的网络数据
    *           输出参数
    *           readData—— 已解码的数据
    * 返 回 值：-1  —— 校验失败，后面就不要解码了
    *          >0  —— 照片长度
    * 创 建 人：yqhe
    * 创建日期：2014/5/30 13：48
    *******************************************************************/
    static int GetPhotoSize(const char* buf, bool needCheck = true)
    {
        return DecodeData::GetPhotoSize(buf, needCheck);
    }
public:
    /*******************************************************************
    *          解码指令
    * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称：Command
    * 功    能：解码指令
    * 说    明：只解码头数据，便于确定是什么请求，
    * 		   因为还不知道是什么指令，因此不计算校验
    * 参    数：
    *           输入参数
    *           buf     —— 网络上获取的数据
    * 			输出参数
    * 			pCtrlCmd —— 解码后的头数据，校验值总置为0
    * 返 回 值：无
    * 创 建 人：yqhe
    * 创建日期：2014/3/18 11：17
    *******************************************************************/
    static void Command(const char *buf, PControlCmd ctrlCmd)
    {
    	DecodeData::Command(buf, ctrlCmd);

    	return;
    }

    /*******************************************************************
    *          解码远程控制重启指令
    * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称：RemoteReset
    * 功    能：解码远程控制重启指令
    * 说    明：
    * 参    数：
    *           输入参数
    *           buf     —— 网络上获取的数据
    * 			输出参数
    * 			pCtrlCmd —— 解码后的数据
    * 返 回 值：true —— 解码成功；false —— 解码失败，一般原因为校验计算不符
    * 创 建 人：yqhe
    * 创建日期：2014/3/18 10：45
    *******************************************************************/
	static bool RemoteReset(const char *buf, ControlCmd ctrlCmd)
	{
		return (WithoutDataCmd(buf, ctrlCmd));
	}

    /*******************************************************************
    *          解码心跳指令
    * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称：KeepAlive
    * 功    能：解码心跳指令
    * 说    明：
    * 参    数：
    *           输入参数
    *           buf —— 网络上获取的数据
    * 返 回 值：true —— 解码成功；false —— 解码失败，一般原因为校验计算不符
    * 创 建 人：yqhe
    * 创建日期：2014/3/18 10：50
    *******************************************************************/
    static bool KeepAlive(const char *buf, ControlCmd ctrlCmd)
    {
        return (WithoutDataCmd(buf, ctrlCmd));
	}

    ////////////////////////////////////
    // added at 20140318 by yqhe

	// 解码增加人员请求
	static bool AddPerson(const char *buf, std::vector<PersonBase> &vod)
	{
		PNetPersonBaseArray pTemp = (PNetPersonBaseArray)buf;
		return(ArrayDataCmd(pTemp, vod));
	}

	// 解码更新人员请求
	static bool UpdatePerson(const char *buf, std::vector<PersonBase> &vod)
	{
		PNetPersonBaseArray pTemp = (PNetPersonBaseArray)buf;
		return(ArrayDataCmd(pTemp, vod));
	}

	// 解码删除人员请求
	static bool DeletePerson(const char *buf, std::vector<Uuid> &vod)
	{
		PNetUuidArray pTemp = (PNetUuidArray)buf;
		return(ArrayDataCmd(pTemp, vod));
	}

    // added at 20140530 by yqhe
    // 读取网络数据中，照片的长度
    static int GetPersonPhotoSize(const char* buf)
    {
        return (GetPhotoSize(buf));
    }

	// 解码增加人员照片请求
    // 修改照片解码的方式, modified at 20140530 13:49 by yqhe
    // 如果不知道照片占用多少内存，可先调用GetPersonPhotoSize
	static bool AddPersonPhoto(const char *buf, PersonPhoto &photo)
	{
        return (PhotoDataCmd(buf, photo, true));
	}

	// 解码修改人员照片请求
    // 修改照片解码的方式, modified at 20140530 13:49 by yqhe
    // 如果不知道照片占用多少内存，可先调用GetPersonPhotoSize
	static bool UpdatePersonPhoto(const char *buf, PersonPhoto &photo)
	{
        return (PhotoDataCmd(buf, photo, true));
	}

	// 解码删除人员照片请求
	static bool DeletePersonPhoto(const char *buf, std::vector<Uuid> &vod)
	{
		PNetUuidArray pTemp = (PNetUuidArray)buf;
		return(ArrayDataCmd(pTemp, vod));
	}

	////////////////////////////////////

    ////////////////////////////////////
    // added at 20140319 by yqhe
	// 解码增加人员请求
	static bool AddAdmin(const char *buf, std::vector<PersonBase> &vod)
	{
		PNetPersonBaseArray pTemp = (PNetPersonBaseArray)buf;
		return(ArrayDataCmd(pTemp, vod));
	}

	// 解码更新管理员请求
	static bool UpdateAdmin(const char *buf, std::vector<PersonBase> &vod)
	{
		PNetPersonBaseArray pTemp = (PNetPersonBaseArray)buf;
		return(ArrayDataCmd(pTemp, vod));
	}

	// 解码删除管理员请求
	static bool DeleteAdmin(const char *buf, std::vector<Uuid> &vod)
	{
		PNetUuidArray pTemp = (PNetUuidArray)buf;
		return(ArrayDataCmd(pTemp, vod));
	}

	// 解码增加管理员照片请求
	static bool AddAdminPhoto(const char *buf, PersonPhoto &photo)
	{
        return(PhotoDataCmd(buf, photo, true));
	}

	// 解码修改管理员照片请求
	static bool UpdateAdminPhoto(const char *buf, PersonPhoto &photo)
	{
        return(PhotoDataCmd(buf, photo, true));
	}

	// 解码删除管理员照片请求
	static bool DeleteAdminPhoto(const char *buf, std::vector<Uuid> &vod)
	{
		PNetUuidArray pTemp = (PNetUuidArray)buf;
		return(ArrayDataCmd(pTemp, vod));
	}

	// 解码增加管理员虹膜特征请求
	static bool AddAdminIris(const char *buf, std::vector<SyncIrisData> &vod)
	{
		PNetSyncIrisData pTemp = (PNetSyncIrisData)buf;
		return (ArrayDataCmd(pTemp, vod, true));
	}

	// 解码删除管理员虹膜特征请求
	static bool DeleteAdminIris(const char *buf, std::vector<Uuid> &vod)
	{
		PNetUuidArray pTemp = (PNetUuidArray)buf;
		return(ArrayDataCmd(pTemp, vod));
	}

	// 解码增加虹膜特征请求
	static bool AddPersonIris(const char *buf, std::vector<SyncIrisData> &vod)
	{
		PNetSyncIrisData pTemp = (PNetSyncIrisData)buf;
		return (ArrayDataCmd(pTemp, vod, true));
	}

	// 解码删除虹膜特征请求
	static bool DeletePersonIris(const char *buf, std::vector<Uuid> &vod)
	{
		PNetUuidArray pTemp = (PNetUuidArray)buf;
		return(ArrayDataCmd(pTemp, vod));
	}

	// 解码上传虹膜注册数据请求
	static bool UploadPersonIris(const char *buf, std::vector<UploadIrisData> &vod)
	{
		PNetUploadIrisDataArray pTemp = (PNetUploadIrisDataArray)buf;
		return(ArrayDataCmd(pTemp, vod, true));
	}

	// 解码上传识别记录请求
	static bool UploadRecogRecord(const char *buf, std::vector<RecogRecord> &vod)
	{
		PNetRecogRecordArray pTemp = (PNetRecogRecordArray)buf;
		return(ArrayDataCmd(pTemp, vod));
	}

	// 解码实时发送识别记录请求
	static bool SendRecogRecord(const char *buf, std::vector<RecogRecord> &vod)
	{
		PNetRecogRecordArray pTemp = (PNetRecogRecordArray)buf;
		return(ArrayDataCmd(pTemp, vod));
	}

	////////////////////////////////////

    ////////////////////////////////////
    // added at 20140320 by yqhe
	// 解码远程控制关机请求
    static bool SrvCtrlShutdown(const char *buf, ControlCmd ctrlCmd)
    {
        return (WithoutDataCmd(buf, ctrlCmd));
	}

    ////////////////////////////////////
    // added at 20140607 17:19 by yqhe
    // 解码远程控制重启请求
    static bool SrvCtrlReset(const char *buf, ControlCmd ctrlCmd)
    {
        return (WithoutDataCmd(buf, ctrlCmd));
    }
    ////////////////////////////////////

    // 解码远程设置时间请求
    static bool SrvCtrlAdjustTime(const char *buf, TimeString &srvTime)
    {
        return (BlockDataCmd(buf, srvTime));
    }

    // 解码以图像方式进行集群识别请求
    static bool ClusterImageIdentify (const char *buf, std::vector<ClusterImageItem> &vod)
	{
    	PNetClusterImageIdenArray pTemp = (PNetClusterImageIdenArray)buf;
		return(ArrayDataCmd(pTemp, vod, true));
	}

    // 解码以特征方式进行集群识别请求
    static bool ClusterFeatureIdentify (const char *buf, std::vector<ClusterFeatureItem> &vod)
	{
    	PNetClusterFeatureIdenArray pTemp = (PNetClusterFeatureIdenArray)buf;
		return(ArrayDataCmd(pTemp, vod, true));
	}

    // 解码配置图像集群识别的识别参数请求
    static bool ClusterImageIdentifyConfig(const char *buf, PClusterImageIdenParam clusterIdenParam)
    {
    	return (BlockDataCmd(buf, clusterIdenParam));
    }

    // 解码配置特征集群识别的识别参数请求
    static bool ClusterFeatureIdentifyConfig(const char *buf, PClusterFeatureIdenParam clusterIdenParam)
    {
    	return (BlockDataCmd(buf, clusterIdenParam));
    }

    ////////////////////////////////////

    ////////////////////////////////////
    // added at 20140430 by yqhe
    // 解码设备发到服务器的同步令牌请求
    // 此请求已删除或无用
    static bool SyncToken(const char *buf, int &token)
    {
    	return (BlockDataCmd(buf, token));
    }

    // 解码设备发到服务器的设备连接参数请求
    static bool DevConnect(const char *buf, DevConnInfo &devConnInfo)
    {
    	return (BlockDataCmd(buf, devConnInfo));
    }

    // 解码获得人员列表请求
    static bool GetPersonList (const char *buf, ControlCmd ctrlCmd)
	{
        return (WithoutDataCmd(buf, ctrlCmd));
	}

    // 解码设置设备IP请求
    static bool SrvCtrlAdjustDevIP(const char *buf, IpSetting &ipSetting)
	{
        return (BlockDataCmd(buf, ipSetting));
	}

    // 解码设置设备连接服务器IP请求
    static bool SrvCtrlAdjustSrvIP(const char *buf, ListenIp &srvIp)
	{
        return (BlockDataCmd(buf, srvIp));
	}

    // 解码要求获得设备信息请求
    static bool GetDevInfo(const char *buf, ControlCmd ctrlCmd)
    {
        return (WithoutDataCmd(buf, ctrlCmd));
    }

    // 解码设备要求获得服务器当前时间
    static bool GetSrvTime(const char *buf, ControlCmd ctrlCmd)
    {
        return (WithoutDataCmd(buf, ctrlCmd));
    }
    ////////////////////////////////////

    ////////////////////////////////////
    // added at 20140503 by yqhe
    // 获得设备上指定识别记录
    static bool GetRecogRecord(const char *buf, GetRecogRecord &getRecordCondi )
	{
        return (BlockDataCmd(buf, getRecordCondi));
	}
    ////////////////////////////////////
};

