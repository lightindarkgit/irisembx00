/*****************************************************************************
** 文 件 名：XmlElement.h
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
** 主要模块说明: XmlElement类定义
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#pragma once
#include "XmlNode.h"
#include "XmlAttribute.h"
#include "XmlComment.h"

namespace IKXml
{
class XmlElement : public XmlNode
{
public:
    //Public members
    explicit XmlElement(_xmlNode* node);
    ~XmlElement();

    //定义XML属性类列表
    typedef std::list<XmlAttribute *> XmlAttributeList;

    /*****************************************************************************
    *                        设置名称空间声明
    *  函 数 名：SetNameSpaceDeclaration
    *  功    能：
    *  说    明：
    *  参    数：
    *           nsURI，名称空间统一资源标识符
    *           nsPrefix，名称空间别名
    *  返 回 值：
    *           成功返回true， 否则返回false
    *  创 建 人：L.Wang
    *  创建时间：2014-02-26
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    bool SetNameSpaceDeclaration(const std::string &nsURI, const std::string &nsPrefix = std::string());



    /*****************************************************************************
    *                        元素是否拥有属性
    *  函 数 名：HasAttr
    *  功    能：
    *  说    明：
    *  参    数：
    *  返 回 值：
    *           是返回true， 否则返回false
    *  创 建 人：L.Wang
    *  创建时间：2014-02-26
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    bool HasAttr()const;

    /*****************************************************************************
    *                        获取属性列表
    *  函 数 名：GetAttributes
    *  功    能：
    *  说    明：如果没有属性，则返回的属性列表为空列表
    *  参    数：
    *  返 回 值：
    *           XmlAttributeList对象列表
    *  创 建 人：L.Wang
    *  创建时间：2014-02-26
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    XmlAttributeList GetAttributes();

    /*****************************************************************************
    *                        获取指定属性
    *  函 数 名：GetAttr
    *  功    能：
    *  说    明：如果没有指定属性，则返回的NULL指针
    *  参    数：
    *          name， 属性名称
    *          nsPrifix， 属性名称别名
    *  返 回 值：
    *           XmlAttribute对象指针
    *  创 建 人：L.Wang
    *  创建时间：2014-02-26
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    XmlAttribute* GetAttr(const std::string& name, const std::string& nsPrefix = std::string()) const;

    /*****************************************************************************
    *                        设置属性
    *  函 数 名：SetAttr
    *  功    能：
    *  说    明：
    *  参    数：
    *           attrName， 属性名称
    *           attrVal， 属性值
    *           nsPrifix， 属性名称别名
    *  返 回 值：
    *           新设置的属性对象指针
    *  创 建 人：L.Wang
    *  创建时间：2014-02-26
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    XmlAttribute* SetAttr(const std::string& attrName, const std::string& attrVal, const std::string& nsPrefix = std::string());

    /*****************************************************************************
    *                        移除指定属性
    *  函 数 名：RemoveAttr
    *  功    能：
    *  说    明：
    *  参    数：
    *           attrName， 属性名称
    *           nsPrifix， 属性名称别名
    *  返 回 值：
    *           成功返回true， 其他返回false
    *  创 建 人：L.Wang
    *  创建时间：2014-02-26
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    bool RemoveAttr(const std::string& name, const std::string& nsPrefix = std::string());



    /*****************************************************************************
    *                        是否拥有文本子节点
    *  函 数 名：HasChildText
    *  功    能：
    *  说    明：
    *  参    数：
    *  返 回 值：
    *           是返回true， 其他返回false
    *  创 建 人：L.Wang
    *  创建时间：2014-02-26
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    bool HasChildText()const;

    /*****************************************************************************
    *                        获取文本子节点
    *  函 数 名：GetChildText
    *  功    能：
    *  说    明：如果没有文本字节点则返回NULL指针
    *  参    数：
    *  返 回 值：
    *           文本节点对象指针
    *  创 建 人：L.Wang
    *  创建时间：2014-02-26
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    XmlText* GetChildText()const;

    /*****************************************************************************
    *                        添加文本子节点
    *  函 数 名：AddChildText
    *  功    能：
    *  说    明：如果添加文本子节点失败则返回NULL指针
    *  参    数：
    *           content， 文本节点字符串
    *  返 回 值：
    *           文本节点对象指针
    *  创 建 人：L.Wang
    *  创建时间：2014-02-26
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    XmlText* AddChildText(const std::string& content = std::string());

    /*****************************************************************************
    *                        设置文本子节点
    *  函 数 名：SetChildText
    *  功    能：
    *  说    明：
    *  参    数：
    *           content， 文本节点字符串
    *  返 回 值：
    *           成功返回true， 失败返回false
    *  创 建 人：L.Wang
    *  创建时间：2014-02-26
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    bool SetChildText(const std::string& content);



    /*****************************************************************************
    *                        添加注释节点
    *  函 数 名：AddChildComment
    *  功    能：
    *  说    明：如果添加注释子节点失败则返回NULL指针
    *  参    数：
    *           comment， 注释节点内容字符串
    *  返 回 值：
    *           XmlComment对象指针
    *  创 建 人：L.Wang
    *  创建时间：2014-02-26
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    XmlComment* AddChildComment(const std::string& comment);


    /*****************************************************************************
    *                        是否有子元素
    *  函 数 名：HasChildElement
    *  功    能：
    *  说    明：
    *  参    数：
    *  返 回 值：
    *           是返回true， 其他返回false
    *  创 建 人：L.Wang
    *  创建时间：2014-02-26
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
//    bool HasChildElement();

protected:
    //Protected members
    std::string GetNameSpaceURIForPrefix(const std::string& nsPrefix) const;
private:
    //Private members
};
}
