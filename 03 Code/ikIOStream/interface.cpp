#include "interface.h"
#include <sstream>
/******************************************************************************************
** 文件名:   INTERFACE_CPP
×× 主要类:   需要导入导出的数据库表结构
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   刘中昌
** 日  期:   2014-01-25
** 修改人:
** 日  期:
** 描  述: 重载输入运算符实现
 *×× 主要模块说明:
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
ofstream & operator<<(ofstream &os,const Person &person)
{

    //os.write(person,sizeof(person));
//    std::stringstream ss;
//    ss<<person.personID<< person.name<< person.sex<< person.cardID<< person.IDcard<< person.memo<< person.departID<< person.departName<<person.disableTime;
    os.write(person.personID,sizeof(person.personID));
    os.write(person.name,sizeof(person.name));
    os.write(person.sex,sizeof(person.sex));
    os.write(person.cardID,sizeof(person.cardID));
    os.write(person.IDcard,sizeof(person.IDcard));
    os.write(person.memo,sizeof(person.memo));
    os<<person.departID;
    os.write(person.departName,sizeof(person.departName));
    os.write(person.disableTime,sizeof(person.disableTime));
//    os<<ss;
    return os;
}

ifstream & operator>>(ifstream &is, Person &person)
{
//    std::stringstream ss;
//    ss>>person.personID>> person.name>>  person.sex>>  person.cardID>>  person.IDcard>>  person.memo>>  person.departID>>  person.departName>> person.disableTime;
//    is>>ss;
    //is>>person.personID>> person.name>>  person.sex>>  person.cardID>>  person.IDcard>>  person.memo>>  person.departID>>  person.departName>> person.disableTime;
    //is.read(person,sizeof(person);
    is.read(person.personID,sizeof(person.personID));
    is.read(person.name,sizeof(person.name));
    is.read(person.sex,sizeof(person.sex));
    is.read(person.cardID,sizeof(person.cardID));
    is.read(person.IDcard,sizeof(person.IDcard));
    is.read(person.memo,sizeof(person.memo));
    is>>person.departID;
    is.read(person.departName,sizeof(person.departName));
    is.read(person.disableTime,sizeof(person.disableTime));

    return is;
}

ofstream & operator<<(ofstream &os,const Depart &depart)
{
//    os<<depart.departID<<depart.departName<<depart.parentID<<depart.memo;
    os<<depart.departID;
    os.write(depart.departName,sizeof(depart.departName));
    os<<depart.parentID;
    os.write(depart.memo,sizeof(depart.memo));
    return os;
}

ifstream & operator>>(ifstream &is, Depart &depart)
{
    //is>>depart.departID>>depart.departName>>depart.parentID>>depart.memo;
    is>>depart.departID;
    is.read(depart.departName,sizeof(depart.departName));
    is>>depart.parentID;
    is.read(depart.memo,sizeof(depart.memo));
    return is;
}
ofstream & operator<<(ofstream &os,const PersonImage &personImage)
{
    //os<<personImage.imageID<<personImage.personID<<personImage.personImageLength<<personImage.memo;
    os.write(personImage.imageID,sizeof(personImage.imageID));
    os.write(personImage.personID,sizeof(personImage.personID));
    os<<personImage.personImageLength;
    os.write(personImage.memo,sizeof(personImage.memo));
    if(personImage.personImageLength>=0)
    {
        os.write(personImage.personImage,personImage.personImageLength);
    }
    return os;
}

ifstream & operator>>(ifstream &is, PersonImage &personImage)
{
//    is>>personImage.imageID>>personImage.personID>>personImage.personImageLength>>personImage.memo;
//    if(personImage.personImageLength>=0)
//    {
//        is.read(personImage.personImage,personImage.personImageLength);
//    }
    is.read(personImage.imageID,sizeof(personImage.imageID));
    is.read(personImage.personID,sizeof(personImage.personID));
    is>>personImage.personImageLength;
    is.read(personImage.memo,sizeof(personImage.memo));
    if(personImage.personImageLength>=0)
    {
        personImage.personImage = (char *)malloc(personImage.personImageLength * sizeof(char));
        is.read(personImage.personImage,personImage.personImageLength);
    }
    return is;
}

ofstream & operator<<(ofstream &os,const IrisDataBase &irisDataBase)
{

    //os<<irisDataBase.irisDataID<<irisDataBase.personID<<irisDataBase.irisCode
    //<<irisDataBase.eyePic<<irisDataBase.matchIrisCode
    //     <<irisDataBase.devSn<<irisDataBase.eyeFlag<<irisDataBase.regTime
    //     <<irisDataBase.regPalce<<irisDataBase.createTime<<irisDataBase.updateTime
    //     <<irisDataBase.memo<<irisDataBase.pupilRow<<irisDataBase.pupilCol
    //     <<irisDataBase.pupilRadius<<irisDataBase.pupilVisiblePercent<<irisDataBase.irisRow
    //     <<irisDataBase.irisCol<<irisDataBase.irisRadius<<irisDataBase.focusScore
    //     <<irisDataBase.irisVisiblePercent<<irisDataBase.spoofValue<<irisDataBase.interlaceValue
    //     <<irisDataBase.qualityLevel<<irisDataBase.qualityScore<<irisDataBase.superSign;

     os<<irisDataBase.irisDataID;
     os.write(irisDataBase.personID,sizeof(irisDataBase.personID));
     os.write(irisDataBase.irisCode,sizeof(irisDataBase.irisCode));

     os<<irisDataBase.eyePicLength;
     os.write(irisDataBase.eyePic,sizeof(irisDataBase.eyePicLength));
     os<<irisDataBase.matchIrisCodeLength;
     os.write(irisDataBase.matchIrisCode,sizeof(irisDataBase.matchIrisCodeLength));

     os.write(irisDataBase.devSn,sizeof(irisDataBase.devSn));
     os<<irisDataBase.eyeFlag;
     os.write(irisDataBase.regTime,sizeof(irisDataBase.regTime));


     os.write(irisDataBase.regPalce,sizeof(irisDataBase.regPalce));
     os.write(irisDataBase.createTime,sizeof(irisDataBase.createTime));
     os.write(irisDataBase.updateTime,sizeof(irisDataBase.updateTime));


     os.write(irisDataBase.memo,sizeof(irisDataBase.memo));
     os<<irisDataBase.pupilRow;
     os<<irisDataBase.pupilCol;


     os<<irisDataBase.pupilRadius;
     os<<irisDataBase.pupilVisiblePercent;
     os<<irisDataBase.irisRow;

     os<<irisDataBase.irisCol;
     os<<irisDataBase.irisRadius;
     os<<irisDataBase.focusScore;

     os<<irisDataBase.irisVisiblePercent;
     os<<irisDataBase.spoofValue;
     os<<irisDataBase.interlaceValue;

     os<<irisDataBase.qualityLevel;
     os<<irisDataBase.qualityScore;
     os<<irisDataBase.superSign;

    return os;
}

ifstream & operator>>(ifstream &is, IrisDataBase &irisDataBase)
{
    is>>irisDataBase.irisDataID;
    is.read(irisDataBase.personID,sizeof(irisDataBase.personID));
    is.read(irisDataBase.irisCode,sizeof(irisDataBase.irisCode));

    is>>irisDataBase.eyePicLength;
    if(irisDataBase.eyePicLength >0)
    {
        irisDataBase.eyePic = (char *)malloc(irisDataBase.eyePicLength * sizeof(char));
        is.read(irisDataBase.eyePic,sizeof(irisDataBase.eyePicLength));
    }

    is>>irisDataBase.matchIrisCodeLength;
    if(irisDataBase.matchIrisCodeLength>0)
    {
        irisDataBase.matchIrisCode = (char *)malloc(irisDataBase.matchIrisCodeLength * sizeof(char));
        is.read(irisDataBase.matchIrisCode,sizeof(irisDataBase.matchIrisCodeLength));
    }

    is.read(irisDataBase.devSn,sizeof(irisDataBase.devSn));
    is>>irisDataBase.eyeFlag;
    is.read(irisDataBase.regTime,sizeof(irisDataBase.regTime));


    is.read(irisDataBase.regPalce,sizeof(irisDataBase.regPalce));
    is.read(irisDataBase.createTime,sizeof(irisDataBase.createTime));
    is.read(irisDataBase.updateTime,sizeof(irisDataBase.updateTime));


    is.read(irisDataBase.memo,sizeof(irisDataBase.memo));
    is>>irisDataBase.pupilRow;
    is>>irisDataBase.pupilCol;


    is>>irisDataBase.pupilRadius;
    is>>irisDataBase.pupilVisiblePercent;
    is>>irisDataBase.irisRow;

    is>>irisDataBase.irisCol;
    is>>irisDataBase.irisRadius;
    is>>irisDataBase.focusScore;

    is>>irisDataBase.irisVisiblePercent;
    is>>irisDataBase.spoofValue;
    is>>irisDataBase.interlaceValue;

    is>>irisDataBase.qualityLevel;
    is>>irisDataBase.qualityScore;
    is>>irisDataBase.superSign;
    return is;
}


ofstream & operator<<(ofstream &os,const PersonRecLog &recLog)
{
//    os << recLog.recLogID<<recLog.personID<<recLog.irisDataID<<recLog.recogTime<<recLog.recogType
    //   <<recLog.atPostion<<recLog.devSeq<<recLog.devSN<<recLog.devType<<recLog.delTime<<recLog.memo;
    os<<recLog.recLogID;
    os.write(recLog.personID,sizeof(recLog.personID));
    os.write(recLog.irisDataID,sizeof(recLog.irisDataID));
    os.write(recLog.recogTime,sizeof(recLog.recogTime));
    os<<recLog.recogType;

    os.write(recLog.atPostion,sizeof(recLog.atPostion));
    os<<recLog.devSeq;
    os.write(recLog.devSN,sizeof(recLog.devSN));
    os<<recLog.devType;
    os.write(recLog.delTime,sizeof(recLog.delTime));
    os.write(recLog.memo,sizeof(recLog.memo));
    return os;
}

ifstream & operator>>(ifstream &is, PersonRecLog &recLog)
{
//    is >> recLog.recLogID>>recLog.personID>>recLog.irisDataID>>recLog.recogTime>>recLog.recogType
//       >> recLog.atPostion>>recLog.devSeq>>recLog.devSN>>recLog.devType>>recLog.delTime>>recLog.memo;
    is>>recLog.recLogID;
    is.read(recLog.personID,sizeof(recLog.personID));
    is.read(recLog.irisDataID,sizeof(recLog.irisDataID));
    is.read(recLog.recogTime,sizeof(recLog.recogTime));
    is>>recLog.recogType;

    is.read(recLog.atPostion,sizeof(recLog.atPostion));
    is>>recLog.devSeq;
    is.read(recLog.devSN,sizeof(recLog.devSN));
    is>>recLog.devType;
    is.read(recLog.delTime,sizeof(recLog.delTime));
    is.read(recLog.memo,sizeof(recLog.memo));

    return is;
}








//ifstream& operator >>(ifstream  &ins, Person &p) // Overload ifstream >> (2)
//{

//    //os<<p.personID<<p.name<<p.sex<<p.cardID<<p.IDcard<<p.memo<<p.departID<<p.departName<<p.disableTime;
//    ins>>p.personID>>p.name>>p.sex>>p.cardID>>p.IDcard>>p.memo>>p.departID>>p.departName>>p.disableTime;
//    //ins>>p.buf;
//    //ins.read(p.buf,1024);
//    return ins;
//}

//ofstream& operator <<(ofstream  &os,const Person &p) // Overload ifstream >> (2)
//{
//    os<<p.personID<<" "<<p.name<<" "<<p.sex<<" "<<p.cardID<<" "<<p.IDcard<<" "<<p.memo<<" "<<p.departID<<" "<<p.departName<<" "<<p.disableTime<<" ";
//    //os<<p.buf;
//    //os.write(p.buf,1025);

//    //ins>>p.personID>>p.name>>p.sex>>p.cardID>>p.IDcard>>p.memo>>p.departID>>p.departName>>p.disableTime;
//    return os;
//}


//ofstream& operator<< ( ofstream &os, const IrisDataBase &irisData )
//{
//    os<<irisData.irisDataID<<irisData.personID<<irisData.irisCode<<irisData.eyePic<<irisData.matchIrisCode
//        <<irisData.devSn<<irisData.eyeFlag<<irisData.regTime<<irisData.regPalce<<irisData.createTime
//          <<irisData.updateTime<<irisData.memo<<irisData.pupilRow<<irisData.pupilCol<<irisData.pupilRadius
//            <<irisData.pupilVisiblePercent<<irisData.irisRow<<irisData.irisCol<<irisData.irisRadius
//              <<irisData.irisVisiblePercent<<irisData.focusScore;

//    return os;
//}
//ifstream& operator>> ( ifstream &is,const IrisDataBase &irisData )
//{/*
//    is>>irisData.irisDataID>>irisData.personID>>irisData.irisCode>>irisData.eyePic>>irisData.matchIrisCode
//        >>irisData.devSn>>irisData.eyeFlag>>irisData.regTime>>irisData.regPalce>>irisData.createTime
//          >>irisData.updateTime>>irisData.memo>>irisData.pupilRow>>irisData.pupilCol>>irisData.pupilRadius
//            >>irisData.pupilVisiblePercent>>irisData.irisRow>>irisData.irisCol>>irisData.irisRadius
//              >>irisData.irisVisiblePercent>>irisData.focusScore;*/
//    return is;
//}
//ofstream& operator<< ( ofstream &os,const PersonRecLog &recLog )
//{
//    os << recLog.recLogID<<recLog.personID<<recLog.irisDataID<<recLog.recogTime<<recLog.recogType
//          <<recLog.atPostion<<recLog.devSeq<<recLog.devSN<<recLog.devType<<recLog.delTime<<recLog.memo;

//    return os;
//}

//ifstream& operator>> ( ifstream &is,const PersonRecLog &recLog)
//{
////    is >> recLog.recLogID>>recLog.personID>>recLog.irisDataID>>recLog.recogTime>>recLog.recogType
////          >>recLog.atPostion>>recLog.devSeq>>recLog.devSN>>recLog.devType>>recLog.delTime>>recLog.memo;
////    return is;
//}
