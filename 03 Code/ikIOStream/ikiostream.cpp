//#include "ikiostream.h"
//#include "pubfun.h"
////using namespace std;

///**********************************************************************************************************************************
//*                       导出函数
//*  函 数 名： ExportToFileBySQLite
//*  功    能： 将文件数据导出
//*  说    明：
//*  参    数：
//*           fileName:导出文件名
//*           vecT    :要导出数据
//*  返 回 值：
//*  创 建 人：刘中昌
//*  创建时间：2014-01-17
//*  修 改 人：
//*  修改时间：
//*********************************************************************************************************************************/
//int IkIOStream::ExportToFileBySQLite(const char *fileName, const std::vector< Person>& vecT)
//{
//    if(!_dataBase.InitDataBase(fileName))
//    {
//        return -1;
//    }

//    _dataBase.DropTable("Person");

////    string creatTableSQL = "";
//    const string creatTableSQL = "CREATE TABLE Person( \
//                  personID INTEGER PRIMARY KEY, \
//                  name NVARCHAR, \
//                  sex NVARCHAR, \
//                  cardID NVARCHAR, \
//                  IDcard NVARCHAR, \
//                  departName NVARCHAR, \
//                  memo NVARCHAR, \
//                  departID INTEGER,  \
//                  disableTime DATE);";

//    if(!_dataBase.CreateTable(creatTableSQL))
//    {
//        return -3;
//    }

//    if(!_dataBase.BatchBegin())
//    {
//        return -4;
//    }

//    for (int index =0; index <vecT.size();index++)
//    {
//        string insertSQL = "INSERT INTO Person VALUES ( ";
//        if(vecT[index].personID == 0)
//        {
//            continue;
//        }
//        insertSQL+= Num2Str(vecT[index].personID);
//        insertSQL += " , \"";
//        insertSQL += vecT[index].name;
//        insertSQL += "\" , \"";
//        insertSQL += vecT[index].sex;
//        insertSQL += "\" ,\" ";
//        insertSQL += vecT[index].cardID;
//        insertSQL += "\" ,\" ";
//        insertSQL += vecT[index].IDcard;
//        insertSQL += "\" , \"";
//        insertSQL += vecT[index].departName;
//        insertSQL += "\" , \"";
//        insertSQL += vecT[index].memo;
//        insertSQL += "\" , ";
//        insertSQL += Num2Str(vecT[index].departID);
//        insertSQL += " , \"";
//        insertSQL += vecT[index].disableTime;
//        insertSQL +="\");";
//        if(!_dataBase.BatchInsert(insertSQL))
//        {
//            return -5;
//        }
//    }

//    if(!_dataBase.BatchCommit())
//    {
//        return -6;
//    }

//    _dataBase.CloseDataBase();

//    return 0;
//}

///**********************************************************************************************************************************
//*                       导入函数
//*  函 数 名： ImportFromFileBySQLite
//*  功    能： 将文件数据导入
//*  说    明：
//*  参    数：
//*           fileName:导出文件名
//*           vecT    :要导出数据
//*  返 回 值：
//*  创 建 人：刘中昌
//*  创建时间：2014-01-17
//*  修 改 人：
//*  修改时间：
//*********************************************************************************************************************************/
//int IkIOStream::ImportFromFileBySQLite(const char *fileName,  std::vector< Person >& vecT)
//{
//    if(!_dataBase.InitDataBase(fileName))
//    {
//        return -1;
//    }
//    QueryResult queryResult;
//    if(_dataBase.SelectSQL("SELECT * from Person;",queryResult))
//    {
//        int  index = queryResult.col;

//        // query 的字段值是连续的，从第0索引到第 nColumn - 1索引都是字段名称，
//        //从第 nColumn 索引开始，后面都是字段值，它把一个二维的表（传统的行列表示法）用一个扁平的形式来表示
//        for(int  row = 0; row < queryResult.row ; row++ )
//        {
//            Person tempPerson;

//                tempPerson.personID    = Str2Int(queryResult.result[index]);
//                tempPerson.name        = Num2Str(queryResult.result[index+1]);
//                tempPerson.sex         = Num2Str(queryResult.result[index+2]);
//                tempPerson.cardID      = Num2Str(queryResult.result[index+3]);
//                tempPerson.IDcard      = Num2Str(queryResult.result[index+4]);
//                tempPerson.departName  = Num2Str(queryResult.result[index+5]);
//                tempPerson.memo        = Num2Str(queryResult.result[index+6]);
//                tempPerson.departID    = Str2Int(queryResult.result[index+7]);
//                tempPerson.disableTime = Num2Str(queryResult.result[index+8]);
//                index += queryResult.col;
//                vecT.push_back(tempPerson);
//        }

//    }
//    //清楚缓存数据
//   _dataBase.ClearResult(queryResult);
//}
