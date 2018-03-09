/******************************************************************************************
** 文件名:   dbTable.h
×× 主要类:   dbTable\ViewPersonIris
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2014-02-27
** 修改人:
** 日  期:
** 描  述: 数据库表映射类
** ×× 主要模块说明：
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/

#pragma once
#include "featureCommon.h"
#include <soci/soci.h>
#include <soci/postgresql/soci-postgresql.h>
#include <iomanip>
#include "../../databaseLib/IKDatabaseLib/include/IKDatabaseWrapper.h"
#include "../../databaseLib/IKDatabaseLib/include/IKDatabaseLib.h"

//人员虹膜信息的视图数据结构体－－可根据实际情况进行增删
typedef struct __ViewPersonIris
{
    IkUuid PersonID;
    std::string PersonName;
    std::string Sex;
    std::string CardID;         //卡号ID，定位卡
    std::string  IDCard;        //身份证号

    int  DepartID;
    std::string DepartName;
    std::string DisableTime;
    IkUuid IrisID;
    IkByteaIrisCode IrisData;

    std::string WorkSn;
    std::string DevSn;
    int EyeFlag;
    std::string RegTime;
    std::string RegPalce;

}ViewPersonIris,*PViewPersonIris;

//处理SOCI中关于数据库的数据写入和读出的相关函数
namespace soci
{
    //static std::tm defaultTime;                     //给定一个默认的时间值，目的是在下面的get set中能够通过编译
    template <> struct type_conversion<ViewPersonIris>
    {
        typedef values base_type;
        static void from_base(values const&v,indicator ind,ViewPersonIris &vp)
        {
            if(ind == i_null)
            {
                std::cout<<"Null value not allowed for ViewPersonIris data type  "<<std::endl;
                return ;
            }

            vp.PersonID = v.get<std::string>("person_personid","");
            vp.PersonName = v.get<std::string>("name","");
            vp.Sex = v.get<std::string>("sex","");
            vp.CardID = v.get<std::string>("cardid","");
            vp.IDCard = v.get<std::string>("idcard","");

            vp.DepartID = v.get<int>("departid",0);
            vp.DepartName = v.get<std::string>("departname","");
            vp.DisableTime = v.get<std::string>("disabletime","");
            vp.IrisID = v.get<std::string>("irisdataid","");
            vp.IrisData = v.get<std::string>("iriscode","");

            vp.WorkSn = v.get<std::string>("worksn","");
            vp.DevSn = v.get<std::string>("devsn","");
            vp.EyeFlag = v.get<int>("eyeflag",0);
            vp.RegTime = v.get<std::string>("regtime","");
            vp.RegPalce = v.get<std::string>("regpalce","");

        }

        static void to_base(const ViewPersonIris&vp,values &v,indicator &ind)
        {
            //视图只有读没有写
            ind = i_ok;
        }
    };
}
//数据库的直接操作类
class DbTable {
public:
	DbTable(){}
	virtual ~DbTable(){}
public:
	/*****************************************************************************************
	*                         获得当前指定位置的数据
	*  函 数 名：GetPersonIrisData
	*  功    能：将字符指针转成UUID
	*  说    明：
	*  参    数：offset:取得数据值得偏移量
	*  			num:取得实际获取的数据量
	*  返 回 值：实际取得的数据大小
	*  创 建 人：fjf
	*  创建时间：2014-03-03
	*  修 改 人：
	*  修改时间：
	******************************************************************************************/
	int GetPersonIrisData(std::vector<ViewPersonIris> &vecData,int offset = 0,int num = 1000)
	{
		//偏移量转化成字符串
		std::string queryString = "";
        std::string offsetStr;
        std::stringstream strStream;
        strStream << offset;
        offsetStr = strStream.str();

        //取得带偏移量的SQL语句
		queryString = "SELECT person_personid,name,sex,cardid,idcard,departid,departname,"
					                "disabletime,irisdataid,iriscode,worksn,devsn,eyeflag,regtime,regpalce"
					                " FROM v_irisdata order by person_personid OFFSET " +offsetStr + " LIMIT 40000;";
		dbAccess.Query(queryString,vecData);

		return vecData.size();
	}
	int GetSuperIrisData(std::vector<ViewPersonIris> &vecData)
	{
		std::string queryString = "SELECT person_personid,name,sex,cardid,idcard,departid,departname,"
					                "disabletime,irisdataid,iriscode,worksn,devsn,eyeflag,regtime,regpalce"
					                " FROM v_irisdata where superid >0;";//is NOT NULL

		dbAccess.Query(queryString,vecData);
		return vecData.size();
	}
public:
	/*****************************************************************************************
	*                         获得当前视图数据量
	*  函 数 名：GetPersonIrisDataNum
	*  功    能：根据虹膜数据量来判定视图中的数据量
	*  说    明：
	*  参    数：无
	*  返 回 值：取得虹膜数据表中数据的数量
	*  创 建 人：fjf
	*  创建时间：2014-03-03
	*  修 改 人：
	*  修改时间：
	******************************************************************************************/
	int GetPersonIrisDataNum(void)
	{
		int count = 0;
		std::string countString = "SELECT count(*) FROM irisdatabase";
		dbAccess.Query(countString,count);

		return count;
	}
private:
	DatabaseAccess dbAccess;
};


