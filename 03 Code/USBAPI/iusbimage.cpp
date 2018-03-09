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

static bool s_isExit = true;

IkUSBAPI::IkUSBAPI()
{
    _ikBuf = new unsigned char[TransBufferSize];
    _isOpen = false;
}

IkUSBAPI::~IkUSBAPI()
{
    Close();
//    pthread_cancel(tid);             //关于pthread_cancel，有一点额外的说明，它是从外部终止子线程，子线程会在最近的取消点，退出线程，而在我们的代码里，最近的取消点肯定就是pthread_cond_wait()了。关于取消点的信息，有兴趣可以google,这里不多说了
//    pthread_join(tid, NULL);
    delete[] _ikBuf;
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
    SimpleLockForMutex lock(_imageMutex);
    if(_isOpen)
    {
        return E_OK;
    }
    //打开USB设备
    IkUSBDevice *dev = new IkUSBDevice(TransferBufCallBack,(void *)this);
    _ikCallBack = nullptr;
    _ikDevice.reset(dev);
    if(E_OK ==_ikDevice->Open())
    {

        IkBulkEndPoint *endPoint = _ikDevice->GetBulkEndPoint(DirFromDevice);
        if(!endPoint)
        {
           return E_NO_ENDPOINT;
        }
        //获取端点
        _ikEndPoint.reset(endPoint);

        _dealWithUSBTransferBuf.reset(new DealWithUSBTransferBuf());

        //开始传输
        _ikEndPoint->BeginDataTransfer(_ikBuf,_bufLength);
        _isOpen =true;
        //启动异步串口数据接收线程
        std::cout << "USB设备初始化成功！" << std::endl;
        return pthread_create(&_pollThread, NULL, PollThreadLoop, (void *)this);
    }else
    {
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
int IkUSBAPI::GetImage(OUT unsigned char * image,OUT unsigned int& length)
{
    SimpleLockForMutex lock(_imageMutex);

    if(_isOpen && _dealWithUSBTransferBuf->GetImage(image,length))
    {
        return E_OK;
    }
     std::cout << "获取图像，但设备未打开或获取失败！" << std::endl;
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
int IkUSBAPI::GetImage(IN IkImageLoadeCallBackFunc callBack,IN const unsigned long outTime)
{
    if(_ikDevice->IsOpen())
    {
        _ikCallBack = callBack;
        std::cout << "设备已打开！" << std::endl;
        return E_OK;
    }
    std::cout << "获取图像，但设备未打开！" << std::endl;
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
    SimpleLockForMutex lock(_imageMutex);
    if(_isOpen)
    {
        _ikDevice->Close();
        _isOpen =false;
        s_isExit = 1;
        pthread_cancel(_pollThread);             //关于pthread_cancel，有一点额外的说明，它是从外部终止子线程，子线程会在最近的取消点，退出线程，而在我们的代码里，最近的取消点肯定就是pthread_cond_wait()了。关于取消点的信息，有兴趣可以google,这里不多说了
        pthread_join(_pollThread, nullptr);
        std::cout << "USB设备关闭！" << std::endl;
    }

    //_isOpen =false;
   // std::cout << "USB设备关闭！" << std::endl;
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
    //yichangchuli
    IkUSBAPI* ikUSBAPI = static_cast<IkUSBAPI*>(arg);

    while(s_isExit)
    {
        if(ikUSBAPI->_ikDevice->IkUSBTransferEvent())
        {
            s_isExit = false;
        }
        if(ikUSBAPI->_ikCallBack)
        {
            unsigned int len;
            if(ikUSBAPI->_ikCallBack && ikUSBAPI->_dealWithUSBTransferBuf->GetImage(ikUSBAPI->_ikBuf,len))
            {
                (ikUSBAPI->_ikCallBack)(0,ikUSBAPI->_ikBuf,len);
            }
            else if(ikUSBAPI->_ikCallBack)
            {
                (ikUSBAPI->_ikCallBack)(0,nullptr,0);
            }
        }
    }
     std::cout << "PollThreadLoop进程退出！" << std::endl;
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
    IkUSBAPI *ikUSBAPI = static_cast<IkUSBAPI*>(caller);

    if(!ikUSBAPI)
    {
        s_isExit = false;
         std::cout << "ikUSBAPI不存在！" << std::endl;
        return;
    }

    if(TransferCompleted != status)
    {
        ikUSBAPI->_transferStatus = status;
        s_isExit = false;
         std::cout << "usb传输未完成！" << std::endl;
        return;
    }
    //添加数据
    //ikUSBAPI->_dealWithUSBTransferBuf->AppendBuf(transferBuf,length);
    ikUSBAPI->_ikBuf = ikUSBAPI->_dealWithUSBTransferBuf->DealBuf(ikUSBAPI->_ikBuf);

    //继续传输
    ikUSBAPI->_ikEndPoint->BeginDataTransfer(ikUSBAPI->_ikBuf,ikUSBAPI->_bufLength);

}


