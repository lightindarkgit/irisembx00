#ifndef IIMPORTANDEXPORT_H
#define IIMPORTANDEXPORT_H

#include<iostream>
#include <mutex>
#include <thread>
#include <condition_variable>

enum TableType
{
    PersonRecLog,
    IrisDataBase,
    PersonImage,
    Person
};
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
typedef  void  (* IkImportOrExportCallBackFunc)
( int                resultType        //返回结果
 );


class IImportAndExport
{
public:
    ~IImportAndExport();
    int ExportToFile(const char *fileName ,TableType tableType,IkImportOrExportCallBackFunc callback);
    int ImportFromFile(const char *fileName,TableType tableType,IkImportOrExportCallBackFunc callback);

private:
    int ExportPersonRecLogToFile(const char *fileName);
    int ImportPersonRecLogFromFile(const char *fileName);

    int ExportIrisDataBaseToFile(const char *fileName);
    int ImportIrisDataBaseFromFile(const char *fileName);

    int ExportPersonImageToFile(const char *fileName);
    int ImportPersonImageFromFile(const char *fileName);

    int ExportPersonToFile(const char *fileName);
    int ImportPersonFromFile(const char *fileName);

private :
    IImportAndExport();
    int OpenSQLiteDB();
    int OpenPostgresDB();
    static void * DealDatagThread(void *arg);

private :
    bool  _isOpen;
    IkImportOrExportCallBackFunc _callBack; //回调
    std::thread _dealDataThread;            //线程
    std::mutex _mutex;                      //mutex锁
    std::condition_variable _identCapImgCV; //condition_variabl
};

#endif // IIMPORTANDEXPORT_H
