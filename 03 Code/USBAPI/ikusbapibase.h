
#ifndef __IKUSBAPIBASE_H__
#define __IKUSBAPIBASE_H__



/******************************************************************************************
** 文件名:   IKUSBAPIBASE_H
×× 主要类:   IkUSBEndPoint、IkControlEndPoint、IkBulkEndPoint、IkUSBDevice等
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   刘中昌
** 日  期:   2013-11-05
** 修改人:
** 日  期:
** 描  述:   USB通信底层封装类
 *×× 主要模块说明: libusb底层C++封装、为顶层应用提供方便
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
#include "libusb.h"
#include "usbcommon.h"

//Guid结构的定义
typedef struct _Guid {
    unsigned long  Data1;
    unsigned long  Data2;
    unsigned long  Data3;
    unsigned long  Data4;
} Guid;

//传输类型
typedef enum {ModeBuffered, ModeDirect } TransferModeType;
//传输目标
typedef enum {TargetDevice, TargetIntfc, TargetEndPT, TargetOther } ContralTransferTargrtType;
//请求类型
typedef enum {RequestTypeStandard, RequestTypeClass, RequestTypeVendor } RequestType;
//控制传输类型
typedef enum {DirToDevice, DirFromDevice } TransferDirType;

//默认GUID值 {AE18AA60-7F6A-11d4-97DD-00010229B959}
static const Guid IkUsbDrvGuid = {0xae18aa60, 0x7f6a11d4, 0x97dd0001, 0x0229b959};


typedef  void  (* IkTransferBufCallBackFunc)
(TransferStatus  status,
 unsigned  char		*transferBuf, 		//USB传输数据
 int                length,
 void               *caller );//调用者


/**********************************************************************************************************************************
*                       端点描述信息结构
*  类    名：IKDeviceDescriptor
*  功    能：端点描述信息
*  说    明：
*  创 建 人：刘中昌
*  创建时间：2013-11-12
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
struct IKEndPointDescriptor {
    //描述大小
    unsigned char  Length;


    //描述类型::LIBUSB_DT_ENDPOINT
    unsigned char  DescriptorType;

    //端点的地址描述了一个描述符
    unsigned char  EndpointAddress;

    //适用于端点时使用bConfigurationValue配置
    unsigned char  Attributes;

    //最大数据包大小这个端点能够发送/接收
    unsigned short MaxPacketSize;

    //轮询间隔数据传输端点
    unsigned char  Interval;

    //对音频设备只有:速率同步反馈是提供。
    unsigned char  Refresh;

    //对音频设备只有:地址如果同步端点
    unsigned char  SynchAddress;

};

/**********************************************************************************************************************************
*                        设备描述信息结构
*  类    名：IKDeviceDescriptor
*  功    能：设备描述信息
*  说    明：
*  创 建 人：刘中昌
*  创建时间：2013-11-12
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
struct IKDeviceDescriptor {

    //描述大小
    unsigned char  Length;

    //描述类型::LIBUSB_DT_DEVICE
    unsigned char  DescriptorType;

    //USB规范版本号码在二进制编码十进制
    unsigned short CdUSB;

    // libusb_class_code值
    unsigned char  DeviceClass;

    //同上
    unsigned char  DeviceSubClass;

    //同上
    unsigned char  DeviceProtocol;

    //最大数据包大小为端点0
    unsigned char  MaxPacketSize0;

    //VID
    unsigned short VendorID;

    //PID
    unsigned short ProductID;

    //设备版本号码在二进制编码十进制
    unsigned short CodeDevice;

    //字符串描述符描述的制造商 索引
    unsigned char  Manufacturer;

    //字符串描述符描述产品 索引
    unsigned char  Product;

    //字符串描述符包含设备序列号 索引
    unsigned char  SerialNumber;

    //可能的配置的数量
    unsigned char  NumConfigurations;
};

/**********************************************************************************************************************************
*                        端点传输类
*  类    名：IkUSBEndPoint
*  功    能：数据传输
*  说    明：
*  创 建 人：刘中昌
*  创建时间：2013-11-12
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
class IkUSBEndPoint
{
public:

    IkUSBEndPoint(libusb_device_handle *handle=nullptr);
    ~IkUSBEndPoint();

    //设置回调
    void SetCallBack(IkTransferBufCallBackFunc callBack = nullptr,void * caller=nullptr);

    //传输准备
    virtual int    BeginDataTransfer(unsigned char* buf, int &len)= 0;

    //传输完成
    virtual bool   FinishDataTransfer(unsigned char* buf, int &len);
    //传输等待
    bool           WaitForTransfer(unsigned long timeOut = 0);

    //获取传输大小
    unsigned int   GetTransferSize(void);
    //设置传输大小
    void           SetTransferSize(unsigned int Transfer);
    //重置传输
    bool Reset();
    //取消传输
    void Abort();

    inline IkTransferBufCallBackFunc GetCallBack()
    {
        return _endPointCallBack;
    }

    //获取端点信息
    IKEndPointDescriptor GetEndPointDescriptor();
    //设置端点信息
    void SetEndpointDescriptor(const libusb_endpoint_descriptor endPoint);

    //传输提交
    int TransferSubmit();
protected:

    //释放传输
    void ReleaseTransfer();

    //回调函数
    static void TransferResultCallBack(libusb_transfer *transfer);

    //传输状态重新定义
    TransferStatus SetTransferStatus(libusb_transfer_status status);


protected:
    //设备句柄
    struct	libusb_device_handle *_devHandle;
    //传输数据指针
    struct	libusb_transfer      *_transferBuf;

    //端点描述
    IKEndPointDescriptor  _endPointDescriptor;
    //超时时间
    unsigned long         _timeOut;
    //传输类型
    TransferModeType      _transferMode;
    //回掉函数指针
    IkTransferBufCallBackFunc _endPointCallBack;
    //调用者
    void * _caller;
};

/**********************************************************************************************************************************
*                        控制传输端点类
*  类    名：IkControlEndPoint
*  功    能：控制传输
*  说    明：
*  创 建 人：刘中昌
*  创建时间：2013-11-12
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
class IkControlEndPoint : public IkUSBEndPoint
{
public:
    IkControlEndPoint(libusb_device_handle *handle=nullptr,ContralTransferTargrtType transferTargrtType = TargetDevice,
                      RequestType requestType = RequestTypeStandard, TransferDirType dirType = DirFromDevice);

    bool Read(unsigned char* buf, int &len);
    bool Write(unsigned char* buf, int &len);
    int BeginDataTransfer(unsigned char* buf, int &len);

private:

    //libusb_control_setup
    ContralTransferTargrtType _target;
    RequestType               _reqType;
    TransferDirType    _direction;

    unsigned char    _reqCode;
    unsigned long    _value;
    unsigned long    _index;

};

/**********************************************************************************************************************************
*                        批量传输端点类
*  类    名：IkBulkEndPoint
*  功    能：批量传输
*  说    明：
*  创 建 人：刘中昌
*  创建时间：2013-11-12
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
class IkBulkEndPoint : public IkUSBEndPoint
{
public:
    IkBulkEndPoint(libusb_device_handle *handle=nullptr);

    int BeginDataTransfer(unsigned char* buf, int &len);
};

/**********************************************************************************************************************************
*                        接口信息结构
*  类    名：IkUSBinterface
*  功    能：USB 设备配置接口相关信息
*  说    明：
*  创 建 人：刘中昌
*  创建时间：2013-11-12
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
struct IkUSBinterface
{
public:
    //端点
    IkUSBEndPoint *EndPoints[MaxEndPoints];  /* Holds pointers to all the interface's endpoints,
                                            plus a pointer to the Control endpoint zero */
    //描述的大小
    unsigned char   Length;
    //描述类型libusb_descriptor_type::LIBUSB_DT_INTERFACE LIBUSB_DT_INTERFACE
    unsigned char   DescriptorType;
    //该接口号
    unsigned char   InterfaceNumber;
    //使用的值来选择这个交替设置这个接口
    unsigned char   AlternateSetting;
    //使用该接口的端点数
    unsigned char   NumEndpoints;

    //该接口的源码类
    unsigned char   InterfaceClass;
    //该接口的源码类子类
    unsigned char   InterfaceSubClass;
    //bInterfaceClass 和 bInterfaceSubClass值
    unsigned char   InterfaceProtocol;
    //索引的字符串描述符描述这个接口
    unsigned char   Interface;

    //交替的接口个数
    unsigned char   AltSettings;
    //总长度
    unsigned short  TotalLength;
};


/**********************************************************************************************************************************
*                        设备配置信息结构
*  类    名：IkUSBConfig
*  功    能：USB 设备配置相关信息
*  说    明：
*  创 建 人：刘中昌
*  创建时间：2013-11-12
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
struct IkUSBConfig
{
public:

    //接口数组
    IkUSBinterface *_interfaces[MaxInterfaces];

    //配置长度
    unsigned char   Length;
    //配置描述类型
    unsigned char   DescriptorType;
    //配置总长度
    unsigned short  TotalLength;
    //接口个数
    unsigned char   Numinterfaces;

    //标志符
    unsigned char   ConfigurationValue;
    //配置索引
    unsigned char   Configuration;
    //属性
    unsigned char   Attributes;
    //最大功耗的USB设备
    unsigned char   MaxPower;
    //交换接口
    unsigned char Altinterfaces;
};


/**********************************************************************************************************************************
*                        设备类
*  类    名：IkUSBDevice
*  功    能：USB 设备类可以通过该类打开、获取设备相关信息
*  说    明：
*  创 建 人：刘中昌
*  创建时间：2013-11-12
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
class IkUSBDevice
{
public:

    //构造函数 传入参数回调函数和GUID
    IkUSBDevice(IkTransferBufCallBackFunc callBack = nullptr,void *caller =nullptr, Guid guid = IkUsbDrvGuid);
    ~IkUSBDevice();

    //获取设备个数
    int DeviceCount();

    //打开设备
    int   Open();

    //关闭设备
    void Close();

    //重置设备
    bool   Reset();

    //是否成功打开设备
    bool   IsOpen(void);

    //获取配置信息
    int GetUSBConfig(int index, IkUSBConfig &ikUSBConfig);

    //获取设备信息
    const IKDeviceDescriptor GetDeviceDescriptor();

    //获取块端点
    IkBulkEndPoint* GetBulkEndPoint(TransferDirType dirType);

    //获取控制端点
    IkControlEndPoint* GetControlEndPoint();

    //传输事件
    int IkUSBTransferEvent();

    friend class IkUSBEndPoint;

private:
    //设在设备描述信息
    void SetDeviceDescriptor(libusb_device_descriptor *desc);

    //设置传输
    void SetEndPointPtrs();

    //int                SetConfig(int cfg);
    //unsigned char       GetConfigNum(void)     { return _cfgNum; }
    //unsigned char       GetInterfaceNum(void)  { return _intfceNum; }

private:

    IkTransferBufCallBackFunc _ikCallBack;
    void * _caller;

    //端点类
    IkBulkEndPoint         *_bulkInEndPt ;
    IkBulkEndPoint         *_bulkOutEndPt;
    IkControlEndPoint      *_controlEndPt;

    //给设备设定的GUID
    Guid                _guid;
    //设备描述
    IKDeviceDescriptor  _devDescriptor;
    //设备配置
    IkUSBConfig         _ikUSBConfig;

    //接口地址
    unsigned char       _interfaces;
    //配置地址
    unsigned char       _configs;

    //上下文
    struct	libusb_context 	     *_ctx;
    //设备链表
//    struct	libusb_device 	     **_devList;
    //设备
    struct	libusb_device 	     *_dev;
    //设备句柄
    struct	libusb_device_handle *_devHandle;

    //    unsigned char       _devNum;
    //    unsigned char       _cfgNum;
    //    unsigned char       _intfceNum;   /* The current selected interface's binterfaceNumber */
    //    unsigned char       _intfceIndex; /* The entry in the Config's interfaces table matching to intfcNum and AltSetting */
};

#endif
