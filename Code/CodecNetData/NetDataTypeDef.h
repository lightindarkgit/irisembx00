/*
 * NetDataTypeDef.h
 * 网络传输数据编解码需要的常量及枚举定义
 *
 *  Created on: 2014年3月18日
 *      Author: yqhe
 */

#pragma once

#include <string.h>
#include <vector>
#include <list>

///////////////////////////////////////////////////////////
// 常用常量定义
const int g_nameLen         = 64;           // 名称长度，如姓名、部门名称
const int g_snLen           = 64;           // 序号长度，如工号、部门号
const int g_errorLen        = 260;          // 错误说明长度
const int g_memoLen         = 128;          // 补充说明字符串长度
const int g_timeLen         = 20;           // 时间字符串长度
                                            // 时间字符串示例：2013-01-01 00:00:00
const int g_cardNoLen       = 16;           // 卡号长度
const int g_idCardNoLen     = 20;           // 身份证号长度
const int g_versionLen      = 20;           // 版本号长度
const int g_enrollFeatureLen= 512;          // 注册特征长度
const int g_idenFeatureLen  = 1024;         // 识别特征长度
const int g_BMPHead         = 1078;         // 虹膜图像使用的BMP头长度
const int g_imageLen        = 307200;       // 虹膜图像长度，640*480
const int g_colorUsed		= 256;			// 调色板用到的颜色
const int g_ipLen       	= 16;			// IP地址字符串长度

const int g_tokenLen		= 6;			// 令牌种类
const int g_headOccupy		= 10;			// 纯头部占字节数
const int g_respDataStart	= 14;			// 回应数据起始位置
const int g_arraySizeOccupy = 4;			// 数组类型的数据，数组长度占字节数
const int g_checkSumOccupy  = 2;			// 校验占字节数

const int g_ctrlCmdCheckLen = g_headOccupy; // 无数据指令，需要计算校验的数据字节数

const int g_maxOtherOccupy  = 32;           // 除纯数据外，其他信息占用的最多字节数，比实际占用的要多

const int g_successAckLen	= 4;			// 成功响应，只有错误码，占4字节
const int g_errorAckLen		= 264;			// 出错相应，有错误码及错误描述字符串，占264字节

const int g_head0			= 0x53;			// 同步头0
const int g_head1			= 0x53;			// 同步头1
const int g_defaultSubCode  = 0xaaaa;		// 缺省子码

const int g_success			= 0x00;			// 错误码：成功
const int g_error			= -1;			// 错误码：失败，仅用于表示一般性失败
// end of 常用常量定义
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// 常用枚举量定义
// 人员类型
enum class EnumPersonType
{
    ordinaryPerson  = 1,
    adminPerson     = 2
};

// 性别标志
enum class EnumSex
{
	male			= 1,
	female			= 2
};

// 眼睛标志
enum class EnumEye
{
	both			= 0,
	left			= 1,
	right			= 2
};

// 联网虹膜特征处理标志
enum class EnumSyncIris
{
	add				= 1,
	update			= 2,
	del				= 3
};

// 识别结果搜索模式
enum class EnumFindMode
{
	exhaustion		= 0,	// 穷尽所有查找
	stopWhenFind	= 1		// 找到合适的即停止
};

// 设备工作模式
enum class EnumWorkMode
{
	online			= 0,	// 联网工作模式
	offline			= 1		// 脱机工作模式
};

// 设备工作状态
enum class EnumWorkStatus
{
	invalid			= 0,	// 无效状态
	operatorCtrl	= 1,    // 操作员控制状态
	identify		= 2,	// 识别状态
	saving			= 3		// 低功耗状态
};

// 获取识别记录类型
enum class EnumGetRecord
{
	all				= 0,	// 所有符合时间要求的识别记录
	sended			= 1,	// 符合时间要求中的已发送识别记录
	notyet			= 2		// 符合时间要求的，尚未发送的识别记录
};

// 令牌种类分类
enum class EnumTokenType
{
	person			= 0,	// 人员令牌
	feature			= 1,	// 特征令牌
	photo			= 2,	// 照片令牌
	reserved1		= 3,
	reserved2		= 4,
	reserved3		= 5
};
// end of 常用枚举量定义
///////////////////////////////////////////////////////////


