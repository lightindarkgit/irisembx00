/*****************************************************************************
** 文 件 名： SysNetInterface.h
** 主 要 类： SysNetInterface, SysStatusSignal
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：Wang.L
** 创建时间：20014-05-01
**
** 修 改 人：
** 修改时间：
** 描  述:  系统配置网络操作接口，用于设置设备的网络及系统时间等，信号类用于监视系统状态的变化
** 主要模块说明:
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#pragma once
#include <string>
#include <QObject>

class SysStatusSignal : public QObject
{
    Q_OBJECT
public:
    static SysStatusSignal* GetInstance();
    static void Release();
    void SysTimeUpdated();

signals:
    void sigSysTimeUpdated();

private:
    SysStatusSignal();
    static SysStatusSignal* _sigIns;
    static int _refCount;
};


//类名：SysNetInterface
//系统信息网络访问和控制类，可供外部获取设备的网络信息，设备关联的服务器网络信息以及设备的当前
//  系统时间、设置设备的网络信息配置、设置设备所关联的服务器网络信息及设备的系统时间
class SysNetInterface
{
public:
//    SysNetInterface();
    /*****************************************************************************
    *                        获取系统时间
    *  函 数 名：GetSysTime
    *  功    能：设备端的对外访问接口，用于获取设备的当前系统时间
    *  说    明：返回的时间格式为： yyyy-MM-dd hh-mm:ss 例如：“1970-01-20 00：00：00”
    *  参    数：
    *  返 回 值：设备的当前系统时间，失败返回空字符串
    *  创 建 人：L.Wang
    *  创建时间：2014-05-22
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static const std::string GetSysTime();
    /*****************************************************************************
    *                        设置系统时间
    *  函 数 名：SetSysTime
    *  功    能：设备端的对外操作接口，用于设置设备的当前系统时间
    *  说    明：传入的时间格式要求符合： yyyy-MM-dd hh-mm:ss 例如：“1970-01-20 00：00：00”
    *  参    数：
    *  返 回 值：成功返回true，失败返回false
    *  创 建 人：L.Wang
    *  创建时间：2014-05-22
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static bool SetSysTime(const std::string& time);

    /*****************************************************************************
    *                        获取软件版本号
    *  函 数 名：GetSoftVer
    *  功    能：设备端的对外访问接口，用于获取设备的软件版本号
    *  说    明：软件的版本号格式为:"vx.x.x.x"，其中‘v’后的第一‘x’表示主版本号，第二个‘x’表示子版
    *           本号，第三个‘x’表示阶段版本号，第四个‘x’表示6位日期版本号，如：“v0.1.1.140526”
    *  参    数：
    *  返 回 值：返回版本号字符串
    *  创 建 人：L.Wang
    *  创建时间：2014-05-22
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static const std::string GetSoftVer();


    /*****************************************************************************
    *                        获取设备SN号
    *  函 数 名：GetDevSN
    *  功    能：设备端的对外访问接口，用于获取设备的SN编号
    *  说    明：设备的SN编号格式为以“：”连接的六段十六进制字符串，如“00:0C:29:0F:26:1F”
    *
    *  参    数：
    *  返 回 值：返回设备的SN号字符串
    *  创 建 人：L.Wang
    *  创建时间：2014-05-22
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static const std::string GetDevSN();
    /*****************************************************************************
    *                        获取设备IP地址
    *  函 数 名：GetDevIP
    *  功    能：设备端的对外访问接口，用于获取设备的IP地址
    *  说    明：设备的IP地址格式为以“.”连接的四段字符串格式的IP值，如“192.168.10.2”
    *
    *  参    数：
    *  返 回 值：返回设备的IP地址字符串，失败情况下返回空字符串
    *  创 建 人：L.Wang
    *  创建时间：2014-05-22
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static const std::string GetDevIP();
    /*****************************************************************************
    *                        获取服务器IP地址
    *  函 数 名：GetSrvIP
    *  功    能：设备端的对外访问接口，用于获取设备所连接的服务器IP地址
    *  说    明：服务器的IP地址格式为以“.”连接的四段字符串格式的IP值，如“192.168.1.1”
    *
    *  参    数：
    *  返 回 值：返回服务器的IP地址字符串，失败情况下返回空字符串
    *  创 建 人：L.Wang
    *  创建时间：2014-05-22
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static const std::string GetSrvIP();
    /*****************************************************************************
    *                        获取服务器服务端口
    *  函 数 名：GetSrvPort
    *  功    能：设备端的对外访问接口，用于获取设备所连接的服务器服务端口
    *  说    明：服务器的服务端口格式为以整数的字符串值，如“9999”
    *
    *  参    数：
    *  返 回 值：返回服务器的端口字符串，失败情况下返回空字符串
    *  创 建 人：L.Wang
    *  创建时间：2014-05-22
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static const std::string GetSrvPort();

    /*****************************************************************************
    *                        设置设备的网络信息
    *  函 数 名：SetDevNetInfo
    *  功    能：设备端的对外控制接口，用于设置设备的网络信息
    *  说    明：IP地址，子网掩码和网关格式均为以“.”连接的字符串格式
    *
    *  参    数：
    *  返 回 值：成功返回true，失败情况下返回false
    *  创 建 人：L.Wang
    *  创建时间：2014-05-22
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static bool SetDevNetInfo(const std::string& ip, const std::string& mask, const std::string& gate);


    /*****************************************************************************
    *                        设置服务器的网络信息
    *  函 数 名：SetSrvNetInfo
    *  功    能：设备端的对外控制接口，用于设置设备所连接的服务器网络信息
    *  说    明：IP地址为以“.”连接的字符串格式，服务端口为字符串格式的端口值
    *
    *  参    数：
    *  返 回 值：成功返回 0，失败情况下返回-1,权限不够返回-2
    *  创 建 人：L.Wang
    *  创建时间：2014-05-22
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static bool SetSrvNetInfo(const std::string& ip, const int port);

    /*****************************************************************************
    *                        获取设备工作状态
    *  函 数 名：isSingalworkMode
    *  功    能：
    *  说    明：
    *
    *  参    数：
    *  返 回 值：联机工作返回true，否则返回false
    *  创 建 人：L.Wang
    *  创建时间：2014-05-22
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static bool isSingalworkMode();

    /*****************************************************************************
    *                        获取进入省电模式的时间
    *  函 数 名：GetPowerSavingSettings
    *  功    能：
    *  说    明：
    *
    *  参    数：
    *  返 回 值：进入省电模式的时间，单位为分钟
    *  创 建 人：L.Wang
    *  创建时间：2014-06-26
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static int GetPowerSavingSettings();

    /*****************************************************************************
    *                        获取系统语言
    *  函 数 名：GetSysLanguage
    *  功    能：
    *  说    明：
    *
    *  参    数：
    *  返 回 值：系统当前的语言，chinese为中文，english为英语
    *  创 建 人：L.Wang
    *  创建时间：2014-06-26
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static const std::string GetSysLanguage();


private:
    static const std::string GetCurrentInterface();
};
