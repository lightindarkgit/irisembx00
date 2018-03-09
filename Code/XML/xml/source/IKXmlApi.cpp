/*****************************************************************************
** 文 件 名：IKXmlApi.cpp
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
** 主要模块说明: IKXmlDocument
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include "../include/IKXml.h"
#include "../IKXmlApi.h"

#include <regex.h>
#include <sys/types.h>



using namespace IKXml;





IKXmlDocument::IKXmlDocument():
    _xmlDoc(NULL)
{
    //TO DO
    _xmlDoc = new XmlDocument();
    _xmlDoc->CreateRootNode("IKEMBX00");
}

IKXmlDocument::~IKXmlDocument()
{
    //TO DO
    delete _xmlDoc;
}

/*****************************************************************************
*                       从文件获取XML数据
*  函 数 名：LoadFromFile
*  功    能：
*  说    明：
*  参    数：fileName，XML文件名称，当不包含路径时，即为相对路径
*  返 回 值：
*           成功返回true，其他返回false， 可通过GetLastError获取错误原因
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool IKXmlDocument::LoadFromFile(const std::string &fileName)
{
    //TO DO
    if(fileName.empty())
    {
        _errMsg = "file name is invalid";
        return false;
    }

   return _xmlDoc->LoadFromFile(fileName);
}

/*****************************************************************************
*                       从字符串获取XML数据
*  函 数 名：LoadFromString
*  功    能：
*  说    明：
*  参    数：xmlStr，XML数据字符串
*  返 回 值：
*           成功返回true，其他返回false，可通过GetLastError获取错误原因
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool IKXmlDocument::LoadFromString(const std::string &xmlStr)
{
    //TO DO
    if(xmlStr.empty())
    {
        _errMsg = std::string("empty XML string");
        return false;
    }

    return _xmlDoc->LoadFromMemory(xmlStr);
}

/*****************************************************************************
*                       从输入流中获取XML数据
*  函 数 名：LoadFromStream
*  功    能：
*  说    明：
*  参    数：is，XML数据流
*  返 回 值：
*           成功返回true，其他返回false，可通过GetLastError获取错误原因
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool IKXmlDocument::LoadFromStream(std::istream &is)
{
    //TO DO
    return _xmlDoc->LoadFromSteam(is);
}

/*****************************************************************************
*                       存储XML数据到文件
*  函 数 名：SaveToFile
*  功    能：
*  说    明：
*  参    数：
*           fileName，XML文件目标名称，不指定路径时，即认为相对路径
*           encod，XML存档编码格式，当不指定时默认为UTF-8编码格式
*  返 回 值：
*           成功返回true，其他返回false，可通过GetLastError获取错误原因
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool IKXmlDocument::SaveToFile(const std::string &fileName, const std::string &encod)
{
    //TO DO
    if(fileName.empty())
    {
        _errMsg = std::string("target file name is invalid");
        return false;
    }

    return _xmlDoc->SaveToFile(fileName, encod);
}

/*****************************************************************************
*                       输出XML数据到字符串
*  函 数 名：WriteToString
*  功    能：
*  说    明：
*  参    数：
*           encod，XML存档编码格式，当不指定时默认为UTF-8编码格式
*  返 回 值：
*           成功返回XML字符串，其他返回空，，当返回空时可通过GetLastError获取错误原因
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
std::string IKXmlDocument::WriteToString(const std::string &encod)
{
    //TO DO
    return _xmlDoc->WriteToString(encod);
}

/*****************************************************************************
*                       输出XML数据到输出流
*  函 数 名：WriteToStream
*  功    能：
*  说    明：
*  参    数：
*           os，输出流
*           encod，XML存档编码格式，当不指定时默认为UTF-8编码格式
*  返 回 值：
*           成功返回true，其他返回false，可通过GetLastError获取错误原因
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool IKXmlDocument::WriteToStream(std::ostream &os, const std::string &encod)
{
    //TO DO
    return _xmlDoc->WriteToStream(os, encod);
}

/*****************************************************************************
*                       测试指定路径的元素是否存在
*  函 数 名：Exist
*  功    能：
*  说    明：
*  参    数：
*           xpath，XML path路径
*  返 回 值：
*           存在返回true，其他返回false
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool IKXmlDocument::Exist(const std::string &xpath)
{
    //TO DO
    XmlElement* root = _xmlDoc->GetRootElement();
    if(!root)
    {
        _errMsg = std::string("can not get the root element");
        return false;
    }

    return root->Find(xpath).size() > 0;
}

/*****************************************************************************
*                       在指定路径下增加元素
*  函 数 名：AddElement
*  功    能：
*  说    明：
*  参    数：
*           xpath，XML path路径
*  返 回 值：
*           存在返回true，其他返回false
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool IKXmlDocument::AddElement(const std::string &xpath, const std::string& name, const std::string& value)
{
    //TO DO
    XmlElement* elem = GetElement(xpath);
    if(NULL == elem)
    {
        _errMsg = std::string("element does not exist");
        return false;
    }

    XmlElement* subElem = elem->AddChild(name);
    if(NULL != subElem)
    {
        if(value.empty())
        {
            return true;
        }
        else
        {
            return subElem->SetChildText(value);
        }
    }
    else
    {
        _errMsg = "failed to add a new element";
        return false;
    }
}

/*****************************************************************************
*                       获取指定路径元素的值
*  函 数 名：GetElementValue
*  功    能：
*  说    明：
*  参    数：
*           xpath，XML path路径
*  返 回 值：
*           成功返回元素值字符串，当返回值空时可通过GetLastError获取错误原因
*  创 建 人：L.Wang
*  创建时间：2014-03-13
*  修 改 人：
*  修改时间：
*****************************************************************************/
std::string IKXmlDocument::GetElementValue(const std::string &xpath)
{
    // TO DO
    XmlElement* elem = GetElement(xpath);
    if(NULL == elem)
    {
        _errMsg = std::string("element does not exist");
        return std::string("");
    }

    if(elem->HasChildText())
    {
        return elem->GetChildText()->GetContent();
    }

    return std::string("");
}

/*****************************************************************************
*                       设置指定路径元素的值
*  函 数 名：SetElementValue
*  功    能：
*  说    明：
*  参    数：
*           xpath，XML path路径
*  返 回 值：
*           成功返回true，其他返回false
*  创 建 人：L.Wang
*  创建时间：2014-03-13
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool IKXmlDocument::SetElementValue(const std::string &xpath, const std::string &value)
{
    //TO DO
    XmlElement* elem = GetElement(xpath);
    if(NULL == elem)
    {
        _errMsg = std::string("element does not exist");
        return false;
    }


    return elem->SetChildText(value);
}

/*****************************************************************************
*                       删除指定路径的元素
*  函 数 名：RemoveElement
*  功    能：
*  说    明：
*  参    数：
*           xpath，XML path路径
*  返 回 值：
*           存在返回true，其他返回false
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool IKXmlDocument::RemoveElement(const std::string &xpath)
{
    //TO DO
    XmlElement* targetElem = GetElement(xpath);
    if(NULL == targetElem)
    {
        _errMsg = std::string("element does not exist");
        return false;
    }

    XmlNode* parentNode = targetElem->GetParentNode();
    parentNode->RemoveChild(targetElem);

    return true;
}

/*****************************************************************************
*                        获取最后出错原因
*  函 数 名：GetLastError
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：
*           返回错误字符串
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
std::string IKXmlDocument::GetLastError()const
{
    //TO DO
    return _errMsg;
}

/*****************************************************************************
*                        获取指定路径的元素
*  函 数 名：GetElement
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：
*           返回错误字符串
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
XmlElement* IKXmlDocument::GetElement(const std::string &xpath)
{
    //TO DO
    XmlElement* root = _xmlDoc->GetRootElement();
    if(!root)
    {
        _errMsg = std::string("can not get the root element");
        return NULL;
    }

    XmlElement* e = NULL;
    NodeSet nodes = root->Find(xpath);
    for(unsigned int iter = 0; iter < nodes.size(); ++iter)
    {
        XmlNode *node = nodes.at(iter);
        if(XmlNode::ELEMENT_NODE == node->GetType())
        {
            e = node->ToXmlElement();
        }
    }

    return e;
}

