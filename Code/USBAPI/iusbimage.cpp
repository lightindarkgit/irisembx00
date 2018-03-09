/******************************************************************************************
 ** 文件名:   IUSBIMAGE_CPP
 **
 ** Copyright (c) 中科虹霸有限公司
 ** 创建人:   刘中昌
 ** 日  期:   2013-11-06
 ** 修改人:
 ** 日  期:
 ** 描  述:   USB通信获取图像接口
 *×× 主要模块说明: 该接口为应用提供简单方便的获取USB图像流
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 *****************************************************************************************/
#include "iusbimage.h"
#include "pthread.h"

#include <stdio.h>
#include <signal.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "simplelock.h"
#include <unistd.h>
#include "../Common/Exectime.h"

static bool s_isExit = true;

	IkUSBAPI::IkUSBAPI():_ikBuf(nullptr)
						 ,_isOpen(false)
{
	Exectime etm(__FUNCTION__);
	_ikBuf = new unsigned char[TransBufferSize];
	_ikBufCmd = new unsigned char[14];
	_dealWithUSBTransferBuf.reset(new DealWithUSBTransferBuf());
	int err =pthread_create(&_pollThread, NULL, PollThreadLoop, (void *)this);
	if(err!=0)
	{
		std::cout<<"线程PollThreadLoop创建失败！"<<std::endl;
	}
}

IkUSBAPI::~IkUSBAPI()
{
	Exectime etm(__FUNCTION__);
	Close();
	if(nullptr == _ikBuf)
	{
		delete[] _ikBuf;
	}

	if(nullptr == _ikBufCmd)
	{
		delete[] _ikBufCmd;
	}

	if(!pthread_cancel(_pollThread))
	{
		pthread_join(_pollThread, nullptr);
	}
}

/**********************************************************************************************************************************
 *                        初始化函数
 *  函 数 名：Init
 *  功    能：初始化USB的设备
 *  说    明：
 *  参    数：EndPointNum:端点号：高速设备有31个端点，15IN,15OUT；低速3个端点，1IN,1OUT；除了端点0可IN可OUT，其它只能是IN或者OUT
 *  返 回 值：错误描述, 异常返回
 *  创 建 人：刘中昌
 *  创建时间：2013-11-14
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int IkUSBAPI::Init(IN int endPointNum)
{
	Exectime etm(__FUNCTION__);
	if(_isOpen)
	{
		return E_OK;
	}
	//std::cout<<"Open 开始"<<std::endl;
	SimpleLockForMutex lock(_imageMutex);
	//打开USB设备
	IkUSBDevice *dev = new IkUSBDevice(TransferBufCallBack,(void *)this);
	_ikCallBack = nullptr;
	//    if(dev )
	//std::shared_ptr<PeopleNode> pTemp = std::make_shared<PeopleNode>();

	//    std::shared_ptr<IkUSBDevice> ikDevice;
	//    ikDevice  = std::make_shared<IkUSBDevice>(TransferBufCallBack,(void *)this);
	_ikDevice.reset(dev);
	if(E_OK ==_ikDevice->Open())
	{
		//获取从USB输入端点
		IkBulkEndPoint *endPointIn = _ikDevice->GetBulkEndPoint(DirFromDevice);
		if(!endPointIn)
		{
			return E_NO_ENDPOINT;
		}
		//获取输出到USB设备端点
		IkBulkEndPoint *endPointOut = _ikDevice->GetBulkEndPoint(DirToDevice);
		if(!endPointOut)
		{
			return E_NO_ENDPOINT;
		}

		//获取端点
		_ikEndPointIn.reset(endPointIn);
		_ikEndPointOut.reset(endPointOut);

		//开始传输
		_ikEndPointIn->BeginDataTransfer(_ikBuf,_bufLength);
		_isOpen =true;
		//启动异步串口数据接收线程
		std::cout << "USB设备初始化成功！" << std::endl;
		{
			s_isExit = true;
		}
		_sendEventwaiter.signal();
		//pthread_detach(_pollThread);
		//std::cout<<"Open 结束  !"<<std::endl;
		return E_OK;
	}else
	{
		std::cout << "USB设备初始化失败！" << std::endl;
		return E_DEVICE_NO_OPEN;
	}
}

/**********************************************************************************************************************************
 *                        实时获取图像函数
 *  函 数 名：GetImage
 *  功    能：获得当前图像
 *  说    明：未获得数据阻塞
 *  参    数：IkImageLoadeCallbackFunc:   回调函数
 *  返 回 值：错误描述, 异常返回  -1超时
 *  创 建 人：刘中昌
 *  创建时间：2013-11-14
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int IkUSBAPI::GetPreview(OUT unsigned char * image,OUT unsigned int& length)
{
	Exectime etm(__FUNCTION__);
	if(_isOpen )
	{
		if(_dealWithUSBTransferBuf->GetPreview(image,length))
		{
			return E_OK;
		}
		else
		{

			if(_transferStatus == TransferNoDevice  || _transferStatus == TransferError )
			{
				std::cout << "USB设备传输错误！" << std::endl;
				return E_NO_TRANSFER_ERROR;
			}

			std::cout << "获取图像超时！" << std::endl;
			return E_TIME_OVER;
		}
	}
	else
	{
		std::cout << "获取图像，但设备未打开或获取失败！" << std::endl;
		usleep(0);
	}
	return E_AGAIN;
}

/**********************************************************************************************************************************
 *                        异步获取图像函数
 *  函 数 名：GetImage
 *  功    能：获得当前图像
 *  说    明：未获得数据阻塞
 *  参    数：IkImageLoadeCallbackFunc:   回调函数
 *  返 回 值：错误描述, 异常返回  -1超时
 *  创 建 人：刘中昌
 *  创建时间：2013-11-14
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int IkUSBAPI::GetPreview(IN IkImageLoadeCallBackFunc callBack,IN const unsigned long outTime)
{
	Exectime etm(__FUNCTION__);
	if(_isOpen)
	{
		_ikCallBack = callBack;
		std::cout << "设备已打开！" << std::endl;
		return E_OK;
	}
	std::cout << "获取图像设备未打开！" << std::endl;
	return E_NXIO;
}

/**********************************************************************************************************************************
 *                        停止USB读取操作函数
 *  函 数 名：Close
 *  功    能：关闭USB的操作包括读取和写入
 *  说    明：关闭USB、清理内存
 *  参    数： 无
 *  返 回 值：true 操作成功；false操作失败
 *  创 建 人：刘中昌
 *  创建时间：2013-11-14
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
void IkUSBAPI::Close()
{
	Exectime etm(__FUNCTION__);
	//std::cout<<"Close 开始"<<std::endl;
	SimpleLockForMutex lock(_imageMutex);
	if(_isOpen)
	{
		_isOpen =false;
		s_isExit = false;

		timespec tim;
		tim.tv_sec = time(nullptr);
		tim.tv_nsec = 500;
		tim.tv_sec +=10;
		// std::cout<<"wait() 前"<<std::endl;
		_waiter.wait(tim);
		//_waiter.wait();
		//        std::cout<<"wait() 后"<<std::endl;
		//_ikEndPointIn->Release();
		// std::cout<<"Release() 后"<<std::endl;
		//_ikEndPointOut->Release();
		_ikDevice->Close();
		// std::cout << "USB设备关闭！" << std::endl;
	}
	// std::cout << "Close 结束！" << std::endl;
}

/**********************************************************************************************************************************
 *                        发送命令函数
 *  函 数 名：SendCmd
 *  功    能：通过命令控制USB相关操作
 *  说    明：暂时为开通该功能
 *  参    数：EndPointNum：端点值  cmd 命令
 *  返 回 值：0 操作成功；-1操作失败
 *  创 建 人：刘中昌
 *  创建时间：2013-11-14
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int IkUSBAPI::SendCmd(IN int EndPointNum, IN char *cmd)
{
	Exectime etm(__FUNCTION__);

}



/**********************************************************************************************************************************
 *                        发送命令函数
 *  函 数 名：SendCmd
 *  功    能：通过命令控制USB相关操作
 *  说    明：暂时为开通该功能
 *  参    数：EndPointNum：端点值  cmd 命令
 *  返 回 值：0 操作成功；-1操作失败
 *  创 建 人：刘中昌
 *  创建时间：2013-11-14
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int IkUSBAPI::SendCmd(Command cmd)
{
	Exectime etm(__FUNCTION__);
	//    std::cout<<"sendCmd 1"<<std::endl;
	//    SimpleLockForMutex lock(_imageMutex);
	if(_isOpen)
	{
		memcpy(_ikBufCmd,&cmd,sizeof(cmd));
		int len =14;

		_ikEndPointOut->BeginDataTransfer(_ikBufCmd,len);
	}
	//    std::cout<<"sendCmd 2"<<std::endl;
}


/**********************************************************************************************************************************
 *                        获取小图函数
 *  函 数 名：GetPictureTaken
 *  功    能：通过命令控制USB相关操作
 *  说    明：
 *  参    数：buf：端点值
 *  返 回 值：0 操作成功；-1操作失败
 *  创 建 人：刘中昌
 *  创建时间：2013-11-14
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int IkUSBAPI::GetPictureTaken(unsigned char * leftImage,unsigned int &leftLen, unsigned char * rightImage, unsigned int &rightLen)
{
	Exectime etm(__FUNCTION__);
	//    std::cout<<"GetPictureTaken 1"<<std::endl;
	if(_isOpen )
	{
		if(_dealWithUSBTransferBuf->GetPictureTaken(leftImage, leftLen, rightImage, rightLen))
		{
			return E_OK;
		}
		else
		{
			std::cout << "获取图像超时！" << std::endl;
			return E_TIME_OVER;
		}
	}
	else
	{
		std::cout << "获取图像，但设备未打开或获取失败！" << std::endl;
		usleep(0);
	}
	//    std::cout<<"GetPictureTaken 2"<<std::endl;
	return E_AGAIN;
}


/**********************************************************************************************************************************
 *                        循环提交图像数据
 *  函 数 名：PollThreadLoop
 *  功    能：循环提交图像数据
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2013-11-14
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
void *IkUSBAPI::PollThreadLoop(void *arg)
{
	Exectime etm(__FUNCTION__);
	//yichangchuli
	IkUSBAPI* ikUSBAPI = static_cast<IkUSBAPI*>(arg);

	while(true)
	{
		ikUSBAPI->_sendEventwaiter.wait();
		while(s_isExit)
		{
			if(ikUSBAPI->_ikDevice->IkUSBTransferEvent())
			{
				std::cout<<"ikUSBAPI->_ikDevice->IkUSBTransferEvent() 失败"<<std::endl;
				s_isExit = false;
			}

			//        if(ikUSBAPI->_ikCallBack)
			//        {
			//            unsigned int len;
			//            if(ikUSBAPI->_ikCallBack && ikUSBAPI->_dealWithUSBTransferBuf->GetPreview(ikUSBAPI->_ikBuf,len))
			//            {
			//                (ikUSBAPI->_ikCallBack)(0,ikUSBAPI->_ikBuf,len);
			//            }
			//            else if(ikUSBAPI->_ikCallBack)
			//            {
			//                (ikUSBAPI->_ikCallBack)(0,nullptr,0);
			//            }
			//            //            else
			//            //            {
			//            //                sleep(0);
			//            //            }
			//        }
		}
		ikUSBAPI->_waiter.signal();
		std::cout << "PollThreadLoop进程退出！" << std::endl;
	}

	return ((void*)0);
}

/**********************************************************************************************************************************
 *                        获取数据回调函数
 *  函 数 名：TransferBufCallback
 *  功    能：获取数据回调
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：刘中昌
 *  创建时间：2013-11-14
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
void  IkUSBAPI::TransferBufCallBack(TransferStatus  status,unsigned char *transferBuf,
		int length,  void * caller)
{
	Exectime etm(__FUNCTION__);

	//struct timeval tv_NowTime;
	//gettimeofday(&tv_NowTime,NULL);
	//std::cout<<"TransferBufCallBack 开始 秒："<< tv_NowTime.tv_sec<< "微秒："<<tv_NowTime.tv_usec<<std::endl;
	// std::cout<<"----TransferBufCallBack------"<<std::endl;
	IkUSBAPI *ikUSBAPI = static_cast<IkUSBAPI*>(caller);

	if(!ikUSBAPI)
	{
		s_isExit = false;
		std::cout << "ikUSBAPI不存在！" << std::endl;
		return;
	}

	if(TransferCompleted != status)
	{

		if(status == TransferTimedOut)
		{
			std::cout << "usb传输超时！"<< status << std::endl;
			ikUSBAPI->_ikEndPointIn->BeginDataTransfer(ikUSBAPI->_ikBuf,ikUSBAPI->_bufLength);

		}else
		{
			ikUSBAPI->_transferStatus = status;
			//        ikUSBAPI->Close();
			std::cout << "usb传输失败！"<< status << std::endl;
		}
		//        ikUSBAPI->Close();
		//        usleep(3000000);
		//        ikUSBAPI->Init();
		return;
	}
	//添加数据
	//ikUSBAPI->_dealWithUSBTransferBuf->AppendBuf(transferBuf,length);
	//std::cout<<"数据传输获得数据并处理！"<<std::endl;
	ikUSBAPI->_ikBuf = ikUSBAPI->_dealWithUSBTransferBuf->DealBuf(ikUSBAPI->_ikBuf);
	//std::cout<<"继续传输！"<<std::endl;
	if(ikUSBAPI->_isOpen)
	{
		//继续传输
		int err = ikUSBAPI->_ikEndPointIn->BeginDataTransfer(ikUSBAPI->_ikBuf,ikUSBAPI->_bufLength);
		//std::cout<<err<<std::endl;
	}
	//gettimeofday(&tv_NowTime,NULL);
	//std::cout<<"TransferBufCallBack 结束 秒："<< tv_NowTime.tv_sec<< "微秒："<<tv_NowTime.tv_usec<<std::endl;
}


