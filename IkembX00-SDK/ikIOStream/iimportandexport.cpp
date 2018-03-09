#include "iimportandexport.h"

// 用于输出信息
#include <iostream>
#include <vector>

#include "interface.h"
#include "IKBackendSession.h"
#include "IKDatabaseLib.h"
#include "IKDatabaseType.h"
#include "unistd.h"
#include "uuid/uuid.h"
//#include "../IrisAppLib/IrisManager/uuidControl.h"
//#include "uuidControl.h"
#include "../Common/Exectime.h"

IImportAndExport* IImportAndExport::s_iImportAndExport = nullptr;
//std::mutex IImportAndExport::s_mutex;

//获取目标数据库中人员表中的人员ID集合
bool GetPersonIDs(std::vector<IkUuid>& pIDs);


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
std::string OutPutTime(const std::tm &dispTime)
{
	Exectime etm(__FUNCTION__);
	char mbstr[100];

	std::tm bakTime = dispTime;
	std::time_t tt = std::mktime(&bakTime);

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

/*****************************************************************************
 *                         单例模式获取实例
 *  函 数 名：GetIImportAndExport
 *  功    能：获取实例
 *  说    明：
 *  参    数：
 *
 *  返 回 值：IImportAndExport实例
 *  创 建 人：liuzhch
 *  创建时间：2013-02-11
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
IImportAndExport * IImportAndExport::GetIImportAndExport()
{
	Exectime etm(__FUNCTION__);
	if(nullptr == s_iImportAndExport)
	{
		s_iImportAndExport = new IImportAndExport();
	}

	return s_iImportAndExport;
}

IImportAndExport::IImportAndExport()
{
	Exectime etm(__FUNCTION__);
	_dealDataType = NOport;
}

IImportAndExport::~IImportAndExport()
{
	Exectime etm(__FUNCTION__);
	if(nullptr != s_iImportAndExport)
	{
		delete s_iImportAndExport;
	}
}


/**********************************************************************************************************************************
 *                       去除记录集中无效的记录
 *  函 数 名： RemoveInvalidRecogLogRecord
 *  功    能： 将给定的记录数据集中的无效记录（目标数据库中无对应人员）删除
 *  说    明：
 *  参    数：
 *           rec:记录数据集
 *  返 回 值： 被删除的无效记录数
 *  创 建 人：Wang.L
 *  创建时间：2015-10-08
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
	template<typename T>
int IImportAndExport::RemoveInvalidRecord(std::vector<T> &rec)
{
	Exectime etm(__FUNCTION__);
	std::vector<IkUuid> personIDs;
	GetPersonIDs(personIDs);

	int recrmvedCnt = 0;
	for(uint index = 0; index < rec.size(); )
	{
		if(personIDs.end() == find(personIDs.begin(), personIDs.end(), rec[index].personID))
		{
			rec.erase((rec.begin() + index));
			recrmvedCnt++;

			std::cout << "[IImportAndExport.RemoveInvalidRecord]invalid records removed with personid: " <<  rec[index].personID.Unparse() <<  std::endl;
		}
		else
		{
			//仅在当前记录有效时才递增记录索引，否则可能会使一些无效记录成为漏网之鱼；
			++index;
		}
	}

	std::cout << "[IImportAndExport.RemoveInvalidRecord]number of invalid records removed: " <<  recrmvedCnt <<  std::endl;


	return recrmvedCnt;
}

/**********************************************************************************************************************************
 *                       导出函数
 *  函 数 名： ExportToFile
 *  功    能：将数据导出
 *  说    明：
 *  参    数：
 *           fileName:导出文件名
 *           vecT    :要导出数据表名
 *  返 回 值：0为成功，-1为tableList空，否则 返回值为tableList索引的表导出出错
 *  创 建 人：刘中昌
 *  创建时间：2014-01-10
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int IImportAndExport::ExportToMoreFile(const std::string fileName ,const vector<TableType> tableList)
{
	Exectime etm(__FUNCTION__);
	if(tableList.size() == 0)
	{
		return -1;
	}

	int index=0;
	int err = 0;
	for(;index <tableList.size();index ++)
	{
		if(ExportToFile(fileName,tableList[index])!=0)
		{
			err += (1<<index);
		}
	}

	return err;
	//是否全部导出成功
	//    if(index == tableList.size())
	//    {
	//        return 0;
	//    }
	//    else
	//    {
	//        return -1;
	//    }
}

/**********************************************************************************************************************************
 *                       导入函数
 *  函 数 名： ExportToFile
 *  功    能：将数据导出
 *  说    明：
 *  参    数：
 *           fileName:导出文件名
 *           vecT    :要导出数据表名
 *  返 回 值：0为成功，-1为tableList空，否则 返回值为tableList索引的表导入出错
 *  创 建 人：刘中昌
 *  创建时间：2014-01-10
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int IImportAndExport::ImportFromMoreFile(const std::string fileName,const vector<TableType> tableList)
{
	Exectime etm(__FUNCTION__);
	if(tableList.size() == 0)
	{
		return -1;
	}

	int index=0;
	int err = 0;
	for(;index <tableList.size();index ++)
	{
		if(0 != ImportFromFile(fileName,tableList[index]))
		{
			err += (1<<index);
		}
	}

	return err;
	//是否全部导入成功
	//    if(index == tableList.size())
	//    {
	//        return 0;
	//    }
	//    else
	//    {
	//        return -1;
	//    }
}


/**********************************************************************************************************************************
 *                       导出函数
 *  函 数 名： ExportToFile
 *  功    能：将数据导出
 *  说    明：
 *  参    数：
 *           fileName:导出文件名
 *           vecT    :要导出数据
 *  返 回 值：-1为表名错误，0成功，其它导出失败。
 *  创 建 人：刘中昌
 *  创建时间：2014-01-10
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int IImportAndExport::ExportToFile(const string fileName ,const TableType tableType)
{
	Exectime etm(__FUNCTION__);
	//_callBack = callback;
	_dealDataType = Export;

	switch(tableType)
	{
		case TBPersonRecLog:
			return ExportPersonRecLogToFile("sqlite3:instant:"+ fileName);
			break;
		case TBIrisDataBase:
			return ExportIrisDataBaseToFile("sqlite3:instant:"+ fileName);
			break;
		case TBPersonImage:
			return ExportPersonImageToFile("sqlite3:instant:"+ fileName);
			break;
		case TBPerson:
			return ExportPersonToFile("sqlite3:instant:"+ fileName);
			break;
		default:
			break;
	}

	//    _tableType = tableType;
	//    _fileName = fileName;
	//   _threadWaiter.signal();
	return -1;
}
/**********************************************************************************************************************************
 *                       导入函数
 *  函 数 名： ImportFromFile
 *  功    能： 将文件数据导入
 *  说    明：
 *  参    数：
 *           fileName:导出文件名
 *           vecT    :要导出数据
 *  返 回 值：-1为表名错误，0成功，其它导入失败。
 *  创 建 人：刘中昌
 *  创建时间：2014-01-10
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int IImportAndExport::ImportFromFile(const string fileName,const TableType tableType)
{
	Exectime etm(__FUNCTION__);
	//_callBack = callback;
	_dealDataType = Import;

	switch(tableType)
	{
		case TBPersonRecLog:
			return ImportPersonRecLogFromFile("sqlite3:instant:"+ fileName);
			break;
		case TBIrisDataBase:
			return ImportIrisDataBaseFromFile("sqlite3:instant:"+ fileName);
			break;
		case TBPersonImage:
			return ImportPersonImageFromFile("sqlite3:instant:"+ fileName);
			break;
		case TBPerson:
			return ImportPersonFromFile("sqlite3:instant:"+ fileName);
			break;
		default:
			break;
	}
	//    _tableType = tableType;
	//    _fileName = fileName;
	//  _threadWaiter.signal();
	return -1;
}

/**********************************************************************************************************************************
 *                       导出函数
 *  函 数 名： ExportPersonRecLogToFile
 *  功    能：将数据导出
 *  说    明：从postgresql->sqlite
 *  参    数：
 *           fileName:导出文件名
 *           vecT    :要导出数据
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-01-10
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int IImportAndExport::ExportPersonRecLogToFile(const string fileName)
{
	Exectime etm(__FUNCTION__);
	try
	{
		//打开数据库
		DatabaseAccess dbAccess;
		DatabaseAccess dbSqlite(fileName,connPool);

		int err =0;

		std::vector<PersonRecLog> vecPersonRecLog;
		vecPersonRecLog.clear();

		//读取数据库中数据
		if(dbSuccess != dbAccess.Query(SelectSqliteDataTable(TBPersonRecLog),vecPersonRecLog))
		{
			dbSqlite.Close();
			dbAccess.Close();
			std::cout<<"pSQL 查询 PersonRecLog 表错误！"<<std::endl;
			return -2;
		}

		//如果没有数据返回


		//创建数据库表并导出数据
		dbSqlite.Query(DropSqliteDataTable(TBPersonRecLog));
		if(dbSqlite.Query(CreatSqliteDataTable(TBPersonRecLog))==dbSuccess)
		{
			if(vecPersonRecLog.size()<1)
			{
				dbSqlite.Close();
				dbAccess.Close();
				return 0;
			}
			err = dbSqlite.Insert(InsertSqliteDataTable(TBPersonRecLog),vecPersonRecLog);
		}

		dbSqlite.Close();
		dbAccess.Close();
		if(err !=0)
		{
			std::cout<<"导出识别记录失败！"<<std::endl;
			return -1;
		}else
		{
			return err;
		}
	}catch(...)
	{
		return -1;
	}
}
/**********************************************************************************************************************************
 *                       导入函数
 *  函 数 名： ImportPersonRecLogFromFile
 *  功    能： 将文件数据导入
 *  说    明：从sqlite->postgresql
 *  参    数：
 *           fileName:导出文件名
 *           vecT    :要导出数据
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-01-10
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int IImportAndExport::ImportPersonRecLogFromFile(const string fileName)
{
	Exectime etm(__FUNCTION__);
	try
	{
		//打开数据库
		DatabaseAccess dbAccess;
		DatabaseAccess dbSqlite(fileName,connPool);
		int err =0;

		std::vector<PersonRecLog> vecPersonRecLog;
		vecPersonRecLog.clear();

		//读取数据库中数据
		if(dbSuccess != dbSqlite.Query(SelectSqliteDataTable(TBPersonRecLog),vecPersonRecLog))
		{
			dbSqlite.Close();
			dbAccess.Close();
			std::cout<<"Sqlite 查询 PersonRecLog 表错误！"<<std::endl;
			return -2;
		}
		//如果没有数据返回
		if(vecPersonRecLog.size()<1)
		{
			dbSqlite.Close();
			dbAccess.Close();
			//_callBack(1);
			return 0;
		}
		else
		{
			//防止插重
			int recgIdMax =0;
			err = dbAccess.Query("select max(reclogid) from personreclog; ",recgIdMax);
			if(err != dbSuccess)
			{
				dbSqlite.Close();
				dbAccess.Close();
				std::cout<<"Query sql failed. [ select max(reclogid) from personreclog; ]" <<std::endl;
				return -3;
			}
			for(int index =0;index<vecPersonRecLog.size();index++)
			{
				vecPersonRecLog[index].recLogID = recgIdMax + 1;
				recgIdMax++;
			}
		}

		//导入数据
		err = dbAccess.Insert(InsertSqliteDataTable(TBPersonRecLog),vecPersonRecLog);

		dbSqlite.Close();
		dbAccess.Close();
		if(err !=0)
		{
			std::cout<<"导入识别记录失败！"<<std::endl;
			return -1;
		}else
		{
			return err;
		}
	}catch(...)
	{
		return -1;
	}
}

/**********************************************************************************************************************************
 *                       导出函数
 *  函 数 名： ExportIrisDataBaseToFile
 *  功    能：将数据导出
 *  说    明：从postgresql->sqlite
 *  参    数：
 *           fileName:导出文件名
 *           vecT    :要导出数据
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-01-10
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int IImportAndExport::ExportIrisDataBaseToFile(const string fileName)
{
	Exectime etm(__FUNCTION__);
	try
	{
		//打开数据库
		DatabaseAccess dbAccess;
		DatabaseAccess dbSqlite(fileName,connPool);
		int err=0;

		std::vector<IOIrisDataBase> vecIrisDataBase;
		vecIrisDataBase.clear();

		//读取数据库中数据
		//    dbAccess.Query(SelectSqliteDataTable(TBIrisDataBase),vecIrisDataBase);
		if(dbSuccess != dbAccess.Query(SelectSqliteDataTable(TBIrisDataBase),vecIrisDataBase))
		{
			dbSqlite.Close();
			dbAccess.Close();
			std::cout<<"pSQL 查询 TBIrisDataBase 表错误！"<<std::endl;
			return -2;
		}


		//创建数据库表并导出数据
		dbSqlite.Query(DropSqliteDataTable(TBIrisDataBase));
		if(dbSqlite.Query(CreatSqliteDataTable(TBIrisDataBase))==dbSuccess)
		{    //如果没有数据返回
			if(vecIrisDataBase.size()<1)
			{
				dbSqlite.Close();
				dbAccess.Close();
				// _callBack(1);
				return 0;
			}
			err =  dbSqlite.Insert(InsertSqliteDataTable(TBIrisDataBase),vecIrisDataBase);
		}

		dbSqlite.Close();
		dbAccess.Close();
		if(err !=0)
		{
			std::cout<<"导出虹膜失败！"<<std::endl;
			return -1;
		}else
		{
			return err;
		}
	}catch(...)
	{
		return -1;
	}

}
/**********************************************************************************************************************************
 *                       导入函数
 *  函 数 名： ImportIrisDataBaseFromFile
 *  功    能： 将文件数据导入
 *  说    明：从sqlite->postgresql
 *  参    数：
 *           fileName:导出文件名
 *           vecT    :要导出数据
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-01-10
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int IImportAndExport::ImportIrisDataBaseFromFile(const string fileName)
{
	Exectime etm(__FUNCTION__);
	try
	{
		//打开数据库
		DatabaseAccess dbAccess;
		DatabaseAccess dbSqlite(fileName,connPool);
		int err =0;

		std::vector<IOIrisDataBase> vecIrisDataBase;
		std::vector<IOIrisDataBase> vecIrisDataBaseforInsert;
		vecIrisDataBase.clear();
		vecIrisDataBaseforInsert.clear();

		//读取数据库中数据
		//    dbSqlite.Query(SelectSqliteDataTable(TBIrisDataBase),vecIrisDataBase);
		if(dbSuccess != dbSqlite.Query(SelectSqliteDataTable(TBIrisDataBase),vecIrisDataBase))
		{
			dbSqlite.Close();
			dbAccess.Close();
			std::cout<<"[IImportAndExport.ImportIrisDataBaseFromFile]Sqlite 查询 IrisDataBase 表错误！"<<std::endl;
			return -2;
		}
		//如果没有数据返回
		if(vecIrisDataBase.size()<1)
		{
			dbSqlite.Close();
			dbAccess.Close();
			// _callBack(1);
			return 0;
		}
		else
		{
			std::vector<IkUuid> irisDataUUID;

			dbAccess.Query("select irisdataid from irisdatabase; ",irisDataUUID);
			for(int index =0;index <vecIrisDataBase.size();index++ )
			{
				int idSize=0;
				for(;idSize<irisDataUUID.size();idSize ++)
				{
					if(vecIrisDataBase[index].irisDataID == irisDataUUID[idSize])
					{
						break;
					}
				}

				if(idSize==irisDataUUID.size())
				{
					vecIrisDataBaseforInsert.push_back(vecIrisDataBase[index]);
				}
			}
		}

		//增加去除无效识别记录By：Wang.L @20151009
		RemoveInvalidRecord(vecIrisDataBaseforInsert);

		struct timeval t_start,t_end;
		gettimeofday(&t_start,nullptr);
		//导入数据
		err = dbAccess.Insert(InsertSqliteDataTable(TBIrisDataBase),vecIrisDataBaseforInsert);

		gettimeofday(&t_end,nullptr);
		double timediff =(t_end.tv_sec - t_start.tv_sec)* 1000000 + t_end.tv_usec -t_start.tv_usec;
		std::cout << "[IImportAndExport.ImportIrisDataBaseFromFile]导入虹膜数据用时: " << timediff <<std::endl;
		dbSqlite.Close();
		dbAccess.Close();
		if(err !=0)
		{
			std::cout<<"[IImportAndExport.ImportIrisDataBaseFromFile]导入虹膜数据失败！"<<std::endl;
			return -1;
		}else
		{
			return err;
		}
	}catch(...)
	{
		return -1;
	}
}

/**********************************************************************************************************************************
 *                       导出函数
 *  函 数 名： ExportPersonImageToFile
 *  功    能：将数据导出
 *  说    明：从postgresql->sqlite
 *  参    数：
 *           fileName:导出文件名
 *           vecT    :要导出数据
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-01-10
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int IImportAndExport::ExportPersonImageToFile(const string fileName)
{
	Exectime etm(__FUNCTION__);
	try
	{
		//打开数据库
		DatabaseAccess dbAccess;
		DatabaseAccess dbSqlite(fileName, connPool);
		int err =0;

		std::vector<PersonImage> vecPersonImage;
		vecPersonImage.clear();

		//读取数据库中数据
		//    dbAccess.Query(SelectSqliteDataTable(TBPersonImage),vecPersonImage);
		if(dbSuccess != dbAccess.Query(SelectSqliteDataTable(TBPersonImage),vecPersonImage))
		{
			dbSqlite.Close();
			dbAccess.Close();
			std::cout<<"pSQL 查询 PersonImage 表错误！"<<std::endl;
			return -2;
		}
		//如果没有数据返回


		//创建数据库表并导出数据
		dbSqlite.Query(DropSqliteDataTable(TBPersonImage));
		if( dbSqlite.Query(CreatSqliteDataTable(TBPersonImage))==dbSuccess)
		{
			if(vecPersonImage.size()<1)
			{
				dbSqlite.Close();
				dbAccess.Close();
				return 0;
			}
			err =  dbSqlite.Insert(InsertSqliteDataTable(TBPersonImage),vecPersonImage);
		}

		dbSqlite.Close();
		dbAccess.Close();
		if(err !=0)
		{
			std::cout<<"导出人员图像失败！"<<std::endl;
			return -1;
		}else
		{
			return err;
		}
	}catch(...)
	{
		return -1;
	}
}
/**********************************************************************************************************************************
 *                       导入函数
 *  函 数 名： ImportPersonImageFromFile
 *  功    能： 将文件数据导入
 *  说    明：从sqlite->postgresql
 *  参    数：
 *           fileName:导出文件名
 *           vecT    :要导出数据
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-01-10
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int IImportAndExport::ImportPersonImageFromFile(const string fileName)
{
	Exectime etm(__FUNCTION__);
	try
	{
		//打开数据库
		DatabaseAccess dbAccess;
		DatabaseAccess dbSqlite(fileName,connPool);
		int err =0;

		std::vector<PersonImage> vecPersonImage;
		std::vector<PersonImage> vecPersonImageforInsert;
		vecPersonImage.clear();
		vecPersonImageforInsert.clear();

		//读取数据库中数据
		//    dbSqlite.Query(SelectSqliteDataTable(TBPersonImage),vecPersonImage);
		if(dbSuccess != dbSqlite.Query(SelectSqliteDataTable(TBPersonImage),vecPersonImage))
		{
			dbSqlite.Close();
			dbAccess.Close();
			std::cout<<"Sqlite 查询 PersonImage 表错误！"<<std::endl;
			return -2;
		}

		//如果没有数据返回
		if(vecPersonImage.size()<1)
		{
			dbSqlite.Close();
			dbAccess.Close();
			return 0;
		}
		else
		{
			std::vector<IkUuid> imageUUID;

			err = dbAccess.Query("select imageid from personimage; ",imageUUID);
			if(err != dbSuccess)
			{
				dbSqlite.Close();
				dbAccess.Close();
				std::cout << "Query sql failed. [select imageid from personimage;]" << std::endl;
				return -3;
			}
			for(int index =0;index <vecPersonImage.size();index++ )
			{
				int idSize=0;
				for(;idSize<imageUUID.size();idSize ++)
				{
					if(vecPersonImage[index].imageID == imageUUID[idSize])
					{
						break;
					}
				}

				if(idSize==imageUUID.size())
				{
					vecPersonImageforInsert.push_back(vecPersonImage[index]);
				}
			}
		}


		//增加去除无效人员图像记录By：Wang.L @20151009
		RemoveInvalidRecord(vecPersonImageforInsert);


		//导入数据
		err = dbAccess.Insert(InsertSqliteDataTable(TBPersonImage),vecPersonImageforInsert);

		dbSqlite.Close();
		dbAccess.Close();
		if(err !=0)
		{
			std::cout<<"导入人员图像失败！"<<std::endl;
			return -1;
		}else
		{
			return err;
		}
	}catch(...)
	{
		return -1;
	}
}

/**********************************************************************************************************************************
 *                       导出函数
 *  函 数 名： ExportPersonToFile
 *  功    能：将数据导出
 *  说    明：从postgresql->sqlite
 *  参    数：
 *           fileName:导出文件名
 *           vecT    :要导出数据
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-01-10
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int IImportAndExport::ExportPersonToFile(const string fileName)
{
	Exectime etm(__FUNCTION__);
	try
	{
		//打开数据库
		DatabaseAccess dbAccess;
		DatabaseAccess dbSqlite(fileName,connPool);
		int err =0;
		std::vector<Person>  vecPerson;
		vecPerson.clear();

		//读取数据库中数据
		if(dbAccess.Query(SelectSqliteDataTable(TBPerson),vecPerson)!=dbSuccess)
		{
			dbSqlite.Close();
			dbAccess.Close();
			std::cout<<"从数据库获取人员信息失败！"<<std::endl;
			return -2;
		}



		//创建数据库表并导出数据
		dbSqlite.Query(DropSqliteDataTable(TBPerson));
		if( dbSqlite.Query(CreatSqliteDataTable(TBPerson)) ==dbSuccess)
		{
			//如果没有数据返回
			if(vecPerson.size()<1)
			{
				dbSqlite.Close();
				dbAccess.Close();
				std::cout<<"数据库中无人员信息！"<<std::endl;
				return 0;
			}
			err = dbSqlite.Insert(InsertSqliteDataTable(TBPerson),vecPerson);
		}

		dbSqlite.Close();
		dbAccess.Close();
		if(err !=0)
		{
			std::cout<<"导出人员信息失败！"<<std::endl;
			return -1;
		}else
		{
			return err;
		}

		return -1;
	}catch(...)
	{
		return -1;
	}

}
/**********************************************************************************************************************************
 *                       导入函数
 *  函 数 名： ImportPersonFromFile
 *  功    能： 将文件数据导入
 *  说    明：从sqlite->postgresql
 *  参    数：
 *           fileName:导出文件名
 *           vecT    :要导出数据
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-01-10
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int IImportAndExport::ImportPersonFromFile(const string fileName)
{
	Exectime etm(__FUNCTION__);
	try
	{
		//打开数据库
		DatabaseAccess dbAccess;
		DatabaseAccess dbSqlite(fileName,connPool);
		int err =0;

		std::vector<Person>  vecPerson;
		std::vector<Person>  vecPersonForInsert;
		vecPerson.clear();
		vecPersonForInsert.clear();

		//读取数据库中数据
		//    dbSqlite.Query(SelectSqliteDataTable(TBPerson),vecPerson);
		if(dbSuccess != dbSqlite.Query(SelectSqliteDataTable(TBPerson),vecPerson))
		{
			dbSqlite.Close();
			dbAccess.Close();
			std::cout<<"Sqlite 查询 Person 表错误！"<<std::endl;
			return -2;
		}

		//如果没有数据返回
		if(vecPerson.size()<1)
		{
			dbSqlite.Close();
			dbAccess.Close();
			//std::cout<<"文件 \""<<fileName<<"\"无人员信息！"<<std::endl;
			return 0;
		}
		else
		{
			std::vector<IkUuid> personUUID;
			std::vector<std::string> vecWorkSN;

			err = dbAccess.Query("select personid from person; ",personUUID);
			if( err != dbSuccess)
			{
				dbSqlite.Close();
				dbAccess.Close();
				std::cout<< "Query sql  failed. [select personid from person;]" << std::endl;
				return -3;
			}

			err = dbAccess.Query("select worksn from person; ",vecWorkSN);
			if( err != dbSuccess)
			{
				dbSqlite.Close();
				dbAccess.Close();
				std::cout<< "Query sql  failed. [select worksn from person;]" << std::endl;
				return -3;
			}

			for(int index =0;index <vecPerson.size();index++ )
			{
				int idSize=0;
				for(;idSize<personUUID.size();idSize ++)
				{
					if(vecPerson[index].personID == personUUID[idSize])
					{
						break;
					}
				}

				if(idSize==personUUID.size())
				{
					vecPersonForInsert.push_back(vecPerson[index]);
				}
			}

			vecPerson.clear();

			for(int index =0;index <vecPersonForInsert.size();index++ )
			{
				int idSize=0;
				for(;idSize<vecWorkSN.size();idSize ++)
				{
					if(!vecPersonForInsert[index].workSn.empty() && vecPersonForInsert[index].workSn == vecWorkSN[idSize])
					{
						break;
					}
				}

				if(idSize==vecWorkSN.size())
				{
					vecPerson.push_back(vecPersonForInsert[index]);
				}
			}

		}
		//导入数据
		err =  dbAccess.Insert(InsertSqliteDataTable(TBPerson),vecPerson);

		dbSqlite.Close();
		dbAccess.Close();
		if(err !=0)
		{
			std::cout<<"导入人员信息失败！"<<std::endl;
			return -1;
		}else
		{
			return err;
		}
	}catch(...)
	{
		return -1;
	}
}

int IImportAndExport::TestSQL()
{
	Exectime etm(__FUNCTION__);
	//    int testResult =0;
	//    DatabaseAccess dbAccess;

	//    //DatabaseAccess dbAccess("postgresql:instant:host=10.2.1.111 dbname=irisApp user=postgres password=123456",connSingle);
	//    //testResult = dbAccess.Query(DropSqliteDataTable(TBPersonImage));
	//    //std::cout<<testResult<<endl;

	//    //测试创建数据表
	//    //     testResult =  dbAccess.Query(CreatSqliteDataTable(TBPersonRecLog));
	//    //     std::cout<<testResult<<endl;
	//    //     testResult =  dbAccess.Query(CreatSqliteDataTable(TBIrisDataBase));
	//    //     std::cout<<testResult<<endl;
	//    //     testResult =  dbAccess.Query(CreatSqliteDataTable(TBPersonImage));
	//    //     std::cout<<testResult<<endl;
	//    //     testResult =  dbAccess.Query(CreatSqliteDataTable(TBPerson));
	//    //     std::cout<<testResult<<endl;


	//    std::vector<Person>  vecPerson;
	//    std::vector<PersonImage> vecPersonImage;
	//    std::vector<PersonRecLog> vecPersonReclog;
	//    std::vector<IrisDataBase> vecIrisDataBase;

	//    UuidControl uuidCtrl;

	//    //    for(int i=0;i<10;i++)
	//    //    {
	//    //        //uuid
	//    //        Person person;
	//    //        PersonImage personImage;
	//    //        PersonRecLog personRecg;
	//            IrisDataBase irisData;

	//            // 生成人员UUID
	//            uuid_t udPerson;
	//            uuid_t udIrisDataID;
	//            uuid_t udImageID;
	//            uuidCtrl.GetUUID(udPerson);
	//            uuidCtrl.GetUUID(udIrisDataID);
	//            uuidCtrl.GetUUID(udImageID);

	//    //        person.personID = udPerson;
	//    //        person.name = "Test";
	//    //        person.sex = (i%2==0)?"男":"女";
	//    //        //person.disableTime ="2014-02-23 14:50:05";
	//    //        time_t tim(NULL);
	//    //        person.disableTime = *localtime(&tim);//nullptr;//

	//    //        person.departID =1;

	//    //        personImage.imageID = udImageID;
	//    //        personImage.personID = udPerson;
	//    //        personImage.memo = "memo";
	//    //        personImage.personImage ="sodaifoaidjfoajfojadsoijaofjoadjs";

	//    //        personRecg.delTime = *localtime(&tim);
	//    //        personRecg.personID =udPerson;

	//    //        //          int tm_sec;			/* Seconds.	[0-60] (1 leap second) */
	//    //        //          int tm_min;			/* Minutes.	[0-59] */
	//    //        //          int tm_hour;			/* Hours.	[0-23] */
	//    //        //          int tm_mday;			/* Day.		[1-31] */
	//    //        //          int tm_mon;			/* Month.	[0-11] */
	//    //        //          int tm_year;			/* Year	- 1900.  */
	//    //        //          int tm_wday;			/* Day of week.	[0-6] */
	//    //        //          int tm_yday;			/* Days in year.[0-365]	*/
	//    //        //          int tm_isdst;	       /* DST.		[-1/0/1]*/
	//    //        //          personRecg.recogTime.tm_sec = 4;//"2014-02-23 14:50:05";
	//    //        //          personRecg.recogTime.tm_min = 4;//"2014-02-23 14:50:05";
	//    //        //          personRecg.recogTime.tm_hour = 4;//"2014-02-23 14:50:05";
	//    //        //          personRecg.recogTime.tm_mday = 4;//"2014-02-23 14:50:05";
	//    //        //          personRecg.recogTime.tm_mon = 4;//"2014-02-23 14:50:05";
	//    //        //          personRecg.recogTime.tm_year = 2014;//

	//    //        personRecg.recogTime = *localtime(&tim);
	//    //        personRecg.recLogID = i +1;
	//    //        personRecg.irisDataID = udIrisDataID;
	//    //        personRecg.devSN = "000-000-000-000";
	//    //        personRecg.devType = 3;
	//    //        personRecg.recogType =3;
	//    //        personRecg.atPostion ="beijing";
	//    //        //          std::string irisCode;
	//    //        //          std::string eyePic;
	//    //        //          std::string matchIrisCode;
	//    //        //          std::string devSn ;
	//    //        //          int  eyeFlag;
	//    //        //          std::string regTime         ;
	//    //        //          std::string regPalce      ;
	//    //        //          std::string createTime    ;
	//    //        //          std::string updateTime;
	//    //        //          std::string memo;
	//            irisData.personID =udPerson;
	//            irisData.irisDataID =udIrisDataID;
	//            //          irisData.memo = "aaab4bb" ;
	//            unsigned char testCode[512];
	//            for(int count =0; count<512;count++)
	//            {
	//                testCode[count] =( unsigned char )count;
	//            }

	//            irisData.irisCode.Set(&testCode[0],512);// = "aaab4bb" ;
	//    //        //          irisData.eyePic = "aa4abbb" ;
	//    //        //          irisData.matchIrisCode = "aaa4bbb" ;
	//            irisData.devSn = "aaabb4b" ;

	//    //        irisData.createTime = *localtime(&tim);
	//    //        irisData.updateTime  = *localtime(&tim);
	//    //        irisData.regTime  =  *localtime(&tim);
	//    //        irisData.regPalce  = "北京beijing";

	//    //        vecPerson.push_back(person);
	//    //        vecPersonImage.push_back(personImage);
	//    //        vecPersonReclog.push_back(personRecg);
	//            vecIrisDataBase.push_back(irisData);
	//    //    }

	//    //    //测试插入信息
	//    //    testResult =  dbAccess.Insert(InsertSqliteDataTable(TBPerson),vecPerson);
	//    //    std::cout<<testResult<<endl;
	//    //    testResult =  dbAccess.Insert(InsertSqliteDataTable(TBPersonImage),vecPersonImage);
	//    //    std::cout<<testResult<<endl;
	//    //    testResult =  dbAccess.Insert(InsertSqliteDataTable(TBIrisDataBase),vecIrisDataBase);
	//    //    std::cout<<testResult<<endl;
	//    //    testResult =  dbAccess.Insert(InsertSqliteDataTable(TBPersonRecLog),vecPersonReclog);
	//    //    std::cout<<testResult<<endl;


	//    //测试选择信息
	////    testResult =  dbAccess.Query(SelectSqliteDataTable(TBPerson),vecPerson);
	//////    std::cout<<vecPerson.begin()->sex<<std::endl;
	//////    std::cout<<vecPerson.size()<<std::endl;
	////    std::cout<<testResult<<std::endl;
	////    testResult =  dbAccess.Query(SelectSqliteDataTable(TBPersonImage),vecPersonImage);
	////    std::cout<<testResult<<std::endl;
	//    testResult =  dbAccess.Query(SelectSqliteDataTable(TBIrisDataBase),vecIrisDataBase);
	//    std::cout<<vecIrisDataBase[0].regTime<<std::endl;
	////    std::cout<<testResult<<std::endl;
	////    testResult =  dbAccess.Query(SelectSqliteDataTable(TBPersonRecLog),vecPersonReclog);
	////    std::cout<<testResult<<std::endl;

	return 0;
}

/**********************************************************************************************************************************
 *                       删除表语句函数
 *  函 数 名： DropSqliteDataTable
 *  功    能： 获取删除表的SQL 语句
 *  说    明：
 *  参    数：
 *           tableType:表名类型
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-02-18
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
const string IImportAndExport::DropSqliteDataTable(const TableType tableType)
{
	Exectime etm(__FUNCTION__);
	string queryString;
	std::ostringstream oss;

	switch(tableType)
	{
		case TBPersonRecLog:
			oss<<" DROP TABLE personreclog;";
			break;
		case TBIrisDataBase:
			oss<<" DROP TABLE irisdatabase;";
			break;
		case TBPersonImage:
			oss<<" DROP TABLE personimage;";
			break;
		case TBPerson:
			oss<<" DROP TABLE person;";
			break;
		default:
			break;
	}

	queryString = oss.str();
	return queryString;
}

/**********************************************************************************************************************************
 *                       创建表语句函数
 *  函 数 名： GetCreatSqliteDataTable
 *  功    能： 获取创建表的SQL 语句
 *  说    明： 测试通过
 *  参    数：
 *           tableType:表名类型
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-02-18
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
const std::string IImportAndExport::CreatSqliteDataTable(const TableType tableType)
{
	Exectime etm(__FUNCTION__);
	string queryString;
	std::ostringstream oss;

	switch(tableType)
	{
		case TBPersonRecLog://创建识别记录表
			oss<<" CREATE TABLE personreclog (";
			oss<<" reclogid              int4                      PRIMARY KEY, ";
			oss<<" personid             CHAR(36)                 null,";
			oss<<" irisdataid            CHAR(36)                 null,";
			oss<<" recogtime             TIMESTAMP WITH TIME ZONE null,";
			oss<<" recogtype             INT2                     null,";
			oss<<" atpostion             varchar                  null, ";
			oss<<" devsn                 varchar                  null,";
			oss<<" devtype               int2                     null,";
			oss<<" deltime               TIMESTAMP WITH TIME ZONE null,";
			oss<<" uploadstatus          int4                     null,";
			oss<<" memo                  varchar                  null);";
			break;
		case TBIrisDataBase:
			oss<<" create table irisdatabase (";
			oss<<" irisdataid           CHAR(36)             PRIMARY KEY,";
			oss<<" personid             CHAR(36)             null,";
			oss<<" iriscode             varchar              null,";
			oss<<" eyepic               varchar              null,";
			oss<<" matchiriscode        varchar              null,";
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
			break;
		case TBPersonImage:
			oss<<" create table personimage ( ";
			oss<<" imageid              CHAR(36)             PRIMARY KEY, ";
			oss<<" personid             CHAR(36)             not null,";
			oss<<" personimage          bytea                not null,";
			oss<<" memo                 TEXT                 null);";

			break;
		case TBPerson:
			oss<<" create table person (";
			oss<<" personid             CHAR(36)             PRIMARY KEY,";
			oss<<" name                 VARCHAR(32)          null,";
			oss<<" sex                  CHAR(2)              null,";
			oss<<" cardid               CHAR(16)             null,";
			oss<<" idcard               CHAR(18)             null,";
			oss<<" memo                 TEXT                 null,";
			oss<<" departid             INT4                 null,";
			oss<<" departname           CHAR(64)             null,";
			oss<<" superid               int4                null,";
			oss<<" hasirisdata           int4                null,";
			oss<<" worksn                varchar             null,";
			oss<<" disabletime       TIMESTAMP WITH TIME ZONE null);";
			break;
		default:
			break;
	}
	queryString = oss.str();

	return queryString;
}

/**********************************************************************************************************************************
 *                       插入SQL语句函数
 *  函 数 名： InsertSqliteDataTable
 *  功    能： 获取插入数据SQL 语句
 *  说    明：
 *  参    数：
 *           tableType:表名类型
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-02-18
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
const string IImportAndExport::InsertSqliteDataTable(const TableType tableType)
{
	Exectime etm(__FUNCTION__);
	string queryString;
	std::ostringstream oss;

	//    insert into blog_notecolumn select 'java' from dual where not exists (select * from blog_notecolumn where columnname='java');
	switch(tableType)
	{
		case TBPersonRecLog:
			oss<<" INSERT INTO personreclog (";
			oss<<" reclogid, ";
			oss<<" personid, ";
			oss<<" irisdataid,";
			oss<<" recogtime,";
			oss<<" recogtype, ";
			oss<<" atpostion, ";
			oss<<" devsn,";
			oss<<" devtype,";
			oss<<" deltime,";
			oss<<" uploadstatus,";
			oss<<" memo) ";
			oss<<" values (";
			oss<<" :reclogid, ";
			oss<<" :personid, ";
			oss<<" :irisdataid,";
			oss<<" :recogtime,";
			oss<<" :recogtype, ";
			oss<<" :atpostion, ";
			oss<<" :devsn,";
			oss<<" :devtype,";
			oss<<" :deltime,";
			oss<<" :uploadstatus,";
			oss<<" :memo); ";
			break;
		case TBIrisDataBase:
			oss<<" INSERT INTO irisdatabase (";
			oss<<" irisdataid,";
			oss<<" personid,";
			oss<<" iriscode,";
			oss<<" eyepic,";
			oss<<" matchiriscode,";
			oss<<" devsn,";
			oss<<" eyeflag,";
			oss<<" regtime,";
			oss<<" regpalce,";
			oss<<" createtime,";
			oss<<" updatetime,";
			oss<<" memo,";
			oss<<" pupilrow,";
			oss<<" pupilcol,";
			oss<<" pupilradius,";
			oss<<" irisrow,";
			oss<<" iriscol,";
			oss<<" irisradius,";
			oss<<" focusscore,";
			oss<<" percentvisible,";
			oss<<" spoofvalue,";
			oss<<" interlacevalue,";
			oss<<" qualitylevel,";
			oss<<" qualityscore";
			oss<<") values  ( ";
			oss<<" :irisdataid,";
			oss<<" :personid,";
			oss<<" :iriscode,";
			oss<<" :eyepic,";
			oss<<" :matchiriscode,";
			oss<<" :devsn,";
			oss<<" :eyeflag,";
			oss<<" :regtime,";
			oss<<" :regpalce,";
			oss<<" :createtime,";
			oss<<" :updatetime,";
			oss<<" :memo,";
			oss<<" :pupilrow,";
			oss<<" :pupilcol,";
			oss<<" :pupilradius,";
			oss<<" :irisrow,";
			oss<<" :iriscol,";
			oss<<" :irisradius,";
			oss<<" :focusscore,";
			oss<<" :percentvisible,";
			oss<<" :spoofvalue,";
			oss<<" :interlacevalue,";
			oss<<" :qualitylevel,";
			oss<<" :qualityscore);";
			break;
		case TBPersonImage:
			oss<<" INSERT INTO  PersonImage ( ";
			oss<<" imageid, ";
			oss<<" personid,";
			oss<<" personimage,";
			oss<<" memo)";
			oss<<" values ( ";
			oss<<" :imageid, ";
			oss<<" :personid,";
			oss<<" :personimage,";
			oss<<" :memo);";

			break;
		case TBPerson:
			oss<<" INSERT INTO  Person (";
			oss<<" personid,";
			oss<<" name,";
			oss<<" sex,";
			oss<<" cardid,";
			oss<<" idcard,";
			oss<<" memo,";
			oss<<" departid,";
			oss<<" departname,";
			oss<<" superid,";
			oss<<" hasirisdata,";
			oss<<" worksn,";
			oss<<" disabletime";
			oss<<") values (";
			oss<<" :personid,";
			oss<<" :name,";
			oss<<" :sex,";
			oss<<" :cardid,";
			oss<<" :idcard,";
			oss<<" :memo,";
			oss<<" :departid,";
			oss<<" :departname,";
			oss<<" :superid,";
			oss<<" :hasirisdata,";
			oss<<" :worksn,";
			oss<<" :disabletime);";
			break;
		default:
			break;
	}
	queryString = oss.str();

	return queryString;
}

/**********************************************************************************************************************************
 *                       选择SQL语句函数
 *  函 数 名： SelectSqliteDataTable
 *  功    能： 获取查询SQL 语句
 *  说    明：
 *  参    数：
 *           tableType:表名类型
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2014-02-18
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
const string IImportAndExport::SelectSqliteDataTable(const TableType tableType)
{
	Exectime etm(__FUNCTION__);
	string queryString;
	std::ostringstream oss;

	switch(tableType)
	{
		case TBPersonRecLog:
			oss<<" SELECT ";
			oss<<" reclogid, ";
			oss<<" personid, ";
			oss<<" irisdataid,";
			oss<<" recogtime,";
			oss<<" recogtype, ";
			oss<<" atpostion, ";
			oss<<" devsn,";
			oss<<" devtype,";
			oss<<" deltime,";
			oss<<" uploadstatus,";
			oss<<" memo ";
			oss<<" FROM personreclog;";
			break;
		case TBIrisDataBase:
			oss<<" SELECT ";
			oss<<" irisdataid,";
			oss<<" personid,";
			oss<<" iriscode,";
			oss<<" eyepic,";
			oss<<" matchiriscode,";
			oss<<" devsn,";
			oss<<" eyeflag,";
			oss<<" regtime,";
			oss<<" regpalce,";
			oss<<" createtime,";
			oss<<" updatetime,";
			oss<<" memo,";
			oss<<" pupilrow,";
			oss<<" pupilcol,";
			oss<<" pupilradius,";
			oss<<" irisrow,";
			oss<<" iriscol,";
			oss<<" irisradius,";
			oss<<" focusscore,";
			oss<<" percentvisible,";
			oss<<" spoofvalue,";
			oss<<" interlacevalue,";
			oss<<" qualitylevel,";
			oss<<" qualityscore ";
			oss<<" FROM irisdatabase ;";
			break;
		case TBPersonImage:
			oss<<" SELECT  ";
			oss<<" imageid, ";
			oss<<" personid,";
			oss<<" personimage,";
			oss<<" memo ";
			oss<<" FROM  personimage ;";
			break;
		case TBPerson:
			oss<<" SELECT ";
			oss<<" personid,";
			oss<<" name,";
			oss<<" sex,";
			oss<<" cardid,";
			oss<<" idcard,";
			oss<<" memo,";
			oss<<" departid,";
			oss<<" departname,";
			oss<<" superid,";
			oss<<" hasIrisdata,";
			oss<<" worksn,";
			oss<<" disabletime";
			oss<<" FROM person;";

			break;
		default:
			break;
	}
	queryString = oss.str();

	return queryString;
}



/**********************************************************************************************************************************
 *                       获取人员表中的人员ID集合
 *  函 数 名： GetPersonIDs
 *  功    能： 获取目标数据库中人员表的所有人员ID集合，用于去除识别记录、虹膜特征记录和人员图像记录中的无效记录
 *  说    明：
 *  参    数：
 *           pIDs：用于返回查询得到的人员ID集合
 *  返 回 值：成功返回true，其他返回false
 *  创 建 人：Wang.L
 *  创建时间：2015-10-08
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
bool GetPersonIDs(std::vector<IkUuid>& pIDs)
{
	Exectime etm(__FUNCTION__);
	//打开数据库
	DatabaseAccess dbAccess;
	return dbSuccess == dbAccess.Query("select personid from person; ", pIDs);
}

