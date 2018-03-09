/******************************************************************************************
** �ļ���:   GTEST.CPP
���� ��Ҫ��:   (��)
**
** Copyright (c) �пƺ�����޹�˾
** ������:   fjf
** ��  ��:   2013-12-09
** �޸���:
** ��  ��:
** ��  ��:   GTEST����ģ��
** ���� ��Ҫģ��˵����
**
** ��  ��:   1.0.0
** ��  ע:
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

//������������---����Ҫʵ��
class TestSocketCom :public ::testing::TestWithParam<int>
{};
TEST_P(InitTest, InitResult)
{
	SocketAPI sApi;
	int num = GetParam();
	EXPECT_TRUE(sApi.Start());
}
INSTANTIATE_TEST_CASE_P(ResulstData,InitTest,testing::Values(1,3,5,7,9));

//�̼�������
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
	*                        ��ʼ����ǰ��������
	*  �� �� ����InitEnv
	*  ��    �ܣ��Բ��Ե���Ҫ�����������������г�ʼ��
	*  ˵    ����
	*  ��    ����
	*
	*  �� �� ֵ��
	*  �� �� �ˣ�fjf
	*  ����ʱ�䣺2013-12-25
	*  �� �� �ˣ�
	*  �޸�ʱ�䣺
	*****************************************************************************/
	void InitEnv()
	{
	}
	/*****************************************************************************
	*                        �ͷŵ�ǰ��������Դ����
	*  �� �� ����ReleaseRes
	*  ��    �ܣ��Բ��Եĵ�ǰ������ʹ�õ���Դ�����ͷ�
	*  ˵    ����
	*  ��    ����
	*
	*  �� �� ֵ��
	*  �� �� �ˣ�fjf
	*  ����ʱ�䣺2013-12-25
	*  �� �� �ˣ�
	*  �޸�ʱ�䣺
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


