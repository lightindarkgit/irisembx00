/*******************************************************************
 ** 文件名称：IKDatabaseSession.cpp
 ** 主 要 类：BackendFactory
 ** 描    述：数据库连接创建工厂类
 **
 ** 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
 ** 创 建 人：yqhe
 ** 创建日期：2014/2/20 11:13:37
 **
 ** 版    本：0.0.1
 ** 备    注：命名及代码编写遵守C++编码规范
 *******************************************************************/
#include <iostream>
#include <sstream>

#include <algorithm>

#include "IKBackendSession.h"
#include "../../../Common/Exectime.h"

using namespace std;
using namespace soci;

/*******************************************************************
 *           去掉左边空格
 * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
 * 函数名称：LeftTrim
 * 功    能：去掉左边空格
 * 说    明：
 * 参    数：
 * 返 回 值：无
 * 创 建 人：yqhe
 * 创建日期：2014/3/3 16:51:59
 *******************************************************************/
inline std::string& LeftTrim(std::string &str)
{
	Exectime etm(__FUNCTION__);
    std::string::iterator iter = std::find_if(str.begin(),
        str.end(),
			not1(std::ptr_fun<int, int>(::isspace)));
	str.erase(str.begin(), iter);

	return str;
}

/*******************************************************************
 *           去掉右边空格
 * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
 * 函数名称：RightTrim
 * 功    能：去掉右边空格
 * 说    明：
 * 参    数：
 * 返 回 值：无
 * 创 建 人：yqhe
 * 创建日期：2014/3/3 16:52:38
 *******************************************************************/
inline std::string& RightTrim(std::string &str)
{
	Exectime etm(__FUNCTION__);
    std::string::reverse_iterator rev_iter = std::find_if(str.rbegin(),
        str.rend(),
        not1(std::ptr_fun<int, int>(::isspace)));
    str.erase(rev_iter.base(),str.end());
    return str;
}

/*******************************************************************
 *           去掉两边的空格
 * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
 * 函数名称：Trim
 * 功    能：去掉两边的空格
 * 说    明：
 * 参    数：
 * 返 回 值：无
 * 创 建 人：yqhe
 * 创建日期：2014/3/3 16:54:04
 *******************************************************************/
inline std::string& Trim(std::string &st)
{
	Exectime etm(__FUNCTION__);
    return LeftTrim(RightTrim(st));
}

/*******************************************************************
 *           用于根据特定字符分开std::string
 * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
 * 函数名称：split
 * 功    能：用于根据特定字符分开std::string
 * 说    明：
 * 参    数：
 * 返 回 值：无
 * 创 建 人：yqhe
 * 创建日期：2014/3/3 16:48:00
 *******************************************************************/
static std::vector<std::string> &Split(const std::string &s,
    char delim,
    std::vector<std::string> &elems)
{
	Exectime etm(__FUNCTION__);
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim))
    {
        Trim(item);
        elems.push_back(item);
    }

    return elems;
}

/*******************************************************************
 *           用于根据特定字符分开std::string
 * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
 * 函数名称：split
 * 功    能：用于根据特定字符分开std::string
 * 说    明：
 * 参    数：
 * 返 回 值：无
 * 创 建 人：yqhe
 * 创建日期：2014/3/3 16:48:35
*******************************************************************/
static std::vector<std::string> Split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	Split(s, delim, elems);
	return elems;
}

/*******************************************************************
 *           创建数据库连接
 * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
 * 函数名称：CreateBackend
 * 功    能：创建数据库连接
 * 说    明：
 * 参    数：
 *           输入参数
 *           connString —— 数据库连接参数
 *                           空字符串，打开本地postgresql中irisApp数据库
 *                           "postgresql:xml:testname"，打开缺省XML配置文件中，postgresql->testname指定的数据库
 *                           "postgresql:instant:host=10.2.1.117 port=5432 dbname=IrisAppTest user=postgres password=123456"，打开即时指定的数据库
 *                           更多示例请参见说明书
 *           connType   —— 数据库连接类型
 * 返 回 值：nullptr —— 创建失败；其他 —— 成功
 * 创 建 人：yqhe
 * 创建日期：2014/2/20 9:11:52
 *******************************************************************/
soci::session* BackendFactory::CreateBackend(const std::string &connString,
				EnumDBConnType connType)
{
	Exectime etm(__FUNCTION__);
    IBackendSession *backend = nullptr;

    if (connString.empty())
    {
        backend = new PsqlSession();

        return CheckBackend(backend);
    }

    std::vector<std::string> vecElems = Split(connString, ':');

    if ((vecElems.size() != 3) && (vecElems.size() != 4))
    {
        return CheckBackend(backend);
    }

    // 可能是输入带盘符的文件目录格式，把它们+起来
    if (vecElems.size() == 4)
    {
        vecElems[2] = vecElems[2] + ":" + vecElems[3];
    }

    //int count = 1;
    //for (auto it=vecElems.begin(); it!=vecElems.end(); ++it)
    //{
    //    std::cout << count << "times : " << *it << std::endl;
    //    count++;
    //}

    std::vector<std::string>::const_iterator vecIt;

    // backend名称
    vecIt = vecElems.begin();
    std::string backendName = *vecIt;

    // 使用什么配置
    ++vecIt;
    std::string configName = *vecIt;
    EnumDBConfigType configType = xmlConfig;
    if ("instant"==*vecIt)
    {
        configType = instanceConfig;
    }

    // 配置参数
    ++vecIt;
    std::string configParam = *vecIt;

    if ("postgresql" == backendName)
    {
        // postgresql 参数设置
        //std::cout << "enter postgresql!" << std::endl;

        backend = new PsqlSession(configType, configParam);

        return CheckBackend(backend);
    }
    else if ("sqlite3" == backendName)
    {
        // sqlite参数设置
        //std::cout << "enter sqlite3!" << std::endl;
        backend = new SqliteSession(configType, configParam);

        return CheckBackend(backend);
    }

    return CheckBackend(backend);

}

/*******************************************************************
 *           确认backend的有效性，并返回session指针
 * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
 * 函数名称：CheckBackend
 * 功    能：确认backend的有效性，并返回session指针
 * 说    明：
 * 参    数：
 * 返 回 值：无
 * 创 建 人：yqhe
 * 创建日期：2014/3/3 11:14:58
 *******************************************************************/
soci::session* BackendFactory::CheckBackend(IBackendSession *backend)
{
	Exectime etm(__FUNCTION__);
    if (backend)
    {
        return backend->GetSession();
    }

    return nullptr;
}

void BackendFactory::Close(const std::string& backendName)
{
	Exectime etm(__FUNCTION__);
	IBackendSession *backend = nullptr;

    if ("postgresql" == backendName)
    {
        // postgresql 参数设置
        //std::cout << "enter postgresql!" << std::endl;

		backend = new PsqlSession();
		backend->Close();

		delete backend;

	} 
	else if ("sqlite3" == backendName) 
	{
		backend = new SqliteSession();
		backend->Close();

		delete backend;
	}

    return;
}


void BackendFactory::ReleaseSession(const std::string &backendName, 
				soci::session* pSession)
{
	Exectime etm(__FUNCTION__);
	IBackendSession *backend = nullptr;

    if ("postgresql" == backendName)
    {
        // postgresql 参数设置
        //std::cout << "enter postgresql!" << std::endl;

        backend = new PsqlSession();
        backend->ReleaseSession(pSession);
    }
    else if ("sqlite3" == backendName)
    {
        backend = new SqliteSession();
        backend->ReleaseSession(pSession);
    }

	if(backend != nullptr)
	{
		delete backend;
		backend = nullptr;
	}

    return;
}

