#ifndef INTERFACE_H
#define INTERFACE_H
/******************************************************************************************
** 文件名:   INTERFACE_H
×× 主要类:   需要导入导出的数据库表结构
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   刘中昌
** 日  期:   2014-01-25
** 修改人:
** 日  期:
** 描  述: 定义表结构以及重载输入运算符
 *×× 主要模块说明:
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
#include <string>
#include<fstream>
#include <iostream>

using namespace std;

const int  TIMESTAMPLENTH = 32;
struct Person
{
    char    personID[16];
    char name[32];
    char sex[2];
    char cardID[16];
    char IDcard[18];
    char memo[128];
    int departID;
    char departName[64];
    char disableTime[TIMESTAMPLENTH];
};
ofstream & operator<<(ofstream &os,const Person &person);
ifstream & operator>>(ifstream &is, Person &person);

struct Depart
{
    int  departID;
    char departName[64];
    int  parentID;
    char memo[128];
};
ofstream & operator<<(ofstream &os,const Depart &depart);
ifstream & operator>>(ifstream &is, Depart &depart);

struct PersonImage
{
    ~PersonImage()
    {
        if (personImageLength>0)
        {
            //free(personImage);
            personImageLength =0;
        }
    }
    char imageID[16];
    char personID[16];
    int  personImageLength;
    char memo[128];
    char * personImage;
};
ofstream & operator<<(ofstream &os,const PersonImage &personImage);
ifstream & operator>>(ifstream &is, PersonImage &personImage);

struct IrisDataBase
{
    ~IrisDataBase()
    {
        if (eyePicLength>0)
        {
            //free(eyePic);
            eyePicLength =0;

        }
        if (matchIrisCodeLength>0)
        {
            //free(matchIrisCode);
            matchIrisCodeLength =0;
        }
    }

    int    irisDataID                     ;
    char    personID[16]                  ;
    char irisCode[256]                    ;
    char eyePicLength               ;
    char* eyePic                ;
    char matchIrisCodeLength         ;
    char* matchIrisCode         ;
    char devSn[32]                        ;
    int  eyeFlag                          ;
    char regTime[TIMESTAMPLENTH]          ;
    char regPalce[TIMESTAMPLENTH]         ;
    char createTime[TIMESTAMPLENTH]       ;
    char updateTime[TIMESTAMPLENTH]       ;
    char memo[128]                        ;
    int pupilRow            ;
    int pupilCol            ;
    int pupilRadius         ;
    int pupilVisiblePercent ;
    int irisRow             ;
    int irisCol             ;
    int irisRadius          ;
    int focusScore          ;
    int irisVisiblePercent  ;

    int spoofValue    ;
    int interlaceValue;
    int qualityLevel  ;
    int qualityScore  ;
    short superSign   ;

};
ofstream & operator<<(ofstream &os,const IrisDataBase &irisDataBase);
ifstream & operator>>(ifstream &is, IrisDataBase &irisDataBase);


//识别信息
struct PersonRecLog
{
    int   recLogID;
    char  personID[16] ;
    char  irisDataID[16];
    char  recogTime[TIMESTAMPLENTH];
    short recogType;
    char  atPostion[64];
    int   devSeq;
    char  devSN[32];
    short devType;
    char  delTime[TIMESTAMPLENTH];
    char  memo[128];
};
ofstream & operator<<(ostream &os,const PersonRecLog &recLog);
ifstream & operator>>(istream &is, PersonRecLog &recLog);

#endif // INTERFACE_H
