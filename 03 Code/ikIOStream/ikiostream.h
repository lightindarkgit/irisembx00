#ifndef IKIOSTREAM_H
#define IKIOSTREAM_H
/******************************************************************************************
** 文件名:   IKIOSTREAM_H
×× 主要类:   IkIOStream
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   刘中昌
** 日  期:   2013-11-22
** 修改人:
** 日  期:
** 描  述:  数据导入导出函数
 *×× 主要模块说明:
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
#include<string>
#include <vector>
#include <QVector>
#include <fstream>
#include <iostream>
#include "interface.h"
#include "database.h"

//using namespace std;
class IkIOStream
{
public:
    IkIOStream(){}
    template<typename T>
    static int ExportToFile(const char *fileName, const std::vector< T>&);
    template<typename T>
    static int ImportFromFile(const char *fileName,  std::vector< T >&);

////    template<typename T>
//    int ExportToFileBySQLite(const char *fileName, const std::vector< Person>& vecT);
////    template<typename T>
//    int ImportFromFileBySQLite(const char *fileName,  std::vector< Person >& vecT);
private:
    DataBase _dataBase;
};

/**********************************************************************************************************************************
*                       导出函数
*  函 数 名： ExportToFile
*  功    能：将数据导出
*  说    明：
*  参    数：
*           fileName:导出文件名
*           vecT    :要导出数据
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-01-10
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
template<typename T>
int IkIOStream::ExportToFile(const char *fileName,const std::vector< T >&vecT)
{
    std::ofstream out;
    out.open(fileName);
    if(!out.is_open())
    {
        return -1;
    }

    if(!fileName)
    {
        return -1;
    }

    for (int index =0; index <vecT.size();index++)
    {
        out<<vecT[index];

    }
    out.close();
    return 0;
}

/**********************************************************************************************************************************
*                       导入函数
*  函 数 名： ImportFromFile
*  功    能： 将文件数据导入
*  说    明：
*  参    数：
*           fileName:导出文件名
*           vecT    :要导出数据
*  返 回 值：
*  创 建 人：刘中昌
*  创建时间：2014-01-10
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
template<typename T>
int IkIOStream::ImportFromFile(const char *fileName, std::vector< T > &vecT)
{
    if(!fileName)
    {
        return -1;
    }

    std::ifstream in;
    in.open(fileName);
    if(!in.is_open())
    {
        return -1;//统一定义  文件未打开
    }

    while(!in.eof())
    {
        T temp;
        in>>temp;
        vecT.push_back(temp);
    }

    in.close();

    return 0;
}
#endif // IKIOSTREAM_H
