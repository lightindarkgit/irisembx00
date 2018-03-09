/*******************************************************************
 ** 文件名称：main.cpp
 ** 主 要 类：
 ** 描    述：测试网络数据编解码功能
 **
 ** 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
 ** 创 建 人：yqhe
 ** 创建日期：2014/3/14 15:32
 **
 ** 版    本：0.0.1
 ** 备    注：命名及代码编写遵守C++编码规范
 *******************************************************************/
#include <iostream>
#include <sstream>

#include <iomanip>      // 控制格式输出

#include <memory>

#include "CodecNetData.h"

const int noDataLen = 12;

void TestEncodeIris(char* buf);
void TestDecodeIris(const char *buf);

/*******************************************************************
 *         打印编码后的数据，用于检查
 * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
 * 函数名称：main
 * 功    能：打印编码后的数据，用于检查
 * 说    明：
 * 参    数：
 * 		   输入参数
 * 		   buf ——
 * 返 回 值：无
 * 创 建 人：yqhe
 * 创建日期：2014/3/14 15:45
 *******************************************************************/
void printEncodeData(const char *buf, int len, const std::string &info)
{
	std::cout << "================================" << std::endl;
	std::cout << info << std::endl;
	for (int count=0; count<len; count++)
	{
		if ((count!=0)&&(count%16==0))
		{
			std::cout << std::endl;
			std::cout << std::endl;
		}

		int c = (unsigned char)buf[count];
		std::cout << std::hex << std::setw(2) << std::setfill('0') << c << ", ";

	}
	std::cout << "--over!" << std::endl;
	std::cout << "================================" << std::endl;
}

/*******************************************************************
 *           主程序
 * 版权所有：北京中科虹霸科技有限公司 (IrisKing Inc.)
 * 函数名称：main
 * 功    能：主程序
 * 说    明：
 * 参    数：
 * 返 回 值：无
 * 创 建 人：yqhe
 * 创建日期：2014/3/14 15:40
 *******************************************************************/
int main( void )
{
	char buf[noDataLen];

	EncodeNetData::RemoteReset(buf);
	printEncodeData(buf, 12, "远程重启编码");

	ControlCmd ctrlCmd;
	DecodeNetData::RemoteReset(buf, ctrlCmd);

	EncodeNetData::KeepAlive(buf);
	printEncodeData(buf, 12, "心跳请求编码");

	char bufHasData[1024];

	// 测试特征编码请求
	TestEncodeIris(bufHasData);
	// 测试虹膜解码
	TestDecodeIris(bufHasData);

	std::cout << std::endl;
	std::cout << "按回车键退出本次测试..." << std::endl;
	std::cin.peek();

	return 0;
}

// 测试虹膜特征特征编码
void TestEncodeIris(char *bufHasData)
{
	SyncIrisData irisData;
	irisData.EyeFlag = EnumEye::right;
	uuid_generate(irisData.Fid);
	uuid_generate(irisData.Pid);

	char uuidOut[64];
	uuid_unparse(irisData.Fid, uuidOut);
	std::cout << "Feature Data uuid : " << uuidOut << std::endl;

	uuid_unparse(irisData.Pid, uuidOut);
	std::cout << "Person  Data uuid : " << uuidOut << std::endl;

	// for (int i=0; i<g_enrollFeatureLen; i++)
	memset(irisData.FeatureData, 0, g_enrollFeatureLen);
	for (int i=0; i<16; i++)
	{
		irisData.FeatureData[i] = 0x66;
	}

	// char bufHasData[1024];
	memset(bufHasData, 0, 1024);
	std::vector<SyncIrisData> vecIrisData;
	vecIrisData.push_back(irisData);
	EncodeNetData::AddPersonIris(vecIrisData, bufHasData);

	printEncodeData(bufHasData, 64, "增加人员特征请求编码");

}

// 测试虹膜特征解码
void TestDecodeIris(const char *bufHasData)
{
	std::vector<SyncIrisData> vod;
	DecodeNetData::AddPersonIris(bufHasData, vod);

	std::cout << "================================" << std::endl;
	std::cout << "特征请求解码" << std::endl;


	for (auto vecIt=vod.begin(); vecIt!=vod.end(); ++vecIt)
	{
		char uuidOut[64];
		uuid_unparse(vecIt->Fid, uuidOut);
		std::cout << "Feature Data uuid : " << uuidOut << std::endl;

		uuid_unparse(vecIt->Pid, uuidOut);
		std::cout << "Person  Data uuid : " << uuidOut << std::endl;

		printEncodeData((char*)&vecIt->FeatureData, 64, "脱机特征解码");

	}

	std::cout << "--over!" << std::endl;
	std::cout << "================================" << std::endl;
}

