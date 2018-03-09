/*****************************************************************************
** 文 件 名：XmlElement.cpp
** 主 要 类：
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：L.Wang
** 创建时间：20014-02-18
**
** 修 改 人：
** 修改时间：
** 描  述:
** 主要模块说明: XmlElement
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include <libxml/tree.h>
#include <iostream>
#include "../include/XmlText.h"
#include "../include/XmlElement.h"



namespace IKXml {
XmlElement::XmlElement(_xmlNode *node):
    XmlNode(node)
{
    //TO DO
}

XmlElement::~XmlElement()
{
    //TO DO
}


bool XmlElement::SetNameSpaceDeclaration(const std::string &nsURI, const std::string &nsPrefix)
{
    //TO DO
    xmlNewNs(CObj(), nsURI.empty() ? NULL : BAD_CAST nsURI.c_str(), nsPrefix.empty() ? NULL : BAD_CAST nsPrefix.c_str());

    return true;
}


bool XmlElement::HasAttr()const
{
    //TO DO
    return XmlNode::HasAttr();
}


XmlElement::XmlAttributeList XmlElement::GetAttributes()
{
    //TO DO
    XmlAttributeList attrLst;

    for(xmlAttr* attr = CObj()->properties; attr; attr = attr->next)
    {
        if(attr)
        {
            XmlNode::CreateWrapper(reinterpret_cast<xmlNode *>(attr));
            attrLst.push_back(reinterpret_cast<XmlAttribute*> (attr->_private));
        }
    }

    return attrLst;
}


XmlAttribute* XmlElement::GetAttr(const std::string &name, const std::string &nsPrefix) const
{
    //TO DO
    xmlAttr* attr;
    if(nsPrefix.empty())
    {
        attr = xmlHasProp(const_cast<xmlNode*>(CObj()), BAD_CAST name.c_str());
    }
    else
    {
        std::string nsURI = GetNameSpaceURIForPrefix(nsPrefix);
        attr = xmlHasNsProp(const_cast<xmlNode*>(CObj()), BAD_CAST name.c_str(), BAD_CAST nsURI.c_str());

    }

    if(attr)
    {
        XmlNode::CreateWrapper(reinterpret_cast<xmlNode *> (attr));
        return reinterpret_cast<XmlAttribute*>(attr->_private);
    }


    return NULL;
}


XmlAttribute* XmlElement::SetAttr(const std::string &attrName, const std::string &attrVal, const std::string &nsPrefix)
{
    //TO DO
    xmlAttr* attr = NULL;
    if(nsPrefix.empty())
    {
        attr = xmlSetProp(const_cast<xmlNode*>(CObj()), BAD_CAST attrName.c_str(), BAD_CAST attrVal.c_str());
    }
    else
    {
        xmlNs* ns = xmlSearchNs(CObj()->doc, CObj(), BAD_CAST nsPrefix.c_str());
        if(ns)
        {
            attr = xmlSetNsProp(CObj(), ns, BAD_CAST attrName.c_str(), BAD_CAST attrVal.c_str());
        }
        else
        {
            return NULL;
        }
    }


    //属性类型映射
    if(attr)
    {
        XmlNode::CreateWrapper(reinterpret_cast<_xmlNode *>(attr));
        return reinterpret_cast<XmlAttribute*>(attr->_private);
    }


    return NULL;
}


bool XmlElement::RemoveAttr(const std::string &name, const std::string &nsPrefix)
{
    //TO DO
    int rtnVal;
    if(nsPrefix.empty())
    {
        rtnVal = xmlUnsetProp(CObj(), BAD_CAST name.c_str());

        return (0 == rtnVal);
    }
    else
    {
        xmlNs* ns = xmlSearchNs(CObj()->doc, CObj(), BAD_CAST nsPrefix.c_str());
        rtnVal = xmlUnsetNsProp(CObj(), ns, BAD_CAST name.c_str());

        return (0 == rtnVal);
    }
}

XmlText* XmlElement::GetChildText() const
{
    //TO DO
    for(xmlNode* child = CObj()->children; child; child = child->next)
    {
        if(XML_TEXT_NODE == child->type)
        {
            XmlNode::CreateWrapper(child);
            return reinterpret_cast<XmlText*>(child->_private);
        }
    }


    return NULL;
}

XmlText* XmlElement::AddChildText(const std::string &content)
{
    //TO DO
    if(XML_ELEMENT_NODE == CObj()->type)
    {
        xmlNode* node = xmlNewText(BAD_CAST content.c_str());
        xmlAddChild(CObj(), node);

        XmlNode::CreateWrapper(node);
        return reinterpret_cast<XmlText*>(node->_private);
    }

    return NULL;
}

bool XmlElement::SetChildText(const std::string &content)
{
    //TO DO
    XmlText* text = GetChildText();
    if(text)
    {
        return text->SetContent(content);
    }
    else
    {
        text = AddChildText(content);

        return NULL != text;
    }


}


bool XmlElement::HasChildText() const
{
    //TO DO
    return NULL != GetChildText();
}

XmlComment* XmlElement::AddChildComment(const std::string &comment)
{
    //TO DO
    xmlNode* commentNode = xmlNewComment(BAD_CAST comment.c_str());
    commentNode = xmlAddChild(CObj(), commentNode);

    XmlNode::CreateWrapper(commentNode);
    return static_cast<XmlComment*>(commentNode->_private);
}

std::string XmlElement::GetNameSpaceURIForPrefix(const std::string &nsPrefix) const
{
    //TO DO
    std::string nsURI;
    xmlNs* ns = xmlSearchNs(CObj()->doc, const_cast<xmlNode *>(CObj()), BAD_CAST nsPrefix.c_str());

    if(ns && ns->href)
    {
        nsURI = (char *)ns->href;
    }

    return nsURI;
}

//bool XmlElement::hasChildElement()
//{
//    return HasChild();
//}

}

