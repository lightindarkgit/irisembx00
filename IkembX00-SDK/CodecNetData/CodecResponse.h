/*
 * CodecResponse.h
 * 回应编解码
 *
 *  Created on: 2014年3月18日
 *      Author: yqhe
 */

#pragma once

#include "CodecUtil.h"

// 回应编码
// 某方收到请求后，发回对方的回应
// 此类为对这些回应的编码
class EncodeResponse
{
private:
	EncodeResponse()
    {
    };

	~EncodeResponse()
    {
    };

private:
    /*******************************************************************
    *         编码成功回应
    * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称：Success
    * 功    能：编码成功回应
    * 说    明：
    * 参    数：
    *           输入参数
    *           reqCode —— 请求码
    *           输出参数
    *           buf     —— 已编码的数据
    * 返 回 值：实际编码后数据长度
    * 创 建 人：yqhe
    * 创建日期：2014/3/18 14：07
    * 修 改 人：yqhe
    * 修改日期：2014/5/18 11:24
    * 修改内容：编码带返回值，便于调用者确定发送数据长度
    *******************************************************************/
    static int Success(int reqCode, char *buf)
    {
		// 设置编码数据中的头数据
		// 数据长度为0，使用缺省值
		EncodeNetHeadData::Set(reqCode, g_successAckLen, buf);

    	PNormalSuccessAck pAck = (PNormalSuccessAck)buf;

    	pAck->ErrorCode = g_success;

		// 计算校验
		//校验：去除校验两个字节后的整个帧
        int checkLen = g_headOccupy + g_successAckLen;
		pAck->Check = CheckData::Calc(buf, checkLen);

        return (checkLen + g_checkSumOccupy);

    };

    static int Error(int reqCode, const NormalAck &ack, char *buf)
    {
		// 设置编码数据中的头数据
		EncodeNetHeadData::Set(reqCode, g_errorAckLen, buf);

		PNormalErrorAck pAck = (PNormalErrorAck)buf;

		// 设置错误码
		pAck->ErrorCode = ack.ErrorCode;

		// 设置错误提示信息
		// Windows下推荐使用strncpy_s，因此修改使用的函数
		// strncpy(pAck->ErrorString, ack.ErrorString, g_errorLen);
		std::copy(ack.ErrorString, ack.ErrorString+g_errorLen, pAck->ErrorString);

		// 计算校验
		//校验：去除校验两个字节后的整个帧
        int checkLen = g_headOccupy + g_errorAckLen;
		pAck->Check = CheckData::Calc(buf, checkLen);

        return (checkLen + g_checkSumOccupy);
    };

    static int NormalResponse(int reqCode, const NormalAck &ack, char* buf)
    {
    	if (ack.ErrorCode==g_success)
    	{
    		return (Success(reqCode, buf));
    	}
    	else
    	{
    		return (Error(reqCode, ack, buf));
    	}
    }

    /*******************************************************************
    *          编码数组格式数据回应
    * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称：ComplexArrayResponse
    * 功    能：编码数组格式数据回应
    * 说    明：以变长数组形式存在的数据
    * 		   复杂回应，例如集群识别结果
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
    * 创建日期：2014/3/20 12：05
    * 修 改 人：yqhe
    * 修改日期：2014/5/18 11:24
    * 修改内容：编码带返回值，便于调用者确定发送数据长度
    *******************************************************************/
    template<typename DataT, typename NetT>
    static int ComplexArrayResponse(int reqCode,
			const std::vector<DataT> &vod,
			NetT *pNetData,
			bool needCheck=true)
    {
    	return (EncodeData::ArrayDataResponse(reqCode, vod, pNetData, needCheck));
    }

    /*******************************************************************
    *          编码数据结构格式数据回应
    * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称：BlockDataResponse
    * 功    能：编码数据结构格式数据回应
    * 说    明：此数据结构中无变长数组类数据，而是一个定长的数据结构
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
    * 创建日期：2014/4/30 08：23
    *******************************************************************/
    template<typename DataT>
    static int BlockDataResponse(int reqCode,
    		const DataT& writeData,
    		char* buf,
    		bool needCheck=true)
    {
    	return (EncodeData::BlockDataResponse(reqCode, writeData, buf, needCheck));
    }

public:
    // 响应远程重启请求
    static int RemoteReset(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_SRVCTRL_RESET;

    	return (NormalResponse(reqCode, ack, buf));
    }

    // 响应心跳请求
    static int KeepAlive(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_KEEP_ALIVE;

    	return (NormalResponse(reqCode, ack, buf));
    }

    ////////////////////////////////////
    // added at 20140318 by yqhe
    // 响应增加人员请求编码
    static int AddPerson(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_ADD_PERSON_INFO;

    	return (NormalResponse(reqCode, ack, buf));
    }

    // 响应修改人员请求编码
    static int UpdatePerson(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_UPDATE_PERSON_INFO;

    	return (NormalResponse(reqCode, ack, buf));
    }

    // 响应删除人员请求编码
    static int DeletePerson(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_DELETE_PERSON_INFO;

    	return (NormalResponse(reqCode, ack, buf));
    }

    // 响应增加人员照片请求编码
    static int AddPersonPhoto(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_ADD_PERSON_PHOTO;

    	return (NormalResponse(reqCode, ack, buf));
    }

    // 响应修改人员照片请求编码
    static int UpdatePersonPhoto(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_UPDATE_PERSON_PHOTO;

    	return (NormalResponse(reqCode, ack, buf));
    }

    // 响应删除人员照片请求编码
    static int DeletePersonPhoto(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_DELETE_PERSON_PHOTO;

    	return (NormalResponse(reqCode, ack, buf));
    }

    ////////////////////////////////////

    ////////////////////////////////////
    // added at 20140319 by yqhe
    // 响应增加管理员请求编码
    static int AddAdmin(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_ADD_ADMIN_INFO;

    	return (NormalResponse(reqCode, ack, buf));
    }

    // 响应修改管理员请求编码
    static int UpdateAdmin(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_UPDATE_ADMIN_INFO;

    	return (NormalResponse(reqCode, ack, buf));
    }

    // 响应删除管理员请求编码
    static int DeleteAdmin(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_DELETE_ADMIN_INFO;

    	return (NormalResponse(reqCode, ack, buf));
    }

    // 响应增加管理员照片请求编码
    static int AddAdminPhoto(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_ADD_ADMIN_PHOTO;

    	return (NormalResponse(reqCode, ack, buf));
    }

    // 响应修改管理员照片请求编码
    static int UpdateAdminPhoto(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_UPDATE_ADMIN_PHOTO;

    	return (NormalResponse(reqCode, ack, buf));
    }

    // 响应删除管理员照片请求编码
    static int DeleteAdminPhoto(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_DELETE_ADMIN_PHOTO;

    	return (NormalResponse(reqCode, ack, buf));
    }

    // 响应增加特征请求
    static int AddPersonIris(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_ADD_PERSON_IRIS;

    	return (NormalResponse(reqCode, ack, buf));
    }

    // 响应删除特征请求编码
    static int DeletePersonIris(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_DELETE_PERSON_IRIS;

    	return (NormalResponse(reqCode, ack, buf));
    }

    // 响应增加管理员特征请求编码
    static int AddAdminIris(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_ADD_ADMIN_IRIS;

    	return (NormalResponse(reqCode, ack, buf));
    }

    // 响应删除管理员特征请求编码
    static int DeleteAdminIris(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_DELETE_ADMIN_IRIS;

    	return (NormalResponse(reqCode, ack, buf));
    }

    // 响应上传虹膜注册数据请求编码
    static int UploadPersonIris(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_UPLOAD_PERSON_IRIS;

    	return (NormalResponse(reqCode, ack, buf));
    }

    // 响应上传识别记录请求编码
    static int UploadRecogRecord(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_UPLOAD_RECOG_RECORD;

    	return (NormalResponse(reqCode, ack, buf));
    }

    // 响应实时发送识别记录请求编码
    static int SendRecogRecord(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_SEND_RECOG_RECORD;

    	return (NormalResponse(reqCode, ack, buf));
    }

    ////////////////////////////////////

    ////////////////////////////////////
    // added at 20140320 by yqhe
    // 响应远程关机请求编码
    static int SrvCtrlShutdown(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_SRVCTRL_SHUTDOWN;

    	return (NormalResponse(reqCode, ack, buf));
    }

    ////////////////////////////////////
    // added at 20140607 17:16 by yqhe
    // 响应远程远程控制重启请求编码
    static int SrvCtrlReset(const NormalAck &ack, char* buf)
    {
        int reqCode = ACK_SRVCTRL_RESET;
        return (NormalResponse(reqCode, ack, buf));
    }
    ////////////////////////////////////

    // 响应远程调整时间请求编码
    static int SrvCtrlAdjustTime(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_SRVCTRL_ADJUST_TIME;

    	return (NormalResponse(reqCode, ack, buf));
    }

    // 响应集群图像识别请求编码
    // 如果错误码=g_success，则把结果编码到回应中
    // 否则编码错误回应
    static int ClusterImageIdentify(const NormalAck &ack,
    		const std::vector<ClusterIdenResult> &vod,
    		char* buf)
    {
    	int reqCode = ACK_CLUSTER_IMAGE_IDENTIFY;

    	if (g_success==ack.ErrorCode)
    	{
    		PNetClusterIdenResultArray pTemp = (PNetClusterIdenResultArray)buf;
    		return (ComplexArrayResponse(reqCode, vod, pTemp, true));
    	}

    	// 返回错误提示
    	return (NormalResponse(reqCode, ack, buf));
    }

    // 响应集群特征识别请求编码
    // 如果错误码=g_success，则把结果编码到回应中
    // 否则编码错误回应
    static int ClusterFeatureIdentify(const NormalAck &ack,
    		const std::vector<ClusterIdenResult> &vod,
    		char* buf)
    {
    	int reqCode = ACK_CLUSTER_FEATURE_IDENTIFY;

    	if (g_success==ack.ErrorCode)
    	{
    		PNetClusterIdenResultArray pTemp = (PNetClusterIdenResultArray)buf;
    		return (ComplexArrayResponse(reqCode, vod, pTemp, true));
    	}

    	// 返回错误提示
    	return (NormalResponse(reqCode, ack, buf));
    }

    // 响应集群图像识别，控制识别配置请求编码
    static int ClusterImageIdentifyConfig(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_CLUSTER_IMAGE_IDEN_CONFIG;

    	return (NormalResponse(reqCode, ack, buf));
    }

    // 响应集群特征识别，控制识别配置请求编码
    static int ClusterFeatureIdentifyConfig(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_CLUSTER_FEATURE_IDEN_CONFIG;

    	return (NormalResponse(reqCode, ack, buf));
    }

    ////////////////////////////////////


    ////////////////////////////////////
    // added at 20140430 by yqhe
    // 同步令牌
    static int SyncToken(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_SYNC_TOKEN;
    	return (NormalResponse(reqCode, ack, buf));
    }

    // 设备向服务端发送设备连接参数
    // 此函数是对请求回应的编码
    static int DevConnect(const NormalAck &ack,
    		char* buf)
    {
    	int reqCode = ACK_DEV_CONNECT;
    	// 返回回应
    	return (NormalResponse(reqCode, ack, buf));
    }

    // 获得人员列表
    static int GetPersonList(const NormalAck &ack,
    		const std::vector<PersonSimple> &vod,
    		char* buf)
    {
    	int reqCode = ACK_GET_PERSON_LIST;

    	if (g_success==ack.ErrorCode)
    	{
    		PNetPersonSimpleArray pTemp = (PNetPersonSimpleArray)buf;
    		return (ComplexArrayResponse(reqCode, vod, pTemp, true));
    	}

    	// 返回错误提示
    	return (NormalResponse(reqCode, ack, buf));
    }

    // 设置设备IP
    static int SrvCtrlAdjustDevIP(const NormalAck &ack, char* buf)
    {
    	int reqCode = ACK_SRVCTRL_ADJUST_DEVIP;
    	return (NormalResponse(reqCode, ack, buf));
    }

    // 设置设备连接服务器IP
    static int SrvCtrlAdjustSrvIP(const NormalAck &ack, char* buf)
    {
    	int reqCode = REQ_SRVCTRL_ADJUST_SRVIP;
    	return (NormalResponse(reqCode, ack, buf));
    }

    // 获得设备信息
    static int GetDevInfo(const NormalAck &ack,
    		const DeviceInfo &devInfo,
    		char *buf)
    {
    	int reqCode = ACK_GET_DEV_INFO;

    	if (g_success==ack.ErrorCode)
    	{
    		// 返回正确，及设备信息
    		return (BlockDataResponse(reqCode, devInfo, buf));
    	}

    	// 返回错误提示
    	return (NormalResponse(reqCode, ack, buf));
    }

    // 获得服务器当前时间
    static int GetSrvTime(const NormalAck &ack,
    		const TimeString &timeStr,
    		char *buf)
    {
    	int reqCode = ACK_GET_SRV_TIME;

    	if (g_success==ack.ErrorCode)
    	{
    		// 返回正确，及设备信息
    		return (BlockDataResponse(reqCode, timeStr, buf));
    	}

    	// 返回错误提示
    	return (NormalResponse(reqCode, ack, buf));
    }
    ////////////////////////////////////

    ////////////////////////////////////
    // added at 20140503 by yqhe
    // 获得设备上指定识别记录
    static int GetRecogRecord(const NormalAck &ack,
    		const std::vector<RecogRecord> &vod,
    		char *buf )
	{
    	int reqCode = ACK_GET_RECOG_RECORD;
    	if (g_success==ack.ErrorCode)
    	{
    		PNetRecogRecordRespArray pTemp = (PNetRecogRecordRespArray)buf;
    		return (ComplexArrayResponse(reqCode, vod, pTemp, true));
    	}

    	// 返回错误提示
    	return (NormalResponse(reqCode, ack, buf));
	}
    ////////////////////////////////////

}; // end of EncodeResponse


// 回应解码
// 某方发出请求后，收到对方的回应
// 此类为对这些回应的解码
// 调用函数返回值为false，表示解码不正确，解码数据不可使用
class DecodeResponse
{
private:
	DecodeResponse()
    {
    };

	~DecodeResponse()
    {
    };

private:

    /*******************************************************************
    *          解码网络无数据回应
    * 版权所有： 北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称： NormalResponse
    * 功    能：解码网络无数据回应
    * 说    明：
    * 参    数：
    * 			输入参数
    * 			buf —— 需要解码的网络数据
    * 			输出参数
    * 			pAck —— 解码后的回应数据
    * 返 回 值：true —— 解码成功；false —— 解码失败
    * 创 建 人：yqhe
    * 创建日期：2014/3/18 13：35
    *******************************************************************/
	static bool NormalResponse(const char *buf, NormalAck &ack)
	{
		return DecodeData::WithoutDataResponse(buf, ack);
	}

    /*******************************************************************
    *          解码数据为数组型的网络数据回应(ACK_XXX)
    * 版权所有： 北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称： ArrayDataResponse
    * 功    能：解码数据为数组型的网络数据回应
    * 说    明：
    * 参    数：
    * 			输入参数
    * 			pNetData—— 需要解码的网络数据
    *           输出参数
    * 			ack     —— 解码后的回应一般格式
    *           vod     —— 已解码的数组型数据
    * 返 回 值：true —— 解码成功；false —— 解码失败
    * 创 建 人：yqhe
    * 创建日期：2014/5/03 11：12
    *******************************************************************/
    template<typename DataT, typename NetT>
	static bool ArrayDataResponse(
			const NetT *pNetData,
			NormalAck &ack,
			std::vector<DataT> &vod,
			bool needCheck=true)
	{
    	return DecodeData::ArrayDataResponse(pNetData, ack, vod, needCheck);
	}

    /*******************************************************************
    *          解码有数据的结构型网络数据回应(ACK_XXX)
    * 版权所有： 北京中科虹霸科技有限公司 (IrisKing Inc.)
    * 函数名称： BlockDataResponse
    * 功    能：解码有数据的结构型网络数据回应(ACK_XXX)
    * 说    明：
    * 参    数：
    * 			输入参数
    * 			pNetData—— 需要解码的网络数据
    *           输出参数
    *           readData—— 已解码的数据
    * 返 回 值：true —— 解码成功；false —— 解码失败
    * 创 建 人：yqhe
    * 创建日期：2014/5/3 11：42
    *******************************************************************/
    template<typename DataT>
	static bool BlockDataResponse(const char *buf,
			NormalAck &ack,
			DataT &readData,
			bool needCheck=true)
	{
    	return DecodeData::BlockDataResponse(buf, ack, readData, needCheck);
	}

public:
    // 远程重启请求回应解码
    static bool RemoteReset(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    // 心跳请求回应解码
    static bool KeepAlive(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    ////////////////////////////////////
    // added at 20140318 by yqhe
    // 增加人员请求回应解码
    static bool AddPerson(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    // 修改人员请求回应解码
    static bool UpdatePerson(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    // 删除人员请求回应解码
    static bool DeletePerson(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    // 增加人员照片请求回应解码
    static bool AddPersonPhoto(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    // 修改人员照片请求回应解码
    static bool UpdatePersonPhoto(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    // 删除人员照片请求回应解码
    static bool DeletePersonPhoto(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    ////////////////////////////////////

    ////////////////////////////////////
    // added at 20140319 by yqhe
    // 增加管理员请求回应解码
    static bool AddAdmin(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    // 修改管理员请求回应解码
    static bool UpdateAdmin(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    // 删除管理员请求回应解码
    static bool DeleteAdmin(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    // 增加管理员照片请求回应解码
    static bool AddAdminPhoto(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    // 修改管理员照片请求回应解码
    static bool UpdateAdminPhoto(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    // 删除管理员照片请求回应解码
    static bool DeleteAdminPhoto(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    // 增加人员特征请求回应解码
    static bool AddPersonIris(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    // 删除人员特征请求回应解码
    static bool DeletePersonIris(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    // 增加管理员特征请求回应解码
    static bool AddAdminIris(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    // 删除管理员特征请求回应解码
    static bool DeleteAdminIris(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    // 上传虹膜注册数据请求回应解码
    static bool UploadPersonIris(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    // 上传识别记录回应解码
    static bool UploadRecogRecord(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    // 实时发送识别记录回应解码
    static bool SendRecogRecord(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    ////////////////////////////////////

    ////////////////////////////////////
    // added at 20140320 by yqhe
    // 远程控制关机请求回应解码
    static bool SrvCtrlShutdown(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    ////////////////////////////////////
    // added at 20140607 17:18 by yqhe
    // 远程控制重启请求回应解码
    static bool SrvCtrlReset(const char*buf, NormalAck &ack)
    {
        return (NormalResponse(buf, ack));
    }
    ////////////////////////////////////

    // 远程控制调整时间请求回应解码
    static bool SrvCtrlAdjustTime(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    // 以图像方式进行集群识别请求回应解码
	// 如果回应表示执行正确，则可能会有多条识别结果组成数组
	// 如果回应表示执行出错，则只有错误码和错误原因
	// 因此，调用此函数后，需要先检查输出参数ack中的错误码
    static bool ClusterImageIdentify(const char*buf,
    		NormalAck &ack,
    		std::vector<ClusterIdenResult> &vecIdenResult)
    {
    	PNetClusterIdenResultArray pResultResponse = (PNetClusterIdenResultArray)buf;

    	return (ArrayDataResponse(pResultResponse, ack, vecIdenResult, true));
    }

    // 以特征方式进行集群识别请求回应解码
	// 如果回应表示执行正确，则可能会有多条识别结果组成数组
	// 如果回应表示执行出错，则只有错误码和错误原因
	// 因此，调用此函数后，需要先检查输出参数ack中的错误码
    static bool ClusterFeatureIdentify(const char*buf,
    		NormalAck &ack,
    		std::vector<ClusterIdenResult> &vecIdenResult)
    {
    	PNetClusterIdenResultArray pResultResponse = (PNetClusterIdenResultArray)buf;

    	return (ArrayDataResponse(pResultResponse, ack, vecIdenResult, true));
    }

    // 以图像方式进行集群识别，识别控制参数请求回应解码
    static bool ClusterImageIdentifyConfig(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    // 以特征方式进行集群识别，识别控制参数请求回应解码
    static bool ClusterFeatureIdentifyConfig(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    ////////////////////////////////////

    ////////////////////////////////////
    // added at 20140430 by yqhe
    // 设备发到服务器的同步令牌请求回应解码
    // 解码ACK_SYNC_TOKEN
    static bool SyncToken(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    //
    // 解码ACK_DEV_CONNECT
    static bool DevConnect(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    // 解码获得人员列表请求回应
	// 解码ACK_GET_PERSON_LIST
    static bool GetPersonList (const char*buf,
    		NormalAck &ack,
    		std::vector<PersonSimple> &vecPerson)
    {
    	PNetPersonSimpleArray pPersonArray = (PNetPersonSimpleArray)buf;

    	return (ArrayDataResponse(pPersonArray, ack, vecPerson, true));
    }

    // 解码设置设备IP请求回应
	static bool SrvCtrlAdjustDevIP(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

    // 解码设置设备连接服务器IP请求回应
	static bool SrvCtrlAdjustSrvIP(const char*buf, NormalAck &ack)
    {
    	return (NormalResponse(buf, ack));
    }

	// 解码获得设备信息请求回应
    static bool GetDevInfo(const char*buf,
    		NormalAck &ack,
    		DeviceInfo &devInfo)
    {
    	return BlockDataResponse(buf, ack, devInfo);
    }

    // 解码获得服务器当前时间请求回应
    static bool GetSrvTime(const char*buf,
    		NormalAck &ack,
    		TimeString &timeStr)
    {
    	return BlockDataResponse(buf, ack, timeStr);
    }
    ////////////////////////////////////

    ////////////////////////////////////
    // added at 20140503 by yqhe
    // 获得设备上指定识别记录
    static bool GetRecogRecord(const char *buf,
    		NormalAck &ack,
    		std::vector<RecogRecord> &vecRecord
    		 )
	{
    	PNetRecogRecordRespArray pRecogRecordArray = (PNetRecogRecordRespArray)buf;

    	return (ArrayDataResponse(pRecogRecordArray, ack, vecRecord, true));
	}
    ////////////////////////////////////

}; // end of DecodeResponse


