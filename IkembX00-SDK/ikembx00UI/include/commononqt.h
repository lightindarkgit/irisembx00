/******************************************************************************************
** 文件名:   commononqt.h
×× 主要类:   (无)
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   Wang.L
** 日  期:   2015-01-27
** 修改人:
** 日  期:
** 描  述:   基于qt公共头文件定义
** ×× 主要模块说明：
**
** 版  本:   1.0.0
** 备  注:
**
******************************************************************************************/
#pragma once
#include "ikevent.h"
#include <map>
#include <QTimer>
#include "IKDatabaseWrapper.h"

////Declare QTimer
//class QTimer;


extern std::map<std::string, std::string> weekDay;


/*****************************************************************************
*                        电源管理控制类
*  类    名：PowerManager
*  功    能：控制电源的关闭与重新打开
*  说    明：
*  创 建 人：L.Wang
*  创建时间：2014-05-29
*  修 改 人：
*  修改时间：
*****************************************************************************/
class PowerManager : public QObject
{
    Q_OBJECT
public:
    PowerManager();
    ~PowerManager();

    void Reboot();
    void Shutdown();

private slots:
    void SlotHandlePowerOpt();

private:
    void RebootImp();
    void ShutdownImp();
    void InitTimer();
    void CloseInfrared();

private:
    enum class PowerOpt
    {
        SHUTDOWN,
        REBOOT,
        NONE
    } po;

    //QTimer* _tmCounter;
    QTimer _tmCounter;
    IKEventIns _eventIns;
};


//设备工作模式
class WorkMode
{
public:
    static bool isCfgFileExist();
    static bool IsSingleWorkMode();
    static void SetWorkMode(bool singleMode);

private:
    WorkMode();
    ~WorkMode();

    static bool isSingleWorkMode;
};


/*****************************************************************************
*                        获取当前系统时间
*  函 数 名：GetCurrentDateTime
*  功    能：获取当前系统时间，格式为“YYYY-MM-DD HH：MM：SS”
*  说    明：
*  参    数：format,返回日期时间的格式字符串
*  返 回 值：当前系统时间的标准格式字符串
*  创 建 人：L.Wang
*  创建时间：2014-08-14
*  修 改 人：
*  修改时间：
*****************************************************************************/
const std::string GetCurrentDateTime(const std::string& format = "yyyy-MM-dd hh:mm:ss");


/*****************************************************************************
*                        获取配置文件路径
*  函 数 名：GetXmlFilePath
*  功    能：获取系统配置文件的完整路径
*  说    明：
*  参    数：
*  返 回 值：配置文件路径，失败时，返回值为空
*  创 建 人：L.Wang
*  创建时间：2014-05-18
*  修 改 人：
*  修改时间：
*****************************************************************************/
const std::string GetXmlFilePath();




/*****************************************************************************
*                       获取调试日志格式化标记头
*  函 数 名：IsConnectedToNetwork
*  功    能：获取调试函数的函数头格式：“[yyyy-MM-dd hh:mm:ss.zzz@__FUNCTION__]”
*  说    明：
*  参    数：
*  返 回 值：日志格式化标记头字符串
*  创 建 人：L.Wang
*  创建时间：2015-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
const std::string GetLogHead(const char* funcName);


/*****************************************************************************
*                        是否网络已连接
*  函 数 名：IsConnectedToNetwork
*  功    能：判断当前网络是否已经连接
*  说    明：
*  参    数：
*  返 回 值：true:已连接    false: 其他
*  创 建 人：L.Wang
*  创建时间：2015-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool IsConnectedToNetwork();


/*****************************************************************************
*                        十六进制转十进制
*  函 数 名：ConvertCardNumToDec
*  功    能：将输入的十六进制卡号转成十进制
*  说    明：卡号转换规则是，高两位和低四位分别转，请参考韦根卡号转换规则
*  参    数：
*  返 回 值：转换之后的十进制卡号
*  创 建 人：L.Wang
*  创建时间：2016-3-21
*  修 改 人：
*  修改时间：
*****************************************************************************/
QString ConvertCardNumToDec(QString hexValStr);


/*****************************************************************************
*                        十进制转十六进制
*  函 数 名：ConvertCardNumToHex
*  功    能：将输入的十进制卡号转成十六进制
*  说    明：卡号转换规则是，高两位和低四位分别转，请参考韦根卡号转换规则
*  参    数：
*  返 回 值：转换之后的十进制卡号
*  创 建 人：L.Wang
*  创建时间：2016-3-21
*  修 改 人：
*  修改时间：
*****************************************************************************/
QString ConvertCardNumToHex(QString decValStr);

/* ********************************************************/
/**
 * @Name: CheckCardIDAndGen 
 *
 * @Param: personID
 *
 * @Returns: 0 - succ, other - error
 */
/* ********************************************************/
int CheckCardIDAndGen(IkUuid personID = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");

/* ********************************************************/
/**
 * @Name: ConvertCardIdInt2String
 *
 * @Param: cardIdInt
 * @Param: cardIdString
 *
 * @Returns: 0 - succ, other - error
 */
/* ********************************************************/
int ConvertCardIdInt2String(long cardIdInt, std::string& cardIdString);
