/*****************************************************************************
 ** 文 件 名：dboperationinterface.cpp
 ** 主 要 类：PersonInfoInterface
 **
 ** Copyright (c) 中科虹霸有限公司
 **
 ** 创 建 人：L.Wang
 ** 创建时间：20014-04-25
 **
 ** 修 改 人：
 ** 修改时间：
 ** 描  述:   人员信息网络命令操作
 ** 主要模块说明: 人员信息的网络相关操作接口，通过网络对本地人员数据库进行增删改查等操作，以及对人脸图像进行增删改等操作
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 *****************************************************************************/
#include "dboperationinterface.h"
#include "../databaseLib/IKDatabaseLib/include/IKDatabaseLib.h"
#include "../databaseLib/IKDatabaseLib/include/IKTransaction.h"
#include "../Common/Logger.h"
#include "../Common/Exectime.h"

//String stream reference
#include <sstream>
#include<QDateTime>
#include <QDebug>

//Get person id from string type object
const std::string GetPersonID(const std::string& psnID)
{
	
	//TO DO
	return psnID;
}

//Get person id from Person type object
const std::string GetPersonID(const Person& psn)
{
	
	//TO DO
	return psn.personID.Unparse();
}


//Get image id from string type object
const std::string GetImgID(const std::string& imgID)
{
	
	//TO DO
	return imgID;
}

//Get image id from "PersonImage" type object
const std::string GetImgID(const PersonImage& personImg)
{
	
	//TO DO
	return personImg.imageID.Unparse();
}


    template<typename T>
bool DBOperationInterface::GetPersonInfoRecentChanged(T& res, const OptType opt)
{

    //TO DO
    res.clear();


    //Default to handle querying added person information
    /*
    std::string sql("SELECT t1.personid, t1.name, t1.sex, t1.idcard, t1.superid, t1.cardid, t1.worksn, t1.disabletime,"
            "  t1.departid, t1.departname, t2.hasirisdata, t1.memo "
            "FROM person AS t1 LEFT JOIN (SELECT personid, CAST(count(*) AS int) hasirisdata FROM irisdatabase GROUP BY personid) AS t2 "
            "  ON t1.personid = t2.personid "
            "WHERE t1.personid IN (SELECT personid FROM tempperson WHERE tempperson.operation = 0) limit 100;");

    //In case to handle querying edited person information
    if(OptType::editOpt == opt)
    {
        sql = std::string("SELECT t1.personid, t1.name, t1.sex, t1.idcard, t1.superid, t1.cardid, t1.worksn, t1.disabletime,"
                "  t1.departid, t1.departname, t2.hasirisdata, t1.memo "
                "FROM person AS t1 LEFT JOIN (SELECT personid, CAST(count(*) AS int) hasirisdata FROM irisdatabase GROUP BY personid) AS t2 "
                "  ON t1.personid = t2.personid "
                "WHERE t1.personid IN (SELECT personid FROM tempperson WHERE tempperson.operation = 2) limit 100;");
    }
    */

    char szopt[10] = {0};
    sprintf(szopt,"%d",(int)opt);

    std::string sql(" SELECT "
            "t1.personid, "
            "t1.name, "
            "t1.sex, "
            "t1.idcard, "
            "t1.superid, "
            "t1.cardid, "
            "t1.worksn, "
            "t1.disabletime, "
            "t1.departid, "
            "t1.departname, "
            "t1.hasirisdata, "
            "t1.memo "
            "FROM person t1 WHERE t1.personid IN"
            "( "
            " SELECT personid FROM tempperson WHERE "
            " tempperson.OPERATION = ");

    sql.append(szopt).append(
            " GROUP BY personid LIMIT 100)");
    //qDebug()<<QString::fromStdString(sql);

    // LOG_ERROR("%s", sql.c_str());

    //In case to handle querying deleted person information
    if(OptType::deleteOpt == opt)
    {
        sql = std::string("SELECT personid "
                "FROM  tempperson  "
                "WHERE operation = 1 limit 100;");
    }


    //Get conditional result
    DatabaseAccess dbAc;
    if(dbSuccess == dbAc.Query(sql, res))
    {
        //std::cout<<"************"<<res[0].memo<<"   memoLength:"<<res[0].memo.length()<<std::endl;
        return true;
    }
    else
    {
        LOG_ERROR("Quert Sql failed. [%s]",sql.c_str());
        return false;
    }

}

	template<typename T>
bool DBOperationInterface::GetPersonImgRecentChanged(T& res, const OptType opt)
{
	
	//TO DO
	res.clear();

	//Default to handle querying added personimage
	std::string  sql("SELECT * FROM personimage "
			"WHERE personimage.personid IN (SELECT personid FROM temppersonimage WHERE temppersonimage.operation = 0);");

	//In case to handle querying deleted person information
	if(OptType::deleteOpt == opt)
	{
		sql = std::string(" SELECT imageid "
				" FROM temppersonimage  "
				" WHERE operation = 1;");
	}


	//Get conditional result
	DatabaseAccess dbAc;
	if(dbSuccess == dbAc.Query(sql, res))
	{
		return true;
	}
	else
	{
        LOG_ERROR("Quert Sql failed. [%s]",sql.c_str());
		return false;
	}
}


bool DBOperationInterface::GetPersons(std::vector<Person> &persons)
{
	
	//TO DO
	std::string sql("SELECT t1.personid, t1.name, t1.sex, t1.idcard, t1.superid, t1.cardid, t1.worksn, t1.disabletime,"
			"  t1.departid, t1.departname, t2.hasirisdata, t1.memo "
			"FROM person AS t1 LEFT JOIN (SELECT personid, CAST(count(*) AS int) hasirisdata FROM irisdatabase GROUP BY personid) AS t2 "
			"  ON t1.personid = t2.personid ");            //数据源表"

	DatabaseAccess dbAc;
	if(dbSuccess == dbAc.Query(sql, persons))
	{
		return true;
	}
	else
	{
		LOG_ERROR("Quert Sql failed. [%s]",sql.c_str());
		return false;
	}
}


bool DBOperationInterface::GetPersons(std::vector<IkUuid> personids,std::vector<Person> &persons)
{
    
    string strpidset = "";
    if(personids.size() == 0)
    {
        return true;
    }
    else
    {
        strpidset = "(";
        for(int i = 0; i < personids.size(); i ++)
        {
            strpidset.append("\'").append(personids[i].Unparse()).append("\'");
            if(i < personids.size() -1)
            {
                strpidset.append(",");
            }
        }
        strpidset.append(")");
    }

    //TO DO
//    std::string sql("SELECT t1.personid, t1.name, t1.sex, t1.idcard, t1.superid, t1.cardid, t1.worksn, t1.disabletime,"
//            "  t1.departid, t1.departname, t2.hasirisdata, t1.memo "
//            "FROM person AS t1 LEFT JOIN (SELECT personid, CAST(count(*) AS int) hasirisdata FROM irisdatabase GROUP BY personid) AS t2 "
//            "  ON t1.personid = t2.personid where t1.personid in ");            //数据源表"
    std::string sql("SELECT t1.personid, t1.name, t1.sex, t1.idcard, t1.superid, t1.cardid, t1.worksn, t1.disabletime,"
                  "  t1.departid, t1.departname, t1.hasirisdata, t1.memo "
                  " FROM person AS t1  where t1.personid in");            //数据源表"
    sql.append(strpidset);

    DatabaseAccess dbAc;
    if(dbSuccess == dbAc.Query(sql, persons))
    {
        return true;
    }
    else
    {
        LOG_ERROR("Quert Sql failed. [%s]",sql.c_str());
        return false;
    }
}

bool DBOperationInterface::GetPersonRecentAdded(std::vector<Person> &persons)
{
	
	//TO DO
	return GetPersonInfoRecentChanged(persons, OptType::addOpt);
}


bool DBOperationInterface::GetPersonRecentEdited(std::vector<Person>& persons)
{
	
	//TO DO
	return GetPersonInfoRecentChanged(persons, OptType::editOpt);
}


bool DBOperationInterface::GetPersonRecentDeleted(std::vector<std::string>& perIDs)
{
	
	//TO DO
	return GetPersonInfoRecentChanged(perIDs, OptType::deleteOpt);
}

// modified at 20170105 by yqhe
// 修改通过tmptbl查找相关信息的sql语句，提高速度
int DBOperationInterface::AddPersonInfo(const std::vector<Person>& datas)
{
	
	//TO DO
	DatabaseAccess dbAc;

	//创建临时表存储待增加的人员信息
	std::string sql("DROP TABLE  IF EXISTS  tmptbl;"
			"CREATE TABLE tmptbl AS SELECT * FROM person WHERE personid = '';");
	if(dbSuccess != dbAc.Query(sql))
	{
		LOG_ERROR("Quert Sql failed. [%s]",sql.c_str());
		return false;
	}

	//插入待增加的人员信息
	sql = std::string("INSERT INTO tmptbl (personid, name, sex, idcard, superid, worksn, cardid, departid, departname, hasirisdata, disabletime, memo) "
			"VALUES(:personid, :name, :sex, :idcard, :superid, :worksn, :cardid, :departid, :departname, :hasirisdata, :disabletime, :memo); ");
	if(dbSuccess != dbAc.Insert(sql, datas))
	{
		if(dbSuccess != dbAc.Query("DROP TABLE  IF EXISTS  tmptbl;"))
		{
			LOG_ERROR("Quert Sql failed. [DROP TABLE  IF EXISTS  tmptbl;]");
		}
		LOG_ERROR("Quert Sql failed. [%s]",sql.c_str());
		return false;
	}


    ////////////////////////
    // modified at 20170105 by yqhe
	//去除重复的人员信息，并插入到人员表，然后删除临时表
//	sql = std::string("DELETE FROM tmptbl WHERE personid IN (SELECT personid FROM person);"
//			"INSERT INTO person SELECT "
//			"personid,name,worksn,superid,sex,cardid,idcard,memo,departid,"
//			"departname,disabletime,hasirisdata FROM tmptbl;");

    sql = std::string("DELETE FROM tmptbl WHERE personid IN "
                      " (SELECT person.personid FROM person INNER JOIN tmptbl "
                      " ON person.personid=tmptbl.personid);"
                      "INSERT INTO person SELECT "
                      "personid,name,worksn,superid,sex,cardid,idcard,memo,departid,"
                      "departname,disabletime, hasirisdata FROM tmptbl;");
    ////////////////////////

	//liu-改
	int rtnVal = dbAc.Query(sql);
	if(rtnVal != dbSuccess)
	{
        LOG_ERROR("Query sql failed, [%s]",sql.c_str());
         // return rtnVal;
	}

	//无论插入数据是否成功都需要删除临时创建的表
    int rtnVal1 = dbAc.Query("DROP TABLE IF EXISTS tmptbl;");
    if(rtnVal1 != dbSuccess)
	{
		LOG_ERROR("Query sql failed, [DROP TABLE IF EXISTS tmptbl;]");	
        // return rtnVal1;
	}

	return rtnVal;
}


bool DBOperationInterface::UpdatePersonInfo(const std::vector<Person>& datas)
{
	
	//TO DO
	DatabaseAccess dbAc;
	EnumReturnValue rtn = dbSuccess;

	Transaction trn(*(dbAc.GetSession()));
	for(int iter = 0; iter < datas.size() && dbSuccess == rtn; iter++)
	{
//		std::string sql("UPDATE person "
//				"SET name=:name, sex=:sex, idcard=:idcard, superid=:superid, worksn=:worksn, cardid=:cardid, "
//				"departid=:departid, departname=:departname, hasirisdata=:hasirisdata, disabletime=:disabletime, memo=:memo "
//				"WHERE personid=:personid");

        //  去掉更新hasirisdata=:hasirisdata,
        std::string sql("UPDATE person "
                "SET name=:name, sex=:sex, idcard=:idcard, superid=:superid, worksn=:worksn, cardid=:cardid, "
                "departid=:departid, departname=:departname, disabletime=:disabletime, memo=:memo "
                "WHERE personid=:personid");

		//std::cout<<"update:"<<sql<<"  "<<std::endl;
		rtn = dbAc.Update(sql, datas[iter]);
		if(rtn != dbSuccess)
		{
			LOG_ERROR("Query sql failed, [%s]",sql.c_str());	
			break;
		}

	}

	if(dbSuccess == rtn)
	{
		trn.Commit();
		return true;
	}
	else
	{
		trn.Rollback();
		LOG_ERROR("Quert Sql failed. ");
		return false;
	}
}


bool DBOperationInterface::DeletePersonInfo(const std::vector<std::string>& personIDs)
{
	
	//TO DO
	return DeletePersonInfoFromDB(personIDs);
}


bool DBOperationInterface::DeletePersonInfo(const std::vector<Person>& persons)
{
	
	//TO DO
	return DeletePersonInfoFromDB(persons);
}

static std::string GetSQLPidOnlyInTmp()
{
    std::string sql = (" (SELECT tmptbl.personid FROM tmptbl "
            " LEFT JOIN person ON person.personid=tmptbl.personid "
            " WHERE person.personid IS NULL)");

    return sql;
}

bool DBOperationInterface::AddFaceImg(const std::vector<PersonImage>& datas)
{
	
	int nret = 0;
	//TO DO
	DatabaseAccess dbAc;

	std::string sql("DROP TABLE IF EXISTS tmptbl;"
			"CREATE TABLE tmptbl AS SELECT * FROM personimage WHERE imageid = '';");
	if(dbSuccess != dbAc.Query(sql))
	{
		LOG_ERROR("Quert Sql failed. [%s]",sql.c_str());
		return false;
	}


	sql = std::string("INSERT INTO tmptbl (imageid, personid, personimage, memo) "
			"VALUES(:imageid, :personid, :personimage, :memo);");
	if(dbSuccess != dbAc.Insert(sql, datas))
	{
		nret = dbAc.Query("DROP TABLE IF EXISTS tmptbl;");   //unsafe operation
		if(nret != dbSuccess)
		{
			LOG_ERROR("Quert Sql failed. [DROP TABLE IF EXISTS tmptbl;]");
		}
		LOG_ERROR("Quert Sql failed. [%s]",sql.c_str());
		return false;
	}

    ////////////////////////
	std::vector<std::string> imgIds;
    //sql = std::string("SELECT imageid FROM tmptbl WHERE personid NOT IN (SELECT personid FROM person)");
    // modified at 20170105 by yqhe
    // 将NOT IN 修改为 IN
    sql = std::string("SELECT tmptbl.imageid FROM tmptbl WHERE tmptbl.personid IN ");
    sql += GetSQLPidOnlyInTmp() + ";";

	nret = dbAc.Query(sql, imgIds);
	if(nret != dbSuccess)
	{
		LOG_ERROR("Quert Sql failed. [%s]",sql.c_str());
		return false;
	}


//	sql = std::string("DELETE FROM tmptbl WHERE personid NOT IN (SELECT personid FROM person);"
//			"DELETE FROM tmptbl WHERE imageid IN (SELECT imageid FROM personimage); "
//			"DELETE FROM tmptbl WHERE personid NOT IN (SELECT personid FROM person); "
//			"INSERT INTO personimage SELECT * FROM tmptbl;");

        sql = std::string("DELETE FROM tmptbl WHERE personid IN ");
        sql += GetSQLPidOnlyInTmp() + ";";

        sql += std::string("DELETE FROM tmptbl WHERE imageid IN (SELECT imageid FROM personimage); "
                "INSERT INTO personimage SELECT * FROM tmptbl;");

    ////////////////////////

	//删除与本地已经存在的记录重复的记录以及将其他记录插入到本地相应记录表中
	//liu--改
	bool rtnVal = (dbSuccess == dbAc.Query(sql));
	if(!rtnVal)
	{
		LOG_ERROR("Quert Sql failed. [%s]",sql.c_str());
	}

	//无论插入数据是否成功都需要删除临时创建的表
	nret = dbAc.Query("DROP TABLE IF EXISTS tmptbl;");
	if(nret != dbSuccess)
	{
		LOG_ERROR("Quert Sql failed. [DROP TABLE IF EXISTS tmptbl;]");
	}


	if(dbSuccess == rtnVal && imgIds.size() > 0)
	{
		std::cout << "[" << __FUNCTION__  << "@DBOperationInterface]"
			<< "the following face-images are not synchronized as there are not relevent person information: "
			<< std::endl;
		for(auto iter : imgIds)
		{
			std::cout << "\t" << iter << std::endl;
		}
	}

	//返回插入通过网络同步数据到本地的结果
	return rtnVal;
}

// 实际没有更新人员脸部照片的请求
bool DBOperationInterface::UpdateFaceImg(const std::vector<PersonImage>& datas)
{
	
	//TO DO
	DatabaseAccess dbAc;
	EnumReturnValue rtn = dbSuccess;

	Transaction trn(*(dbAc.GetSession()));
	for(int iter = 0; iter < datas.size() && rtn == dbSuccess; iter++)
	{
		//TO DO
		std::string subsql("UPDATE personimage "
				"SET personid=:personid, personimage=:personimage, memo=:memo "
				"WHERE imageid=:imageid");

		rtn = dbAc.Update(subsql, datas[iter]);
		if(rtn != dbSuccess)
		{
			LOG_ERROR("Quert Sql failed. [%s]",subsql.c_str());
			break;
		}
	}


	if(dbSuccess == rtn)
	{
		trn.Commit();
		return true;
	}
	else
	{
		trn.Rollback();
		LOG_ERROR("Quert Sql failed. [%s]");
		return false;
	}
}


bool DBOperationInterface::GetPersonImgRecentAdded(std::vector<PersonImage>& perImgs)
{
	
	//TO DO
	return GetPersonImgRecentChanged(perImgs, OptType::addOpt);
}

bool DBOperationInterface::GetPersonImgRecentDeleted(std::vector<std::string> &ids)
{
	
	//TO DO
	return GetPersonImgRecentChanged(ids, OptType::deleteOpt);
}


bool DBOperationInterface::DeleteFaceImg(const std::vector<std::string> &imgIDs)
{
	
	//TO DO
	return DeleteFaceImgFromDB(imgIDs);
}

bool DBOperationInterface::DeleteFaceImg(const std::vector<PersonImage>& personImgs)
{
	
	//TO DO
	return DeleteFaceImgFromDB(personImgs);
}


/////////////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
 *                       设置识别记录状态函数
 *  函 数 名：SetRecogStatue
 *  功    能：设置识别记录状态
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-26
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int DBOperationInterface::SetRecogStatue(IN const std::vector<int> &lstRecogId)
{
	
	int nret = 0;
	DatabaseAccess dbAc;

	for(int id :lstRecogId)
	{
		std::ostringstream oss;
		oss<<"UPDATE personreclog set uploadstatus = 1 where reclogid = ";
		oss<<id;
		oss<<";";
		nret = dbAc.Query(oss.str());
		if(nret != dbSuccess)
		{
			LOG_ERROR("Query sql failed. [%s]",oss.str().c_str());
			break;
		}
	}

	return nret;
}

/*****************************************************************************
 *                       删除人员中间表函数
 *  函 数 名：DelPersonTemp
 *  功    能：删除人员中间表
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-26
 *  修 改 人：fjf
 *  修改时间：2014-09-09
 *  修改描述：增加操作标记，在实际情况中发现了同一UUID不同操作，删除时方法使用直接删除UUID
 *  造成丢失操作的现象，这里改为删除UUID时同时必须满足操作数一致。
 *****************************************************************************/
int DBOperationInterface::DelPersonTemp(IN const std::vector<std::string> &lstPersonUid,int flag)
{
	int nret = 0;	
	DatabaseAccess dbAc;

	for(std::string uid :lstPersonUid)
	{
		std::ostringstream oss;
		oss<<"delete from tempperson where  personid = \'";
		oss<<uid;
		oss<<"\' and operation=";
		oss<< flag ;
		oss<<";";
		nret = dbAc.Query(oss.str());
		if(nret != dbSuccess)
		{
			LOG_ERROR("Query sql failed. [%s]",oss.str().c_str());
			break;
		}
	}
	return nret;
}

/*****************************************************************************
 *                       删除人员图像中间表函数
 *  函 数 名：DelPersonImage
 *  功    能：删除人员图像中间表
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-26
 *  修 改 人：fjf
 *  修改时间：2014-09-09
 *****************************************************************************/
int DBOperationInterface::DelPersonImageTemp(IN const std::vector<std::string> &lstPersonImage,int flag)
{
	int nret = 0;	
	DatabaseAccess dbAc;

	for(std::string uid :lstPersonImage)
	{
		std::ostringstream oss;
		oss<<"delete from temppersonimage where  imageid = \'";
		oss<<uid;
		oss<<"\' and operation=";
		oss<< flag ;
		oss<<";";
		nret = dbAc.Query(oss.str());
		if(nret != dbSuccess)
		{
			LOG_ERROR("Query sql failed. [%s]",oss.str().c_str());
			break;
		}
	}
	return nret;
}

/*****************************************************************************
 *                       删除人员虹膜中间表函数
 *  函 数 名：DelIrisdata
 *  功    能： 删除人员虹膜中间表
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-26
 *  修 改 人：fjf
 *  修改时间：2014-09-09
 *****************************************************************************/
int DBOperationInterface::DelIrisdataTemp(IN const std::vector<std::string> &lstIrisData,int flag)
{
    int nret = 0;
	DatabaseAccess dbAc;
	for(std::string uid :lstIrisData)
	{
		std::ostringstream oss;
		oss<<"delete from tempirisdatabase where  irisdataid = \'";
		oss<<uid;
		oss<<"\' and operation=";
		oss<< flag ;
		oss<<";";
		nret = dbAc.Query(oss.str());
		std::cout << __FUNCTION__ << oss.str() << std::endl;
		if(nret != dbSuccess)
		{
			LOG_ERROR("Query sql failed. [%s]",oss.str().c_str());
			break;
		}
	}
	return nret;
}



/*****************************************************************************
 *                       添加虹膜函数
 *  函 数 名：AddIrisDataBase
 *  功    能：添加虹膜
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-7
 *  修 改 人：
 *  修改时间：
// modified at 20170105 by yqhe
// 修改通过tmptbl查找相关信息的sql语句，提高速度
 *****************************************************************************/
int DBOperationInterface::AddIrisDataBase(std::vector<IrisDataBase>& vecIrisData,std::vector<irisdataIkUuid> &vecQueryResult)
{
	
	if(vecIrisData.size()<=0)
	{
		//无数据直接返回
		LOG_INFO("No IrisData");
		return 0;
	}

	//打开数据库
	DatabaseAccess dbAccess;
	string queryString("DROP TABLE  IF EXISTS  tmptbl;"
			"CREATE TABLE tmptbl AS SELECT * FROM irisdatabase WHERE irisdataid = '';");
	EnumReturnValue rtn = dbAccess.Query(queryString);
	if(dbSuccess != rtn)
	{
		LOG_ERROR("Query sql failed, %s",queryString.c_str());
		return (int)rtn;
	}

	std::ostringstream oss;

	oss<<" INSERT INTO tmptbl (";
	oss<<" irisdataid,";
	oss<<" personid,";
	oss<<" iriscode,";
	oss<<" devsn,";
	oss<<" eyeflag";
	oss<<") values  ( ";
	oss<<" :irisdataid,";
	oss<<" :personid,";
	oss<<" :iriscode,";
	oss<<" :devsn,";
	oss<<" :eyeflag );";

	queryString = oss.str();

	rtn = dbAccess.Insert(queryString,vecIrisData);
	if(dbSuccess != rtn)
	{
		rtn = dbAccess.Query("DROP TABLE IF EXISTS tmptbl;");
		if(rtn != dbSuccess)
		{
			LOG_ERROR("Query sql failed, [DROP TABLE IF EXISTS tmptbl;]");
		}
		LOG_ERROR("Query sql failed, %s",queryString.c_str());
		return rtn;
	}

    // 去掉没有personid的虹膜记录，不保存
    // not in 的查询方式修改为in
	std::vector<std::string> irisIds;
    // queryString = std::string("SELECT irisdataid FROM tmptbl WHERE personid NOT IN (SELECT personid FROM person)");
    queryString = std::string("SELECT irisdataid FROM tmptbl WHERE personid IN ");
    queryString += GetSQLPidOnlyInTmp() + ";";

	rtn = dbAccess.Query(queryString, irisIds);
	if(rtn != dbSuccess)
	{
		LOG_ERROR("Query sql failed, [%s]",queryString.c_str());
		return rtn;
	}


    // 此处应该写成事务处理
//	queryString = std::string("DELETE FROM tmptbl WHERE personid NOT IN (SELECT personid FROM person);"
//			"DELETE FROM tmptbl WHERE irisdataid IN (SELECT irisdataid FROM irisdatabase); "
//            //"DELETE FROM tmptbl WHERE personid NOT IN (SELECT personid FROM person); "
//            "INSERT INTO irisdatabase SELECT * FROM tmptbl; ");

    queryString = std::string("DELETE FROM tmptbl WHERE personid IN ");
    queryString += GetSQLPidOnlyInTmp() + ";";
    queryString += std::string("DELETE FROM tmptbl WHERE irisdataid IN (SELECT irisdataid FROM irisdatabase); "
            "INSERT INTO irisdatabase SELECT * FROM tmptbl; ");

    rtn = dbAccess.Query(queryString);
	if(rtn != dbSuccess)
	{
		LOG_ERROR("Query sql failed, [%s]",queryString.c_str());
		return rtn;
	}

    string sqlsel = "select irisdataid,personid FROM tmptbl;";
    rtn = dbAccess.Query(sqlsel,vecQueryResult);
    LOG_INFO("################## vecQueryResult.size: %d, ret: %d",vecQueryResult.size(),rtn);
	if(rtn != dbSuccess)
	{
		LOG_ERROR("Query sql failed, [%s]",sqlsel.c_str());
		return rtn;
	}

	if(dbSuccess == rtn && irisIds.size() > 0)
	{
		std::cout << "[" << __FUNCTION__  << "@DBOperationInterface]"
			<< "the following irisdata are not synchronized as there are not relevent person information: "
			<< std::endl;
        for(auto iter : irisIds)
		{
			std::cout << "\t" << iter << std::endl;
		}

	}

	//无论插入数据是否成功都需要删除临时创建的表
	rtn = dbAccess.Query("DROP TABLE IF EXISTS tmptbl;");
	if(rtn != dbSuccess)
	{
		LOG_ERROR("Query sql failed, [DROP TABLE IF EXISTS tmptbl;]");
	}

    return rtn;
}

/*****************************************************************************
 *                       获取令牌函数
 *  函 数 名：GetToken
 *  功    能：获取令牌
 *  说    明：暂时输出int  需要修改令牌为结构体
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-22
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int DBOperationInterface::GetToken(OUT int token[])
{
	
	//打开数据库
	DatabaseAccess dbAccess;
	Token tk;
	int error =  dbAccess.Query("Select * from token;",tk);
	if(error != dbSuccess)
	{
		LOG_ERROR("Query sql failed, [Select * from token;]");
	}

	token[0] = tk.personToken;
	token[1] = tk.irisToken;
	token[2] = tk.imageToken;

	return error;
}

/*****************************************************************************
 *                       设置令牌函数
 *  函 数 名：SetToken
 *  功    能：设置令牌
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-7
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
static int DBOperationInterface::SetToken(IN int token,IN DBType type)
{
	
	//打开数据库
	DatabaseAccess dbAccess;
	std::ostringstream oss;

	if(DBType::DBPerson == type)
	{
		oss<<"update token set persontoken = ";
		oss<<token<<";";

	}else if(DBType::DBPersonPhoto == type)
	{
		oss<<"update token set imagetoken = ";
		oss<<token<<";";

	}else if(DBType::DBIrisData == type)
	{
		oss<<"update token set iristoken = ";
		oss<<token<<";";
	}else
	{
		LOG_ERROR("error DBType, %d",type);
		return -1;
	}

	int ret = dbAccess.Query(oss.str());

    LOG_DEBUG("oss str return %d",ret);
	if(ret != dbSuccess)
	{
		std::string  insertsql = "INSERT INTO token (persontoken, iristoken, imagetoken) VALUES(0, 0, 0)";
		ret = dbAccess.Query(oss.str());
		if(ret != dbSuccess)
			return ret;

		ret = dbAccess.Query(oss.str());
		if(ret != dbSuccess)
		{
			LOG_ERROR("Query sql failed, [%s]",oss.str().c_str());
		}
	}

	return ret;
}

/*****************************************************************************
 *                       获取识别记录函数
 *  函 数 名：GetRecLog
 *  功    能：获取识别记录
 *  说    明：
 *  参    数：status  ：0 所有、1 已发送、2 未发送
 beginTime：  开始时间
 endTime  ： 结束时间
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-22
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int DBOperationInterface::GetRecLog(OUT std::vector<PersonRecLog> &lstRecLog, IN int status,
		IN std::string beginTime,IN std::string endTime)
{
	
	//打开数据库
	DatabaseAccess dbAccess;

	string queryString;
	std::ostringstream oss;

    oss<<" select * from personreclog where 1=1 ";

	if(!beginTime.empty())
	{
        oss<<" and recogtime > \'";
		oss<<beginTime;
		oss<<"\' ";

		//QDateTime dt = QDateTime::currentDateTime();
		//beginTime = dt.addYears(-10).toString("yyyy-MM-dd HH:mm:ss").toStdString();
		//        beginTime = (QDateTime::currentDateTime.addYears(-10)).toString("yyyy-MM-dd HH:mm:ss").toStdString();
	}
	if(!endTime.empty())
	{
		oss<<" and recogtime < \'";
		oss<<endTime;
		oss<<"\' ";

		//QDateTime dt = QDateTime::currentDateTime();
		// endTime = dt.toString("yyyy-MM-dd HH:mm:ss").toStdString();
		//endTime = (QDateTime::currentDateTime).toString("yyyy-MM-dd HH:mm:ss").toStdString();
	}


	if(status == 1)
	{
		//已发送的
		oss<<" and uploadstatus = 1 ";
	}
	else if(status == 2)
	{
		oss<<" and uploadstatus = 0 ";
	}

    oss<<" order by reclogid desc limit 300; ";

	queryString = oss.str();

	LOG_DEBUG("Query sql, %s",queryString.c_str());
	return dbAccess.Query(queryString,lstRecLog);

}

/*****************************************************************************
 *                       删除虹膜函数
 *  函 数 名：DelIrisDataBase
 *  功    能：设置令牌
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-7
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int DBOperationInterface::DelIrisDataBase(std::vector<Uuid>& vecUid,OUT int &token)
{
	int nret = 0;	
	DatabaseAccess dbAc;
	Transaction trn(*(dbAc.GetSession()));

	for (Uuid id: vecUid)
	{
		IkUuid ikuid;
		ikuid.Set(id.uuid);

		token = token < id.OpToken?id.OpToken:token;
		std::ostringstream oss;
		oss<<"Delete From irisdatabase where irisdataid = \'";
		oss<<ikuid.Unparse();
		oss<<"\';";
		nret = dbAc.Query(oss.str());
		if(nret != dbSuccess)
		{
			LOG_ERROR("Query sql failed. [%s]",oss.str().c_str());
			break;
		}
	}

	trn.Commit();
	return nret;
}

/*****************************************************************************
 *                       获取设备添加虹膜信息函数
 *  函 数 名：GetIrisDataBaseRecentAdd
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-22
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool DBOperationInterface::GetIrisDataBaseRecentAdd(std::vector<IrisDataBase> &vecIrisData)
{    //打开数据库
	
	DatabaseAccess dbAccess;
	std::string sql = std::string("SELECT * "
			"FROM irisdatabase AS t1 RIGHT JOIN tempirisdatabase AS t2 ON t1.irisdataid = t2.irisdataid "
			"WHERE t2.operation = 0 limit 3;");
	if(dbSuccess == dbAccess.Query(sql, vecIrisData))
	{
		return true;
	}
	else
	{
		LOG_ERROR("Query sql failed, %s",sql.c_str());
		return false;
	}
}

/*****************************************************************************
 *                       获取设备删除虹膜数据函数
 *  函 数 名：GetIrisDataBaseRecentDel
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-22
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool DBOperationInterface::GetIrisDataBaseRecentDel(std::vector<std::string>& vecUid)
{
	
	//打开数据库
	DatabaseAccess dbAccess;

	std::string sql = std::string(" SELECT irisdataid "
			" FROM tempirisdatabase "
			" WHERE operation = 1");
	if(dbSuccess == dbAccess.Query(sql, vecUid))
	{
		return true;
	}
	else
	{
		LOG_ERROR("Query sql failed, %s",sql.c_str());
		return false;
	}


}

/*****************************************************************************
 *                       设备端添加虹膜数据
 *  函 数 名：DevAddIrisDataBase
 *  功    能：在设备端注册后可调用本函数，将注册虹膜数据保存进虹膜数据库
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：lizhl
 *  创建时间：2014-05-23
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int DBOperationInterface::DevAddIrisDataBase(std::vector<IrisDataBase>& vecIrisData)
{
	
	if(vecIrisData.size()<=0)
	{
		//无数据直接返回
		LOG_INFO("NoData");
		return 0;
	}

	//打开数据库
	DatabaseAccess dbAccess;

	string queryString;
	std::ostringstream oss;

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

	queryString = oss.str();

	LOG_DEBUG("Query sql, %s",queryString.c_str());
	return dbAccess.Insert(queryString,vecIrisData);
}

/*****************************************************************************
 *                       设备端添加临时虹膜数据
 *  函 数 名：DevAddTempIrisDataBase
 *  功    能：在设备端注册后可调用本函数，将注册虹膜数据保存进临时虹膜数据库
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：lizhl
 *  创建时间：2014-05-23
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int DBOperationInterface::DevAddTempIrisDataBase(std::vector<TempIrisDataBase>& vecIrisData)
{
	
	if(vecIrisData.size()<=0)
	{
		//无数据直接返回
		LOG_INFO("NoData");
		return 0;
	}

	//打开数据库
	DatabaseAccess dbAccess;

	string queryString;
	std::ostringstream oss;

	oss<<" INSERT INTO tempirisdatabase (";
	oss<<" personid,";
	oss<<" operation,";
	oss<<" irisdataid";
	oss<<") values  ( ";
	oss<<" :personid,";
	oss<<" :operation,";
	oss<<" :irisdataid);";

	queryString = oss.str();

	LOG_DEBUG("Query sql, %s",queryString.c_str());
	return dbAccess.Insert(queryString,vecIrisData);
}

/*****************************************************************************
 *                       删除设备端虹膜数据函数
 *  函 数 名：DevDelIrisDataBaseAndInsTempIris
 *  功    能：删除设备端同一人员ID下的所有虹膜数据
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：lizhl
 *  创建时间：2014-05-23
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int DBOperationInterface::DevDelIrisDataBaseAndInsTempIris(std::vector<IkUuid>& vecUid)
{
	
	if(vecUid.size()<=0)
	{
		//无数据直接返回
		return 0;
	}

	DatabaseAccess dbAc;

	//    std::vector<IkUuid> vecIrisUUID;
	//    std::ostringstream oss;

	//    //选中传入personid对应的irisid
	//    oss<<"select irisdataid from irisdatabase where personid in ( ";
	//    int ind=0;
	//    for (;ind< vecUid.size()-1;ind ++)
	//    {
	//        oss<<"\'";
	//        oss<<vecUid[ind].Unparse();
	//        oss<<"\',";
	//    }
	//    oss<<"\'";
	//    oss<<vecUid[ind].Unparse();
	//    oss<<"\');";
	//    dbAc.Query(oss.str(),vecIrisUUID);

	//    //记录本次操作所有的personid以及对应的irisid，将相应的删除操作写到临时表中
	//    std::vector<TempIrisDataBase> vecTempIrisDataBase;
	//    TempIrisDataBase tempIrisDataBase;
	//    std::ostringstream tempOss;
	//    uuid_t tempIrisDataId;
	//    uuid_t tempPersonId;//使用王磊传入的id
	//    string queryString;
	//    int personInd, irisInd;
	//    for(personInd = 0; personInd < vecUid.size(); personInd++)
	//    {
	//        for(irisInd = 0; irisInd < vecIrisUUID.size(); irisInd++)
	//        {
	//            vecUid[personInd].Get(tempPersonId);
	//            tempIrisDataBase.personID.Set(tempPersonId);
	//            vecIrisUUID[irisInd].Get(tempIrisDataId);
	//            tempIrisDataBase.irisDataID.Set(tempIrisDataId);
	//            tempIrisDataBase.operation = 1;
	//            vecTempIrisDataBase.push_back(tempIrisDataBase);
	//        }
	//    }
	//    tempOss<<" INSERT INTO tempirisdatabase (";
	//    tempOss<<" personid,";
	//    tempOss<<" operation,";
	//    tempOss<<" irisdataid";
	//    tempOss<<") values  ( ";
	//    tempOss<<" :personid,";
	//    tempOss<<" :operation,";
	//    tempOss<<" :irisdataid);";

	//    queryString = tempOss.str();
	//    dbAc.Insert(queryString,vecTempIrisDataBase);

	//    //删除所有的irisdataid对应的虹膜数据
	//    for (ind = 0; ind < vecIrisUUID.size(); ind++)
	//    {
	//        std::ostringstream ossDel;
	//        ossDel<<"Delete From irisdatabase where irisdataid = ";
	//        ossDel<<"\'";
	//        ossDel<<vecIrisUUID[ind].Unparse();
	//        ossDel<<"\';";
	//        dbAc.Query(ossDel.str());
	//        std::cout << ossDel.str() << std::endl;
	//    }



	int nret = 0;
	//记录本次操作所有的personid以及对应的irisid，将相应的删除操作写到临时表中
    std::vector<TempIrisDataBase> vecTempIrisDataBase;//@#
	TempIrisDataBase tempIrisDataBase;
	std::ostringstream tempOss;
	uuid_t tempIrisDataId;
	uuid_t tempPersonId;//使用王磊传入的id
	string queryString;
	int personInd, irisInd;
	for(personInd = 0; personInd < vecUid.size(); personInd++)
	{
		std::vector<IkUuid> vecIrisUUID;
		std::ostringstream oss;

		//选中传入personid对应的irisid
		oss<<"select irisdataid from irisdatabase where personid in ( ";

		oss<<"\'";
		oss<<vecUid[personInd].Unparse();
		oss<<"\');";
		nret = dbAc.Query(oss.str(),vecIrisUUID);
		if(nret != dbSuccess)
		{
			LOG_ERROR("Query sql failed. [%s]",oss.str().c_str());
			return nret;
		}

		std::cout << oss.str() << std::endl;

		for(irisInd = 0; irisInd < vecIrisUUID.size(); irisInd++)
		{
			vecUid[personInd].Get(tempPersonId);
			tempIrisDataBase.personID.Set(tempPersonId);
			vecIrisUUID[irisInd].Get(tempIrisDataId);
			tempIrisDataBase.irisDataID.Set(tempIrisDataId);
			tempIrisDataBase.operation = 1;
			vecTempIrisDataBase.push_back(tempIrisDataBase);
		}
	}
	tempOss<<" INSERT INTO tempirisdatabase (";
	tempOss<<" personid,";
	tempOss<<" operation,";
	tempOss<<" irisdataid";
	tempOss<<") values  ( ";
	tempOss<<" :personid,";
	tempOss<<" :operation,";
	tempOss<<" :irisdataid);";

	queryString = tempOss.str();
	nret = dbAc.Insert(queryString,vecTempIrisDataBase);
	if(nret != dbSuccess)
	{
		LOG_ERROR("Query sql failed. [%s]",queryString.c_str());
		return nret;
	}

	//在设备上删除
	//删除所有的irisdataid对应的虹膜数据
	for (int ind = 0; ind < vecUid.size(); ind++)
	{
		std::ostringstream ossDel;
		ossDel<<"Delete From irisdatabase where personid = ";
		ossDel<<"\'";
		ossDel<<vecUid[ind].Unparse();
		ossDel<<"\';";
		nret = dbAc.Query(ossDel.str());
		std::cout << ossDel.str() << std::endl;
		if(nret != dbSuccess)
		{
			LOG_ERROR("Query sql failed. [%s]",ossDel.str().c_str());
			return nret;
		}
	}


	return nret;
}

