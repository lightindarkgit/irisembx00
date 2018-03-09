
/******************************************************************************************
 ** 文件名:   IKUSBAPIBASE.CPP
 ×× 主要类:   IkUSBEndPoint、IkControlEndPoint、IkBulkEndPoint、IkUSBDevice等的实现
 **
 ** Copyright (c) 中科虹霸有限公司
 ** 创建人:   刘中昌
 ** 日  期:   2013-11-08
 ** 修改人:
 ** 日  期:
 ** 描  述:   USB通信底层封装类
 *×× 主要模块说明: libusb底层C++封装实现部分
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 *****************************************************************************************/
#include "ikusbapibase.h"
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <tr1/memory>
#include <pthread.h>
#include "iostream"
#include "../Common/Exectime.h"

static struct libusb_context *s_ctx =nullptr;//上下文

/**********************************************************************************************************************************
 *                        USB传输异步回调函数
 *  函 数 名：TransferResultCallBack
 *  功    能：数据传输完成回调函数
 *  说    明：
 *  参    数：
 *  创 建 人：刘中昌
 *  创建时间：2013-11-13
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
void IkUSBEndPoint::TransferResultCallBack(libusb_transfer *transfer)
{
	Exectime etm(__FUNCTION__);
	IkBulkEndPoint *ikEndpoint = static_cast<IkBulkEndPoint*>(transfer->user_data);

	if(!ikEndpoint)
	{
		return;
	}
	//    //判断是否传输成功
	//    if (transfer->status != LIBUSB_TRANSFER_COMPLETED)
	//    {
	//        ikEndpoint->Release();
	//        return;
	//    }
	if(*(ikEndpoint->GetCallBack()))
	{
		(*(ikEndpoint->GetCallBack()))(ikEndpoint->SetTransferStatus(transfer->status),transfer->buffer,transfer->length,ikEndpoint->_caller);
	}
}

/**********************************************************************************************************************************
 *                        USB传输异步回调函数
 *  函 数 名：TransferResultCallBack
 *  功    能：数据传输完成回调函数
 *  说    明：
 *  参    数：
 *  创 建 人：刘中昌
 *  创建时间：2013-11-13
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
void IkUSBEndPoint::TransferResultCallBackOut(libusb_transfer *transfer)
{
	Exectime etm(__FUNCTION__);
	IkBulkEndPoint *ikEndpoint = static_cast<IkBulkEndPoint*>(transfer->user_data);

	if(!ikEndpoint)
	{
		return;
	}
	//判断是否传输成功
	if (transfer->status != LIBUSB_TRANSFER_COMPLETED)
	{
		//        libusb_free_transfer(transfer);
		//        ikEndpoint->Release();
		return;
	}
	if(*(ikEndpoint->GetCallBack()))
	{
		//(*(ikEndpoint->GetCallBack()))(ikEndpoint->SetTransferStatus(transfer->status),transfer->buffer,transfer->length,ikEndpoint->_caller);
	}
}

/**********************************************************************************************************************************
 *                        传输状态转换函数
 *  函 数 名：SetTransferStatus
 *  功    能：传输状态转换
 *  说    明：
 *  参    数：
 *  创 建 人：刘中昌
 *  创建时间：2013-11-27
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
TransferStatus IkUSBEndPoint::SetTransferStatus(libusb_transfer_status status)
{
	Exectime etm(__FUNCTION__);
	switch(status)
	{
		case LIBUSB_TRANSFER_COMPLETED:
			return TransferCompleted;
		case LIBUSB_TRANSFER_ERROR:
			return TransferError;
		case LIBUSB_TRANSFER_TIMED_OUT:
			return TransferTimedOut;
		case LIBUSB_TRANSFER_CANCELLED:
			return TransferCancelled;
		case LIBUSB_TRANSFER_STALL:
			return TransferStall;
		case LIBUSB_TRANSFER_NO_DEVICE:
			return TransferNoDevice;
		case LIBUSB_TRANSFER_OVERFLOW:
			return TransferOverflow;
	}
	return TransferCancelled;
}

/**********************************************************************************************************************************
 *                        初始化函数
 *  函 数 名：IkUSBDevice
 *  功    能：初始化USB的设备
 *  说    明
 *  参    数：Guid 有缺省
 *  创 建 人：刘中昌
 *  创建时间：2013-11-07
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
IkUSBDevice::IkUSBDevice(IkTransferBufCallBackFunc callBack, void *caller, Guid guid)
{
	Exectime etm(__FUNCTION__);
	//pthread_mutex_init(&s_dataCondLock,NULL);
	_guid =guid;
	_ikCallBack = callBack;
	_caller = caller;

	//初始化；
	//s_ctx = nullptr;
	//_devList = nullptr;
	_dev = nullptr;
	_devHandle = nullptr;

	//获取设备上下文
	libusb_init(&s_ctx);
}

/**********************************************************************************************************************************
 *                       析构函数
 *  函 数 名：~IkUSBDevice
 *  功    能：析构USB的设备
 *  说    明：
 *  参    数：
 *  创 建 人：刘中昌
 *  创建时间：2013-11-07
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
IkUSBDevice::~IkUSBDevice()
{
	Exectime etm(__FUNCTION__);
	//如果设备为关闭则先关闭
	// Close();
}

/**********************************************************************************************************************************
 *                       获取USB设备的描述信息函数
 *  函 数 名：GetDeviceDescriptor
 *  功    能：获取USB设备的描述信息
 *  说    明：
 *  参    数：
 *  返    回： 设备描述信息
 *  创 建 人：刘中昌
 *  创建时间：2013-11-07
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
const IKDeviceDescriptor IkUSBDevice::GetDeviceDescriptor()
{
	Exectime etm(__FUNCTION__);
	return _devDescriptor;
}

/**********************************************************************************************************************************
 *                        设备个数函数
 *  函 数 名：DeviceCount
 *  功    能：查看设备个数
 *  说    明：
 *  参    数：
 *  返 回 值：设备数量
 *  创 建 人：刘中昌
 *  创建时间：2013-11-12
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int IkUSBDevice::DeviceCount()
{
	Exectime etm(__FUNCTION__);
	ssize_t error ;
	libusb_device **devList =nullptr;

	if(!devList)
	{
		//通过上下文获取设备列表
		error = libusb_get_device_list(s_ctx, &devList);

		if( error <0)
		{
			libusb_free_device_list(devList, 1);
			return 0;
		}
		libusb_free_device_list(devList, 1);
		return error;
	}



	return  0;
}

/**********************************************************************************************************************************
 *                        USB 打开函数
 *  函 数 名：Open
 *  功    能：打开USB的设备
 *  说    明：
 *  参    数：
 *  返 回 值：错误描述, 异常返回
 *  创 建 人：刘中昌
 *  创建时间：2013-11-07
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int IkUSBDevice::Open()
{
	Exectime etm(__FUNCTION__);
	//通过VID、PID打开设备
	//s_devHandle = libusb_open_device_with_vid_pid(s_ctx,VID,PID);
	//如果已经打开则先关闭
	//    if(IsOpen())
	//    {
	//        Close();
	//    }

	if(!s_ctx)
	{
		if(libusb_init(&s_ctx))
		{
			return E_NO_CONTEXT;
		}
	}
	_devHandle = libusb_open_device_with_vid_pid(s_ctx,VID,PID);

	if(_devHandle)
	{
		_dev =libusb_get_device(_devHandle);
		if(!libusb_claim_interface(_devHandle,0))
		{
			//创建端点 设置回调
			_bulkInEndPt =  new IkBulkEndPoint(_devHandle,DirFromDevice);
			_bulkInEndPt->SetCallBack(_ikCallBack,_caller);
			_bulkOutEndPt = new IkBulkEndPoint(_devHandle,DirToDevice);
			//_bulkOutEndPt->SetCallBack(_ikCallBack,_caller);
			_controlEndPt = new IkControlEndPoint(_devHandle);
			_controlEndPt->SetCallBack(_ikCallBack,_caller);
			return E_OK;
		}
		return E_CLAIM_INTERFACE;
	}
	return E_DEVICE_NO_OPEN;
}

/**********************************************************************************************************************************
 *                        设置传输函数
 *  函 数 名：SetEndPointPtrs
 *  功    能：设置传输
 *  说    明：
 *  参    数：
 *  创 建 人：刘中昌
 *  创建时间：2013-11-15
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
void IkUSBDevice::SetEndPointPtrs()
{
	Exectime etm(__FUNCTION__);
	if (_configs == 0)
	{
		return;
	}

	if (_interfaces == 0)
	{
		return;
	}

	//    for (int j=0; j < _ikUSBConfig._altinterfaces-; j++)
	//    {
	//        if ( //(USBCfgs[CfgNum]->Interfaces[j]->bInterfaceNumber == IntfcNum) &&
	//             (_ikUSBConfig._interfaces[j]->_bAlternateSetting) )
	//        {

	//            IntfcIndex = j;

	//            // Actually change to the alt interface, calling the driver
	//            bSuccess = IoControl(IOCTL_ADAPT_SELECT_INTERFACE, &alt, 1L);

	//            IntfcClass = USBCfgs[CfgNum]->Interfaces[j]->bInterfaceClass;
	//            IntfcSubClass = USBCfgs[CfgNum]->Interfaces[j]->bInterfaceSubClass;;
	//            IntfcProtocol = USBCfgs[CfgNum]->Interfaces[j]->bInterfaceProtocol;;

	//            SetEndPointPtrs();
	//            return bSuccess;
	//        }
	//    }

	//       EndPoints = USBCfgs[CfgNum]->Interfaces[IntfcIndex]->EndPoints;
	//       EndPoints[0] = ControlEndPt;
	//       int eptCount = EndPointCount() - 1;

	//       BulkInEndPt       = NULL;
	//       BulkOutEndPt      = NULL;


	//       for (int i=0; i<eptCount; i++)
	//       {
	//           UCHAR bIn = EndPoints[i+1]->Address & 0x80;
	//           UCHAR attrib = EndPoints[i+1]->Attributes;

	//           EndPoints[i+1]->XferMode = XMODE_DIRECT;

	//           if ((BulkInEndPt == NULL) && (attrib == 2) && bIn) BulkInEndPt = (IkBulkEndPoint *)EndPoints[i+1];
	//           if ((BulkOutEndPt == NULL) && (attrib == 2) && !bIn) BulkOutEndPt = (IkBulkEndPoint *)EndPoints[i+1];

	//       }
}

/**********************************************************************************************************************************
 *                        设在设备描述信息函数
 *  函 数 名：SetDeviceDescriptor
 *  功    能：关设在设备描述信息
 *  说    明：
 *  参    数：
 *  创 建 人：刘中昌
 *  创建时间：2013-11-15
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
void IkUSBDevice::SetDeviceDescriptor(libusb_device_descriptor *desc)
{
	Exectime etm(__FUNCTION__);
	_devDescriptor.CodeDevice = desc->bcdDevice;
	_devDescriptor.CdUSB = desc->bcdUSB;
	_devDescriptor.DescriptorType = desc->bDescriptorType;
	_devDescriptor.DeviceClass= desc->bDeviceClass;
	_devDescriptor.DeviceProtocol = desc->bDeviceProtocol;

	_devDescriptor.DeviceSubClass= desc->bDeviceSubClass;
	_devDescriptor.Length = desc->bLength;
	_devDescriptor.MaxPacketSize0 = desc->bMaxPacketSize0;
	_devDescriptor.NumConfigurations = desc->bNumConfigurations;
	_devDescriptor.ProductID = desc->idProduct;

	_devDescriptor.VendorID = desc->idVendor;
	_devDescriptor.Manufacturer = desc->iManufacturer;
	_devDescriptor.Product = desc->iProduct;
	_devDescriptor.SerialNumber= desc->iSerialNumber;
}

/**********************************************************************************************************************************
 *                        关闭USB设备函数
 *  函 数 名：Close
 *  功    能：关闭USB的设备
 *  说    明：
 *  参    数：
 *  创 建 人：刘中昌
 *  创建时间：2013-11-11
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
void IkUSBDevice::Close()
{
	Exectime etm(__FUNCTION__);
	std::cout<<"libusb_close(_devHandle)开始 " <<std::endl;
	if(_devHandle != NULL)
	{
		libusb_close(_devHandle);
		std::cout<<"libusb_close(_devHandle)结束 " <<std::endl;
		_devHandle = NULL;

		//        if(_bulkInEndPt)
		//        {
		//            delete _bulkInEndPt;
		//        }
		//        if(_bulkOutEndPt)
		//        {
		//            delete _bulkOutEndPt;
		//        }
		//        if(_controlEndPt)
		//        {
		//            delete _controlEndPt;
		//        }
	}
	//    if(_dev)
	//    {
	//        std::cout<<"libusb_unref_device(_dev)开始 " <<std::endl;
	//        libusb_unref_device(_dev);
	//        std::cout<<"libusb_unref_device(_dev)结束 " <<std::endl;
	//        _dev =NULL;
	//    }

	if(s_ctx)
	{
		std::cout<<"libusb_exit(s_ctx)开始 " <<std::endl;
		libusb_exit(s_ctx);
		std::cout<<"libusb_exit(s_ctx)结束 " <<std::endl;
		s_ctx = NULL;
	}
}

/**********************************************************************************************************************************
 *                        重置设备函数
 *  函 数 名：Reset
 *  功    能：重置USB的设备
 *  说    明：
 *  参    数：
 *  返 回 值：错误描述, 异常返回
 *  创 建 人：刘中昌
 *  创建时间：2013-11-11
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
bool IkUSBDevice::Reset()
{
	Exectime etm(__FUNCTION__);
	//重置设备
	int error = libusb_reset_device(_devHandle);

	if ( error < 0)
	{
		return false;
	}
	return true;
}

/**********************************************************************************************************************************
 *                        获取　USB 配置函数函数
 *  函 数 名：GetUSBConfig
 *  功    能：初始化USB的设备
 *  说    明：
 *  参    数：
 *          index       :获取配置信息的索引
 *          ikUSBConfig 配置信息
 *  返 回 值：错误描述, 异常返回
 *  创 建 人：刘中昌
 *  创建时间：2013-11-11
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int IkUSBDevice::GetUSBConfig(int index, IkUSBConfig &ikUSBConfig)
{
	Exectime etm(__FUNCTION__);
	struct libusb_config_descriptor *config;

	if(!_dev)
	{
		return E_NODEV;
	}
	//获取的config信息  ---将其转换成需要的格式传出
	int error = libusb_get_config_descriptor(_dev,index, &config);

	if ( error < 0) {
		return E_ERROR;
	}

	_configs = 1;
	_interfaces = ikUSBConfig.Numinterfaces;

	//配置信息
	ikUSBConfig.ConfigurationValue = config->bConfigurationValue;
	ikUSBConfig.DescriptorType = config->bDescriptorType;
	ikUSBConfig.Length = config->bLength;
	ikUSBConfig.Attributes = config->bmAttributes;

	ikUSBConfig.Numinterfaces = config->bNumInterfaces;
	ikUSBConfig.Configuration = config->iConfiguration;
	ikUSBConfig.MaxPower = config->MaxPower;
	ikUSBConfig.TotalLength = config->wTotalLength;
	try
	{
		//    for(int i=0 ;i<ikUSBConfig.Numinterfaces ;i++)
		//    {
		//        //设置接口信息
		//        ikUSBConfig._interfaces[i]->AltSettings = config->interface[i].altsetting->bAlternateSetting;

		//        ikUSBConfig._interfaces[i]->AlternateSetting = config->interface[i].altsetting->bAlternateSetting;
		//        ikUSBConfig._interfaces[i]->DescriptorType = config->interface[i].altsetting->bDescriptorType;
		//        ikUSBConfig._interfaces[i]->InterfaceClass = config->interface[i].altsetting->bInterfaceClass;
		//        ikUSBConfig._interfaces[i]->InterfaceNumber = config->interface[i].altsetting->bInterfaceNumber;
		//        ikUSBConfig._interfaces[i]->InterfaceProtocol = config->interface[i].altsetting->bInterfaceProtocol;

		//        ikUSBConfig._interfaces[i]->InterfaceSubClass = config->interface[i].altsetting->bInterfaceSubClass;
		//        ikUSBConfig._interfaces[i]->Length = config->interface[i].altsetting->bLength;
		//        ikUSBConfig._interfaces[i]->Interface = config->interface[i].altsetting->iInterface;
		//        ikUSBConfig._interfaces[i]->TotalLength = config->interface[i].altsetting->extra_length;

		//        //设置端点信息
		//        for(int j=0;j<ikUSBConfig._interfaces[i]->NumEndpoints;j++)
		//        {
		//            ikUSBConfig._interfaces[i]->EndPoints[j]->SetEndpointDescriptor(config->interface[i].altsetting->endpoint[j]);
		//        }
		//    }
	}catch(char* str)
	{
		std::cout<<str<<std::endl;
	}

	return E_OK;
}

/**********************************************************************************************************************************
 *                        判断USB是否打开函数
 *  函 数 名：IsOpen
 *  功    能：判断USB是否打开
 *  说    明：
 *  参    数：
 *  返 回 值：错误描述, 异常返回
 *  创 建 人：刘中昌
 *  创建时间：2013-11-11
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
bool IkUSBDevice::IsOpen(void)
{
	Exectime etm(__FUNCTION__);
	if(_devHandle != NULL)
	{
		return true;
	}

	return false;
}

/**********************************************************************************************************************************
 *                        获取块端点函数
 *  函 数 名：GetBulkEndPoint
 *  功    能：通过数据传输方向 获取形影的输入输出块端点
 *  说    明：
 *  参    数：
 *          dirType： DirFromDevice 返回 BulkOutEndPt，DirToDevice 返回 BulkInEndPt
 *  返    回：  块端点
 *  创 建 人：刘中昌
 *  创建时间：2013-11-17
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
IkBulkEndPoint *IkUSBDevice::GetBulkEndPoint(TransferDirType dirType)
{
	Exectime etm(__FUNCTION__);
	if(dirType == DirFromDevice)
	{
		return _bulkInEndPt;
	}

	return _bulkOutEndPt;
}

/**********************************************************************************************************************************
 *                        获取控制端点函数
 *  函 数 名：GetControlEndPoint
 *  功    能：获取控制端点
 *  说    明：
 *  参    数：
 *  返    回：  控制端点
 *  创 建 人：刘中昌
 *  创建时间：2013-11-17
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
IkControlEndPoint *IkUSBDevice::GetControlEndPoint()
{
	Exectime etm(__FUNCTION__);
	return _controlEndPt;
}

/**********************************************************************************************************************************
 *                        IkUSBEndPoint构造函数
 *  函 数 名：IkUSBEndPoint
 *  功    能：端点类构造
 *  说    明：
 *  参    数：
 *  创 建 人：刘中昌
 *  创建时间：2013-11-12
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
IkUSBEndPoint::IkUSBEndPoint(libusb_device_handle *handle, TransferDirType dirType)
{
	Exectime etm(__FUNCTION__);
	if(dirType == DirFromDevice)
	{
		_transferType = EndPointINData;
	}
	else
	{
		_transferType = EndPointOUtData;
	}
	_devHandle = handle;
	_timeOut = 5000;
	_transferMode = ModeBuffered;
	//初始化传输
	_transferBuf = libusb_alloc_transfer(0);
}

/**********************************************************************************************************************************
 *                        IkUSBEndPoint析构函数
 *  函 数 名：~IkUSBEndPoint
 *  功    能：判断USB是否打开
 *  说    明：
 *  参    数：
 *  创 建 人：刘中昌
 *  创建时间：2013-11-12
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
IkUSBEndPoint::~IkUSBEndPoint()
{
	Exectime etm(__FUNCTION__);
	//    if(_devHandle)
	//    {
	//        _devHandle = NULL;
	//    }

	//    if(NULL != _transferBuf)
	//    {
	//        Release();
	//    }
}

/**********************************************************************************************************************************
 *                        终止函数
 *  函 数 名：Release
 *  功    能：释放transfer
 *  说    明：
 *  参    数：
 *  创 建 人：刘中昌
 *  创建时间：2013-11-12
 *  修 改 人：
 *  修改时间：2014-04-23
 *********************************************************************************************************************************/
void IkUSBEndPoint::Release()
{    
	Exectime etm(__FUNCTION__);
	if(_transferBuf)
	{
		libusb_free_transfer(_transferBuf);
	}
	_transferBuf =NULL;
}


/**********************************************************************************************************************************
 *                        传输完成函数
 *  函 数 名：FinishDataTransfer
 *  功    能：传输完成函数
 *  说    明：
 *  参    数：
 *            buf:数据
 *            len:数据长度
 *  返 回 值：错误描述, 异常返回
 *  创 建 人：刘中昌
 *  创建时间：2013-11-12
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
bool IkUSBEndPoint::FinishDataTransfer(unsigned char* buf, int &len)
{
	Exectime etm(__FUNCTION__);

	if(_devHandle || _transferBuf)
	{
		//取消传输
		CancelTransfer();
		return true;
	}
	return false;
}

/**********************************************************************************************************************************
 *                        取消传输函数
 *  函 数 名：CancelTransfer
 *  功    能：取消传输
 *  说    明：
 *  参    数：
 *  创 建 人：刘中昌
 *  创建时间：2013-11-15
 *  修 改 人：
 *  修改时间：2013-04-23
 *********************************************************************************************************************************/
void IkUSBEndPoint::CancelTransfer()
{
	Exectime etm(__FUNCTION__);
	if(_devHandle && _transferBuf)
	{
		//取消传输
		libusb_cancel_transfer(_transferBuf);
	}
}

/**********************************************************************************************************************************
 *                        获取传输数据大小函数
 *  函 数 名：GetTransferSize
 *  功    能：获取传输数据大小
 *  说    明：
 *  参    数：
 *  返 回 值：错误描述, 异常返回
 *  创 建 人：刘中昌
 *  创建时间：2013-11-12
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
unsigned int IkUSBEndPoint::GetTransferSize()
{
	Exectime etm(__FUNCTION__);
	if(_devHandle && _transferBuf)
	{
		return _transferBuf->length;
	}

	return 0;
}

/**********************************************************************************************************************************
 *                        重置函数
 *  函 数 名：Reset
 *  功    能：重置函数 将端点重置为0
 *  说    明：
 *  参    数：
 *  返 回 值：错误描述, 异常返回
 *  创 建 人：刘中昌
 *  创建时间：2013-11-12
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
bool IkUSBEndPoint::Reset()
{
	Exectime etm(__FUNCTION__);
	if(_devHandle)
	{
		//复位指定的endpoint，参数ep 是指bEndpointAddress。
		libusb_clear_halt(_devHandle,_endPointDescriptor.EndpointAddress);
		return true;
	}
	return false;
}

/**********************************************************************************************************************************
 *                        设置传输数据大小函数
 *  函 数 名：SetTransferSize
 *  功    能：设置传输数据大小
 *  说    明：
 *  参    数：
 *           Transfer:传输数据大小
 *  创 建 人：刘中昌
 *  创建时间：2013-11-12
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
void IkUSBEndPoint::SetTransferSize(unsigned int Transfer)
{
	Exectime etm(__FUNCTION__);
	if(_transferBuf)
	{
		libusb_set_iso_packet_lengths(_transferBuf,Transfer);
	}
}

/**********************************************************************************************************************************
 *                       usb传输信号函数
 *  函 数 名：IkUsbTransferEvent
 *  功    能：usb传输信号
 *  说    明：
 *  参    数：
 *  返 回 值：错误描述, 异常返回
 *  创 建 人：刘中昌
 *  创建时间：2013-11-21
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int IkUSBDevice::IkUSBTransferEvent()
{
	Exectime etm(__FUNCTION__);
	if(s_ctx)
	{
		//std::cout<<"---libusb_handle_events ---"<<std::endl;
		return libusb_handle_events(s_ctx);
	}
	return E_NO_CONTEXT;
}
/**********************************************************************************************************************************
 *                        设置端点回调函数
 *  函 数 名：SetCallBack
 *  功    能：设置端点回调函数
 *  说    明：
 *  参    数：
 *  创 建 人：刘中昌
 *  创建时间：2013-11-25
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
void IkUSBEndPoint::SetCallBack(IkTransferBufCallBackFunc callBack, void *caller)
{
	Exectime etm(__FUNCTION__);
	_endPointCallBack = callBack;
	_caller = caller;
}

/**********************************************************************************************************************************
 *                        初始化传输函数
 *  函 数 名：InitCapture
 *  功    能：初始化传输参数
 *  说    明：
 *  参    数：
 *  返 回 值：错误描述, 异常返回
 *  创 建 人：刘中昌
 *  创建时间：2013-11-12
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int IkUSBEndPoint::TransferSubmit()
{
	Exectime etm(__FUNCTION__);
	int error ;

	if(_transferBuf == NULL)
	{
		return E_NO_TRANSFER;
	}
	//提交传输
	error = libusb_submit_transfer(_transferBuf);

	if ( error < 0)
	{
		libusb_release_interface(_devHandle,0);
		return E_NO_TRANSFER ;
	}

	return E_OK;
}

/**********************************************************************************************************************************
 *                        传输等待函数
 *  函 数 名：WaitForTransfer
 *  功    能：传输等待
 *  说    明：
 *  参    数：
 *            timeOut:设置超时时间
 *  返 回 值：是否成功
 *  创 建 人：刘中昌
 *  创建时间：2013-11-12
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
bool IkUSBEndPoint::WaitForTransfer(unsigned long timeOut)
{
	Exectime etm(__FUNCTION__);
	if(!_devHandle)
	{
		return false;
	}
	return false;
}

/**********************************************************************************************************************************
 *                        传输等待函数
 *  函 数 名：SetEndpointDescriptor
 *  功    能：设置端点的信息
 *  说    明：
 *  参    数：
 *            endpoint:端点信息
 *  创 建 人：刘中昌
 *  创建时间：2013-11-17
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
void IkUSBEndPoint::SetEndpointDescriptor(const libusb_endpoint_descriptor endpoint)
{
	Exectime etm(__FUNCTION__);
	_endPointDescriptor.DescriptorType = endpoint.bDescriptorType;
	_endPointDescriptor.EndpointAddress = endpoint.bEndpointAddress;
	_endPointDescriptor.Interval = endpoint.bInterval;
	_endPointDescriptor.Length = endpoint.bLength;

	_endPointDescriptor.Attributes = endpoint.bmAttributes;
	_endPointDescriptor.Refresh = endpoint.bRefresh;
	_endPointDescriptor.SynchAddress = endpoint.bSynchAddress;
	_endPointDescriptor.MaxPacketSize = endpoint.wMaxPacketSize;
}

/**********************************************************************************************************************************
 *                        传输等待函数
 *  函 数 名：GetEndpointDescriptor
 *  功    能：获取端点信息
 *  说    明：
 *  参    数：
 *            timeOut:设置超时时间
 *  返 回 值：端点描述
 *  创 建 人：刘中昌
 *  创建时间：2013-11-17
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
IKEndPointDescriptor IkUSBEndPoint::GetEndPointDescriptor()
{
	Exectime etm(__FUNCTION__);
	return _endPointDescriptor;
}

/**********************************************************************************************************************************
 *                        块传输构造函数
 *  函 数 名：IkBulkEndPoint
 *  功    能：确认块传输
 *  说    明：
 *  参    数：
 *            handle:句柄
 *  返 回 值：错误描述, 异常返回
 *  创 建 人：刘中昌
 *  创建时间：2013-11-11
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
IkBulkEndPoint::IkBulkEndPoint(libusb_device_handle *handle, TransferDirType dirType):IkUSBEndPoint(handle,dirType)
{
	Exectime etm(__FUNCTION__);

}

/**********************************************************************************************************************************
 *                        块传输开始传输函数
 *  函 数 名： BeginDataTransfer
 *  功    能：确认开始传输
 *  说    明：
 *  参    数：
 *            buf:数据
 *            len:数据长度
 *  返 回 值：错误描述, 异常返回
 *  创 建 人：刘中昌
 *  创建时间：2013-11-11
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int IkBulkEndPoint::BeginDataTransfer(unsigned char* buf, int &len)
{
	Exectime etm(__FUNCTION__);
	if(!_devHandle)
	{
		return E_NO_DEVICE_HANDLE;
	}

	//    if(nullptr == _transferBuf)
	//    {
	//      _transferBuf = libusb_alloc_transfer(0);
	//}

	if (!_transferBuf)
	{
		libusb_alloc_transfer(0);
		return E_NO_TRANSFER;
	}

	if(_transferType == EndPointINData)
	{
		//异步--有超时时间设置_transferType
		libusb_fill_bulk_transfer(_transferBuf,
				_devHandle,
				EndPointINData,
				buf,
				TransBufferSize,
				TransferResultCallBack,
				this,
				50 * 1000);
	}
	else
	{
		//异步--有超时时间设置_transferType
		libusb_fill_bulk_transfer(_transferBuf,
				_devHandle,
				EndPointOUtData,
				buf,
				len,
				TransferResultCallBackOut,
				this,
				50*1000);
	}


	//提交传输
	if(TransferSubmit()<0)
	{
		return E_SUBMIT;
	}

	return E_OK;
}

/**********************************************************************************************************************************
 *                        控制传输构造函数
 *  函 数 名：IkControlEndPoint
 *  功    能：构造函数
 *  说    明：
 *  参    数：
 *            handle:句柄
 *            transferTargrtType:传输目标
 *            requestType:问答类型
 *            dirType    ：传输方向
 *  返 回 值：错误描述, 异常返回
 *  创 建 人：刘中昌
 *  创建时间：2013-11-11
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
IkControlEndPoint::IkControlEndPoint(libusb_device_handle *handle,
		ContralTransferTargrtType transferTargrtType,RequestType requestType, TransferDirType dirType):IkUSBEndPoint(handle)
{
	Exectime etm(__FUNCTION__);
	//设置控制传输参数
	_target = transferTargrtType;
	_reqType = requestType;
	_direction = dirType;
}

/**********************************************************************************************************************************
 *                        控制传输读函数
 *  函 数 名：Read
 *  功    能：读数据
 *  说    明：
 *  参    数：
 *            buf:数据
 *            len:数据长度
 *  返 回 值：是否成功
 *  创 建 人：刘中昌
 *  创建时间：2013-11-11
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
bool IkControlEndPoint::Read(unsigned char* buf, int &len)
{
	Exectime etm(__FUNCTION__);
	//_direction = DirFromDevice;
	//    libusb_control_transfer();
	return true;
}

/**********************************************************************************************************************************
 *                        控制传输写函数
 *  函 数 名：Write
 *  功    能：写数据
 *  说    明：
 *  参    数：
 *            buf:数据
 *            len:数据长度
 *  返 回 值：是否成功
 *  创 建 人：刘中昌
 *  创建时间：2013-11-11
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
bool IkControlEndPoint::Write(unsigned char* buf, int &len)
{
	Exectime etm(__FUNCTION__);
	_direction = DirToDevice;

	try
	{

		if(!_devHandle)
		{
			return E_NO_DEVICE_HANDLE;
		}

		if(nullptr == _transferBuf)
		{
			_transferBuf = libusb_alloc_transfer(0);
		}

		if (!_transferBuf)
		{
			return E_NO_TRANSFER;
		}

		//        unsigned char *buf1;

		////        static inline void libusb_fill_control_setup(unsigned char *buffer,
		////            uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
		////            uint16_t wLength)

		//        libusb_fill_control_setup(buf1,,,,,);
		////        setup->bmRequestType = bmRequestType;
		////        setup->bRequest = bRequest;
		////        setup->wValue = libusb_cpu_to_le16(wValue);
		////        setup->wIndex = libusb_cpu_to_le16(wIndex);
		////        setup->wLength = libusb_cpu_to_le16(wLength);
		//        libusb_control_setup controlSetup;
		//        controlSetup.bmRequestType = LIBUSB_ENDPOINT_OUT | LIBUSB_REQUEST_GET_STATUS |LIBUSB_REQUEST_TYPE_STANDARD ;
		//        controlSetup.bRequest;
		//        controlSetup.wIndex;
		//        controlSetup.wLength;
		//        controlSetup.wValue;
		//        //异步--有超时时间设置
		//        libusb_fill_control_transfer(_transferBuf,
		//                                  _devHandle,
		//                                  EndPointData,
		//                                  buf,
		//                                  TransBufferSize,
		//                                  TransferResultCallBack,
		//                                  this,
		//                                  0);


		//提交传输
		if(TransferSubmit()<0)
		{
			return E_SUBMIT;
		}
	}catch(int e)
	{
		std::cout<<"USB 控制写错误！"<<std::endl;
	}
	_direction = DirFromDevice;


	return E_OK;
}


/**********************************************************************************************************************************
 *                        控制传输开始传输函数
 *  函 数 名：BeginDataTransfer
 *  功    能：确认开始传输
 *  说    明：
 *  参    数：
 *  返 回 值：错误描述, 异常返回
 *  创 建 人：刘中昌
 *  创建时间：2013-11-11
 *  修 改 人：
 *  修改时间：
 *********************************************************************************************************************************/
int IkControlEndPoint::BeginDataTransfer(unsigned char* buf, int &len)
{
	Exectime etm(__FUNCTION__);
	//控制传输应该控制 uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
	//    void libusb_fill_control_setup(unsigned char *buffer,
	//        uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex,
	//        uint16_t wLength)
	_transferBuf = libusb_alloc_transfer(0);

	if (!_transferBuf)
	{
		return E_NO_TRANSFER;
	}

	return E_OK;
}

