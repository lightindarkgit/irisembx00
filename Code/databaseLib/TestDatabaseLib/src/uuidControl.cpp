/******************************************************************************************
** 文件名:   UuidControl.cpp
×× 主要类:   UuidControl
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2014-01-19
** 修改人:
** 日  期:
** 描  述: uuid类
** ×× 主要模块说明：
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/

#include "uuidControl.h"

UuidControl::UuidControl()
{
	// TODO Auto-generated constructor stub

}

UuidControl::~UuidControl()
{
	// TODO Auto-generated destructor stub
}
/*****************************************************************************
*                         创建UUID---字符串形式
*  函 数 名：GetUUID
*  功    能：创建UUID字符串并转换成std::string
*  说    明：
*  参    数：	ud:UUID字符串
*  返 回 值：null
*  创 建 人：fjf
*  创建时间：2014-02-11
*  修 改 人：
*  修改时间：
*****************************************************************************/
std::string  UuidControl::GetUUIDString()
{
	uuid_t ud;
	uuid_generate(ud);
	return UUIDToString(ud);
}
/*****************************************************************************
*                         创建UUID
*  函 数 名：GetUUID
*  功    能：创建UUID字符串
*  说    明：
*  参    数：	ud:UUID字符串
*  返 回 值：null
*  创 建 人：fjf
*  创建时间：2014-02-11
*  修 改 人：
*  修改时间：
*****************************************************************************/
void UuidControl::GetUUID(uuid_t ud)
{
	uuid_generate(ud);
	return ;
}
/*****************************************************************************
*                         UUID数组转字符串
*  函 数 名：UUIDToString
*  功    能：将ID数组16个字节转字符串
*  说    明：
*  参    数：strUUID:街坊转换的字符串
*  			uuid:转换的字符串指针（16字节长度的数组）
*  返 回 值：是否成功
*  创 建 人：fjf
*  创建时间：2014-02-11
*  修 改 人：
*  修改时间：
*****************************************************************************/
std::string UuidControl::UUIDToPrintString(uuid_t ud)
{
	char buf[64] = {0};
	GUID guid;
    memcpy(guid.ud, ud, 16);
#ifdef __GNUC__
	snprintf(
#else
	_snprintf_s(
#endif
			buf,
			sizeof(buf),
			"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
			guid.gd.Data1, guid.gd.Data2, guid.gd.Data3,
			guid.gd.Data4[0], guid.gd.Data4[1],
			guid.gd.Data4[2], guid.gd.Data4[3],
			guid.gd.Data4[4], guid.gd.Data4[5],
			guid.gd.Data4[6], guid.gd.Data4[7]);

     return std::string(buf);
}

std::string UuidControl::UUIDToString(uuid_t ud)
{
	char buf[64] = {0};
	GUID guid;
#ifdef __GNUC__
	snprintf(
#else
	_snprintf_s(
#endif
			buf,
			sizeof(buf),
			"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
			guid.gd.Data1, guid.gd.Data2, guid.gd.Data3,
			guid.gd.Data4[0], guid.gd.Data4[1],
			guid.gd.Data4[2], guid.gd.Data4[3],
			guid.gd.Data4[4], guid.gd.Data4[5],
			guid.gd.Data4[6], guid.gd.Data4[7]);

     return std::string(buf);
}
/*****************************************************************************
*                         UUID字符串转数组
*  函 数 名：StringToUUIDCharArray
*  功    能：将ID字符串转数组16个字节
*  说    明：
*  参    数：strUUID:街坊转换的字符串
*  			uuid:转换的字符串指针（16字节长度的数组）
*  返 回 值：是否成功
*  创 建 人：fjf
*  创建时间：2014-01-20
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool UuidControl::StringToUUIDCharArray(const std::string &strUUID,unsigned char*uuid)
{
	bool bRet = false;
	memmove(uuid,strUUID.c_str(),16);
	return bRet;
}


