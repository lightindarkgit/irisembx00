
/******************************************************************************************
** 文件名:   IUSBIMAGE_H
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
#ifndef __IUSBIMAGE_H__
#define __IUSBIMAGE_H__
#include "usbcommon.h"

#include "dealwithusbtransferbuf.h"
#include "ikusbapibase.h"
#include "memory"
#include <string.h>
    /**********************************************************************************************************************************
    *                        回调函数
    *  函 数 名：IkImageLoadeCallbackFunc
    *  功    能：图像取后回调函数
    *  说    明：
    *  参    数：
    *             type :     回调类型 0:一幅图像，1: Error
    *             buf :    要获取的图像数据
    *			 length:   图像大小
    *  返 回 值：错误描述, 异常返回
    *********************************************************************************************************************************/
    typedef  void  (* IkImageLoadeCallBackFunc)
    (IN int                type,        //回调类型
     IN unsigned  char		*buf, 		// 图像数据
     IN int                length);
class IkUSBAPI
{
public:
    IkUSBAPI();
    ~IkUSBAPI();

    /**********************************************************************************************************************************
    *                        初始化函数
    *  函 数 名：Init
    *  功    能：初始化USB的设备
    *  说    明：
    *  参    数：EndPointNum:端点号：高速设备有31个端点，15IN,15OUT；低速3个端点，1IN,1OUT；除了端点0可IN可OUT，其它只能是IN或者OUT
    *  返 回 值：错误描述, 异常返回
    *********************************************************************************************************************************/
    int Init(IN int EndPointNum =0);

    /**********************************************************************************************************************************
    *                        获取图像函数
    *  函 数 名：getCurImage
    *  功    能：获得当前图像
    *  说    明：未获得数据阻塞
    *  参    数：IkImageLoadeCallbackFunc:   回调函数
    *  返 回 值：错误描述, 异常返回  -1超时
    *********************************************************************************************************************************/
    int GetPreview(OUT unsigned char * image,OUT unsigned int& length);
    /**********************************************************************************************************************************
    *                        获取图像函数
    *  函 数 名：getCurImage
    *  功    能：获得当前图像
    *  说    明：未获得数据阻塞
    *  参    数：IkImageLoadeCallbackFunc:   回调函数
    *  返 回 值：错误描述, 异常返回  -1超时
    *********************************************************************************************************************************/
    int GetPictureTaken( unsigned char * leftImage,unsigned int &leftLen, unsigned char * rightImage, unsigned int &rightLen);

    /**********************************************************************************************************************************
     *                        获取图像函数
     *  函 数 名：getCurImage
     *  功    能：获得当前图像
     *  说    明：未获得数据阻塞
     *  参    数：IkImageLoadeCallbackFunc:   回调函数
     *  返 回 值：错误描述, 异常返回  -1超时
     *********************************************************************************************************************************/
    int GetPreview(IN IkImageLoadeCallBackFunc callBack,IN const unsigned long outTime);

    /**********************************************************************************************************************************
    *                        停止USB读取操作函数
    *  函 数 名：Close
    *  功    能：关闭USB的操作包括读取和写入
    *  说    明：关闭USB、清理内存
    *  参    数： 无
    *  返 回 值：true 操作成功；false操作失败
    *********************************************************************************************************************************/
    void Close();

    /**********************************************************************************************************************************
    *                        发送命令函数
    *  函 数 名：SendCmd
    *  功    能：通过命令控制USB相关操作
    *  说    明：
    *  参    数：EndPointNum：端点值  cmd 命令
    *  返 回 值：0 操作成功；-1操作失败
    *********************************************************************************************************************************/
    int SendCmd(Command cmd);


    /**********************************************************************************************************************************
    *                        发送命令函数
    *  函 数 名：SendCmd
    *  功    能：通过命令控制USB相关操作
    *  说    明：
    *  参    数：EndPointNum：端点值  cmd 命令
    *  返 回 值：0 操作成功；-1操作失败
    *********************************************************************************************************************************/
    int SendCmd(IN int EndPointNum, IN char *cmd);
private:

    //上传图片数据线程
    static void *PollThreadLoop(void *arg);

    //内部回调
    static void  TransferBufCallBack(TransferStatus status,
                                     unsigned char *transferBuf,int length, void *caller);

    //friend class USBTest;
private:
    //USB设备的变量定义
    std::shared_ptr<IkUSBDevice> _ikDevice;    //设备
    std::shared_ptr<IkUSBEndPoint> _ikEndPointIn; //端点
    std::shared_ptr<IkUSBEndPoint> _ikEndPointOut; //端点
    IkImageLoadeCallBackFunc _ikCallBack;
    std::shared_ptr<DealWithUSBTransferBuf> _dealWithUSBTransferBuf;
    //数据buf1
    unsigned char *_ikBuf;
    unsigned char *_ikBufCmd;
    int _bufLength;
    bool _isOpen;
    pthread_t _pollThread;

    pthread_mutex_t _imageMutex;

    ThreadWaiter  _waiter;
    ThreadWaiter  _sendEventwaiter;

    //传输状态
    TransferStatus _transferStatus;
};
#endif // IUSBIMAGE_H
