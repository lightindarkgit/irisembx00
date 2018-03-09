/*****************************************************************************
** 文 件 名：typeconvert.h
** 主 要 类：TypeConvert
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：liuzhch
** 创建时间：2014-05-17
**
** 修 改 人：
** 修改时间：
** 描  述:   数据转换操作（数据库结构与网络传输结构之间的转换）
** 主要模块说明:
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/

#ifndef TYPECONVERT_H
#define TYPECONVERT_H
#include <vector>
#include <string>
#include "../ikIOStream/interface.h"
#include "../CodecNetData/CodecNetData.h"

class TypeConvert
{
public:

    //人员信息-数据库转网络
    static bool ConvertPersonUp(const Person& from,  PersonBase& to);
    //简单人员信息-数据库转网络
    static bool ConvertSimplePersonUp(const Person &from, PersonSimple &to);
    //人员信息-网络转数据库  返回token
    static int ConvertPersonDown(const PersonBase &from, Person &to);

    //人员图像-数据库转网络
    static bool ConvertPersonImageUp(const PersonImage& from, PersonPhoto* to);
    //人员图像-数据库转网络
    static bool ConvertPersonImageUp(const IPersonPhoto &from, PersonPhoto *to);
     //人员图像-网络转数据库  返回token
    static int ConvertPersonImageDown(const PersonPhoto* from, PersonImage& to);

    //虹膜数据-数据库转网络
    static bool ConvertIrisUp(const IrisDataBase& from, UploadIrisData& to);
    //虹膜数据-网络转数据库
    static bool ConvertIrisDown(const UploadIrisData& from, IrisDataBase& to);

    //虹膜数据-网络转数据库  返回token
    static int ConvertIrisDown(const SyncIrisData& from, IrisDataBase& to);

    //识别记录-数据库转网络
    static bool ConvertRecLogUp(const PersonRecLog &from, RecogRecord &to);

    //Uuid 转 std::string
    static int ConvertUuidToStr(const std::vector<Uuid> &from, std::vector<std::string> &to);
    //std::string 转 Uuid
    static int ConvertUuidfromStr(const std::vector<std::string> &from, std::vector<Uuid> &to);

};

#endif // TYPECONVERT_H
