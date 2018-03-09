/*****************************************************************************
** 文 件 名：OStreamBuffer.cpp
** 主 要 类：
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：L.Wang
** 创建时间：2014-02-28
**
** 修 改 人：
** 修改时间：
** 描  述:
** 主要模块说明: OStreamBuffer类实现
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include "../include/OStreamBuffer.h"


namespace IKXml {
OStreamBuffer::OStreamBuffer(std::ostream &os, const std::string &encod):
    OPutBuffer(encod),
    _os(os)
{
    //TO DO
}
OStreamBuffer::~OStreamBuffer()
{
    //TO DO
}

/*****************************************************************************
*                        关闭输出流缓冲区
*  函 数 名：DoClose
*  功    能：
*  说    明：
*  参    数：
*
*  返 回 值：
*           成功返回true，其他返回false
*  创 建 人：L.Wang
*  创建时间：2014-02-28
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool OStreamBuffer::DoClose()
{
    if(_os)
    {
        _os.flush();
        return true;
    }

    return false;
}

/*****************************************************************************
*                        写入到输出流缓冲区
*  函 数 名：DoWrite
*  功    能：
*  说    明：
*  参    数：
*          buf， 被写入字符串指针
*          len， 写入字符串长度
*  返 回 值：
*           成功返回true，其他返回false
*  创 建 人：L.Wang
*  创建时间：2014-02-28
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool OStreamBuffer::DoWrite(const char *buf, int len)
{
    if(_os)
    {
        _os.write(buf, len);
        return true;
    }

    return false;
}
}
