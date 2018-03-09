#ifndef PUBFUN_H
#define PUBFUN_H
/******************************************************************************************
** 文件名:   PUBFUN_H
×× 主要函数:   num2str、str2int、str2float
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   刘中昌
** 日  期:   2014-01-20
** 修改人:
** 日  期:
** 描  述:  数据导入导出函数
 *×× 主要模块说明:
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

//公共函数,照这里写的：http://blog.csdn.net/touzani/article/details/1623850
//将流数据转换为字符串
template <typename T>
string Num2Str(T n)
{
	stringstream ss;
    ss<<n;
	return ss.str();
}

//将字符串转换为数字(整数)
int Str2Int(string s)
{
	int num;
    stringstream ss(s);
    ss>>num;
	return num;
}

//将字符串转换为数字(小数)
float Str2Float(string s)
{
	float num;
    stringstream ss(s);
    ss>>num;
	return num;
}
//将秒数转换成   时:分：秒  形式。
string Seconds2String(int seconds)
{
    int hour, min, sec;
    string timeStr;
    hour = seconds/3600;
    min  = (seconds%3600)/60;
    sec  = (seconds%3600)%60;
    char ctime[10];
    sprintf(ctime, "%d:%d:%d", hour, min, sec);       // 将整数转换成字符串
    timeStr=ctime;                                    // 结果
    return timeStr;
}

#endif
