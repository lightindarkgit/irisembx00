/******************************************************************************************
 ** 文件名:   SYNCUSBAPI_CPP
 ×× 主要类:   SyncUSBAPI的实现
 **
 ** Copyright (c) 中科虹霸有限公司
 ** 创建人:   刘中昌
 ** 日  期:   2013-12-20
 ** 修改人:
 ** 日  期:
 ** 描  述:  数据处理函数--根据USB 异步传输图像
 *×× 主要模块说明:
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 *****************************************************************************************/
#include "syncusbapi.h"
#include "iusbimage.h"
#include "../Common/Logger.h"
#include <unistd.h>
#include "../Common/Exectime.h"

static IkUSBAPI s_usbAPI;
SyncUSBAPI::SyncUSBAPI()
{
	Exectime etm(__FUNCTION__);
}
SyncUSBAPI::~SyncUSBAPI()
{
	Exectime etm(__FUNCTION__);
	Close();
}
/**********************************************************************************************************************************
 *                        获取图像函数
 *  函 数 名：getCurImage
 *  功    能：获得当前图像
 *  说    明：未获得数据阻塞
 *  参    数：
 *  返 回 值：错误描述代码
 *********************************************************************************************************************************/
int SyncUSBAPI::GetImage(unsigned char *image, unsigned int &length)
{
	Exectime etm(__FUNCTION__);
	int nret = s_usbAPI.GetPreview(image,length);
	if(nret != 0)
	{
		usleep(200000);
		LOG_ERROR("GetPreview failed. ReOpen USB");
		// 重新打开USB
		Close();
		nret = Open();
	}

	return nret;
}
/**********************************************************************************************************************************
 *                        关闭传输函数
 *  函 数 名：Close
 *  功    能：关闭USB的操作包括读取和写入
 *  说    明：关闭USB、清理内存
 *  参    数： 无
 *  返 回 值：
 *********************************************************************************************************************************/
void SyncUSBAPI::Close()
{
	Exectime etm(__FUNCTION__);
	s_usbAPI.Close();
}
/**********************************************************************************************************************************
 *                        开始传输函数
 *  函 数 名：Open
 *  功    能：打开设备
 *  说    明：
 *  参    数：
 *  返 回 值：错误描述, 异常返回
 *********************************************************************************************************************************/
int SyncUSBAPI::Open()
{
	Exectime etm(__FUNCTION__);
	return s_usbAPI.Init();
}
/**********************************************************************************************************************************
 *                        发送命令函数
 *  函 数 名：SendCmd
 *  功    能：通过命令控制USB相关操作
 *  说    明：
 *  参    数：EndPointNum：端点值  cmd 命令
 *  返 回 值：错误描述代码
 *********************************************************************************************************************************/
int SyncUSBAPI::SendCmd(int EndPointNum, char *cmd)
{
	Exectime etm(__FUNCTION__);
	return s_usbAPI.SendCmd(EndPointNum,cmd);
}

int SyncUSBAPI::GetPictureTaken(IN Command cmd, OUT unsigned char *leftImage, OUT  unsigned int &leftLen,
		OUT unsigned char *rightImage, OUT unsigned int &rightLen)
{
	Exectime etm(__FUNCTION__);
	//Command cmd(leftX,leftY,rightX,rightY);
	s_usbAPI.SendCmd(cmd);
	return s_usbAPI.GetPictureTaken(leftImage,leftLen,rightImage,rightLen);
}
