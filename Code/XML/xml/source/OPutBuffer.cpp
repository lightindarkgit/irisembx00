/*****************************************************************************
** 文 件 名：OPutBuffer.cpp
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
** 主要模块说明: OPutBuffer类实现
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include "../include/OPutBuffer.h"
//W3C提供的库头文件
#include <libxml/globals.h>
#include <libxml/xmlIO.h>
//输出
#include <iostream>

namespace IKXml {
//输出缓冲区操作回调类定义
struct OPutBufferCallback
{
    static int OnWrite(void * cTxt, const char * buf, int len)
    {
        OPutBuffer * tmp = static_cast<OPutBuffer*>(cTxt);
        return tmp->OnWrite(buf, len) ? len : -1;
    }

    static int OnClose(void * cTxt)
    {
        OPutBuffer * tmp = static_cast<OPutBuffer*>(cTxt);
        return tmp->OnClose() ? 0 : -1;
    }
};


OPutBuffer::OPutBuffer(const std::string &encod)
{
    //TO DO
    xmlCharEncodingHandlerPtr handler = NULL;
    if(!encod.empty())
    {
        xmlCharEncoding encoding = xmlParseCharEncoding(encod.c_str());
        if(XML_CHAR_ENCODING_UTF8 == encoding)
        {
            handler = xmlFindCharEncodingHandler(encod.c_str());
        }

        if(!handler)
        {
            std::cerr << "cannot initialize an encoder to " << encod << std::endl;
            return;
        }
    }

    _buf = xmlOutputBufferCreateIO(&OPutBufferCallback::OnWrite,
                                   &OPutBufferCallback::OnClose,
                                   static_cast<void *>(this),
                                   handler);

    if(!_buf)
    {
        std::cerr << "cannot initialize xmlOutputBuffer" << std::endl;
    }
}

OPutBuffer::~OPutBuffer()
{
    //TO DO
}

/*****************************************************************************
*                        获取底层XML输出缓冲区对象
*  函 数 名：CObj
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：
*           _xmlOutputBuffer对象指针
*  创 建 人：L.Wang
*  创建时间：2014-02-28
*  修 改 人：
*  修改时间：
*****************************************************************************/
_xmlOutputBuffer* OPutBuffer::CObj()
{
    //TO DO
    return _buf;
}

/*****************************************************************************
*                        获取底层XML输出缓冲区对象
*  函 数 名：CObj
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：
*           _xmlOutputBuffer对象常量指针
*  创 建 人：L.Wang
*  创建时间：2014-02-28
*  修 改 人：
*  修改时间：
*****************************************************************************/
const _xmlOutputBuffer* OPutBuffer::CObj()const
{
    //TO DO
    return _buf;
}

/*****************************************************************************
*                        关闭缓冲区回调函数接口
*  函 数 名：OnClose
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：
*           成功返回true，其他返回false
*  创 建 人：L.Wang
*  创建时间：2014-02-28
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool OPutBuffer::OnClose()
{
    //TO DO
    int rtnVal = DoClose();
    _buf = NULL;

    return rtnVal;
}

/*****************************************************************************
*                        写缓冲区回调函数接口
*  函 数 名：OnWrite
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
bool OPutBuffer::OnWrite(const char *buf, int len)
{
    return DoWrite(buf, len);
}

/*****************************************************************************
*                        关闭缓冲区
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
bool OPutBuffer::DoClose()
{
    //TO DO
//    return 0 == xmlOutputBufferClose(_buf);
    return true;
}

}
