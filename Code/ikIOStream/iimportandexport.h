#ifndef IIMPORTANDEXPORT_H
#define IIMPORTANDEXPORT_H

#include "interface.h"
//#include "simplelock.h"
#include "thread"
#include <mutex>


/**********************************************************************************************************************************
*                        回调函数
*  函 数 名：IkImageLoadeCallbackFunc
*  功    能：图像取后回调函数
*  说    明：
*  参    数：
*             type :     回调类型 0:一幅图像，1: Error
*             buf :    要获取的图像数据
*			 length:   图像大小
*  返 回 值：错误描述, 异常返回
*********************************************************************************************************************************/
//typedef  void  (* IkImportOrExportCallBackFunc)
//( int                type
// );

class IImportAndExport
{
public:
    static IImportAndExport* GetIImportAndExport();
    ~IImportAndExport();

    int ExportToMoreFile(const std::string fileName ,const std::vector<TableType> tableList);

    int ImportFromMoreFile(const std::string fileName,const std::vector<TableType> tableList);

    int ExportToFile(const std::string fileName ,const TableType tableType);

    int ImportFromFile(const std::string fileName,const TableType tableType);

//    int ExportToFile(const std::string fileName ,TableType tableType,IkImportOrExportCallBackFunc callback);
//    int ImportFromFile(const std::string fileName,TableType tableType,IkImportOrExportCallBackFunc callback);

    int TestSQL();
    IImportAndExport();
private:
    int ExportPersonRecLogToFile(const std::string fileName);
    int ImportPersonRecLogFromFile(const std::string fileName);

    int ExportIrisDataBaseToFile(const std::string fileName);
    int ImportIrisDataBaseFromFile(const std::string fileName);

    int ExportPersonImageToFile(const std::string fileName);
    int ImportPersonImageFromFile(const std::string fileName);

    int ExportPersonToFile(const std::string fileName);
    int ImportPersonFromFile(const std::string fileName);

    //去除无效记录（特征、人员图像等）Added By: Wang.L@20151009
    template<typename T>
    int RemoveInvalidRecord(std::vector<T> &rec);

private :

    const std::string DropSqliteDataTable(const TableType tableType);
    const  std::string CreatSqliteDataTable(const TableType tableType);

    const std::string InsertSqliteDataTable(const TableType tableType);
    const std::string SelectSqliteDataTable(const TableType tableType);

private :

    static IImportAndExport* s_iImportAndExport;
    //--------参数定义
    DealDataType _dealDataType;

};

#endif // IIMPORTANDEXPORT_H
