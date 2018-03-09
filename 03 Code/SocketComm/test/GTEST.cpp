/******************************************************************************************
** 文件名:   GTEST.CPP
×× 主要类:   (无)
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   fjf
** 日  期:   2013-12-09
** 修改人:
** 日  期:
** 描  述:   GTEST测试模块
** ×× 主要模块说明：
**
** 版  本:   1.0.0
** 备  注:
**
******************************************************************************************/

#include "../socketAPI.h"
#include "../socketInterfaceAPI.h"
#include "../Client/netClient.h"
#include "../Client/netClientSelect.h"
#include "../queueThread.h"
#include "/opt/gtest1.6/include/gtest/gtest.h"


TEST(StartTest, ExceptionInput)
{
	SocketAPI sApi;
	EXPECT_EQ(false,sApi.Start());
}

//参数化测试类---不需要实现
class TestSocketCom :public ::testing::TestWithParam<int>
{};
TEST_P(InitTest, InitResult)
{
	SocketAPI sApi;
	int num = GetParam();
	EXPECT_TRUE(sApi.Start());
}
INSTANTIATE_TEST_CASE_P(ResulstData,InitTest,testing::Values(1,3,5,7,9));

//固件测试类
class TestQueue :public testing::Test
{
protected:
	virtual SetUp()
	{
	}
	virtual TearDown()
	{
	}
	/*****************************************************************************
	*                        初始化当前环境函数
	*  函 数 名：InitEnv
	*  功    能：对测试的主要环境参数及变量进行初始化
	*  说    明：
	*  参    数：
	*
	*  返 回 值：
	*  创 建 人：fjf
	*  创建时间：2013-12-25
	*  修 改 人：
	*  修改时间：
	*****************************************************************************/
	void InitEnv()
	{
	}
	/*****************************************************************************
	*                        释放当前环境的资源函数
	*  函 数 名：ReleaseRes
	*  功    能：对测试的当前环境所使用的资源进行释放
	*  说    明：
	*  参    数：
	*
	*  返 回 值：
	*  创 建 人：fjf
	*  创建时间：2013-12-25
	*  修 改 人：
	*  修改时间：
	*****************************************************************************/
	void ReleaseRes()
	{
	}

	NetClient _nc;
	NetClientSelect _ncs;
	QueueThread<int> _qt;
};

TEST_F(TestQueue, TestTryPop)
{}
TEST_F(TestQueue, TestPush)
{}
TEST_F(TestQueue, TestPopSharePtr)
{}
TEST_F(TestQueue, TestPop)
{}


