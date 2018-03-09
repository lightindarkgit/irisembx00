/*****************************************************************************
** 文 件 名：NoneCopyable.h
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
** 主要模块说明: NoneCopyable类定义
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#pragma once

namespace IKXml
{
class NoneCopyable
{
public:
    NoneCopyable();
    virtual ~NoneCopyable();

private:
    NoneCopyable(const NoneCopyable&);
    NoneCopyable& operator=(const NoneCopyable&);
};
}
