/*****************************************************************************
** 文 件 名：XmlAttribute.cpp
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
** 主要模块说明: XmlAttribute类实现
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include <libxml/tree.h>
#include <iostream>
#include "../include/XmlAttribute.h"


using namespace IKXml;

XmlAttribute::XmlAttribute(_xmlNode* node):
    XmlNode(node)
{
    //TO DO
}

XmlAttribute::~XmlAttribute()
{
    //TO DO
}

/*****************************************************************************
*                        获取属性名称
*  函 数 名：GetName
*  功    能：获取属性名称
*  说    明：
*  参    数：
*  返 回 值：
*           属性名称
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
//std::string XmlAttribute::GetName() const
//{
//    std::cout << "[get attribute]@ " << __FUNCTION__ << std::endl;
//    return CObj()->name ? (char *)CObj()->name : "";
//}

/*****************************************************************************
*                        获取属性值
*  函 数 名：GetValue
*  功    能：获取属性值
*  说    明：
*  参    数：
*  返 回 值：
*           属性值字符串
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
std::string XmlAttribute::GetValue() const
{
    xmlChar* val = xmlGetProp(CObj()->parent, CObj()->name);

    std::string rtnVal = val ? (char*) val : "";
//    xmlFree(val);

    return rtnVal;
}

/*****************************************************************************
*                        设置属性值
*  函 数 名：SetValue
*  功    能：修改节点内容
*  说    明：
*  参    数：val，新属性值
*  返 回 值：
*           成功返回true， 失败返回false
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool XmlAttribute::SetValue(const std::string &val)
{
    //TO DO
    xmlSetProp(CObj()->parent, CObj()->name, BAD_CAST val.c_str());


    return true;
}

/*****************************************************************************
*                        获取底层XML属性节点对象
*  函 数 名：CObj
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：
*           xmlAttr对象指针
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
_xmlAttr* XmlAttribute::CObj()
{
    //TO DO
    return reinterpret_cast<_xmlAttr*> (XmlNode::CObj());
}

/*****************************************************************************
*                        获取底层XML属性节点对象
*  函 数 名：CObj
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：
*           xmlAttr对象常量指针
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
const _xmlAttr* XmlAttribute::CObj() const
{
    //TO DO
    return reinterpret_cast<const _xmlAttr*> (XmlNode::CObj());
}
