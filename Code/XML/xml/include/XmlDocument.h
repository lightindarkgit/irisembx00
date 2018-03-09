/*****************************************************************************
** 文 件 名：XmlDocument.h
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
** 主要模块说明: XmlDocument类定义
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#pragma once
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <map>


#include "NoneCopyable.h"
#include "XmlElement.h"
#include "XmlComment.h"

#ifndef XML_CPP
extern "C" {
    struct _xmlDoc;
    struct _xmlEntity;
    struct _xmlParserCtxt;
}
#endif

namespace IKXml {
class XmlDocument : NoneCopyable
{
    class Init
    {
    public:
        Init();
        virtual ~Init();
    };


public:
    //Public members   
    //XmlDocument();
    explicit XmlDocument(const std::string& version = "1.0");
    ~XmlDocument();

    //定义XML Comment列表
    typedef std::list<XmlComment *>        XmlCommentList;

    /*****************************************************************************
    *                        加载xml文档
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
    bool LoadFromFile(const std::string& fileName);
    bool LoadFromMemory(const std::string& val);
    bool LoadFromSteam(std::istream& is);


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
    std::string GetEncoding()const;

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
    std::string GetVersion()const;



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
    XmlElement* GetRootElement();

    /*****************************************************************************
    *                        创建根元素
    *  函 数 名：CreateRootNode
    *  功    能：创建xml文档的根元素
    *  说    明：
    *  参    数：name，根元素名称，
    *           nsURI，名称空间统一资源标识符
    *           nsPrefix，名称空间别名
    *  返 回 值：根元素地址，为NULL时表示创建失败
    *
    *  创 建 人：L.Wang
    *  创建时间：2014-02-26
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    XmlElement* CreateRootNode(const std::string& name,
                               const std::string& nsURI = std::string(),
                               const std::string& nsPrefix = std::string());

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
    XmlElement* CreateRootNodeByImport(XmlNode* node, bool recursive = true);



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
    XmlComment* AddComment(const std::string& comment);

    /*****************************************************************************
    *                        获取文档级注释
    *  函 数 名：GetCommentValues
    *  功    能：获取文档级注释
    *  说    明：
    *  参    数：
    *
    *  返 回 值：XML注释对象集合
    *
    *  创 建 人：L.Wang
    *  创建时间：2014-02-26
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    std::vector<std::string> GetCommentValues()const;

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
    XmlCommentList GetCommentNodes()const;


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
    bool SaveToFile(const std::string& fileName, const std::string& encod = std::string());

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
    std::string WriteToString(const std::string& encod = std::string());

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
    bool WriteToStream(std::ostream& os, const std::string& encod = std::string());



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
    _xmlDoc* CObj();

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
    const _xmlDoc* CObj()const;
protected:
    //Protected members
    virtual bool _writeToFile(const std::string& name, const std::string& encod, bool format = false);
    virtual std::string _writeToString(const std::string& encod, bool format = false);
    virtual bool _writeToStream(std::ostream& os, const std::string& encod, bool format = false);

private:
    XmlDocument(_xmlDoc* doc);
    void ParseFile(const std::string& fileName);
    void ParseContext();
    void FreeXmlDoc();
    bool CheckParserCTxt();


private:
    //Private members
    static Init _int;
    _xmlDoc* _docPtr;          //xml文档指针
    _xmlParserCtxt* _cTxt;
};
}
