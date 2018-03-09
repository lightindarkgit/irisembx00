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
#include <iostream>

#include <sstream>
#include <iomanip>      // 控制格式输出

#include <ctime>

#include <soci.h>
#include "IKDatabaseWrapper.h"

using namespace std;
//需要导入导出的表名
enum TableType
{
    TBPersonRecLog,
    TBIrisDataBase,
    TBPersonImage,
    TBPerson,
    TBNULL
};

// 数据输出类型
enum DealDataType
{
    Import,
    Export,
    NOport
};

struct Person
{
    IkUuid personID ;
    std::string name;
    std::string sex ;
    std::string cardID;
    std::string IDcard;
    std::string memo;
    int departID;
    std::string departName;
    std::string disableTime;
    int superID;
    int hasIrisData;
    std::string workSn;
};

struct PersonDetail
{
    IkUuid personID ;
    std::string name;
    std::string sex ;
    std::string cardID;
    std::string IDcard;
    std::string memo;
    int departID;
    std::string departName;
    std::string disableTime;
    int superID;
    string hasIrisData;
    std::string workSn;
};


struct Token
{
    int personToken;
    int irisToken;
    int imageToken;
};


struct TempPerson
{
    IkUuid personID ;
    int operation ;

    TempPerson()
    {
        //TO DO
    }

    TempPerson(const IkUuid& pID, const int opt):
        personID(pID),
        operation(opt)
    {
        //TO DO
    }
};

struct DepartName
{
    std::string value;
};

struct Depart
{
    int  departID;
    std::string departName;
    int  parentID;
    std::string memo;
};


struct PersonImage
{
    IkUuid imageID  ;
    IkUuid personID ;
    std::string memo;
    IkByteaLarge personImage;
};

//网络人员定义接口结构
struct IPersonPhoto
{
    uuid_t          Pid;
    uuid_t			PhotoId;
    int				OpToken;	// 增加操作令牌, added at 20140429
    // 值由X00服务控制，设备只负责接收、保存
    char			Memo[128];  // 网络定义值
    int             PhotoSize;
    char*            Photo;
};

//ofstream & operator<<(ofstream &os,const PersonImage &personImage);
//ifstream & operator>>(ifstream &is, PersonImage &personImage);


struct TempPersonImage
{
    IkUuid imageID  ;
    IkUuid personID ;
    int operation ;

    TempPersonImage()
    {
        //TO DO
    }

    TempPersonImage(const IkUuid& imgID, const IkUuid& pID, const int opt) :
        imageID(imgID),
        personID(pID),
        operation(opt)
    {
        //TO DO
    }
};

struct IrisDataBase
{

    IkUuid   irisDataID     ;
    IkUuid    personID      ;
    IkByteaIrisCode irisCode;
    IkByteaLarge eyePic  ;
    IkByteaIrisCode matchIrisCode;
    std::string devSn     ;
    int  eyeFlag          ;
    std::string regTime   ;
    std::string regPalce  ;
    std::string createTime    ;
    std::string updateTime    ;
    std::string memo      ;
    int pupilRow          ;
    int pupilCol          ;
    int pupilRadius       ;
    int irisRow           ;
    int irisCol           ;
    int irisRadius        ;
    int focusScore        ;
    int percentVisible    ;

    int spoofValue        ;
    int interlaceValue    ;
    int qualityLevel      ;
    int qualityScore      ;

};

struct TempIrisDataBase
{
    IkUuid   irisDataID   ;
    IkUuid    personID    ;
    int operation         ;

    TempIrisDataBase()
    {
        //TO DO
    }

    TempIrisDataBase(const IkUuid& irisFeaID, const IkUuid& pID, const int opt) :
        irisDataID(irisFeaID),
        personID(pID),
        operation(opt)
    {
        //TO DO
    }
};

struct IOIrisDataBase
{

    IkUuid   irisDataID     ;
    IkUuid    personID      ;
    std::string irisCode;
    std::string eyePic  ;
    std::string matchIrisCode;
    std::string devSn     ;
    int  eyeFlag          ;
    std::string regTime   ;
    std::string regPalce  ;
    std::string createTime    ;
    std::string updateTime    ;
    std::string memo      ;
    int pupilRow          ;
    int pupilCol          ;
    int pupilRadius       ;
    int irisRow           ;
    int irisCol           ;
    int irisRadius        ;
    int focusScore        ;
    int percentVisible    ;

    int spoofValue        ;
    int interlaceValue    ;
    int qualityLevel      ;
    int qualityScore      ;

};
//ofstream & operator<<(ofstream &os,const IrisDataBase &irisDataBase);
//ifstream & operator>>(ifstream &is, IrisDataBase &irisDataBase);


//识别信息
struct PersonRecLog
{
    int   recLogID;
    IkUuid  personID ;
    IkUuid  irisDataID;
    std::string  recogTime;
    int recogType;
    std::string  atPostion;
    std::string  devSN;
    int  devType;
    std::string  delTime;
    int uploadstatus;
    std::string  memo;
};

struct Operator
{
    int operatorID;
    std::string name;
    std::string password;
    IkUuid personID ;
    std::string sex ;
    std::string memo;
};

struct TblNames
{
    std::string name;
};

struct irisdataIkUuid
{
    IkUuid personid;
    IkUuid featureid;
};

// 绑定Person结构与数据库数据
namespace soci
{

/*****************************************************************************
*                        绑定Person结构与数据库数据
*  功    能：数据结构绑定，用于数据库都写数据
*  说    明：
*  创 建 人：刘中昌
*  创建时间：2013-10-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
template <> struct type_conversion<Person>
{
    typedef values base_type;
    static void from_base (values const &v, indicator ind, Person &p)
    {
        if(ind == i_null)
        {
            std::cout<<"Null value not allowed for Person data type  "<<std::endl;
            return ;
        }
        p.personID    = v.get<std::string>("personid","");
        p.name        = v.get<std::string>("name","");
        p.sex         =  v.get<std::string>("sex","");
        p.cardID      = v.get<std::string>("cardid","");

        p.IDcard      = v.get<std::string>("idcard","");
        p.memo        =  v.get<std::string>("memo","");
        p.departID    = v.get<int>("departid",0);
        p.departName  =  v.get<std::string>("departname","");
        p.disableTime =  v.get<std::string>("disabletime","");
        p.hasIrisData  =  v.get<int>("hasirisdata",0);
        p.workSn  =  v.get<std::string>("worksn","");
        p.superID =  v.get<int>("superid", 0);

    }// end of from base

    static void to_base(const Person &p, values &v, indicator &ind)
    {
        v.set("personid",p.personID);
        v.set("name",p.name);
        v.set("sex",p.sex,p.sex.empty()?i_null:i_ok);
        v.set("cardid",p.cardID,p.cardID.empty()?i_null:i_ok);

        v.set("idcard",p.IDcard,p.IDcard.empty()?i_null:i_ok);
        v.set("memo",p.memo,p.memo.empty()?i_null:i_ok);
        v.set("departid",p.departID,p.departID<1?i_null:i_ok);
        v.set("departname",p.departName,p.departName.empty()?i_null:i_ok);
        v.set("disabletime",p.disableTime ,p.disableTime.empty()?i_null:i_ok);
        v.set("superid",p.superID);
        v.set("worksn",p.workSn,p.workSn.empty()?i_null:i_ok);
        v.set("hasirisdata",p.hasIrisData);

        ind = i_ok;
    } // end of to_base

}; // end of template type_conversion


/*****************************************************************************
*                        绑定Person结构与数据库数据
*  功    能：数据结构绑定，用于数据库都写数据
*  说    明：
*  创 建 人：刘中昌
*  创建时间：2013-10-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
template <> struct type_conversion<PersonDetail>
{
    typedef values base_type;
    static void from_base (values const &v, indicator ind, PersonDetail &p)
    {
        if(ind == i_null)
        {
            std::cout<<"Null value not allowed for Person data type  "<<std::endl;
            return ;
        }
        p.personID    = v.get<std::string>("personid","");
        p.name        = v.get<std::string>("name","");
        p.sex         =  v.get<std::string>("sex","");
        p.cardID      = v.get<std::string>("cardid","");

        p.IDcard      = v.get<std::string>("idcard","");
        p.memo        =  v.get<std::string>("memo","");
        p.departID    = v.get<int>("departid",0);
        p.departName  =  v.get<std::string>("departname","");
        p.disableTime =  v.get<std::string>("disabletime","");
        p.hasIrisData  =  v.get<string>("hasirisdata","否");
        p.workSn  =  v.get<std::string>("worksn","");
        p.superID =  v.get<int>("superid", 0);

    }// end of from base

    static void to_base(const Person &p, values &v, indicator &ind)
    {
        v.set("personid",p.personID);
        v.set("name",p.name);
        v.set("sex",p.sex,p.sex.empty()?i_null:i_ok);
        v.set("cardid",p.cardID,p.cardID.empty()?i_null:i_ok);

        v.set("idcard",p.IDcard,p.IDcard.empty()?i_null:i_ok);
        v.set("memo",p.memo,p.memo.empty()?i_null:i_ok);
        v.set("departid",p.departID,p.departID<1?i_null:i_ok);
        v.set("departname",p.departName,p.departName.empty()?i_null:i_ok);
        v.set("disabletime",p.disableTime ,p.disableTime.empty()?i_null:i_ok);
        v.set("superid",p.superID);
        v.set("worksn",p.workSn,p.workSn.empty()?i_null:i_ok);
        v.set("hasirisdata",p.hasIrisData);

        ind = i_ok;
    } // end of to_base

}; // end of template type_conversion

///*****************************************************************************
//*                        绑定Person结构与数据库数据
//*  功    能：数据结构绑定，用于数据库都写数据
//*  说    明：
//*  创 建 人：刘中昌
//*  创建时间：2013-10-24
//*  修 改 人：
//*  修改时间：
//*****************************************************************************/
//template <> struct type_conversion<Person_t>
//{
//    typedef values base_type;
//    static void from_base (values const &v, indicator ind, Person_t &p)
//    {
//        if(ind == i_null)
//        {
//            std::cout<<"Null value not allowed for Person data type  "<<std::endl;
//            return ;
//        }
//        p.personID    = v.get<std::string>("personid","");
//        p.name        = v.get<std::string>("name","");
//        p.sex         =  v.get<std::string>("sex","");
//        p.cardID      = v.get<std::string>("cardid","");

//        p.IDcard      = v.get<std::string>("idcard","");
//        p.memo        =  v.get<std::string>("memo","");
//        p.departID    = v.get<int>("departid",0);
//        p.departName  =  v.get<std::string>("departname","");
//        p.disableTime =  v.get<std::string>("disabletime","");
//        p.hasIris  =  v.get<std::string>("hasiris","否");
//        p.workSn  =  v.get<std::string>("worksn","");
//        p.superID =  v.get<int>("superid", 0);

//    }// end of from base

//    static void to_base(const Person_t &p, values &v, indicator &ind)
//    {
//        v.set("personid",p.personID);
//        v.set("name",p.name);
//        v.set("sex",p.sex,p.sex.empty()?i_null:i_ok);
//        v.set("cardid",p.cardID,p.cardID.empty()?i_null:i_ok);

//        v.set("idcard",p.IDcard,p.IDcard.empty()?i_null:i_ok);
//        v.set("memo",p.memo,p.memo.empty()?i_null:i_ok);
//        v.set("departid",p.departID,p.departID<1?i_null:i_ok);
//        v.set("departname",p.departName,p.departName.empty()?i_null:i_ok);
//        v.set("disabletime",p.disableTime ,p.disableTime.empty()?i_null:i_ok);
//        v.set("superid",p.superID);
//        v.set("worksn",p.workSn,p.workSn.empty()?i_null:i_ok);
//        v.set("hasiris",p.hasIris);

//        ind = i_ok;
//    } // end of to_base

//}; // end of template type_conversion


/*****************************************************************************
*                        绑定TempPerson结构与数据库数据
*  功    能：数据结构绑定，用于数据库都写数据
*  说    明：
*  创 建 人：刘中昌
*  创建时间：2014-05-14
*  修 改 人：
*  修改时间：
*****************************************************************************/
template <> struct type_conversion<TempPerson>
{
    typedef values base_type;
    static void from_base (values const &v, indicator ind, TempPerson &p)
    {
        if(ind == i_null)
        {
            std::cout<<"Null value not allowed for Person data type  "<<std::endl;
            return ;
        }
        p.personID    = v.get<std::string>("personid","");
        p.operation   =  v.get<int>("operation", -1);

    }// end of from base

    static void to_base(const TempPerson &p, values &v, indicator &ind)
    {
        v.set("personid",p.personID);
        v.set("operation",p.operation);

        ind = i_ok;
    } // end of to_base

}; // end of template type_conversion

/*****************************************************************************
*                        绑定DepartName结构与数据库数据
*  功    能：数据结构绑定，用于数据库都写数据
*  说    明：
*  创 建 人：刘中昌
*  创建时间：2013-10-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
template <> struct type_conversion<DepartName>
{
    typedef values base_type;
    static void from_base (values const &v, indicator ind, DepartName &departName)
    {
        if(ind == i_null)
        {
            std::cout<<"Null value not allowed for Depart data type  "<<std::endl;
            return ;
        }

        departName.value = v.get<std::string>("departname","");
    }// end of from base
    static void to_base(const DepartName &departName, values &v, indicator &ind)
    {
        v.set("departname",departName.value,departName.value.empty()?i_null:i_ok);
        ind = i_ok;
    } // end of to_base

};

/*****************************************************************************
*                        绑定Token结构与数据库数据
*  功    能：数据结构绑定，用于数据库都写数据
*  说    明：
*  创 建 人：刘中昌
*  创建时间：2014-05-22
*  修 改 人：
*  修改时间：
*****************************************************************************/
template <> struct type_conversion<Token>
{
    typedef values base_type;
    static void from_base(values const &v, indicator ind, Token &token)
    {
        if(ind == i_null)
        {
            std::cout<<"Null value not allowed for Token data type  "<<std::endl;
            return ;
        }
        token.personToken   = v.get<int>("persontoken",0);
        token.irisToken = v.get<int>("iristoken",0);
        token.imageToken   =  v.get<int>("imagetoken",0);

    }// end of from base
    static void to_base(const Token &token, values &v, indicator &ind)
    {
        v.set("persontoken",token.personToken);
        v.set("iristoken",token.irisToken);
        v.set("imagetoken",token.imageToken);

        ind = i_ok;
    } // end of to_base

};

/*****************************************************************************
*                        绑定Depart结构与数据库数据
*  功    能：数据结构绑定，用于数据库都写数据
*  说    明：
*  创 建 人：刘中昌
*  创建时间：2013-10-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
template <> struct type_conversion<Depart>
{
    typedef values base_type;
    static void from_base (values const &v, indicator ind, Depart &depart)
    {
        if(ind == i_null)
        {
            std::cout<<"Null value not allowed for Depart data type  "<<std::endl;
            return ;
        }
        depart.departID   = v.get<int>("departid",0);
        depart.departName = v.get<std::string>("departname","");
        depart.parentID   =  v.get<int>("parentid",0);
        depart.memo       = v.get<std::string>("memo","");

    }// end of from base
    static void to_base(const Depart &depart, values &v, indicator &ind)
    {
        v.set("departid",depart.departID);
        v.set("departname",depart.departName,depart.departName.empty()?i_null:i_ok);
        v.set("parentid",depart.parentID,depart.parentID < 1 ?i_null:i_ok);
        v.set("memo",depart.memo,depart.memo.empty()?i_null:i_ok);
        ind = i_ok;
    } // end of to_base

};

/*****************************************************************************
*                        绑定PersonImage结构与数据库数据
*  功    能：数据结构绑定，用于数据库都写数据
*  说    明：
*  创 建 人：刘中昌
*  创建时间：2013-10-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
template <> struct type_conversion<PersonImage>
{
    typedef values base_type;
    static void from_base (values const &v, indicator ind, PersonImage &p)
    {
        if(ind == i_null)
        {
            std::cout<<"Null value not allowed for PersonImage data type  "<<std::endl;
            return ;
        }
        p.imageID     = v.get<std::string>("imageid","");
        p.personID    = v.get<std::string>("personid","");
        p.memo        =  v.get<std::string>("memo","");
        p.personImage = v.get<std::string>("personimage","");

    }// end of from base

    static void to_base(const PersonImage &p, values &v, indicator &ind)
    {
        v.set("imageid",p.imageID);
        v.set("personid",p.personID);
        v.set("memo",p.memo,p.memo.empty()?i_null:i_ok);
        v.set("personimage",p.personImage,p.personImage.Empty()?i_null:i_ok);
        ind = i_ok;
    } // end of to_base
}; // end of template type_conversion

/*****************************************************************************
*                        绑定TempPersonImage结构与数据库数据
*  功    能：数据结构绑定，用于数据库都写数据
*  说    明：
*  创 建 人：刘中昌
*  创建时间：2014-05-14
*  修 改 人：
*  修改时间：
*****************************************************************************/
template <> struct type_conversion<TempPersonImage>
{
    typedef values base_type;
    static void from_base (values const &v, indicator ind, TempPersonImage &p)
    {
        if(ind == i_null)
        {
            std::cout<<"Null value not allowed for PersonImage data type  "<<std::endl;
            return ;
        }
        p.imageID     = v.get<std::string>("imageid","");
        p.personID    = v.get<std::string>("personid","");
        p.operation   = v.get<int>("operation", -1);

    }// end of from base

    static void to_base(const TempPersonImage &p, values &v, indicator &ind)
    {
        v.set("imageid",p.imageID);
        v.set("personid",p.personID);
        v.set("operation",p.operation);
        ind = i_ok;
    } // end of to_base
}; // end of template type_conversion


/*****************************************************************************
*                        绑定IrisDataBase结构与数据库数据
*  功    能：数据结构绑定，用于数据库都写数据
*  说    明：
*  创 建 人：刘中昌
*  创建时间：2013-10-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
template <> struct type_conversion<IrisDataBase>
{

    typedef values base_type;
    static void from_base (values const &v, indicator ind, IrisDataBase &data)
    {
        if(ind == i_null)
        {
            std::cout<<"Null value not allowed for IrisDataBase data type  "<<std::endl;
            return ;
        }
        data.irisDataID = v.get<std::string>("irisdataid","");
        data.personID   = v.get<std::string>("personid","");
        data.irisCode   = v.get<std::string>("iriscode","");
        data.eyePic     = v.get<std::string>("eyepic","");

        data.matchIrisCode = v.get<std::string>("matchiriscode","");
        data.devSn         = v.get<std::string>("devsn","");
        data.eyeFlag       = v.get<int>("eyeflag",0);
        data.regTime       = v.get<std::string>("regtime","");

        data.regPalce    = v.get<std::string>("regpalce","");
        data.createTime  = v.get<std::string>("createtime","");
        data.updateTime  = v.get<std::string>("updatetime","");
        data.memo        = v.get<std::string>("memo","");

        data.pupilRow    = v.get<int>("pupilrow",0);
        data.pupilCol    = v.get<int>("pupilcol",0);
        data.pupilRadius = v.get<int>("pupilradius",0);

        data.irisRow    = v.get<int>("irisrow",0);
        data.irisCol    = v.get<int>("iriscol",0);
        data.irisRadius = v.get<int>("irisradius",0);
        data.focusScore = v.get<int>("focusscore",0);

        data.percentVisible = v.get<int>("percentvisible",0);
        data.spoofValue         = v.get<int>("spoofvalue",0);
        data.interlaceValue     = v.get<int>("interlacevalue",0);
        data.qualityLevel       = v.get<int>("qualitylevel",0);

        data.qualityScore = v.get<int>("qualityscore",0);

    }// end of from base

    static void to_base(const IrisDataBase &data, values &v, indicator &ind)
    {
        v.set("irisdataid",data.irisDataID, data.irisDataID.Unparse().empty()? i_null:i_ok);
        v.set("personid",  data.personID, data.personID.Unparse().empty()? i_null:i_ok);
        v.set("iriscode",  data.irisCode,data.irisCode.Empty()?i_null:i_ok);
        v.set("eyepic",    data.eyePic,data.eyePic.Empty()?i_null:i_ok);

        v.set("matchiriscode",data.matchIrisCode,data.matchIrisCode.Empty()?i_null:i_ok);
        v.set("devsn",        data.devSn,data.devSn.empty()?i_null:i_ok);
        v.set("eyeflag",      data.eyeFlag);
        v.set("regtime",      data.regTime,data.regTime.empty()?i_null:i_ok);

        v.set("regpalce",  data.regPalce,data.regPalce.empty()?i_null:i_ok);
        v.set("createtime",data.createTime,data.createTime.empty()?i_null:i_ok);
        v.set("updatetime",data.updateTime,data.updateTime.empty()?i_null:i_ok);
        v.set("memo",      data.memo,data.memo.empty()?i_null:i_ok);

        v.set("pupilrow", data.pupilRow);
        v.set("pupilcol", data.pupilCol);
        v.set("pupilradius", data.pupilRadius);

        v.set("irisrow", data.irisRow);
        v.set("iriscol", data.irisCol);
        v.set("irisradius", data.irisRadius);
        v.set("focusscore", data.focusScore);

        v.set("percentvisible",data.percentVisible);
        v.set("spoofvalue", data.spoofValue);
        v.set("interlacevalue", data.interlaceValue);
        v.set("qualitylevel",   data.qualityLevel);

        v.set("qualityscore", data.qualityScore);

        ind = i_ok;
    } // end of to_base
}; // end of template type_conversion

/*****************************************************************************
*                        绑定TempIrisDataBase结构与数据库数据
*  功    能：数据结构绑定，用于数据库都写数据
*  说    明：
*  创 建 人：刘中昌
*  创建时间：2013-10-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
template <> struct type_conversion<TempIrisDataBase>
{

    typedef values base_type;
    static void from_base (values const &v, indicator ind, TempIrisDataBase &data)
    {
        if(ind == i_null)
        {
            std::cout<<"Null value not allowed for IrisDataBase data type  "<<std::endl;
            return ;
        }

        data.irisDataID = v.get<std::string>("irisdataid","");
        data.personID   = v.get<std::string>("personid","");
        data.operation = v.get<int>("operation",0);
    }// end of from base

    static void to_base(const TempIrisDataBase &data, values &v, indicator &ind)
    {
        v.set("irisdataid",data.irisDataID);
        v.set("personid",  data.personID);
        v.set("operation", data.operation);

        ind = i_ok;
    } // end of to_base
}; // end of template type_conversion

/*****************************************************************************
*                        绑定IOIrisDataBase结构与数据库数据
*  功    能：数据结构绑定，用于数据库都写数据
*  说    明：
*  创 建 人：刘中昌
*  创建时间：2014-05-14
*  修 改 人：
*  修改时间：
*****************************************************************************/
template <> struct type_conversion<IOIrisDataBase>
{

    typedef values base_type;
    static void from_base (values const &v, indicator ind, IOIrisDataBase &data)
    {
        if(ind == i_null)
        {
            std::cout<<"Null value not allowed for IrisDataBase data type  "<<std::endl;
            return ;
        }
        data.irisDataID = v.get<std::string>("irisdataid","");
        data.personID   = v.get<std::string>("personid","");
        data.irisCode   = v.get<std::string>("iriscode","");
        data.eyePic     = v.get<std::string>("eyepic","");

        data.matchIrisCode = v.get<std::string>("matchiriscode","");
        data.devSn         = v.get<std::string>("devsn","");
        data.eyeFlag       = v.get<int>("eyeflag",0);
        data.regTime       = v.get<std::string>("regtime","");

        data.regPalce    = v.get<std::string>("regpalce","");
        data.createTime  = v.get<std::string>("createtime","");
        data.updateTime  = v.get<std::string>("updatetime","");
        data.memo        = v.get<std::string>("memo","");

        data.pupilRow    = v.get<int>("pupilrow",0);
        data.pupilCol    = v.get<int>("pupilcol",0);
        data.pupilRadius = v.get<int>("pupilradius",0);

        data.irisRow    = v.get<int>("irisrow",0);
        data.irisCol    = v.get<int>("iriscol",0);
        data.irisRadius = v.get<int>("irisradius",0);
        data.focusScore = v.get<int>("focusscore",0);

        data.percentVisible = v.get<int>("percentvisible",0);
        data.spoofValue         = v.get<int>("spoofvalue",0);
        data.interlaceValue     = v.get<int>("interlacevalue",0);
        data.qualityLevel       = v.get<int>("qualitylevel",0);

        data.qualityScore = v.get<int>("qualityscore",0);

    }// end of from base

    static void to_base(const IOIrisDataBase &data, values &v, indicator &ind)
    {
        v.set("irisdataid",data.irisDataID);
        v.set("personid",  data.personID);
        v.set("iriscode",  data.irisCode,data.irisCode.empty()?i_null:i_ok);
        v.set("eyepic",    data.eyePic,data.eyePic.empty()?i_null:i_ok);

        v.set("matchiriscode",data.matchIrisCode,data.matchIrisCode.empty()?i_null:i_ok);
        v.set("devsn",        data.devSn,data.devSn.empty()?i_null:i_ok);
        v.set("eyeflag",      data.eyeFlag);
        v.set("regtime",      data.regTime,data.regTime.empty()?i_null:i_ok);

        v.set("regpalce",  data.regPalce,data.regPalce.empty()?i_null:i_ok);
        v.set("createtime",data.createTime,data.createTime.empty()?i_null:i_ok);
        v.set("updatetime",data.updateTime,data.updateTime.empty()?i_null:i_ok);
        v.set("memo",      data.memo,data.memo.empty()?i_null:i_ok);

        v.set("pupilrow", data.pupilRow);
        v.set("pupilcol", data.pupilCol);
        v.set("pupilradius", data.pupilRadius);

        v.set("irisrow", data.irisRow);
        v.set("iriscol", data.irisCol);
        v.set("irisradius", data.irisRadius);
        v.set("focusscore", data.focusScore);

        v.set("percentvisible",data.percentVisible);
        v.set("spoofvalue", data.spoofValue);
        v.set("interlacevalue", data.interlaceValue);
        v.set("qualitylevel",   data.qualityLevel);

        v.set("qualityscore", data.qualityScore);

        ind = i_ok;
    } // end of to_base
}; // end of template type_conversion

/*****************************************************************************
*                        绑定PersonRecLog结构与数据库数据
*  功    能：数据结构绑定，用于数据库都写数据
*  说    明：
*  创 建 人：刘中昌
*  创建时间：2013-10-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
template <> struct type_conversion<PersonRecLog>
{
    typedef values base_type;
    static void from_base (values const &v, indicator ind, PersonRecLog &recg)
    {
        if(ind == i_null)
        {
            std::cout<<"Null value not allowed for PersonRecLog data type  "<<std::endl;
            return ;
        }

        recg.recLogID   = v.get<int>("reclogid",0);
        recg.personID   = v.get<std::string>("personid","");
        recg.irisDataID = v.get<std::string>("irisdataid","");
        recg.recogTime  = v.get<std::string>("recogtime","");
        recg.atPostion = v.get<std::string>("atpostion","");
        recg.devSN     = v.get<std::string>("devsn","");


        recg.delTime = v.get<std::string>("deltime","");
        recg.memo    = v.get<std::string>("memo","");
        recg.devType = v.get<int>("devtype",0);
        recg.uploadstatus = v.get<int>("uploadstatus",0);

    }// end of from base

    static void to_base(const PersonRecLog &recg, values &v, indicator &ind)
    {
        v.set("reclogid",recg.recLogID);
        v.set("personid",recg.personID);
        v.set("irisdataid",recg.irisDataID);
        v.set("recogtime",recg.recogTime,recg.recogTime.empty()?i_null:i_ok);

        v.set("recogtype",recg.recogType);
        v.set("atpostion",recg.atPostion,recg.atPostion.empty()?i_null:i_ok);
        v.set("devsn",recg.devSN,recg.devSN.empty()?i_null:i_ok);

        v.set("devtype",recg.devType);
        v.set("deltime",recg.delTime,recg.delTime.empty()?i_null:i_ok);
        v.set("memo",recg.memo,recg.memo.empty()?i_null:i_ok);
        v.set("uploadstatus",recg.uploadstatus);

        ind = i_ok;
    } // end of to_base
}; // end of template type_conversion

/*****************************************************************************
*                        绑定Operator结构与数据库数据
*  功    能：数据结构绑定，用于数据库都写数据
*  说    明：
*  创 建 人：刘中昌
*  创建时间：2013-10-24
*  修 改 人：
*  修改时间：
*****************************************************************************/
template <> struct type_conversion<Operator>
{
    typedef values base_type;
    static void from_base (values const &v, indicator ind, Operator &data)
    {
        if(ind == i_null)
        {
            std::cout<<"Null value not allowed for Operator data type  "<<std::endl;
            return ;
        }

        data.operatorID   = v.get<int>("operatorid",0);
        data.name = v.get<std::string>("name","");
        data.password = v.get<std::string>("password","");
        data.personID = v.get<std::string>("personid","");
        data.sex      =  v.get<std::string>("sex","");
        data.memo     = v.get<std::string>("memo","");

    }// end of from base
    static void to_base(const Operator &data, values &v, indicator &ind)
    {
        v.set("operatorid",data.operatorID);
        v.set("name",data.name,data.name.empty()?i_null:i_ok);
        v.set("password",data.password,data.password.empty()?i_null:i_ok);
        v.set("personid",data.personID);
        v.set("sex",data.sex,data.sex.empty()?i_null:i_ok);
        v.set("memo",data.memo,data.memo.empty()?i_null:i_ok);
        ind = i_ok;
    } // end of to_base

};


/*****************************************************************************
*                        绑定用户数据表名称结构与数据库数据
*  功    能：数据表名称结构绑定，用于数据库读数据
*  说    明：只读绑定
*  创 建 人：Wang.l
*  创建时间：2014-06-03
*  修 改 人：
*  修改时间：
*****************************************************************************/
template <> struct type_conversion<TblNames>
{
    typedef values base_type;
    static void from_base (values const &v, indicator ind, TblNames &p)
    {
        if(ind == i_null)
        {
            std::cout<<"Null value not allowed for Person data type  "<<std::endl;
            return ;
        }
        p.name    = v.get<std::string>("tablename","");

    }// end of from base

//    static void to_base(const TblNames &p, values &v, indicator &ind)
//    {
//        v.set("tablename",p.personID);
//        v.set("operation",p.operation);

//        ind = i_ok;
//    } // end of to_base

}; // end of template type_conversion

template <> struct type_conversion<irisdataIkUuid>
{
    typedef values base_type;
    static void from_base (values const &v, indicator ind, irisdataIkUuid &p)
    {
        if(ind == i_null)
        {
            std::cout<<"Null value not allowed for Person data type  "<<std::endl;
            return ;
        }

        p.featureid = v.get<std::string>("irisdataid","");
        p.personid = v.get<std::string>("personid","");
    }
}; // end of template type_conversion

}
#endif // INTERFACE_H
