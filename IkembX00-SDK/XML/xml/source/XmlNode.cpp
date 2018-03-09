/*****************************************************************************
** 文 件 名：XmlNode.cpp
** 主 要 类：
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：L.Wang
** 创建时间：20014-02-26
**
** 修 改 人：
** 修改时间：
** 描  述:
** 主要模块说明: XmlNode类实现
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include "../include/XmlElement.h"
#include "../include/XmlAttribute.h"
#include "../include/XmlDocument.h"
#include "../include/XmlText.h"
#include "../include/XmlNode.h"


namespace IKXml {
XmlNode::XmlNode(_xmlNode* node) :
    _nodPtr(node)
{
    //TO DO
    _nodPtr->_private = this;
}

XmlNode::~XmlNode()
{
    //TO DO
}

std::string XmlNode::GetName()const
{
    std::string nodeName;
    if(_nodPtr && _nodPtr->name)
    {
        nodeName = (const char*)_nodPtr->name;
    }


    return nodeName;
}

bool XmlNode::SetName(const std::string &name)
{
    //TO DO
    if(!_nodPtr)
    {
        return false;
    }

    xmlNodeSetName(_nodPtr, BAD_CAST name.c_str());


    return true;
}


bool XmlNode::SetNameSpace(const std::string& nsPrefix)
{
    xmlNs* ns = xmlSearchNs(CObj()->doc, CObj(), BAD_CAST nsPrefix.c_str());
    if(NULL != ns)
    {
        xmlSetNs(CObj(), ns);

        return true;
    }

    return false;
}

std::string XmlNode::GetNameSpaceURI() const
{
    //TO DO
    std::string nsURI;
    if(_nodPtr && _nodPtr->ns && _nodPtr->ns->href)
    {
        nsURI = (const char*)_nodPtr->ns->href;
    }


    return nsURI;
}

std::string XmlNode::GetNameSpacePrefix()const
{
    //TO DO
    std::string nsPrefix;
    if(_nodPtr && _nodPtr->ns && _nodPtr->ns->prefix)
    {
        nsPrefix = (const char*)_nodPtr->ns->prefix;
    }

    return nsPrefix;
}

int XmlNode::GetLineNo()const
{
    //TO DO
    if(_nodPtr)
    {
        return XML_GET_LINE(_nodPtr);
    }


    return 0;
}


bool XmlNode::HasChild()const
{
    if(_nodPtr)
    {
        return (NULL != _nodPtr->children);
    }

    return false;
}


XmlNode::NodeList XmlNode::GetChildren(const std::string &name)const
{
    //TO DO
    NodeList nodeChildren;
    if(!_nodPtr || !_nodPtr->children)  //如果节点本身无效或节点没有子节点
    {
        return nodeChildren;
    }

    xmlNode* childPtr = _nodPtr->children;
    do
    {
        if(name.empty() || name == (const char*)(childPtr->name))
        {
            XmlNode::CreateWrapper(childPtr);
            if(childPtr->_private)
            {
                nodeChildren.push_back(reinterpret_cast<XmlNode*>(childPtr->_private));
            }
//            std::cout << "child info: " << (const char*)(childPtr->name) << "\t_private: " << childPtr->_private << std::endl;
        }
    }while((childPtr = childPtr->next));


    return nodeChildren;
}

XmlElement* XmlNode::AddChild(const std::string &name, const std::string &nsPrefix)
{
    //TO DO
    if(!_nodPtr)
    {
        return NULL;
    }


    xmlNode* node = 0;
    xmlNs* ns = 0;

    if(XML_ELEMENT_NODE != _nodPtr->type)
    {
        std::cout << "type error!" << std::endl;
        return NULL;
    }

    //Ignore the namespace if none was specified:
    if(!nsPrefix.empty())
    {
      //Use the existing namespace if one exists:
      ns = xmlSearchNs(_nodPtr->doc, _nodPtr, BAD_CAST nsPrefix.c_str());
      if (!ns)
      {
          std::cout << "namespace error!" << std::endl;
          return NULL;
      }
    }

    node = xmlAddChild(_nodPtr, xmlNewNode(ns, (const xmlChar*)name.c_str()));

    if(node)
    {
        XmlNode::CreateWrapper(node);
        return static_cast<XmlElement*>(node->_private);
    }
    else
    {
        return NULL;
    }
}

void XmlNode::RemoveChild(XmlNode *node)
{
    if(!_nodPtr)
    {
        return;
    }

    xmlUnlinkNode(node->CObj());
    xmlFreeNode(node->CObj());
}

XmlNode* XmlNode::ImportNode(const XmlNode *node, bool recursive)
{
    //TO DO
    if(!node || !_nodPtr)
    {
        return NULL;
    }

    //Create the node, by copying:
    xmlNode* importedNode = xmlDocCopyNode(const_cast<xmlNode*>(node->CObj()), _nodPtr->doc, recursive);
    if (!importedNode)
    {
        return NULL;
    }

    //Add the node:
    xmlNode* addedNode = xmlAddChild(this->CObj(), importedNode);
    if (!addedNode)
    {
      xmlFreeNode(importedNode);
      return NULL;
    }

    XmlNode::CreateWrapper(importedNode);
    return static_cast<XmlNode*>(importedNode->_private);
}


XmlNode* XmlNode::GetFirstChild()const
{
    //TO DO
    if(_nodPtr && _nodPtr->children)
    {
        XmlNode::CreateWrapper(_nodPtr->children);
        if(_nodPtr->children->_private)
        {
            return reinterpret_cast<XmlNode*>(_nodPtr->children->_private);
        }
    }

    return NULL;
}

XmlNode* XmlNode::GetLastChild()const
{
    //TO DO
    if(_nodPtr && _nodPtr->last)
    {
        XmlNode::CreateWrapper(_nodPtr->last);
        if(_nodPtr->last->_private)
        {
            return reinterpret_cast<XmlNode*>(_nodPtr->last->_private);
        }
    }

    return NULL;
}

XmlNode* XmlNode::GetParentNode()const
{
    //TO DO
    if(_nodPtr && _nodPtr->parent)
    {
        XmlNode::CreateWrapper(_nodPtr->parent);
        if(_nodPtr->parent->_private)
        {
            return reinterpret_cast<XmlNode*>(_nodPtr->parent->_private);
        }
    }

    return NULL;
}

XmlNode* XmlNode::GetPreviousSibling()const
{
    //TO DO
    if(_nodPtr && _nodPtr->prev)
    {
        XmlNode::CreateWrapper(_nodPtr->prev);
        if(_nodPtr->prev->_private)
        {
            return reinterpret_cast<XmlNode*>(_nodPtr->prev->_private);
        }
    }

    return NULL;
}

XmlNode* XmlNode::GetNextSibling()const
{
    //TO DO
    if(_nodPtr && _nodPtr->next)
    {
        XmlNode::CreateWrapper(_nodPtr->next);
        if(_nodPtr->next->_private)
        {
            return reinterpret_cast<XmlNode*>(_nodPtr->next->_private);
        }
    }

    return NULL;
}


XmlNode::XmlNodeType XmlNode::GetType()const
{
    //TO DO
    if(_nodPtr)
    {
        switch(_nodPtr->type)
        {
        case XML_ELEMENT_NODE:
//            std::cout << __FUNCTION__ << ": ELEMENT_NODE"  << _nodPtr->line << std::endl;
            return ELEMENT_NODE;

        case XML_ATTRIBUTE_NODE:
//            std::cout << __FUNCTION__ << ": ATTRIBUTE_NODE" << _nodPtr->line << std::endl;
            return ATTRIBUTE_NODE;

        case XML_TEXT_NODE:
//            std::cout << __FUNCTION__ << ": TEXT_NODE" << _nodPtr->line << std::endl;
            return TEXT_NODE;

        case XML_COMMENT_NODE:
//            std::cout << __FUNCTION__ << ": COMMENT_NODE" << _nodPtr->line << std::endl;
            return COMMENT_NODE;

        case XML_DOCUMENT_NODE:
            return DOCUMENT_NODE;

        default:
            std::cout << __FUNCTION__ << ": default type" << _nodPtr->line << std::endl;
            return (XmlNodeType)_nodPtr->type;
        }
    }

    std::cout << __FUNCTION__ << ": NULL node ptr = " << _nodPtr << std::endl;
    return NULL_NODE;
}

bool XmlNode::IsElement()const
{
    //TO DO
    return ELEMENT_NODE == GetType();
}

bool XmlNode::IsAttr()const
{
    //TO DO
    return ATTRIBUTE_NODE == GetType();
}

bool XmlNode::IsComment()const
{
    //TO DO
    return COMMENT_NODE == GetType();
}

bool XmlNode::IsText()const
{
    //TO DO
    return TEXT_NODE == GetType();
}


XmlElement* XmlNode::ToXmlElement()
{
    if(_nodPtr)
    {
        XmlNode::CreateWrapper(_nodPtr);
        if(_nodPtr->_private)
        {
            return reinterpret_cast<XmlElement *> (_nodPtr->_private);
        }
    }

    return NULL;
}


XmlAttribute* XmlNode::ToXmlAttribute()
{
    //TO DO
    if(_nodPtr)
    {
        XmlNode::CreateWrapper(_nodPtr);
        if(_nodPtr->_private)
        {
            return reinterpret_cast<XmlAttribute *> (_nodPtr->_private);
        }
    }

    return NULL;
}

XmlText* XmlNode::ToXmlText()
{
    //TO DO
    if(_nodPtr)
    {
        XmlNode::CreateWrapper(_nodPtr);
        if(_nodPtr->_private)
        {
            return reinterpret_cast<XmlText *> (_nodPtr->_private);
        }
    }

    return NULL;
}


XmlComment* XmlNode::ToXmlComment()
{
    //TO DO
    if(_nodPtr)
    {
        XmlNode::CreateWrapper(_nodPtr);
        if(_nodPtr->_private)
        {
            return reinterpret_cast<XmlComment *> (_nodPtr->_private);
        }
    }

    return NULL;
}

std::string XmlNode::GetPath()const
{
    //TO DO
    xmlChar* path = xmlGetNodePath(_nodPtr);
    std::string nodePath = path ? (char*) path : "";
    xmlFree(path);

    return nodePath;
}

NodeSet XmlNode::Find(const std::string &xmlPath) const
{
    //TO DO
    NodeSet nodeSet;
    if(!_nodPtr && !_nodPtr->doc)
    {
        return nodeSet;
    }

    xmlXPathContext* pathContext = xmlXPathNewContext(_nodPtr->doc);
    pathContext->node = _nodPtr;

    xmlXPathObject* pathObj = xmlXPathEval(BAD_CAST xmlPath.c_str(), pathContext);


    if(!pathObj || XPATH_NODESET !=  pathObj->type)
    {
        xmlXPathFreeObject(pathObj);
        xmlXPathFreeContext(pathContext);

        std::cout << __FUNCTION__ << ": XML path is not valid" << std::endl;
        return nodeSet;
    }

    xmlNodeSet* nodes = pathObj->nodesetval;
    if(nodes)
    {
        for(int iter = 0; iter != nodes->nodeNr; ++iter)
        {
            XmlNode::CreateWrapper(nodes->nodeTab[iter]);
            nodeSet.push_back(static_cast<XmlNode*>(nodes->nodeTab[iter]->_private));
        }
    }

    xmlXPathFreeObject(pathObj);
    xmlXPathFreeContext(pathContext);


    return nodeSet;
}

_xmlNode* XmlNode::CObj()
{
    return _nodPtr;
}

const _xmlNode* XmlNode::CObj()const
{
    return _nodPtr;
}

void XmlNode::CreateWrapper(_xmlNode *node)
{
    if(!node)
    {
        std::cout << __FUNCTION__ << ": NULL node" << std::endl;
        return;
    }

    if(node->_private)
    {
        //Node already wrapped, skip
//        std::cout << __FUNCTION__ << ": already has wrapped node" << std::endl;
        return;
    }

    switch (node->type)
    {
      case XML_ELEMENT_NODE:
      {
        node->_private = new IKXml::XmlElement(node);
        break;
      }
      case XML_ATTRIBUTE_NODE:
      {
        node->_private = new IKXml::XmlAttribute(node);
        break;
      }
      case XML_TEXT_NODE:
      {
        node->_private = new IKXml::XmlText(node);
        break;
      }
      case XML_COMMENT_NODE:
      {
        node->_private = new IKXml::XmlComment(node);
        break;
      }
//      case XML_CDATA_SECTION_NODE:
//      {
//        node->_private = new IKXml::XmlCdataNode(node);
//        break;
//      }
//      case XML_PI_NODE:
//      {
//        node->_private = new IKXml::XmlProcessingInstructionNode(node);
//        break;
//      }
//      case XML_DTD_NODE:
//      {
//        node->_private = new IKXml::XmlDtd(reinterpret_cast<xmlDtd*>(node));
//        break;
//      }
//      //case XML_ENTITY_NODE:
//      //{
//      //  assert(0 && "Warning: XML_ENTITY_NODE not implemented");
//      //  //node->_private = new xmlpp::ProcessingInstructionNode(node);
//      //  break;
//      //}
//      case XML_ENTITY_REF_NODE:
//      {
//        node->_private = new IKXml::XmlEntityReference(node);
//        break;
//      }
      case XML_DOCUMENT_NODE:
      {
        // do nothing. For Documents it's the wrapper that is the owner.
        std::cout << __FUNCTION__ << ": document node" << std::endl;
        break;
      }
      default:
      {
        // good default for release versions
        node->_private = new IKXml::XmlNode(node);
        std::cerr << __FUNCTION__ << "Warning: new node of unknown type created: " << node->type << std::endl;
        break;
      }
    }
}

void XmlNode::FreeWrappers(_xmlNode *attrNode)
{
    //TO DO
    if(!attrNode)
    {
        return;
    }

    //释放所有子节点
    for(xmlNode* child = attrNode->children; child; child = child->next)
    {
        FreeWrappers(child);
    }

    //删除本地节点
    switch(attrNode->type)
    {
      //没有属性的节点类型
//      case XML_DTD_NODE:
//        delete static_cast<Dtd*>(attrNode->_private);
//        attrNode->_private = 0;
//        return;
      case XML_ATTRIBUTE_NODE:
      case XML_ELEMENT_DECL:
      case XML_ATTRIBUTE_DECL:
      case XML_ENTITY_DECL:
        delete static_cast<XmlNode*>(attrNode->_private);
        attrNode->_private = 0;
        return;
      case XML_DOCUMENT_NODE:
        //暂不释放
        return;
      default:
        delete static_cast<XmlNode*>(attrNode->_private);
        attrNode->_private = 0;
        break;
    }

    //Walk the attributes list.
    //Note that some "derived" struct have a different layout, so
    //_xmlNode::properties would be a nonsense value, leading to crashes,
    //(and shown as valgrind warnings), so we return above, to avoid
    //checking it here.
    for(xmlAttr* attr = attrNode->properties; attr; attr = attr->next)
    {
        FreeWrappers(reinterpret_cast<xmlNode*>(attr));
    }
}


bool XmlNode::HasAttr()const
{
    //TO DO
    if(_nodPtr)
    {
        return (NULL != _nodPtr->properties);
    }

    return false;
}
}
