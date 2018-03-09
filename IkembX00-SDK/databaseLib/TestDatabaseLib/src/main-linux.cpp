/*******************************************************************
** 文件名称：main.cpp
** 主 要 类：
** 描    述：测试IKDatabaseLib提供的功能
**
** 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
** 创 建 人：yqhe
** 创建日期：2014/2/19 8:50:51
**
** 版    本：0.0.1
** 备    注：命名及代码编写遵守C++编码规范
*******************************************************************/
#include <iostream>
#include <sstream>

#include <iomanip>      // 控制格式输出

#include <stdio.h>

#include <memory>

#include <libpq-fe.h>
#include <uuid/uuid.h>

#include "IKDatabaseLib.h"
#include "IKTransaction.h"

#include "DataAccessType.h"

const char pidField[]     = "personid";
const char nameField[]    = "name";
const char workSnField[]  = "worksn";
const char timeField[]    = "create_time";
const char departField[]  = "departname";
const char byteaField[]   = "testbytea";

const char irisIdField [] = "irisdataid";
const char irisCodeField[]= "iriscode";
const char irisImageField[]="eyepic";
const char devSnField[]   = "devsn";

const char operatorIdField[] = "operatorid";
const char passwdField[]  = "password";
const char sexField[]     = "sex";
const char memoField[]    = "memo";

/*******************************************************************
*           把std::tm格式的数据整理成string类型，可供输出
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：outputTime
* 功    能：把std::tm格式的数据整理成string类型，可供输出
* 说    明：日期+时间，格式为1970-01-01 00:00:00
* 参    数：
*           输入参数
*           dispTime —— 需要显示的时间
* 返 回 值：无
* 创 建 人：yqhe
* 创建日期：2014/2/13 13:25:30
*******************************************************************/
std::string outputTime(const std::tm &dispTime)
{
    char mbstr[100];

    std::tm bakTime = dispTime;
    std::time_t tt = std::mktime(&bakTime);
    // 测试时间格式是否正确，如不正确，就不进行转化
    if (tt == -1)
    {
        mbstr[0] = '\0';
    }
    else
    {
        std::strftime(mbstr, 100, "%Y-%m-%d %H:%M:%S", &bakTime);
    }

    return (mbstr);
}

int times = 1;

void mockInsertOnePerson(std::vector<Person> &vecPerson,
                         std::vector<IrisDataDemo> &vecIris)
{
    Person person;
    // 生成人员UUID
    uuid_t pud;
    uuid_generate(pud);

    char uuidOut[256];
    uuid_unparse(pud, uuidOut);

    std::string testUUIDString(uuidOut);
    std::cout << "Generate person UUID : " << testUUIDString << std::endl;

    // bytea方式保存uuid
    //person.id = escapeBlob(ud, 16);
    //std::cout << "person_id bytea string : " << person.id << ", size=" << person.id.size() << std::endl;
    // 自定义数据结构IkUuid方式保存uuid
    person.id.Set(pud);
    // 取uuid头四个字符加入名字中，以示区别
    person.name = "Test"+ testUUIDString.substr(0, 4) +"的名字";
    // 模拟插入部门数据，有变化
    if (times%2==1)
    {
    	person.depart = "部门" + testUUIDString.substr(0, 2);
    }

    vecPerson.push_back(person);

    // 测试IkUuid == 操作符
    IkUuid comparePud(pud);
    std::cout << "比较相同的person uuid : " << (person.id==comparePud) << std::endl;

    uuid_t iud;
    uuid_generate(iud);
    uuid_unparse(iud, uuidOut);

    std::string testUUIDString2(uuidOut);
    std::cout << "Generate irisData UUID : " << testUUIDString2 << std::endl;

    // 比较不同的IkUuid
    comparePud.Set(iud);
    std::cout << "比较不同的person uuid : " << (person.id==comparePud) << std::endl;

    unsigned char *image;
    // 虹膜数据
    IrisDataDemo *irisData;
    irisData = new IrisDataDemo[1];

    irisData->did.Set(iud);
    irisData->pid.Set(pud);

    // 模拟设置虹膜特征
    const int codeSize = 256;
    unsigned char code [codeSize];

    for (int count=0; count<codeSize; count++)
    {
        code[count] = (unsigned char)(count);
    }
    irisData->irisCode.Set(code, codeSize);

//    code[0] = 0x27;
//    code[1] = 0x28;
//    code[2] = 0x04;
//    code[3] = 0x27;
//    code[4] = 0x28;
//    code[5] = 0x95;
//    irisData->irisCode.Set(code, 6);

    irisData->irisCode.PrintBytea();


    // 模拟设置虹膜图像
    int imageSize = 640*480;
    image = new unsigned char[imageSize];

    std::cout << "print alloc pointer : " << static_cast<const void *>(image) << std::endl;

    for (int count=0; count<imageSize; count++)
    {
        image[count] = (unsigned char)(count + 16*times);
    }

    image[0] = 0x27;

    times ++;

    irisData->irisImage.Set(image, imageSize);
    irisData->irisImage.PrintBytea();

    vecIris.push_back(*irisData);
}

// 在sqlite数据库中创建irisdatabase表
void createIrisTable(DatabaseAccess &dbSqlit)
{
    std::ostringstream oss;
    oss<<" create table irisdatabase (";
    oss<<" irisdataid           CHAR(36)             PRIMARY KEY,";
    oss<<" personid             CHAR(36)             null,";
    oss<<" iriscode             bytea                null,";
    oss<<" eyepic               bytea                null,";
    oss<<" matchiriscode        bytea                null,";
    oss<<" devsn                VARCHAR(32)          null,";
    oss<<" eyeflag              INT4                 null,";
    oss<<" regtime              TIMESTAMP WITH TIME ZONE null,";
    oss<<" regpalce             varchar                  null,";
    oss<<" createtime           TIMESTAMP WITH TIME ZONE null,";
    oss<<" updatetime           TIMESTAMP WITH TIME ZONE null,";
    oss<<" memo                 TEXT                 null,";
    oss<<" pupilrow             INT4                 null,";
    oss<<" pupilcol             INT4                 null,";
    oss<<" pupilradius          INT4                 null,";
    oss<<" irisrow              INT4                 null,";
    oss<<" iriscol              INT4                 null,";
    oss<<" irisradius           INT4                 null,";
    oss<<" focusscore           INT4                 null,";
    oss<<" percentvisible       INT4                 null,";
    oss<<" spoofvalue           INT4                 null,";
    oss<<" interlacevalue       INT4                 null,";
    oss<<" qualitylevel         INT4                 null,";
    oss<<" qualityscore         INT4                 null);";

    std::string queryString = oss.str();

    dbSqlit.Query(queryString);

    std::cout << "create irisdatabase table in sqlite over!" << std::endl;
}

typedef struct __uT
{
	uuid_t uu;
}uT;


/*******************************************************************
*           主程序
* 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
* 函数名称：main
* 功    能：主程序
* 说    明：
* 参    数：
* 返 回 值：无
* 创 建 人：yqhe
* 创建日期：2014/2/19 8:54:26
*******************************************************************/
int main( void )
{
    DatabaseAccess dbAccess;
    std::string queryString;

    std::vector<Person> vecPerson;
    std::vector<IrisDataDemo> vecIris;

    int testcount, i;
    DatabaseAccess*  da1[20];

    testcount = 12;
    for (i = 0; i < testcount; i++)
    {
        if (i == 9)
        {
            std::cout << "here" << std::endl;
        }
        std::cout << "get " << i << " session begin" << std::endl;
        da1[i] = new DatabaseAccess();
        std::cout << "get " << i << " session end" << std::endl;

    }

    DatabaseAccess dbtest;
    EnumReturnValue ret = dbtest.Query("SELECT * FROM person");
    std::cout << "ret=" << ret << std::endl;

    // 设置编码，Windows下设置为GBK
    // Linux下用缺省的UNICODE
    // queryString = "set client_encoding to 'UNICODE';";
    // dbAccess.Query(queryString);

    // 查询记录数量，一个返回值
    queryString = "SELECT count(*) from person;";
    int count = 0;

    dbAccess.Query(queryString, count);

    // 显示查询结果
    std::cout << "query person count=" << count << std::endl;

    // 测试获得psql的连接
//    PGconn *_conn = dbAccess.GetPsqlBackend();
//    PQexec(_conn, "DELETE FROM irisdatabase;");
//    PQexec(_conn, "DELETE FROM person;");

    // 测试插入
    // 先删除以前插入的数据
//    std::cout << std::endl;
//    std::cout << "删除之前插入的数据……" << std::endl;
//    queryString = "DELETE FROM irisdatabase;";
//    dbAccess.Query(queryString);
//
//    queryString = "DELETE FROM person;";
//    dbAccess.Query(queryString);

    std::cout << std::endl;
    std::cout << "测试插入数据到Person表……" << std::endl;

    // 模拟人员数据
    vecPerson.clear();

    // 模拟插入一个人的数据
    mockInsertOnePerson(vecPerson, vecIris);

    // 模拟插入第二个人的数据
    // mockInsertOnePerson(vecPerson, vecIris);
    soci::session* pSession = dbAccess.GetSession();
    Transaction tr(*pSession);

    // 向数据库中写Person表
    queryString = "INSERT INTO person (personid, name, departname) VALUES (:personid, :name, :departname);";
    //dbAccess.Insert(queryString, vecPerson);
    dbAccess.InsertWithoutTrans(queryString, vecPerson);

    tr.Commit();

    // 向数据库中写irisdatabase表
    std::cout << "wirte to irisdatabase " << vecIris.size() << " 条记录" << std::endl;
    auto testIt = vecIris.begin();
    for (; testIt!=vecIris.end(); ++testIt)
    {
        testIt->irisImage.PrintBytea();
    }

//    测试插入一条记录的接口，测试完毕后可去掉，因为与后面的插入多条记录冲突
//    std::cout << "插入一条记录" << std::endl;
//    testIt = vecIris.begin();
//    queryString = "INSERT INTO irisdatabase (irisdataid, personid, iriscode, eyepic)
//    VALUES (:irisdataid, :personid, :iriscode, :eyepic);";
//    dbAccess.Insert(queryString, *testIt);

    std::cout << "插入多条记录" << std::endl;
//    queryString = "INSERT INTO irisdatabase (irisdataid, personid, iriscode, eyepic)
//    VALUES (:irisdataid, :personid, :iriscode, :eyepic);";

    queryString = "INSERT INTO irisdatabase (irisdataid, personid, iriscode, eyepic) \
    VALUES (:irisdataid, :personid, :iriscode, :eyepic);";
    dbAccess.Insert(queryString, vecIris);
    // dbAccess.GetPsqlBackend()
    // 测试修改某人数据
    // 修改刚插入的第一个人的姓名和工号
    {
//        auto pIt = vecPerson.begin();
//        std::string orgName = pIt->name;
//        std::string workSn  = orgName;
//        std::string modName = "修改后的名字";
//
//        std::stringstream ss;
//        ss << "UPDATE person SET name=:name, worksn=:worksn WHERE name = '";
//        ss << orgName ;
//        ss << "' ;";
//
//        queryString = ss.str();
//
//        Person updateP;
//        updateP.name   = modName;
//        updateP.workSn = workSn;
//        dbAccess.Update(queryString, updateP);
//
//        ss.str("");
//
//        Operator updateO;
//        updateO.password = "12377";
//
//        ss << "UPDATE operator SET password=:password WHERE name = '";
//        ss << "admin" ;
//        ss << "' ;";
//
//        queryString = ss.str();
//
//        std::cout << queryString << std::endl;
//        dbAccess.Update(queryString, updateO);
//        ss << "UPDATE irisdatabase SET iriscode = '";
//        ss << vecIris.begin()->irisCode.PrepareDbString();
//        ss << "';";
//
//        PGconn *_conn = dbAccess.GetPsqlBackend();
//        queryString = ss.str();
//
//        std::cout << std::endl;
//        std::cout << queryString.c_str() << std::endl;
//        std::string errMsg = PQresultErrorMessage(PQexec(_conn, queryString.c_str()));
//
//        std::cout << "errMsg : " << errMsg << std::endl;

    }



    // 查询人员标中的部门记录
    std::cout << std::endl;
    std::cout << "查询人员表中的部门数据……" << std::endl;
    // std::vector<std::string> vecDepartString;
    std::vector<Person> vecPerson2;


    queryString = "SELECT personid, name, departname FROM person;";
    // dbAccess.Query(queryString, vecDepartString);
    dbAccess.Query(queryString, vecPerson2);

    std::vector<Person>::const_iterator it = vecPerson2.begin();
    {
    int count = 1;
    for (it=vecPerson2.begin(); it!=vecPerson2.end(); ++it)
    {
    	std::cout << "Record " << count << ":" << it->depart << "(" << it->depart.length() << ")" << std::endl;
    	count++;
    }
    }


    // 查询刚写入的记录
    std::cout << std::endl;
    std::cout << "查询刚写入的数据……" << std::endl;

    std::vector<IrisDataTrans> vecReadFromPsql;

    // queryString= "SELECT personid, name FROM person WHERE name='Test001的名字' ;";
    queryString =  "SELECT irisdataid, personid, irisCode, eyepic FROM irisdatabase ";
    // queryString += "limit 2; ";

    dbAccess.Query(queryString, vecReadFromPsql);

    std::cout << "查询到" << std::dec << vecReadFromPsql.size() << "条记录。" << std::endl;

    // 显示查询结果
//    std::vector<IrisDataDemo>::const_iterator it = vecIris.begin();
//    for (it=vecIris.begin(); it!=vecIris.end(); ++it)
//    {
//        const IrisDataDemo &pp = *it;
//        char uuidOut[256];
//        uuid_t ud;
//        pp.pid.Get(ud);
//        uuid_unparse(ud, uuidOut);
//
//        char uuidOut2[256];
//        pp.did.Get(ud);
//        uuid_unparse(ud, uuidOut2);
//
//        std::cout << "did   = " << std::setw(16) << uuidOut2 << std::endl;
//        std::cout << "pid   = " << std::setw(16) << uuidOut << std::endl;
////        std::cout << "name  = " << std::setw(16) << pp.name << std::endl;
////        std::cout << "worksn= " << std::setw(16) << pp.worksn << std::endl;
//
//        std::cout << "print code : " << std::endl;
//        pp.irisCode.PrintBytea();
//
//        std::cout << "print image : " << std::endl;
//        pp.irisImage.PrintBytea();
//
//        size_t size = pp.irisImage.Size();
//        unsigned char * testPtr = (unsigned char*) new unsigned char [size];
//
//        pp.irisImage.Get(testPtr, size);
//
//        for (size_t count=0; count<16; count++)
//        {
//            int c = testPtr[count];
//            std::cout << std::hex << std::setw(2) << std::setfill('0') << c << "-";
//        }
//        delete [] testPtr;
//
//        std::cout << std::endl;
//    }

    // 打开sqlite数据库
    // 删除已存在的文件

    std::string sqliteFile = "/home/yqhe/test.db";
    std::cout << "删除SQLite：" << sqliteFile << std::endl;
    remove(sqliteFile.c_str());

    queryString = "sqlite3:instant:" + sqliteFile;
    DatabaseAccess dbSqlite(queryString, connSingle);

    std::cout << "创建SQLite数据库中的表" << std::endl;
    createIrisTable(dbSqlite);

    std::cout << "向SQLite数据库插入从postgresql读出的数据" << std::endl;
    // vecReadFromPsql是从postgresql数据库中查询到的数据
    queryString = "INSERT INTO irisdatabase (irisdataid, personid, iriscode, eyepic) \
    VALUES (:irisdataid, :personid, :iriscode, :eyepic);";
    dbSqlite.Insert(queryString, vecReadFromPsql);

    // 查询刚写入的记录
    std::cout << std::endl;
    std::cout << "查询刚写入SQLite的数据……" << std::endl;

    std::vector<IrisDataTrans> vecReadFromSqlite;

    // queryString= "SELECT personid, name FROM person WHERE name='Test001的名字' ;";
    queryString =  "SELECT irisdataid, personid, irisCode, eyepic FROM irisdatabase ";
    // queryString += "limit 2; ";

    dbSqlite.Query(queryString, vecReadFromSqlite);

    std::cout << "查询到" << std::dec << vecReadFromSqlite.size() << "条记录。" << std::endl;

    // 删除postgresql中的虹膜特征记录，将sqlite中的数据导入到postgresql
    std::cout << "删除postgresql中原有数据……" << std::endl;
    queryString = "DELETE FROM irisdatabase;";
    dbAccess.Query(queryString);

    // 向postgresql写入从SQLite查出的数据
    std::cout << "向postgresql写入从SQLite查出的数据……" << std::endl;
    queryString = "INSERT INTO irisdatabase (irisdataid, personid, iriscode, eyepic) \
    VALUES (:irisdataid, :personid, :iriscode, :eyepic);";
    dbAccess.Insert(queryString, vecReadFromSqlite);

    // 从postgresql查询并显示数据
    std::vector<IrisDataDemo> vecLastResult;
    queryString =  "SELECT irisdataid, personid, irisCode, eyepic FROM irisdatabase ";
    // queryString += "limit 2; ";

    dbAccess.Query(queryString, vecLastResult);

    std::cout << "查询到" << std::dec << vecLastResult.size() << "条记录。" << std::endl;


    // 显示查询结果
    {
    std::vector<IrisDataDemo>::const_iterator it = vecLastResult.begin();
    for (it=vecLastResult.begin(); it!=vecLastResult.end(); ++it)
    {
        const IrisDataDemo &pp = *it;
        char uuidOut[256];
        uuid_t ud;
        pp.pid.Get(ud);
        uuid_unparse(ud, uuidOut);

        char uuidOut2[256];
        pp.did.Get(ud);
        uuid_unparse(ud, uuidOut2);

        std::cout << "did   = " << std::setw(16) << uuidOut2 << std::endl;
        std::cout << "pid   = " << std::setw(16) << uuidOut << std::endl;

//        std::cout << "name  = " << std::setw(16) << pp.name << std::endl;
//        std::cout << "worksn= " << std::setw(16) << pp.worksn << std::endl;

        std::cout << "print code : " << std::endl;
        pp.irisCode.PrintBytea();
        {
			size_t size = pp.irisCode.Size();
			unsigned char * testPtr = (unsigned char*) new unsigned char [size];

			pp.irisCode.Get(testPtr, size);

			for (size_t count=0; count<256; count++)
			{
				if ((count!=0)&&(count%0x10)==0)
				{
					std::cout << std::endl;
				}

				int c = testPtr[count];
				std::cout << std::hex << std::setw(2) << std::setfill('0') << c << "-";

			}
			delete [] testPtr;
			std::cout << std::endl;
        }


        std::cout << "print image : " << std::endl;
        pp.irisImage.PrintBytea();

        size_t size = pp.irisImage.Size();
        unsigned char * testPtr = (unsigned char*) new unsigned char [size];

        pp.irisImage.Get(testPtr, size);

        for (size_t count=0; count<16; count++)
        {
            int c = testPtr[count];
            std::cout << std::hex << std::setw(2) << std::setfill('0') << c << "-";
        }
        delete [] testPtr;

        std::cout << std::endl;
    }
    }

    std::cout << "before close sqlite" << std::endl;
	dbSqlite.Close();
	std::cout << "before close access" << std::endl;
    dbAccess.Close();

    // 之后测试删除sqlite的数据库，然后再往里写入，看看是什么现象
//    std::string sqliteFile = "/home/yqhe/test.db";
//    std::cout << "删除SQLite：" << sqliteFile << std::endl;
//    remove(sqliteFile.c_str());

    queryString = "sqlite3:instant:" + sqliteFile;
    DatabaseAccess dbSqlite2(queryString, connSingle);

    std::cout << "创建SQLite数据库中的表" << std::endl;
    createIrisTable(dbSqlite2);

    std::cout << "向SQLite数据库插入从postgresql读出的数据" << std::endl;
    // vecReadFromPsql是从postgresql数据库中查询到的数据
    queryString = "INSERT INTO irisdatabase (irisdataid, personid, iriscode, eyepic) \
    VALUES (:irisdataid, :personid, :iriscode, :eyepic);";
    dbSqlite2.Insert(queryString, vecReadFromPsql);

    std::vector<uT> vecU;
    uT tstU;
    uuid_generate(tstU.uu);
    vecU.push_back(tstU);

    std::cout << std::endl;
    std::cout << "按回车键退出本次测试..." << std::endl;
    std::cin.peek();

    return 0;
}
