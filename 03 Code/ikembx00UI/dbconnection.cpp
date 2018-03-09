#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include "dbconnection.h"

bool createConnection(QWidget* parent)
{
    //TO DO
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QString dbPath("../database/personinfo.db");

    if(!QFile::exists(dbPath))
    {
        QSqlQuery query;
        query.exec(QString("CREATE TABLE person (workid int primary key not NULL, name varchar, gender varchar,"
                           "idnum varchar, departid int, iris varchar, card varchar, note varchar)"));

        if(!query.isActive())
        {
            QMessageBox::warning(parent, QString::fromUtf8("创建数据库"), query.lastError().text());
            return false;
        }

        query.exec(QString("CREATE TABLE depart (id int primary key not NULL, name varchar)"));
        if(!query.isActive())
        {
            QMessageBox::warning(parent, QString::fromUtf8("创建数据库"), query.lastError().text());
            return false;
        }
    }

    db.setDatabaseName(dbPath);
    if(!db.open())
    {
        QMessageBox::critical(parent, QString::fromUtf8("连接数据库"), QString::fromUtf8("数据库打开失败！"), QMessageBox::Ok);
        return false;
    }

    return true;
}
