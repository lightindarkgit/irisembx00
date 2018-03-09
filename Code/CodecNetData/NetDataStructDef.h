/*
 * NetDataStructDef.h
 * 网络传输数据结构定义
 *
 *  Created on: 2014年3月18日
 *      Author: yqhe
 */

#pragma once

#include "NetDataTypeDef.h"

#ifdef _WIN32
#include <objbase.h>
#else
#include <uuid/uuid.h>
#endif

#pragma warning (disable:4200)

#pragma pack(push)      // 保存对齐状态
#pragma pack(1)         // 设定为1字节对齐

//头数据结构
typedef struct __HeadData {
	char Head0;                  // Head0和Head1为同步头，固定为0x5353
	char Head1;
	unsigned short RequestCode;
	unsigned short RequestSubCode;
} HeadData, *PHeadData;

//头数据结构1：增加长度
typedef struct __HeadDataHasLen {
	char Head0;
	char Head1;
	unsigned short RequestCode;               //请求码
	unsigned short RequestSubCode;            //请求子码
	int Len;                       //数据区长度=错误码+实际长度
} HeadDataHasLen, *PHeadDataHasLen;

// 头数据结构2（回应）：增加返回值
typedef struct __HeadDataWithErrorCode {
	char Head0;
	char Head1;
	unsigned short RequestCode;               //请求码
	unsigned short RequestSubCode;            //请求子码
	int Len;                       //数据区长度=错误码+实际长度
	int ErrorCode;				   //错误码
} HeadDataWithErrorCode, *PHeadDataWithErrorCode;

//标准命令的结构体---适用于无数据区传送的命令：请求和回应都使用本结构体
//为了方便也处理了一部分命令如下面的重启和心跳
// REQ_KEEP_ALIVE
// REQ_SRVCTRL_RESET
typedef struct __ControlCmd {
	char Head0;
	char Head1;
	unsigned short RequestCode;
	unsigned short RequestSubCode;
	int Len;
	unsigned short Check;
} ControlCmd, *PControlCmd;

// 常用的成功执行回应结构体
typedef struct __NormalSuccessAck {
	char Head0;
	char Head1;
	unsigned short RequestCode;
	unsigned short RequestSubCode;
	int Len;
	int ErrorCode;
	unsigned short Check;
} NormalSuccessAck, *PNormalSuccessAck;

// 常用的出错执行回应结构体
// 增加出错原因
typedef struct __NormalErrorAck {
	char Head0;
	char Head1;
	unsigned short RequestCode;
	unsigned short RequestSubCode;
	int Len;
	int ErrorCode;
	char ErrorString[g_errorLen];
	unsigned short Check;
} NormalErrorAck, *PNormalErrorAck;

// 成功执行，带数据结构的回应结构体，未定义Check，实际填充的时候写入
typedef struct __BlockSuccessAck {
	char Head0;
	char Head1;
	unsigned short RequestCode;
	unsigned short RequestSubCode;
	int Len;
	int ErrorCode;
} BlockSuccessAck, *PBlockSuccessAck;

////////////////////////////////
// added at 20140319 by yqhe
// uuid_t不能直接push到vector中，需要在外面包一层
typedef struct __Uuid {
	uuid_t uuid;
	int OpToken;	// 增加操作令牌, added at 20140509
					// 值由X00服务控制，设备只负责接收、保存
					// 设备发到服务器时，设置为0即可
} Uuid, *PUuid;

typedef struct __NetIntArray {
	HeadDataHasLen Hd;
	int ArraySize;
	int ArrayData[0];
} NetIntArray, *PNetIntArray;

////////////////////////////////

////////////////////////////////
// added at 20140318 by yqhe
// 网络传送UUID数组结构
typedef struct __NetUuidArray {
	HeadDataHasLen Hd;
	int ArraySize;
	Uuid ArrayData[0];
} NetUuidArray, *PNetUuidArray;

////////////////////////////////
// added at 20140429 by yqhe
// 简要人员信息
typedef struct __PersonSimple {
	uuid_t Pid;
	int HasFeature;
	char PersonName[g_nameLen];
	char PersonSn[g_snLen];
} PersonSimple, *PPersonSimple;

// 网络传送简要人员数据结构
typedef struct __NetPersonSimpleArray {
	HeadDataWithErrorCode Hd;

	int ArraySize;
	PersonSimple ArrayData[0];
} NetPersonSimpleArray, *PNetPersonSimpleArray;
////////////////////////////////

// 人员信息
typedef struct __PersonBase {
	uuid_t Pid;
	int OpToken;	// 增加操作令牌, added at 20140429
					// 值由X00服务控制，设备只负责接收、保存
	char PersonName[g_nameLen];
	char PersonSn[g_snLen];
	EnumPersonType PersonType;
	EnumSex PersonSex;
	char IdCardNumber[g_idCardNoLen];
	char DepartName[g_nameLen];
	char CardNumber[g_cardNoLen];
	char Memo[g_memoLen];
} PersonBase, *PPersonBase;

// 网络传送基本人员数据结构
typedef struct __NetPersonBaseArray {
	HeadDataHasLen Hd;
	int ArraySize;
	PersonBase ArrayData[0];
} NetPersonBaseArray, *PNetPersonBaseArray;

// 人员照片
typedef struct __PersonPhoto {
	uuid_t Pid;
	uuid_t PhotoId;
	int OpToken;	// 增加操作令牌, added at 20140429
					// 值由X00服务控制，设备只负责接收、保存
	char Memo[g_memoLen];
	int PhotoSize;
	char Photo[0];
} PersonPhoto, *PPersonPhoto;

// 网络传送人员照片 数据结构
typedef struct __NetPersonPhoto {
	HeadDataHasLen Hd;
	char Photo[0];
} NetPersonPhoto, *PNetPersonPhoto;

////////////////////////////////

//数据传输部分----从服务器到客户端用ADD，从客户端到服务器用GET开头;两个方向一致的，不带有这两个头。
//虹膜数据结构体
// deleted at 200140429 by yqhe
// 去掉联网虹膜、脱机虹膜的区别，两者统一
// 由X00服务发往设备的虹膜特征数据结构体
typedef struct __SyncIrisData {
	uuid_t Fid;
	uuid_t Pid;
	int OpToken;	// 增加操作令牌, added at 20140429
					// 值由X00服务控制，设备只负责接收、保存
	EnumEye EyeFlag;
	unsigned char FeatureData[g_enrollFeatureLen];
} SyncIrisData, *PSyncIrisData;

// 联网虹膜特征网络传输数据结构
// 最后两位的校验最后加上
typedef struct __NetSyncIrisData {
	HeadDataHasLen Hd;
	int ArraySize;
	SyncIrisData ArrayData[0];
} NetSyncIrisData, *PNetSyncIrisData;

// BMP文件头
// 12Bytes
typedef struct __BmpFileHeader {
	unsigned short type;		//  位图文件类型'BM'
	int bfSize;
	short bfReserved1;
	short bfReserved2;
	int bfOffBist;
} BmpFileHeader, *PBmpFileHeader;

// BMP信息头
// 40Bytes
typedef struct __BmpInfoHeader {
	int biSize;
	int biWidth;
	int biHeight;
	short biPlanes;
	short biBitCount;
	int biCompression;
	int biImageSize;
	int biXPelsPerMeter;
	int biYPelsPerMeter;
	int biClrUsed;
	int biClrImportant;
} BmpInfoHeader, *PBmpInfoHeader;

// 颜色表项
// 4Bytes
typedef struct __RGBQuad {
	unsigned char rgbBlue;
	unsigned char rgbGreen;
	unsigned char rgbRed;
	unsigned char rgbReserved;
} RGBQuad, *PRGBQuad;

// BMP信息
// 1064Bytes
typedef struct __BmpInfo {
	BmpInfoHeader bmiHeader;
	RGBQuad bmiColors[g_colorUsed];
} BmpInfo, *PBmpInfo;

// 虹膜图像，带有BMP头
// 308278Bytes
typedef struct __IrisImageData {
	BmpFileHeader bmiFileHeader;
	BmpInfo bimInfo;
	unsigned char irisImageRaw[g_imageLen];
} IrisImageData, *PIrisImageData;

// 单幅注册结果，包括特征、图像和一些分数类计算结果
// 309862Bytes
typedef struct __IrisEnrollInfo {
	int pupilRow;
	int pupilCol;
	int pupilRadius;
	int irisRow;
	int irisCol;
	int irisRadius;
	int focusScore;
	int percentVisible;
	int spoofValue;
	int interlaceValue;
	int qualityLevel;
	int qualityScore;
	unsigned char EnrollTemplate[g_enrollFeatureLen];
	unsigned char RecogTemplate[g_idenFeatureLen];
	IrisImageData IrisImage;		// 带调色板的BMP格式数据
} IrisEnrollInfo, *PIrisEnrollInfo;

// 上传单幅虹膜注册数据
// 309982Bytes
typedef struct __UploadIrisData {
	char DeviceSn[g_snLen];
	uuid_t Fid;
	uuid_t Pid;
	EnumEye EyeFlag;
	char EnrollTime[g_timeLen];
	IrisEnrollInfo IrisEnrollData;
} UploadIrisData, *PUploadIrisData;

// 联网上传虹膜注册数据网络传输数据结构
// 最后两位的校验最后加上
typedef struct __NetUploadIrisDataArray {
	HeadDataHasLen Hd;
	int ArraySize;
	UploadIrisData ArrayData[0];
} NetUploadIrisDataArray, *PNetUploadIrisDataArray;

// 识别记录
typedef struct __RecogRecord {
	char DeviceSn[g_snLen];
	uuid_t Fid;					// 特征ID
	uuid_t Pid;					// 人员ID
	char Name[g_nameLen];		// 人员姓名
	int RIndex;					// 识别记录序列号
	EnumEye EyeFlag;
	char IdenTime[g_timeLen];	// 识别时间
} RecogRecord, *PRecogRecord;

// 识别记录网络传输数据结构
// 最后两位的校验最后加上
typedef struct __NetRecogRecordArray {
	HeadDataHasLen Hd;
	int ArraySize;
	RecogRecord ArrayData[0];
} NetRecogRecordArray, *PNetRecogRecordArray;

// 图像集群识别单条数据
// 308286Bytes
typedef struct __ClusterImageItem {
	int Index;
	EnumEye EyeFlag;
	IrisImageData IrisImage;		// 带调色板的BMP格式数据
} ClusterImageItem, *PClusterImageItem;

// 图像集群识别数据，网络传输结构
typedef struct __NetClusterImageIdenArray {
	HeadDataHasLen Hd;
	int ArraySize;
	ClusterImageItem ArrayData[0];
} NetClusterImageIdenArray, *PNetClusterImageIdenArray;

// 图像集群识别，识别控制参数
typedef struct __ClusterImageIdenParam {
	EnumFindMode FindMode;
	int MaxMatchNum;
} ClusterImageIdenParam, *PClusterImageIdenParam;

// 特征集群识别单条数据
// 520Bytes
typedef struct __ClusterFeatureItem {
	int Index;
	EnumEye EyeFlag;
	unsigned char RecogTemplate[g_idenFeatureLen];	// 用于识别的特征
} ClusterFeatureItem, *PClusterFeatureItem;

// 特征集群识别数据，网络传输结构
typedef struct __NetClusterFeatureIdenArray {
	HeadDataHasLen Hd;
	int ArraySize;
	ClusterFeatureItem ArrayData[0];
} NetClusterFeatureIdenArray, *PNetClusterFeatureIdenArray;

// 特征集群识别，识别控制参数
typedef struct __ClusterFeatureIdenParam {
	EnumFindMode FindMode;
	int MaxMatchNum;
	char AlgVersion[g_versionLen];
} ClusterFeatureIdenParam, *PClusterFeatureIdenParam;

// 集群识别结果
typedef struct __ClusterIdenResult {
	int Index;
	uuid_t Pid;
	char PersonName[g_nameLen];
	char PersonSn[g_snLen];
	int Score;
} ClusterIdenResult, PClusterIdenResult;

// 集群识别结果，网络传输结构
// 此为回应数据，因此有ErrorCode字段
typedef struct __NetClusterIdenResultArray {
	HeadDataWithErrorCode Hd;
	int ArraySize;
	ClusterIdenResult ArrayData[0];
} NetClusterIdenResultArray, *PNetClusterIdenResultArray;

// 时间设置
typedef struct __TimeString {
	char TimeStr[g_timeLen];
} TimeString, *PTimeString;

/////////////////////////
// added at 20140429 by yqhe
// ip地址设置
typedef struct __IpSetting {
	char Ip[g_ipLen];			// 设置IP
	char SubnetMask[g_ipLen];	// 子网掩码
	char GateWay[g_ipLen];		// 网关
} IpSetting, *PIpSetting;

// 侦听地址设置
typedef struct __ListenIp {
	char Ip[g_ipLen];			// 侦听地址
	int Port;					// 侦听端口
} ListenIp, *PListenIp;

// 设备信息
typedef struct __DeviceInfo {
	char Ip[g_ipLen];
	EnumWorkMode Mode;
	EnumWorkStatus Status;
	char DeviceSn[g_snLen];
	char Version[g_versionLen];
	char TimeStr[g_timeLen];
} DeviceInfo, *PDeviceInfo;
/////////////////////////

/////////////////////////
// added at 20140503 by yqhe
// 获取指定识别记录的条件
typedef struct __GetRecogRecord {
	EnumGetRecord getType;
	int startTimeValid;
	int endTimeValid;
	char startTimeStr[g_timeLen];
	char endTimeStr[g_timeLen];
} GetRecogRecord, *PGetRecogRecord;

typedef struct __NetRecogRecordRespArray {
	HeadDataWithErrorCode Hd;
	int ArraySize;
	RecogRecord ArrayData[0];
} NetRecogRecordRespArray, *PNetRecogRecordRespArray;
/////////////////////////

/////////////////////////
// added at 20140512 by yqhe
// 设备连接基本参数
typedef struct __DevConnInfo {
	char Ip[g_ipLen];			// 设备IP
	char DeviceSn[g_snLen];
	int Tokens[g_tokenLen];
} DevConnInfo, *PDevConnInfo;
/////////////////////////

#pragma pack(pop)       // 恢复对齐状态

// 内部使用数据结构体，非网络传输用
// 普通回应
typedef struct __NormalAck {
	int ErrorCode;
	char ErrorString[g_errorLen];

	__NormalAck() {
		ErrorCode = g_error;		// 缺省值为一般性失败
		ErrorString[0] = '\0';
	}
	;

	// 通过网络正确回应设置数据
	void Set(const NormalSuccessAck &ack) {
		ErrorCode = ack.ErrorCode;
	}

	void Set(const NormalErrorAck &ack) {
		ErrorCode = ack.ErrorCode;
		// Windows下推荐使用strncpy_s，因此修改使用的函数
		// strncpy(ErrorString, ack.ErrorString, g_errorLen);
		std::copy(ack.ErrorString, ack.ErrorString + g_errorLen, ErrorString);
	}
} NormalAck, *PNormalAck;

