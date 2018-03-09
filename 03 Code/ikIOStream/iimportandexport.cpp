#include "iimportandexport.h"

// 用于输出信息
#include <iostream>

// SOCI相关头文件
#include<soci.h>
#include <soci-sqlite3.h>

//bool IImportAndExport::_isOpen = false;
IImportAndExport::IImportAndExport()
{
    //启动线程显示图像
    _dealDataThread = std::thread(DealDatagThread, (void*)this);
    _isOpen = false;
}

IImportAndExport::~IImportAndExport()
{
    //线程join
    _dealDataThread.join();
}

/*****************************************************************************
*                           数据导入导出线程
*  函 数 名：DealDatagThread
*  功    能：采图
*  说    明：
*  参    数：arg：输入参数
*
*  返 回 值：NULL
*  创 建 人：liuzhch
*  创建时间：2014-2-17
*  修 改 人：
*  修改时间：
*****************************************************************************/
void* IImportAndExport::DealDatagThread(void* arg)
{

}

int IImportAndExport::ExportToFile(const char *fileName ,TableType tableType, IkImportOrExportCallBackFunc callback)
{
    _callBack = callback;

    switch(tableType)
    {
    case PersonRecLog:
        ExportPersonToFile(fileName);
        break;
    case IrisDataBase:
        ExportIrisDataBaseToFile(fileName);
        break;
    case PersonImage:
        ExportPersonImageToFile(fileName);
        break;
    case Person:
        ExportPersonRecLogToFile(fileName);
        break;
    default:
        break;
    }

    return 0;
}

int IImportAndExport::ImportFromFile(const char *fileName,TableType tableType, IkImportOrExportCallBackFunc callback)
{
    _callBack = callback;
    switch(tableType)
    {
    case PersonRecLog:
        ImportPersonFromFile(fileName);
        break;
    case IrisDataBase:
        ImportIrisDataBaseFromFile(fileName);
        break;
    case PersonImage:
        ImportPersonImageFromFile(fileName);
        break;
    case Person:
        ImportPersonRecLogFromFile(fileName);
        break;
    default:
        break;
    }

    return 0;
}
 int IImportAndExport::ExportPersonRecLogToFile(const char *fileName)
 {

 }

 int IImportAndExport::ImportPersonRecLogFromFile(const char *fileName)
 {

 }

 int IImportAndExport::ExportIrisDataBaseToFile(const char *fileName)
 {

 }

 int IImportAndExport::ImportIrisDataBaseFromFile(const char *fileName)
 {

 }

 int IImportAndExport::ExportPersonImageToFile(const char *fileName)
 {

 }

 int IImportAndExport::ImportPersonImageFromFile(const char *fileName)
 {

 }

 int IImportAndExport::ExportPersonToFile(const char *fileName)
 {

 }

 int IImportAndExport::ImportPersonFromFile(const char *fileName)
 {

 }


// using namespace soci;
 std::string connectString;
 soci::backend_factory const &backEnd = *soci::factory_sqlite3();

 int IImportAndExport::OpenSQLiteDB()
 {
     try
     {
         soci::session sql(backEnd, connectString);
         {

             try { sql << "drop table test1"; }
             catch (soci_error const &) {} // ignore if error

             sql <<
             "create table test1 ("
             "    id integer,"
             "    name varchar(100)"
             ")";

             sql << "insert into test1(id, name) values(7, \'John\')";

             rowid rid(sql);
             sql << "select oid from test1 where id = 7", into(rid);

             int id;
             std::string name;

             sql << "select id, name from test1 where oid = :rid",
             into(id), into(name), use(rid);

             assert(id == 7);
             assert(name == "John");

             sql << "drop table test1";
         }


         _isOpen = true;
         return 0;
     }
     catch (soci::soci_error const & e)
     {
         std::cout << "SOCIERROR: " << e.what() << '\n';
     }
     catch (std::exception const & e)
     {
         std::cout << "EXCEPTION: " << e.what() << '\n';
     }

 }


 int IImportAndExport::OpenPostgresDB()
 {
          try
          {
              // 打开本地postgresql的mydb数据库，用户名为postgres，密码为123456
              std::string databaseName = "mydb";
              std::string userName = "postgres";
              std::string userPasswd = "123456";

              soci::session sql(soci::sqlite3, "dbname="+databaseName+" user=" +userName+ " password=" + userPasswd);

              std::cout << "连接本地数据库" << databaseName << "成功！" << std::endl;
              _isOpen = true;
          }
     //     catch (soci::sqlite3_soci_error const &e)
     //     {

     //         // postgresql特有的错误信息
     //         std::cerr << "PostgreSQL error:"
     //                   << e.sqlstate()          // sqlstat()为postgresql独有，长度为5的字符串
     //                   << " "
     //                   << e.what()
     //                   << std::endl;
     //     }
          catch (std::exception const &e)
          {
              // 其他错误信息
              std::cerr << "Some other error:" << e.what() << std::endl;
          }
 }
