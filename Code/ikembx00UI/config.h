/*****************************************************************************
** 文 件 名：irisdboperate.h
** 主 要 类：IrisDBOperate
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建  人：liuzhch
** 创建时间：2014-03-17
**
** 修 改 人：
** 修改时间：
** 描  述:   数据库操作
** 主要模块说明:
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H
#include "string"
class Config
{
public:
    static bool GetIdentMode();
    static bool GetBellTimes();
    static bool GetOutWigan();
    static bool GetOutRelay();

    static bool SetIdentMode();
    static bool SetBellTimes();
    static bool SetOutWigan();
    static bool SetOutRelay();

    static bool GetServerIP(std::string &serverIP,int & port);
    static bool SetServerIP(const std::string serverIP,const int port);



//    IKXml::IKXmlDocument doc;
//    if(doc.LoadFromFile("ikembx00.xml"))
//    {
};

#endif // CONFIG_H
