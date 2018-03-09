#include "ikiostream.h"
#include "iimportandexport.h"
#include "../Common/Exectime.h"

IImportAndExport * s_importAndExport =  nullptr;
std::shared_ptr<ikIOStream> ikIOStream::s_ioStream;

ikIOStream::ikIOStream()
{
	Exectime etm(__FUNCTION__);
	s_importAndExport = IImportAndExport::GetIImportAndExport();
}

/*****************************************************************************
 *                         单例模式获取实例
 *  函 数 名：GetIkIOStream
 *  功    能：获取实例
 *  说    明：
 *  参    数：
 *
 *  返 回 值：ikIOStream实例
 *  创 建 人：liuzhch
 *  创建时间：2013-02-11
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
ikIOStream * ikIOStream::GetIkIOStream()
{
	Exectime etm(__FUNCTION__);
	if(nullptr == s_ioStream)
	{
		s_ioStream.reset(new ikIOStream());
	}

	return s_ioStream.get();
}

ikIOStream::~ikIOStream()
{
	Exectime etm(__FUNCTION__);
	//    if(nullptr != s_ioStream)
	//    {
	//        delete s_ioStream;
	//    }
	if(nullptr != s_importAndExport)
	{
		delete s_importAndExport;
	}
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
int ikIOStream::ExportToMoreFile(const std::string fileName ,const std::vector<TableType> tableList)
{
	Exectime etm(__FUNCTION__);
	return s_importAndExport->ExportToMoreFile(fileName ,tableList);
}
/**********************************************************************************************************************************
 *                       导入函数
 *  函 数 名： ExportToFile
 *  功    能：将数据导出
 *  说    明：
 *  参    数：
 *           fileName:导入文件名
 *           vecT    :要导入数据表名
 *  返 回 值：0为成功，-1为tableList空，否则 返回值为tableList索引的表导入出错
 *  创 建 人：刘中昌
 *  创建时间：2014-01-10
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int ikIOStream::ImportFromMoreFile(const std::string fileName,const std::vector<TableType> tableList)
{
	Exectime etm(__FUNCTION__);
	return s_importAndExport->ImportFromMoreFile(fileName,tableList);
}
/**********************************************************************************************************************************
 *                       导出函数
 *  函 数 名： ExportToFile
 *  功    能：将数据导出
 *  说    明：
 *  参    数：
 *           fileName:导出文件名
 *           vecT    :要导出数据表名
 *  返 回 值：-1为表名错误，0成功，其它导出失败。
 *  创 建 人：刘中昌
 *  创建时间：2014-01-10
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int ikIOStream::ExportToFile(const std::string fileName ,const TableType tableType)
{
	Exectime etm(__FUNCTION__);
	return s_importAndExport->ExportToFile(fileName ,tableType);
}

/**********************************************************************************************************************************
 *                       导入函数
 *  函 数 名： ImportFromFile
 *  功    能： 将文件数据导入
 *  说    明：
 *  参    数：
 *           fileName:导出文件名
 *           vecT    :要导入数据表名
 *  返 回 值：-1为表名错误，0成功，其它导入失败。
 *  创 建 人：刘中昌
 *  创建时间：2014-01-10
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int ikIOStream::ImportFromFile(const std::string fileName,const TableType tableType)
{
	Exectime etm(__FUNCTION__);
	return s_importAndExport->ImportFromFile(fileName, tableType);
}
/**********************************************************************************************************************************
 *                       释放函数
 *  函 数 名： Release
 *  功    能： 释放指针
 *  说    明：
 *  参    数：
 *  创 建 人：刘中昌
 *  创建时间：2014-03-10
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
void ikIOStream::Release()
{
	Exectime etm(__FUNCTION__);

}

