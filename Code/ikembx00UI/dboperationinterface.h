/*****************************************************************************
** 文 件 名：dboperationinterface.h
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
#pragma once
#include <vector>
#include <string>
#include "../ikIOStream/interface.h"
#include "typeconvert.h"
#include "../common.h"


const std::string GetPersonID(const std::string& psnID);
const std::string GetPersonID(const Person& psn);
const std::string GetImgID(const std::string& imgID);
const std::string GetImgID(const PersonImage& personImg);
enum class DBType
{
    DBPerson,
    DBPersonPhoto,
    DBIrisData
};

class DBOperationInterface
{
public:
    //    static int GetPersonCount();
    /*****************************************************************************
    *                        获取设备上的人员信息
    *  函 数 名：GetPersons
    *  功    能：查询并返回当前设备上的数据库中的人员信息
    *  说    明：只有当返回值为true时，返回的人员信息列表才有效
    *  参    数：persons，人员信息查询结果输出
    *  返 回 值：
    *  创 建 人：L.Wang
    *  创建时间：2014-04-28
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static bool GetPersons(std::vector<Person> &persons);
    static bool GetPersons(std::vector<IkUuid> personids,std::vector<Person> &persons);
    /*****************************************************************************
    *                        获取设备上的新增加的人员ID
    *  函 数 名：GetPersons
    *  功    能：查询并返回当前设备上的新增加的人员ID
    *  说    明：返回结果中只包括尚未tempperson表中被标记为增加操作的人员ID
    *  参    数：ids，人员ID查询结果输出
    *  返 回 值：
    *  创 建 人：L.Wang
    *  创建时间：2014-05-20
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static bool GetPersonRecentAdded(std::vector<Person> &persons);

    /*****************************************************************************
    *                        获取设备上新修改的人员ID
    *  函 数 名：GetPersonRecentEdited
    *  功    能：查询并返回当前设备上新修改人员信息的人员ID
    *  说    明：返回结果中只包括tempperson表中的被标记为修改操作的人员ID集合
    *  参    数：ids，人员ID查询结果输出
    *  返 回 值：
    *  创 建 人：L.Wang
    *  创建时间：2014-05-20
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static bool GetPersonRecentEdited(std::vector<Person> &persons);

    /*****************************************************************************
    *                        获取设备上最近删除的人员ID
    *  函 数 名：GetPersonRecentDeleted
    *  功    能：查询并返回当前设备上人员信息被最近删除的人员ID
    *  说    明：返回结果中只包括tempperson表中被标记为删除操作的人员ID集合
    *  参    数：ids，人员ID查询结果输出
    *  返 回 值：
    *  创 建 人：L.Wang
    *  创建时间：2014-05-20
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static bool GetPersonRecentDeleted(std::vector<std::string> &ids);

    /*****************************************************************************
    *                        增加人员信息
    *  函 数 名：AddPersonInfo
    *  功    能：向数据库中添加人员信息
    *  说    明：
    *  参    数：persons，待添加的人员信息集合
    *  返 回 值：成功返回ture，其他返回false
    *  创 建 人：L.Wang
    *  创建时间：2014-04-28
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static int AddPersonInfo(const std::vector<Person>& persons);

    /*****************************************************************************
    *                        更新人员信息
    *  函 数 名：UpdatePersonInfo
    *  功    能：对给定的人员信息进行更新
    *  说    明：
    *  参    数：persons，待更新的人员信息集合
    *  返 回 值：成功返回true，其他返回false
    *  创 建 人：L.Wang
    *  创建时间：2014-04-28
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static bool UpdatePersonInfo(const std::vector<Person>& persons);

    /*****************************************************************************
    *                        删除人员信息
    *  函 数 名：DeletePersonInfo
    *  功    能：删除指定的人员信息
    *  说    明：
    *  参    数：pids，待删除的人员信息所对应的personid集合
    *  返 回 值：成功返回true，其他返回false
    *  创 建 人：L.Wang
    *  创建时间：2014-04-28
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static bool DeletePersonInfo(const std::vector<std::string> &pids);



    /*****************************************************************************
    *                        获取设备上的新增加的人员图像ID
    *  函 数 名：GetPersonImgRecentAdded
    *  功    能：查询并返回当前设备上的新增加人员图像的人员ID
    *  说    明：返回结果中只包括尚未temppersonimage表中被标记为增加操作的人员ID
    *  参    数：ids，人员ID查询结果输出
    *  返 回 值：
    *  创 建 人：L.Wang
    *  创建时间：2014-05-20
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static bool GetPersonImgRecentAdded(std::vector<PersonImage> &perImgs);

    /*****************************************************************************
    *                        获取设备上最近删除人员图像的人员ID
    *  函 数 名：GetPersonImgRecentDeleted
    *  功    能：查询并返回当前设备上人员信息被最近删除人员图像的人员ID
    *  说    明：返回结果中只包括temppersonimage表中被标记为删除操作的人员ID集合
    *  参    数：ids，人员ID查询结果输出
    *  返 回 值：
    *  创 建 人：L.Wang
    *  创建时间：2014-05-20
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static bool GetPersonImgRecentDeleted(std::vector<std::string> &ids);

    /*****************************************************************************
    *                        添加人脸图像
    *  函 数 名：AddFaceImg
    *  功    能：为指定的人员添加人脸图像
    *  说    明：
    *  参    数：personImags，待添加的人脸图像信息集合
    *  返 回 值：成功返回true，其他返回false
    *  创 建 人：L.Wang
    *  创建时间：2014-04-28
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static bool AddFaceImg(const std::vector<PersonImage>& personImags);

    /*****************************************************************************
    *                        更新人脸图像
    *  函 数 名：UpdateFaceImg
    *  功    能：更新指定人员的人脸图像
    *  说    明：
    *  参    数：personImags，待更新的人脸图像信息集合
    *  返 回 值：成功返回true，其他返回false
    *  创 建 人：L.Wang
    *  创建时间：2014-04-28
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static bool UpdateFaceImg(const std::vector<PersonImage>& personImags);



    /*****************************************************************************
    *                        删除人脸图像
    *  函 数 名：DeleteFaceImg
    *  功    能：删除指定的人脸图像
    *  说    明：
    *  参    数：imgIDs，待删除的人脸图像ID集合
    *  返 回 值：成功返回true，其他返回false
    *  创 建 人：L.Wang
    *  创建时间：2014-04-28
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static bool DeleteFaceImg(const std::vector<std::string>& imgIDs);


private:
    DBOperationInterface();
    enum class OptType:short{addOpt = 0, deleteOpt, editOpt};

    template<typename T>
    static bool GetPersonInfoRecentChanged(T& res, const OptType opt);
    //    {
    //        //TO DO
    //        res.clear();


    //        //Default to handle querying added person information
    //        std::string sql("SELECT * FROM person "
    //                        "WHERE person.personid IN (SELECT personid FROM tempperson WHERE tempperson.operation = 0)");

    //        //In case to handle querying edited person information
    //        if(OptType::editOpt == opt)
    //        {
    //            sql = std::string("SELECT * FROM person "
    //                              "WHERE person.personid IN (SELECT personid FROM tempperson WHERE tempperson.operation = 2)");
    //        }

    //        //In case to handle querying deleted person information
    //        if(OptType::deleteOpt == opt)
    //        {
    //            sql = std::string("SELECT t1.personid "
    //                              "FROM person AS t1 LEFT JOIN tempperson AS t2 ON t1.personid = t2.personid "
    //                              "WHERE t2.operation = 1");
    //        }


    //        //Get conditional result
    //        DatabaseAccess dbAc;
    //        if(dbSuccess == dbAc.Query(sql, res))
    //        {
    //            return true;
    //        }
    //        else
    //        {
    //            return false;
    //        }
    //    }


    template<typename T>
    static bool GetPersonImgRecentChanged(T& res, const OptType opt);
    //    {
    //        //TO DO
    //        res.clear();

    //        //Default to handle querying added personimage
    //        std::string  sql("SELECT * FROM personimage "
    //                         "WHERE personimage.personid IN (SELECT personid FROM temppersonimage WHERE temppersonimage.operation = 0)");

    //        //In case to handle querying deleted person information
    //        if(OptType::deleteOpt == opt)
    //        {
    //            sql = std::string("SELECT t1.personid "
    //                              "FROM personimage AS t1 LEFT JOIN temppersonimage AS t2 ON t1.personid = t2.personid "
    //                              "WHERE t2.operation = 1");
    //        }


    //        //Get conditional result
    //        DatabaseAccess dbAc;
    //        if(dbSuccess == dbAc.Query(sql, res))
    //        {
    //            return true;
    //        }
    //        else
    //        {
    //            return false;
    //        }
    //    }


    /*****************************************************************************
    *                        删除人员信息
    *  函 数 名：DeletePersonInfo
    *  功    能：删除指定的人员信息
    *  说    明：请使用删除指定人员ID版的DeletePersonInfo
    *  参    数：persons，待删除的人员信息集合
    *  返 回 值：成功返回true，其他返回false
    *  创 建 人：L.Wang
    *  创建时间：2014-04-28
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static bool DeletePersonInfo(const std::vector<Person>& persons);

    /*****************************************************************************
    *                        删除人脸图像
    *  函 数 名：DeleteFaceImg
    *  功    能：删除指定的人脸图像
    *  说    明：请使用删除指定图像ID版的DeleteFaceImg
    *  参    数：personImags，待删除的人脸图像信息集合
    *  返 回 值：成功返回true，其他返回false
    *  创 建 人：L.Wang
    *  创建时间：2014-04-28
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    static bool DeleteFaceImg(const std::vector<PersonImage>& personImags);

    /*****************************************************************************
    *                        删除数据库中的人员信息
    *  函 数 名：DeletePersonInfoFromDB
    *  功    能：从数据库中删除指定的人脸图像
    *  说    明：该方法在删除人员信息的时候会同时删除相关的虹膜数据和人脸图像数据
    *  参    数：recs，待删除的人员信息集合
    *  返 回 值：成功返回true，其他返回false
    *  创 建 人：L.Wang
    *  创建时间：2014-04-28
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    template <typename T>
    static bool DeletePersonInfoFromDB(const std::vector<T>& recs)
    {
        //TO DO
        if(0 == recs.size())
        {
            std::cout << "[" << __FUNCTION__  << "@PersonInfoInterface]"
                      << "no person information to delete!"
                      << std::endl;

            return true;
        }

        //Get person ids to be deleted
        std::string pids;
        EnumReturnValue sqlRes = dbSuccess;
        DatabaseAccess dbAc;
        Transaction trn(*(dbAc.GetSession()));
        char sqlCmd[1024];
        for(int iter = 0; iter < recs.size() && dbSuccess == sqlRes; iter++)
        {
            memset(sqlCmd, 0, sizeof(sqlCmd) / sizeof(sqlCmd[0]));
            sprintf(sqlCmd,
                    "DELETE FROM irisdatabase WHERE personid = '%s';DELETE FROM personimage WHERE personid = '%s';DELETE FROM person WHERE personid = '%s';",
                    GetPersonID(recs[iter]).c_str(), GetPersonID(recs[iter]).c_str(), GetPersonID(recs[iter]).c_str());
            sqlRes = dbAc.Query(sqlCmd);
        }

        if(dbSuccess ==  sqlRes)
        {
            trn.Commit();
            return true;
        }
        else
        {
            trn.Rollback();
            return false;
        }
    }

    /*****************************************************************************
    *                        删除人脸图像
    *  函 数 名：DeleteFaceImg
    *  功    能：删除指定的人脸图像
    *  说    明：请使用删除指定图像ID版的DeleteFaceImg
    *  参    数：personImags，待删除的人脸图像信息集合
    *  返 回 值：成功返回true，其他返回false
    *  创 建 人：L.Wang
    *  创建时间：2014-04-28
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
    template <typename T>
    static bool DeleteFaceImgFromDB(const std::vector<T>& recs)
    {
        //TO DO
        if(0 == recs.size())
        {
            std::cout << "[" << __FUNCTION__  << "@PersonInfoInterface]"
                      << "no person face image to delete!"
                      << std::endl;
            return true;
        }

        //Get image ids to be deleted
        std::string imgids;
        std::string pids;
        EnumReturnValue sqlRes = dbSuccess;
        DatabaseAccess dbAc;
        Transaction trn(*(dbAc.GetSession()));
        for(int iter = 0; iter < recs.size() && dbSuccess == sqlRes; iter++)
        {
            sqlRes = dbAc.Query(std::string("DELETE FROM personimage WHERE imageid = '").append(GetImgID(recs[iter])).append("';"));
        }

        if(dbSuccess ==  sqlRes)
        {
            trn.Commit();
            return true;
        }
        else
        {
            trn.Rollback();
            return false;
        }
    }



    //////-----liuzhch

public:

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
    static int SetRecogStatue(IN const std::vector<int> &lstRecogId);

    /*****************************************************************************
   *                       删除人员中间表函数
   *  函 数 名：DelPersonTemp
   *  功    能：删除人员中间表
   *  说    明：
   *  参    数：
   *  返 回 值：
   *  创 建 人：liuzhch
   *  创建时间：2014-05-26
   *  修 改 人：
   *  修改时间：
   *****************************************************************************/
    static int DelPersonTemp(IN const std::vector<std::string> &lstPersonUid,int flag);

    /*****************************************************************************
   *                       删除人员图像中间表函数
   *  函 数 名：DelPersonImage
   *  功    能：删除人员图像中间表
   *  说    明：
   *  参    数：
   *  返 回 值：
   *  创 建 人：liuzhch
   *  创建时间：2014-05-26
   *  修 改 人：
   *  修改时间：
   *****************************************************************************/
    static int DelPersonImageTemp(IN const std::vector<std::string> &lstPersonImage,int flag);

    /*****************************************************************************
   *                       删除人员虹膜中间表函数
   *  函 数 名：DelIrisdata
   *  功    能： 删除人员虹膜中间表
   *  说    明：
   *  参    数：
   *  返 回 值：
   *  创 建 人：liuzhch
   *  创建时间：2014-05-26
   *  修 改 人：
   *  修改时间：
   *****************************************************************************/
    static int DelIrisdataTemp(IN const std::vector<std::string> &lstIrisData,int flag);





    /*****************************************************************************
   *                       获取识别记录函数
   *  函 数 名：GetRecLog
   *  功    能：获取所有识别记录
   *  说    明：
   *  参    数：
   *  返 回 值：
   *  创 建 人：liuzhch
   *  创建时间：2014-05-6
   *  修 改 人：
   *  修改时间：
   *****************************************************************************/
    static int GetRecLog(OUT std::vector<PersonRecLog> &lstRecLog);

    /*****************************************************************************
   *                        获取识别记录函数
   *  函 数 名：GetRecLog
   *  功    能：通过状态获取识别记录
   *  说    明：0- 未上传  1 - 已上传
   *  参    数：
   *  返 回 值：
   *  创 建 人：liuzhch
   *  创建时间：2014-05-6
   *  修 改 人：
   *  修改时间：
   *****************************************************************************/
    static int GetRecLog(OUT std::vector<PersonRecLog> &lstRecLog, IN int status,
                         IN std::string beginTime,IN std::string endTime);

    /*****************************************************************************
   *                        获取识别记录函数
   *  函 数 名：GetRecLog
   *  功    能：通过时间获取识别记录
   *  说    明：获取起始时间以后的所有识别记录
   *  参    数：timeBegin：起始时间
   *  返 回 值：
   *  创 建 人：liuzhch
   *  创建时间：2014-05-6
   *  修 改 人：
   *  修改时间：
   *****************************************************************************/
    static int GetRecLog(OUT std::vector<PersonRecLog> &lstRecLog, IN std::string timeBegin);

    /*****************************************************************************
   *                        删除人员函数
   *  函 数 名：DeletePersonInfo
   *  功    能：通过uuid删除人员信息
   *  说    明：
   *  参    数：
   *  返 回 值：
   *  创 建 人：liuzhch
   *  创建时间：2014-05-7
   *  修 改 人：
   *  修改时间：
   *****************************************************************************/
    static bool DeletePersonInfo(const vector<Uuid> &uid,OUT int &token)
    {
        std::vector<std::string> lstUuid;
        for (Uuid id: uid)
        {
            IkUuid ikuid;
            token = token<id.OpToken?id.OpToken:token;
            ikuid.Set(id.uuid);
            lstUuid.push_back(ikuid.Unparse());

        }
        return DeletePersonInfo(lstUuid);
    }

    /*****************************************************************************
   *                        删除人员图像函数
   *  函 数 名：DeleteFaceImg
   *  功    能：通过uuid删除人员图像
   *  说    明：
   *  参    数：
   *  返 回 值：
   *  创 建 人：liuzhch
   *  创建时间：2014-05-7
   *  修 改 人：
   *  修改时间：
   *****************************************************************************/
    static bool DeleteFaceImg(IN const vector<Uuid> &uid,OUT int &token)
    {
        std::vector<std::string> lstUuid;
        for (Uuid id: uid)
        {
            IkUuid ikuid;
            ikuid.Set(id.uuid);
            token = token < id.OpToken?id.OpToken:token;
            lstUuid.push_back(ikuid.Unparse());
        }


        return DeleteFaceImg(lstUuid);
    }

    /*****************************************************************************
   *                        获取令牌函数
   *  函 数 名：GetToken
   *  功    能：获取令牌
   *  说    明：
   *  参    数：
   *  返 回 值：
   *  创 建 人：liuzhch
   *  创建时间：2014-05-7
   *  修 改 人：
   *  修改时间：
   *****************************************************************************/
    static int GetToken(OUT int token[]);

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
    static int SetToken(IN int token,IN DBType type);

    /*****************************************************************************
   *                       添加虹膜函数
   *  函 数 名：InsertIrisDataBase
   *  功    能：添加虹膜
   *  说    明：
   *  参    数：
   *  返 回 值：
   *  创 建 人：liuzhch
   *  创建时间：2014-05-7
   *  修 改 人：
   *  修改时间：
   *****************************************************************************/
    static int AddIrisDataBase(std::vector<IrisDataBase> &vecIrisData,std::vector<irisdataIkUuid> &vecout);

    /*****************************************************************************
   *                       添加虹膜函数
   *  函 数 名：InsertIrisDataBase
   *  功    能：添加虹膜
   *  说    明：
   *  参    数：
   *  返 回 值：
   *  创 建 人：liuzhch
   *  创建时间：2014-05-7
   *  修 改 人：
   *  修改时间：
   *****************************************************************************/
    static int DelIrisDataBase(std::vector<Uuid>& vecUid,OUT int &token);

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
    static bool GetIrisDataBaseRecentAdd(std::vector<IrisDataBase> &vecIrisData);

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
    static bool GetIrisDataBaseRecentDel(std::vector<std::string>& vecUid);


    //////-------liuzhch--end

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
    static int DevAddIrisDataBase(std::vector<IrisDataBase>& vecIrisData);

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
    static int DevAddTempIrisDataBase(std::vector<TempIrisDataBase>& vecIrisData);

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
    static int DevDelIrisDataBaseAndInsTempIris(std::vector<IkUuid>& vecUid);
};


