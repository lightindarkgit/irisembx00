/*****************************************************************************
** 文 件 名：XmlContent.cpp
** 主 要 类：
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：L.Wang
** 创建时间：2014-02-26
**
** 修 改 人：
** 修改时间：
** 描  述:
** 主要模块说明: XmlContent类实现
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include "../include/XmlContent.h"
#include <libxml/tree.h>


namespace IKXml {
XmlContent::XmlContent(_xmlNode* node):
    XmlNode(node)
{
    //TO DO
}

XmlContent::~XmlContent()
{
    //TO DO
}

/*****************************************************************************
*                        获取节点内容
*  函 数 名：GetContent
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：
*           返回节点内容字符串
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
std::string XmlContent::GetContent() const
{
    //TO DO
    std::string content;
    if(XML_ELEMENT_NODE != CObj()->type)
    {
        content = (char*)CObj()->content;
    }

    return content;
}

/*****************************************************************************
*                        设置节点内容
*  函 数 名：SetContent
*  功    能：修改节点内容
*  说    明：
*  参    数：content，新节点内容文本
*  返 回 值：
*           成功返回true， 失败返回false
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool XmlContent::SetContent(const std::string &content)
{
    if(XML_ELEMENT_NODE == CObj()->type)
    {
        return false;
    }

    xmlNodeSetContent(CObj(), BAD_CAST content.c_str());


    return true;
}

/*****************************************************************************
*                        节点内容是否为空
*  函 数 名：IsWhiteSpace
*  功    能：判断节点的内容是否为空
*  说    明：
*  参    数：
*  返 回 值：
*           是返回true， 否则返回false
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool XmlContent::IsWhiteSpace() const
{
    return xmlIsBlankNode(const_cast<_xmlNode*>(CObj()));
}
}
