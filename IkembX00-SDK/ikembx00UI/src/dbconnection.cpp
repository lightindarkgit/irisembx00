/******************************************************************************************
 ** 文件名:   dataconnection.cpp
 ×× 主要类:   (无)
 **
 ** Copyright (c) 中科虹霸有限公司
 ** 创建人:   Wang.L
 ** 日  期:   2015-01-28
 ** 修改人:
 ** 日  期:
 ** 描  述:   数据库连接类定义文件
 ** ×× 主要模块说明：
 **
 ** 版  本:   1.0.0
 ** 备  注:   暂时为采用该方式
 **
 ******************************************************************************************/
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include "dbconnection.h"
#include "../Common/Logger.h"
#include "../Common/Exectime.h"


bool createConnection(QWidget* parent)
{
	
	//TO DO
	QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
	db.setHostName("localhost");
	db.setDatabaseName("irisApp");
	db.setUserName("postgres");
	db.setPassword("123456");
	db.setPort(5432);

	if(!db.open())
	{
		LOG_ERROR("连接数据库 failed");
		QMessageBox::critical(parent, QString::fromUtf8("连接数据库"), db.lastError().text(), QMessageBox::Ok);
		return false;
	}

	return true;
}




//bool createConnection(QWidget* parent)
//{
//    //TO DO
//    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
//    QString dbPath("../database/personinfo.db");

//    if(!QFile::exists(dbPath))
//    {
//        QSqlQuery query;
//        query.exec(QString("CREATE TABLE person (workid int primary key not nullptr, name varchar, gender varchar,"
//                           "idnum varchar, departid int, iris varchar, card varchar, note varchar)"));

//        if(!query.isActive())
//        {
//            QMessageBox::warning(parent, QString::fromUtf8("创建数据库"), query.lastError().text());
//            return false;
//        }

//        query.exec(QString("CREATE TABLE depart (id int primary key not nullptr, name varchar)"));
//        if(!query.isActive())
//        {
//            QMessageBox::warning(parent, QString::fromUtf8("创建数据库"), query.lastError().text());
//            return false;
//        }
//    }

//    db.setDatabaseName(dbPath);
//    if(!db.open())
//    {
//        QMessageBox::critical(parent, QString::fromUtf8("连接数据库"), QString::fromUtf8("数据库打开失败！"), QMessageBox::Ok);
//        return false;
//    }

//    return true;
//}


QSqlDatabase DBConnection::db;

DBConnection::
DBConnection(const std::string& dbName,
		const std::string& userName,
		const std::string& passWord,
		const std::string& hostName,
		int port = 5432,
		const std::string& sqlType)
{
	
	db.setDatabaseName(dbName.c_str());
	db.setUserName(userName.c_str());
	db.setPassword(passWord.c_str());
	db.setPort(port);
	db.addDatabase(sqlType.c_str());
	db.setHostName(hostName.c_str());

}

