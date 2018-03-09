/*
 * EncodeNetData.h
 * 网络数据编码
 *
 *  Created on: 2014年3月18日
 *      Author: yqhe
 */

#pragma once

#include "CodecUtil.h"

// 编码网络数据
class EncodeNetData
{
public:
    /*******************************************************************
    *           编码远程控制重启指令
    * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称：RemoteReset
    * 功    能：编码远程控制重启指令
    * 说    明：在调用此函数前，需要已经为buf分配相应的内存，大小为12Bytes
    * 参    数：
    *           输出参数
    *           buf     —— 已编码的数据
    * 返 回 值：实际编码后数据长度
    * 创 建 人：yqhe
    * 创建日期：2014/3/14 14：03
    *******************************************************************/
    static int RemoteReset(char *buf)
	{
        int reqCode = REQ_SRVCTRL_RESET;
        return (WithoutDataCmd(reqCode, buf));
	}

    /*******************************************************************
    *           编码心跳指令
    * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称：KeepAlive
    * 功    能：编码心跳指令
    * 说    明：在调用此函数前，需要已经为buf分配相应的内存，大小为12Bytes
    * 参    数：
    *           输出参数
    *           buf     —— 已编码的数据
    * 返 回 值：实际编码后数据长度
    * 创 建 人：yqhe
    * 创建日期：2014/3/14 14：05
    *******************************************************************/
	static int KeepAlive(char *buf)
	{
        int reqCode = REQ_KEEP_ALIVE;
        return (WithoutDataCmd(reqCode, buf));
	}

	// deleted at 20140429 by yqhe
	// 去掉脱机、联网特征更新之差别；去掉部门相关请求

    ////////////////////////////////////
    // added at 20140318 by yqhe

	// 增加人员
	static int AddPerson(const std::vector<PersonBase> &vod, char *buf)
	{
		int reqCode = REQ_ADD_PERSON_INFO;
		PNetPersonBaseArray pTemp = (PNetPersonBaseArray)buf;
		return (ArrayDataCmd(reqCode, vod, pTemp));
	}

	// 修改人员
    static int UpdatePerson(const std::vector<PersonBase> &vod, char *buf)
	{
		int reqCode = REQ_UPDATE_PERSON_INFO;
		PNetPersonBaseArray pTemp = (PNetPersonBaseArray)buf;
		return (ArrayDataCmd(reqCode, vod, pTemp));
	}

	// 删除人员
    static int DeletePerson(const std::vector<Uuid> &vod, char *buf)
	{
		int reqCode = REQ_DELETE_PERSON_INFO;
		PNetUuidArray pTemp = (PNetUuidArray)buf;
		return (ArrayDataCmd(reqCode, vod, pTemp));
	}
    
	// 增加人员照片
    // 修改照片编码的方式, modified at 20140530 13:52 by yqhe
	static int AddPersonPhoto(const PersonPhoto &photo, char *buf)
	{
		int reqCode = REQ_ADD_PERSON_PHOTO;
        return (PhotoDataCmd(reqCode, photo, buf, true));
	}
    
	// 修改人员照片
    // 修改照片编码的方式, modified at 20140530 13:52 by yqhe
    static int UpdatePersonPhoto(const PersonPhoto &photo, char *buf)
	{
		int reqCode = REQ_UPDATE_PERSON_PHOTO;
        return (PhotoDataCmd(reqCode, photo, buf, true));
    }

	// 删除人员照片
    static int DeletePersonPhoto(const std::vector<Uuid> &vod, char *buf)
	{
		int reqCode = REQ_DELETE_PERSON_PHOTO;
		PNetUuidArray pTemp = (PNetUuidArray)buf;
		return (ArrayDataCmd(reqCode, vod, pTemp));
	}
    ////////////////////////////////////

    ////////////////////////////////////
    // added at 20140319 by yqhe
	// 增加管理员
    static int AddAdmin(const std::vector<PersonBase> &vod, char *buf)
	{
		int reqCode = REQ_ADD_ADMIN_INFO;
		PNetPersonBaseArray pTemp = (PNetPersonBaseArray)buf;
		return (ArrayDataCmd(reqCode, vod, pTemp));
	}

	// 修改管理员
    static int UpdateAdmin(const std::vector<PersonBase> &vod, char *buf)
	{
		int reqCode = REQ_UPDATE_ADMIN_INFO;
		PNetPersonBaseArray pTemp = (PNetPersonBaseArray)buf;
		return (ArrayDataCmd(reqCode, vod, pTemp));
	}

	// 删除管理员
    static int DeleteAdmin(const std::vector<Uuid> &vod, char *buf)
	{
		int reqCode = REQ_DELETE_ADMIN_INFO;
		PNetUuidArray pTemp = (PNetUuidArray)buf;
		return (ArrayDataCmd(reqCode, vod, pTemp));
	}

	// 增加管理员照片
	static int AddAdminPhoto(const PersonPhoto &photo, char *buf)
	{
		int reqCode = REQ_ADD_ADMIN_PHOTO;
		return (BlockDataCmd(reqCode, photo, buf, true));
	}

	// 修改管理员照片
    static int UpdateAdminPhoto(const PersonPhoto &photo, char *buf)
	{
		int reqCode = REQ_UPDATE_ADMIN_PHOTO;
		return (BlockDataCmd(reqCode, photo, buf, true));
	}

	// 删除管理员照片
    static int DeleteAdminPhoto(const std::vector<Uuid> &vod, char *buf)
	{
		int reqCode = REQ_DELETE_ADMIN_PHOTO;
		PNetUuidArray pTemp = (PNetUuidArray)buf;
		return (ArrayDataCmd(reqCode, vod, pTemp));
	}

	// 增加普通人员虹膜特征编码
    static int AddPersonIris(const std::vector<SyncIrisData> &vod, char *buf)
	{
		int reqCode = REQ_ADD_PERSON_IRIS;
		PNetSyncIrisData pTemp = (PNetSyncIrisData)buf;
		return (ArrayDataCmd(reqCode, vod, pTemp, true));
	}

	// 删除普通人员虹膜特征编码
    static int DeletePersonIris(const std::vector<Uuid> &vod, char *buf)
	{
		int reqCode = REQ_DELETE_PERSON_IRIS;
		PNetUuidArray pTemp = (PNetUuidArray)buf;
		return (ArrayDataCmd(reqCode, vod, pTemp));
	}

	// 增加管理员虹膜特征
    static int AddAdminIris(const std::vector<SyncIrisData> &vod, char *buf)
	{
		int reqCode = REQ_ADD_ADMIN_IRIS;
		PNetSyncIrisData pTemp = (PNetSyncIrisData)buf;
		return (ArrayDataCmd(reqCode, vod, pTemp, true));
	}

	// 删除管理员虹膜特征
    static int DeleteAdminIris(const std::vector<Uuid> &vod, char *buf)
	{
		int reqCode = REQ_DELETE_ADMIN_IRIS;
		PNetUuidArray pTemp = (PNetUuidArray)buf;
		return (ArrayDataCmd(reqCode, vod, pTemp));
	}


	// 上传虹膜注册数据请求编码
    static int UploadPersonIris(const std::vector<UploadIrisData> &vod, char *buf)
	{
		int reqCode = REQ_UPLOAD_PERSON_IRIS;
		PNetUploadIrisDataArray pTemp = (PNetUploadIrisDataArray)buf;
		return (ArrayDataCmd(reqCode, vod, pTemp, true));
	}

	// 上传识别记录请求编码
    static int UploadRecogRecord(const std::vector<RecogRecord> &vod, char *buf)
	{
		int reqCode = REQ_UPLOAD_RECOG_RECORD;
		PNetRecogRecordArray pTemp = (PNetRecogRecordArray)buf;
		return (ArrayDataCmd(reqCode, vod, pTemp));
	}

	// 实时发送识别记录请求编码
    static int SendRecogRecord(const std::vector<RecogRecord> &vod, char *buf)
	{
		int reqCode = REQ_SEND_RECOG_RECORD;
		PNetRecogRecordArray pTemp = (PNetRecogRecordArray)buf;
		return (ArrayDataCmd(reqCode, vod, pTemp));
	}

	// 以图像方式进行集群识别请求编码，此请求发送需要识别的图像序列
	// 识别方式以另一个请求设置
    static int ClusterImageIdentify(const std::vector<ClusterImageItem> &vod, char *buf)
	{
		int reqCode = REQ_CLUSTER_IMAGE_IDENTIFY;
		PNetClusterImageIdenArray pTemp = (PNetClusterImageIdenArray)buf;
		return (ArrayDataCmd(reqCode, vod, pTemp, true));
	}

	////////////////////////////////////

    ////////////////////////////////////
    // added at 20140320 by yqhe
	// 远程控制关机
    static int SrvCtrlShutdown(char *buf)
	{
        int reqCode = REQ_SRVCTRL_SHUTDOWN;
        return (WithoutDataCmd(reqCode, buf));
	}

    // 远程控制调整时间
    // 输入参数：
    // time —— 设置的时间字符串结构，固定长度为g_timeLen（20Bytes）
    //		   时间字符串格式 YYYY-MM-DD hh:mm:ss，不足位数的用0补齐
    static int SrvCtrlAdjustTime(const TimeString &time, char *buf)
	{
        int reqCode = REQ_SRVCTRL_ADJUST_TIME;
        return (BlockDataCmd(reqCode, time, buf));
	}

	// 以特征方式进行集群识别请求编码，此请求发送需要识别的特征序列
	// 识别方式以另一个请求设置
    static int ClusterFeatureIdentify(const std::vector<ClusterFeatureItem> &vod, char *buf)
	{
		int reqCode = REQ_CLUSTER_FEATURE_IDENTIFY;
		PNetClusterFeatureIdenArray pTemp = (PNetClusterFeatureIdenArray)buf;
		return (ArrayDataCmd(reqCode, vod, pTemp, true));
	}

	// 配置以图像方式进行集群识别的控制参数
    static int ClusterImageIdentifyConfig(const ClusterImageIdenParam &clusterIdenParam, char *buf)
	{
        int reqCode = REQ_CLUSTER_IMAGE_IDEN_CONFIG;
        return (BlockDataCmd(reqCode, clusterIdenParam, buf));
	}

	// 配置以特征方式进行集群识别的控制参数
    static int ClusterFeatureIdentifyConfig(const ClusterFeatureIdenParam &clusterIdenParam, char *buf)
	{
        int reqCode = REQ_CLUSTER_IMAGE_IDEN_CONFIG;
        return (BlockDataCmd(reqCode, clusterIdenParam, buf));
	}

    ////////////////////////////////////

    ////////////////////////////////////
    // added at 20140429 by yqhe
    // 同步令牌
    static int SyncToken(const int token, char *buf)
    {
    	int reqCode = REQ_SYNC_TOKEN;
    	return (BlockDataCmd(reqCode, token, buf));
    }

    // 设备向服务器发送本设备基本连接情况
    // 编码REQ_DEV_CONNECT
    static int DevConnect(const DevConnInfo &devConnInfo, char *buf)
    {
    	int reqCode = REQ_DEV_CONNECT;
    	return (BlockDataCmd(reqCode, devConnInfo, buf));
    }

    // 获得人员列表
    static int GetPersonList(char *buf)
    {
    	int reqCode = REQ_GET_PERSON_LIST;
        return (WithoutDataCmd(reqCode, buf));
    }

    // 设置设备IP
    static int SrvCtrlAdjustDevIP(const IpSetting &ipSetting, char *buf)
    {
    	int reqCode = REQ_SRVCTRL_ADJUST_DEVIP;
    	return (BlockDataCmd(reqCode, ipSetting, buf));
    }

    // 设置设备连接服务器IP
    static int SrvCtrlAdjustSrvIP(const ListenIp &listenIp, char *buf)
    {
    	int reqCode = REQ_SRVCTRL_ADJUST_SRVIP;
    	return (BlockDataCmd(reqCode, listenIp, buf));
    }

    // 获得设备信息
    static int GetDevInfo(char *buf)
    {
    	int reqCode = REQ_GET_DEV_INFO;
        return (WithoutDataCmd(reqCode, buf));
    }

    // 获得服务器当前时间
    static int GetSrvTime(char *buf)
    {
    	int reqCode = REQ_GET_SRV_TIME;
        return (WithoutDataCmd(reqCode, buf));
    }
    ////////////////////////////////////

    ////////////////////////////////////
    // added at 20140503 by yqhe
    // 获得设备上指定识别记录
    static int GetRecogRecord(const GetRecogRecord &getRecordCondi, char *buf)
	{
		int reqCode = REQ_GET_RECOG_RECORD;
		return (BlockDataCmd(reqCode, getRecordCondi, buf));
	}
    ////////////////////////////////////

    ////////////////////////////////////
    // added at 20140607 17:16 by yqhe
    // 远程控制重启
    static int SrvCtrlReset(char *buf)
    {
        int reqCode = REQ_SRVCTRL_RESET;
        return (WithoutDataCmd(reqCode, buf));
    }
    ////////////////////////////////////


private:
	EncodeNetData()
    {
    };

	~EncodeNetData()
    {
    };

private:
    /*******************************************************************
    *           编码无数据指令
    * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称：WithoutDataCmd
    * 功    能：编码无数据指令
    * 说    明：
    * 参    数：
    *           输入参数
    *           reqCode —— 请求码
    *           输出参数
    *           buf     —— 已编码的数据
    * 返 回 值：实际编码后数据长度
    * 创 建 人：yqhe
    * 创建日期：2014/3/14 14：02
    * 修 改 人：yqhe
    * 修改日期：2014/5/18 11:24
    * 修改内容：编码带返回值，便于调用者确定发送数据长度
    *******************************************************************/
    static int WithoutDataCmd(int reqCode, char *buf)
    {
    	return EncodeData::WithoutDataCmd(reqCode, buf);
    };

    /*******************************************************************
    *           编码数组格式数据指令
    * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称：ArrayDataCmd
    * 功    能：编码数组格式数据指令
    * 说    明：以变长数组形式存在的数据
    * 参    数：
    *           输入参数
    *           reqCode    —— 请求码
    *           vod        —— 待编码的实际数据
    *           needCheck  —— 编码数据是否需要校验
    *           			  一般特征及图像数据（大数据量的）不需要校验
    *           			  缺省值为true（需要校验）
    *           输出参数
    *           pNetData —— 已填充的网络信息
    *           		    注意，调用这个函数，确保NetT这个模版满足
    *           		    有ArraySize和ArrayData元素
    *           		    ArraySize用于描述有多少条ArrayData
    * 返 回 值：实际编码后数据长度
    * 创 建 人：yqhe
    * 创建日期：2014/3/17 17：02
    *******************************************************************/
    template<typename DataT, typename NetT>
	static int ArrayDataCmd(int reqCode,
			const std::vector<DataT> &vod,
			NetT *pNetData,
			bool needCheck=true)
	{
    	return (EncodeData::ArrayDataCmd(reqCode, vod, pNetData, needCheck));
	}

    /*******************************************************************
    *          编码数据结构格式数据指令
    * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称：BlockDataCmd
    * 功    能：编码数据结构格式数据指令
    * 说    明：此数据结构中无变长数组类数据
    * 参    数：
    *           输入参数
    *           reqCode    —— 请求码
    *           writeData  —— 待编码的实际数据
    *           needCheck  —— 编码数据是否需要校验
    *           			  一般特征及图像数据（大数据量的）不需要校验
    *           			  缺省值为true（需要校验）
    *           输出参数
    *           buf     —— 编码好了的网络传输数据
    * 返 回 值：实际编码后数据长度
    * 创 建 人：yqhe
    * 创建日期：2014/3/18 22：02
    *******************************************************************/
    template<typename DataT>
    static int BlockDataCmd(int reqCode, const DataT& writeData, char* buf, bool needCheck=true)
    {
    	return (EncodeData::BlockDataCmd(reqCode, writeData, buf, needCheck));
    }

    /*******************************************************************
    *          编码人员照片数据指令
    * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称：PhotoDataCmd
    * 功    能：编码人员照片数据指令
    * 说    明：由于照片数据结构较为特殊，所以单独编码
    * 参    数：
    *           输入参数
    *           reqCode    —— 请求码
    *           writeData  —— 待编码的实际数据
    *           needCheck  —— 编码数据是否需要校验
    *           			  一般特征及图像数据（大数据量的）不需要校验
    *           			  缺省值为true（需要校验）
    *           输出参数
    *           buf     —— 编码好了的网络传输数据
    * 返 回 值：实际编码后数据长度
    * 创 建 人：yqhe
    * 创建日期：2014/3/18 22：02
    *******************************************************************/
    static int PhotoDataCmd(int reqCode, const PersonPhoto& writeData, char* buf, bool needCheck = true)
    {
        return (EncodeData::PhotoDataCmd(reqCode, writeData, buf, needCheck));
    }
};


