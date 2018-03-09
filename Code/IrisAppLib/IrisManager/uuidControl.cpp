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
#include <stdio.h>
#include "../../Common/Exectime.h"

UuidControl::UuidControl()
{
	Exectime etm(__FUNCTION__);
	// TODO Auto-generated constructor stub

}

UuidControl::~UuidControl()
{
	Exectime etm(__FUNCTION__);
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
	Exectime etm(__FUNCTION__);
	uuid_t ud;
	uuid_generate(ud);
	return StringFromUuid(ud);
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
	Exectime etm(__FUNCTION__);
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
std::string UuidControl::UUIDToString(uuid_t ud)
{
	Exectime etm(__FUNCTION__);
	char buf[64] = {0};
	GUID guid;
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
bool UuidControl::StringToUUIDCharArray(const std::string &sGuid,GUID&guid)
{
	Exectime etm(__FUNCTION__);
	bool bRet = false;
	const char *szGUID = sGuid.c_str();
	memset(&guid, 0, sizeof(guid));//_stscanf--swscanf_s sscanf_s
	int nRet =
		sscanf(szGUID,
				"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
				&guid.gd.Data1,
				&guid.gd.Data2,
				&guid.gd.Data3,
				&guid.gd.Data4[0], &guid.gd.Data4[1],
				&guid.gd.Data4[2], &guid.gd.Data4[3], &guid.gd.Data4[4], &guid.gd.Data4[5], &guid.gd.Data4[6], &guid.gd.Data4[7]);

	return bRet;
}

/*****************************************************************************
 *                         UUID转字符指针
 *  函 数 名：UuidUnparse
 *  功    能：将UUID转化成普通常见的字符串形式
 *  说    明：
 *  参    数：ud:需要转换的UUID
 *  			udout:转换后的转出字符串指针
 *  返 回 值：无
 *  创 建 人：fjf
 *  创建时间：2014-02-24
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/

void UuidControl::UuidUnparse(const uuid_t ud,char * udout)
{
	Exectime etm(__FUNCTION__);
	return uuid_unparse(ud,udout);
}
/*****************************************************************************
 *                         字符指针转UUID
 *  函 数 名：UuidParse
 *  功    能：将字符指针转成UUID
 *  说    明：
 *  参    数：ud:需要转换的UUID
 *  			udin:输入的字符串指针
 *  返 回 值：无
 *  创 建 人：fjf
 *  创建时间：2014-02-24
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int  UuidControl::UuidParse(const char * udin,uuid_t& ud)
{
	Exectime etm(__FUNCTION__);
	return uuid_parse(udin,ud);
}
/*****************************************************************************
 *                         UUID转字符串
 *  函 数 名：StringFromUuid
 *  功    能：使用UUID自带的转换来处理UUID和字符串的转换
 *  说    明：
 *  参    数：ud:需要转换的UUID
 *  返 回 值：字符串UUID
 *  创 建 人：fjf
 *  创建时间：2014-02-24
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
std::string UuidControl::StringFromUuid(uuid_t ud)
{
	Exectime etm(__FUNCTION__);
	char buf[64];
	UuidUnparse(ud,buf);
	return std::string(buf);
}
/*****************************************************************************
 *                         字符串转UUID
 *  函 数 名：UuidFromString
 *  功    能：将字符串转化成UUID
 *  说    明：
 *  参    数：ud:转换的UUID
 *  			sGuid:需要转换的UUID字符串
 *  返 回 值：成功与否
 *  创 建 人：fjf
 *  创建时间：2014-02-24
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int UuidControl::UuidFromString(const std::string &sGuid,uuid_t& ud)
{
	Exectime etm(__FUNCTION__);
	return UuidParse(sGuid.c_str(),ud);
}
/*****************************************************************************
 *                        拷贝UUID
 *  函 数 名：CopyUUID
 *  功    能：将UUID的值拷贝到另外一个UUID
 *  说    明：
 *  参    数：dstUD:目的UUID
 *  			srcUD:源UUID字符串
 *  返 回 值：无
 *  创 建 人：fjf
 *  创建时间：2014-02-28
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void CopyUUID(uuid_t dstUd,const uuid_t srcUd)
{
	Exectime etm(__FUNCTION__);
	uuid_copy(dstUd,srcUd);
}
/*****************************************************************************
 *                         字符串转UUID
 *  函 数 名：CompareUUID
 *  功    能：比较两个UUID是否一样
 *  说    明：
 *  参    数：ud1:第一个UUID
 *  			ud2:第二个UUID
 *  返 回 值：返回结果 0：相等  <0: ud1<ud2   >0:ud1>ud2
 *  创 建 人：fjf
 *  创建时间：2014-02-28
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int CompareUUID(const uuid_t ud1,const uuid_t ud2)
{
	Exectime etm(__FUNCTION__);
	return uuid_compare(ud1,ud2);
}

