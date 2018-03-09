/*****************************************************************************
** 文 件 名：XmlComment.h
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
** 主要模块说明: XmlComment定义
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#pragma once
#include "XmlContent.h"


namespace IKXml {
class XmlComment : public XmlContent
{
public:
    //Public Members
    explicit XmlComment(_xmlNode* node);
    ~XmlComment();
protected:
    //Protected members
private:
    //Private members
};
}
