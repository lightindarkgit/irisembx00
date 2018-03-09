/*****************************************************************************
** 文 件 名：XmlDocument.cpp
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
** 主要模块说明: XmlDocument实现
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include <iostream>
#include <fcntl.h>
#include <unistd.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/parserInternals.h>

#include "../include/XmlDocument.h"
#include "../include/OStreamBuffer.h"

namespace IKXml {
//XmlDocument::XmlDocument():
//    _docPtr(NULL),
//    _cTxt(NULL)
//{
//    //TO DO
//    _docPtr = xmlNewDoc(BAD_CAST "1.0");
//}

/*****************************************************************************
*                        初始化类构造函数
*  函 数 名：Init
*  功    能：完成底层XML相关部件的初始化
*  说    明：
*  参    数：
*  返 回 值：
*
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
XmlDocument::Init::Init()
{
    xmlInitParser();  //为了在多线程环境下安全起见
    xmlKeepBlanksDefault(false);
}

/*****************************************************************************
*                        初始化类析构函数
*  函 数 名：~Init
*  功    能：析构初始化类
*  说    明：
*  参    数：
*  返 回 值：
*
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
XmlDocument::Init::~Init()
{
}

//初始化XmlDocument类的全局成员
XmlDocument::Init XmlDocument::_int;


/*****************************************************************************
*                        XmlDocument类构造函数
*  函 数 名：XmlDocument
*  功    能：初始化XmlDocument类
*  说    明：
*  参    数：version，xml规范版本号
*  返 回 值：
*
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
XmlDocument::XmlDocument(const std::string &version) :
    _docPtr(NULL),
    _cTxt(NULL)
{
    //TO DO
    _docPtr = xmlNewDoc(BAD_CAST version.c_str());
    _docPtr->_private = this;
}

/*****************************************************************************
*                        XmlDocument类构造函数
*  函 数 名：XmlDocument
*  功    能：初始化XmlDocument类
*  说    明：
*  参    数：doc， xmlDoc对象
*  返 回 值：
*
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
XmlDocument::XmlDocument(_xmlDoc *doc) :
    _docPtr(NULL),
    _cTxt(NULL)
{
    //TO DO
    _docPtr = doc;
    _docPtr->_private = this;
}

/*****************************************************************************
*                        XmlDocument类析构函数
*  函 数 名：~XmlDocument
*  功    能：释放内部所使用的资源
*  说    明：
*  参    数：
*  返 回 值：
*
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
XmlDocument::~XmlDocument()
{
    //TO DO
//    std::cout << "Calling FreeXmlDoc()" << std::endl;
    XmlNode::FreeWrappers(reinterpret_cast<xmlNode*>(_docPtr));
    FreeXmlDoc();
}

/*****************************************************************************
*                        从文件中加载xml文档
*  函 数 名：LoadFromFile
*  功    能：解析参数中指定的xml文件
*  说    明：
*  参    数：fileName，xml文件名
*  返 回 值：
*           成功返回true， 失败返回false
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool XmlDocument::LoadFromFile(const std::string &fileName)
{
    //TO DO
    if(0 != access(fileName.c_str(), F_OK))     //If the file doesnot exist
    {
        std::cout << "[ERROR]" << fileName << ": not exist!" << std::endl;
        return false;
    }


    //先放弃持有的当前xml文档，清理相应的内存
    FreeXmlDoc();
    //解析指定的XML文件
    ParseFile(fileName);


    return NULL != _docPtr;
}

/*****************************************************************************
*                        从内存中加载xml文档
*  函 数 名：LoadFromMemory
*  功    能：解析参数中指定的xml文件
*  说    明：
*  参    数：fileName，xml文件名
*  返 回 值：
*           成功返回true， 失败返回false
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool XmlDocument::LoadFromMemory(const std::string &val)
{
    //TO DO
    //先放弃持有的当前xml文档，清理相应的内存
    FreeXmlDoc();
    _cTxt = xmlCreateMemoryParserCtxt(val.c_str(), val.length());
    if(_cTxt)
    {
        //解析xml文档上下文
        ParseContext();
    }


    return NULL != _docPtr;
}

/*****************************************************************************
*                        从输入流中加载xml文档
*  函 数 名：LoadFromSteam
*  功    能：解析参数中指定的xml文件
*  说    明：
*  参    数：fileName，xml文件名
*  返 回 值：
*           成功返回true， 失败返回false
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool XmlDocument::LoadFromSteam(std::istream &is)
{
    //TO DO
    //先放弃持有的当前xml文档，清理相应的内存
    FreeXmlDoc();

    //强制在解析输入流的过创建xml文档
    _cTxt = xmlCreatePushParserCtxt(0, 0, 0, 0, "");
    if(!_cTxt)
    {
        return false;
    }


    std::string inString;
    while(std::getline(is, inString))
    {
        inString.append("\n");
        xmlParseChunk(_cTxt, inString.c_str(), inString.length(), 0);
    }

    xmlParseChunk(_cTxt, 0, 0, 1);

    if(CheckParserCTxt())
    {
        return NULL != _docPtr;
    }


    return false;
}

/*****************************************************************************
*                        获取编码格式
*  函 数 名：GetEncoding
*  功    能：获取xml文件的存取编码格式
*  说    明：
*  参    数：
*  返 回 值：编码格式，值为空时表示未指定编码
*
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
std::string XmlDocument::GetEncoding()const
{
    //TO DO
    std::string encod;
    if(_docPtr)
    {
        if(_docPtr->encoding)
        {
            encod = (const char*)_docPtr->encoding;
        }
    }
    else
    {
        std::cerr << "[Eroor@ " << __FUNCTION__ << "]failed to get xml encoding for invalid _xmlDoc point!" << std::endl;
    }

    return encod;
}

/*****************************************************************************
*                        获取XML规范版本
*  函 数 名：GetVersion
*  功    能：获取xml文件的XML规范版本
*  说    明：
*  参    数：
*  返 回 值：规范版本
*
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
std::string XmlDocument::GetVersion()const
{
    //TO DO
    std::string ver;
    if(_docPtr)
    {
        if(_docPtr->version)
        {
            ver = (const char*)_docPtr->version;
        }
    }
    else
    {
        std::cerr << "[Eroor@ " << __FUNCTION__ << "]failed to get xml version for invalid _xmlDoc point!" << std::endl;
    }

    return ver;
}


/*****************************************************************************
*                        获取根元素
*  函 数 名：GetRootElement
*  功    能：获取xml文档的根元素
*  说    明：
*  参    数：
*  返 回 值：根元素地址，为NULL时表示失败或无根元素
*
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
XmlElement* XmlDocument::GetRootElement()
{
    //TO DO
    if(_docPtr)
    {
        xmlNode* docRoot;
        docRoot = xmlDocGetRootElement(_docPtr);

        //如果节点有效
        if(docRoot)
        {
            XmlNode::CreateWrapper(docRoot);
            return  reinterpret_cast<XmlElement*>(docRoot->_private);
        }
    }
    else
    {
        std::cerr << "[Eroor@ " << __FUNCTION__ << "]failed to get root element for invalid _xmlDoc point!" << std::endl;
    }
    return NULL;
}


/*****************************************************************************
*                        创建根元素
*  函 数 名：CreateRootNode
*  功    能：创建xml文档的根元素
*  说    明：
*  参    数：name，根元素名称，
*           nsURI，名称空间统一资源标识符
*           nsPrefix，名称空间前缀
*  返 回 值：根元素地址，为NULL时表示创建失败
*
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
XmlElement* XmlDocument::CreateRootNode(const std::string &name, const std::string &nsURI, const std::string &nsPrefix)
{
    //TO DO
    if(!_docPtr)
    {
        std::cerr << "[Eroor@ " << __FUNCTION__ << "]failed to create root node for invalid _xmlDoc point!" << std::endl;
        return NULL;
    }

    XmlElement* root = GetRootElement();
    if(root)
    {
//        std::cout  << __FUNCTION__ << "：unlink and free nodes" << std::endl;
        //解除根节点和当前文档树的关联
        xmlUnlinkNode(root->CObj());
        //释放已载入内存中的root节点及其子节点
        xmlFreeNode(root->CObj());
    }

    xmlNode* docNode = xmlNewDocNode(_docPtr, NULL, BAD_CAST name.c_str(), NULL);
    xmlDocSetRootElement(_docPtr, docNode);

    //重新获取新的根元素
    root = GetRootElement();
    if(!nsPrefix.empty())
    {
        root->SetNameSpaceDeclaration(nsURI, nsPrefix);
        root->SetNameSpace(nsPrefix);
    }

    return root;
}

/*****************************************************************************
*                        引用方式创建根元素
*  函 数 名：CreateRootNodeByImport
*  功    能：创建xml文档根元素
*  说    明：
*  参    数：
*           node， 元素节点地址
*           recursive， 是否只引用当前指定的节点，为false时表示递归引用其子节点
*  返 回 值： 返回新的根节点元素
*
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
XmlElement* XmlDocument::CreateRootNodeByImport(XmlNode *node, bool recursive)
{
    //TO DO
    if(!_docPtr)
    {
        std::cerr << "[Eroor@ " << __FUNCTION__ << "]failed to import root node for invalid _xmlDoc point!" << std::endl;
        return NULL;
    }

    xmlNode* importedNode = xmlDocCopyNode(reinterpret_cast<xmlNode*>(node->CObj()), _docPtr, recursive);
    if(!importedNode)
    {
        std::cerr << "[Eroor@ " << __FUNCTION__ << "]cannot copy imported root node!" << std::endl;
        return NULL;
    }

    xmlDocSetRootElement(_docPtr, importedNode);

    return GetRootElement();
}

/*****************************************************************************
*                        添加文档级注释
*  函 数 名：AddComment
*  功    能：为文档添加注释
*  说    明：
*  参    数：
*           comment， 注释字符串
*  返 回 值：XmlComment对象指针
*
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
XmlComment* XmlDocument::AddComment(const std::string& comment)
{
    //TO DO
    if(!_docPtr)
    {
        std::cerr << "[Eroor@ " << __FUNCTION__ << "]cannot add comment to document tree for invalid _xmlDoc point!" << std::endl;
        return NULL;
    }

    xmlNode* node = xmlNewComment(BAD_CAST comment.c_str());
    if(!node)
    {
        std::cerr << "[Eroor@ " << __FUNCTION__ << "]cannot create comment node!" << std::endl;
        return NULL;
    }

    node = xmlAddChild((xmlNode *)_docPtr, node);
    XmlNode::CreateWrapper(node);


    return reinterpret_cast<XmlComment *>(node->_private);
}

/*****************************************************************************
*                        获取文档级注释
*  函 数 名：GetCommentValues
*  功    能：获取文档级注释
*  说    明：
*  参    数：
*
*  返 回 值：XML注释对象指针
*
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
std::vector<std::string> XmlDocument::GetCommentValues()const
{
    //TO DO
    std::vector<std::string> comments;
    if(_docPtr)
    {
        xmlNode* cur = _docPtr->children;
        while(cur)
        {
            if(XML_COMMENT_NODE == cur->type)
            {
                comments.push_back((const char *)cur->content);
            }

            cur = cur->next;
        }
    }
    else
    {
        std::cerr << "[Eroor@ " << __FUNCTION__ << "]cannot get xml comments for invalid _xmlDoc point!" << std::endl;
    }


    return comments;
}

/*****************************************************************************
*                        获取文档级注释节点
*  函 数 名：GetCommentNodes
*  功    能：获取文档级注释节点
*  说    明：
*  参    数：
*
*  返 回 值：XML注释对象指针列表
*
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
XmlDocument::XmlCommentList XmlDocument::GetCommentNodes()const
{
    //TO OD
    XmlCommentList comments;
    if(_docPtr)
    {
        xmlNode* cur = _docPtr->children;
        while(cur)
        {
            if(XML_COMMENT_NODE == cur->type)
            {
                XmlNode::CreateWrapper(cur);
                comments.push_back(reinterpret_cast<XmlComment *> (cur->_private));
            }

            cur = cur->next;
        }
    }
    else
    {
        std::cerr << "[Eroor@ " << __FUNCTION__ << "]cannot get xml comments for invalid _xmlDoc point!" << std::endl;
    }
    return comments;
}

/*****************************************************************************
*                        存储至文件
*  函 数 名：SaveToFile
*  功    能：将xml文档树存储之指定文件
*  说    明：
*  参    数：
*           fileName， 目标文件名
*           encod， 存储编码格式
*  返 回 值：成功返回true，其他返回false
*
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool XmlDocument::SaveToFile(const std::string &fileName, const std::string &encod)
{
    //TO DO
    return _writeToFile(fileName, encod, true);
}

/*****************************************************************************
*                        输出到字符串
*  函 数 名：WriteToString
*  功    能：以指定编码格式将xml文档树输出到字符串
*  说    明：
*  参    数：encod， 编码格式
*  返 回 值：xml文档字符串
*
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
std::string XmlDocument::WriteToString(const std::string &encod)
{
    //TO DO
    return _writeToString(encod, true);
}

/*****************************************************************************
*                        输出到输出流
*  函 数 名：WriteToStream
*  功    能：以指定编码格式将xml文档树输出到指定输出流
*  说    明：
*  参    数：
*           os，输出流
*           encod， 存储编码格式
*  返 回 值：
*
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool XmlDocument::WriteToStream(std::ostream &os, const std::string &encod)
{
    //TO DO
    return _writeToStream(os, encod, true);
}

/*****************************************************************************
*                        存储至文件
*  函 数 名：_writeToFile
*  功    能：
*  说    明：
*  参    数：
*           fileName， 目标文件名
*           encod， 存储编码格式
*           format，是否以格式化方式存储
*  返 回 值：成功返回true，否则返回false
*
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool XmlDocument::_writeToFile(const std::string &name, const std::string &encod, bool format)
{
    //TO DO
    if(!_docPtr)
    {
        std::cerr << "[Eroor@ " << __FUNCTION__ << "]cannot write xml tree to file for invalid _xmlDoc point!" << std::endl;
        return false;
    }

    xmlIndentTreeOutput = format ? 1 : 0;

    int rtnVal;
    rtnVal = xmlSaveFormatFileEnc(name.c_str(), _docPtr, encod.empty() ? NULL : encod.c_str(), format ? 1 : 0);

    return (-1 != rtnVal);
}

/*****************************************************************************
*                        以字符串形式输出
*  函 数 名：_writeToString
*  功    能：将xml文档树以字符串形式输出
*  说    明：
*  参    数：
*           encod， 存储编码格式
*           format，是否以格式化方式输出
*  返 回 值：xml文档字符串
*
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
std::string XmlDocument::_writeToString(const std::string &encod, bool format)
{
    //TO DO
    if(!_docPtr)
    {
        std::cerr << "[Eroor@ " << __FUNCTION__ << "]cannot write xml tree to string for invalid _xmlDoc point!" << std::endl;
        return std::string();
    }

    xmlIndentTreeOutput = format ? 1 : 0;
    xmlChar* buffer = NULL;
    int length = 0;

    xmlDocDumpFormatMemoryEnc(_docPtr, &buffer, &length, encod.empty() ? NULL : encod.c_str(), format ? 1 : 0);
    if(!buffer)
    {
        std::cerr << "[Eroor@ " << __FUNCTION__ << "]cannot write xml tree to string for dumping format memory failed!" << std::endl;
        return std::string();
    }

    std::string res;
    res.assign((char*)buffer, length);
    //释放动态分配的内存
    xmlFree(buffer);

    return res;
}

/*****************************************************************************
*                        输出至输出流
*  函 数 名：_writeToStream
*  功    能：以输出流的形式输出xml文档树
*  说    明：
*  参    数：
*           os，输出流
*           encod， 存储编码格式
*           format，是否以格式化方式输出
*  返 回 值：
*
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool XmlDocument::_writeToStream(std::ostream &os, const std::string &encod, bool format)
{
    //TO DO
    if(!_docPtr)
    {
        std::cerr << "[Eroor@ " << __FUNCTION__ << "]cannot write xml tree to stream for invalid _xmlDoc point!" << std::endl;
        return false;
    }

    //待需要时实现
    OStreamBuffer oBuf(os, encod);
    int rtnVal = xmlSaveFormatFileTo(oBuf.CObj(), _docPtr, encod.c_str(), format ? 1 : 0);
    if(-1 == rtnVal)
    {
        std::cerr << "[Eroor@ " << __FUNCTION__ << "]failed to output xml document to specified output stream!" << std::endl;
        return false;
    }

    return true;
}

/*****************************************************************************
*                        获取底层xml文档对象
*  函 数 名：CObj
*  功    能：获取底层xml文档对象的指针
*  说    明：
*  参    数：
*  返 回 值：_xmlDoc对象的指针
*
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
_xmlDoc* XmlDocument::CObj()
{
    return _docPtr;
}

/*****************************************************************************
*                        获取底层xml文档对象
*  函 数 名：CObj
*  功    能：获取底层xml文档对象的常量指针
*  说    明：
*  参    数：
*  返 回 值：底层xml文档对象的常量指针
*
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
const _xmlDoc* XmlDocument::CObj()const
{
    return _docPtr;
}

/*****************************************************************************
*                        解析xml文件
*  函 数 名：ParseFile
*  功    能：解析指定的xml文件
*  说    明：
*  参    数：fileName，xml文件名
*  返 回 值：
*
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
void XmlDocument::ParseFile(const std::string &fileName)
{
    //TO DO
//    std::cout << __FUNCTION__ << std::endl;
    _cTxt = xmlCreateFileParserCtxt(fileName.c_str());
    if(!_cTxt)
    {
        std::cerr << "[Eroor@ " << __FUNCTION__ << "]failed to create xml parser contexter!" << std::endl;

        return;
    }

    //设置xml文件路径
    if(!_cTxt->directory)
    {
        char* dir = xmlParserGetDirectory(fileName.c_str());
        _cTxt->directory = (char *)xmlStrdup(BAD_CAST dir);
    }

    ParseContext();

//    //解析xml文件得到xml文档树数据
//    _docPtr = xmlParseFile(fileName.c_str());
}

/*****************************************************************************
*                        获取文档上下文对象
*  函 数 名：ParseContext
*  功    能：完成底层XML相关部件的初始化
*  说    明：
*  参    数：
*  返 回 值：
*
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
void XmlDocument::ParseContext()
{
    //TO DO
    if(!_cTxt)
    {
        return;
    }

    //解析xml文档上下文
    int rtnVal = xmlParseDocument(_cTxt);
    if(rtnVal || !CheckParserCTxt())
    {
         return;
    }


    //检出xmlDoc对象
    _docPtr = _cTxt->myDoc;
    if(_docPtr)
    {
        _docPtr->_private = this;
    }
}

/*****************************************************************************
*                        检查解析器上下文的有效性
*  函 数 名：CheckParserCTxt
*  功    能：
*  说    明：
*  参    数：
*  返 回 值：
*          有效返回true， 其他返回false
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
bool XmlDocument::CheckParserCTxt()
{
    if(!_cTxt)
    {
        return false;
    }

    if(!_cTxt->wellFormed)
    {
        FreeXmlDoc();
        std::cerr << "[Waring" << __FUNCTION__ << "]This xml document is not well formed!" << std::endl;
        return false;
    }

    if(_cTxt->errNo)
    {
        FreeXmlDoc();
        std::cerr << "[Error@" << __FUNCTION__ << "]faied to parse xml by parser contexter!" << std::endl;
        return false;
    }

    return true;
}


/*****************************************************************************
*                        文档类资源释放
*  函 数 名：FreeXmlDoc
*  功    能：清理文档类所使用的内存等资源
*  说    明：
*  参    数：
*  返 回 值：
*
*  创 建 人：L.Wang
*  创建时间：2014-02-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
void XmlDocument::FreeXmlDoc()
{
    //TO DO
    //如果当前xmlDoc指针持有有效的xml文档，则将其释放
    if(_docPtr)
    {
        xmlFreeDoc(_docPtr);
        _docPtr = NULL;
    }

    if(_cTxt)
    {
        _cTxt->_private = NULL;
        xmlFreeParserCtxt(_cTxt);
        _cTxt = NULL;
    }
}
}
