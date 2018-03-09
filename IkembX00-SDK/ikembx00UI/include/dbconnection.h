/******************************************************************************************
** 文件名:   dataconnection.h
×× 主要类:   (无)
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   Wang.L
** 日  期:   2015-01-28
** 修改人:
** 日  期:
** 描  述:   数据库连接类声明文件
** ×× 主要模块说明：
**
** 版  本:   1.0.0
** 备  注:   暂时为采用该方式
**
******************************************************************************************/
#pragma once
#include <QWidget>
#include <QSqlDatabase>
#include <string>


bool createConnection(QWidget* parent = nullptr);


class DBConnection
{
public:
    DBConnection(const std::string& dbName = std::string("irisApp"),
                 const std::string& userName = std::string("postgres"),
                 const std::string& passWord = std::string("123456"),
                 const std::string& hostName = std::string("localhost"),
                 int port = 5432,
                 const std::string& sqlType = std::string("QPSQL"));

    ~DBConnection();
private:
    static QSqlDatabase db;
};
