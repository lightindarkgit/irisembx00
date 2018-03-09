/*****************************************************************************
 ** 文 件 名： iknetwork.cpp
 ** 主 要 类： IkNetWork
 **
 ** Copyright (c) 中科虹霸有限公司
 **
 ** 创 建 人：刘中昌
 ** 创建时间：20014-03-05
 **
 ** 修 改 人：
 ** 修改时间：
 ** 描  述:    网络通信业务处理模块
 ** 主要模块说明: 对网络数据组帧、发送相应信号
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 *****************************************************************************/


#include "iknetwork.h"
#include "QDateTime"
#include "CodecNetData.h"
#include "CodecResponse.h"
#include "EncodeNetData.h"

#include "DecodeNetData.h"

#include "iksocket.h"
#include "sysnetinterface.h"
#include "../Common/Logger.h"
#include "../Common/Exectime.h"
#include "../IrisAppLib/IrisManager/irisManager.h"
#include "../IrisAppLib/IrisManager/singleControl.h"
#include "interaction.h"

extern int isDevInfoSend ;
int isDevInfoSend = 0;


bool    g_bsendreconect = false;

IkNetWork::IkNetWork( QObject *parent) :
	QObject(parent),_curState(0),_addLen(0),_dataLen(0),
	_oldALiveCount(0),_aLiveCount(0),_isNetOK(true)
{
    //_bufFrame = new char[g_maxDataSize];
	memset(_bufFrame,0,g_maxDataSize);
	memset(_recBuf,0,g_maxAckSize);
	memset(_recAckBuf,0,g_maxAckSize);
    //_recDBBuf = new char[g_maxDataSize];
	memset(_recDBBuf,0,g_maxDataSize);
	this->startTimer(g_reConnTime);
	InitHandlerMap();
	pthread_create(&_pthreadID, 0, ThreadDealWith, (void*)this);
}

IkNetWork::~IkNetWork()
{
   // delete []_bufFrame; //20170301dj
   // delete []_recDBBuf;
	
	pthread_cancel(_pthreadID);
	pthread_join(_pthreadID, nullptr);
}
/*****************************************************************************
 *                        处理数据线程
 *  函 数 名：ThreadDealWith
 *  功    能：处理通信数据
 *  说    明：如果网络传输数据速度大于处理速度则需要将数据另起线程处理。
 由于测试向数据库中存入一条数据的速度为19毫秒，下发的数据量在1M之内 因此处理速度远小于网络速度
 因此该线程暂时不启动。
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-03-08
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void *IkNetWork::ThreadDealWith(void *arg)
{
	
	//    IkNetWork* dealBuf = static_cast<IkNetWork*>(arg);
	//    while(true)
	//    {
	//        dealBuf->_threadWaiter.wait();
	//        if(dealBuf->_curState == 0)
	//        {
	//            dealBuf->FindHeader(dealBuf->buf,dealBuf->len);
	//        }
	//        else
	//        {
	//            dealBuf->CopyBuf(dealBuf->buf,dealBuf->len);
	//        }
	//    }
}

/*****************************************************************************
 *                        处理通信数据
 *  函 数 名：DealwithNetWork
 *  功    能：处理通信数据
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-03-07
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IkNetWork::DealwithNetWork(char *buf,int len)
{
	

	if(_curState == 0)
	{
		FindHeader(buf,len);
	}
	else
	{
		CopyBuf(buf,len);
	}
	//_threadWaiter.signal();
}

/*****************************************************************************
 *                        查找帧头
 *  函 数 名：FindHeader
 *  功    能：查找帧头
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-03-07
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IkNetWork::FindHeader(char *buf,int len)
{
	
	char *originalBuf = buf;

	int index =0;
	for(;index<len - 1;index++)
	{
		if(originalBuf[1] == 0x53 && originalBuf[0] == 0x53)
		{
			memmove(&_dataLen,&originalBuf[6],4);
			break;
		}
		originalBuf++;
	}

	if(index > len - 10)
	{
		return;
	}

	CopyBuf(originalBuf,len - index);
}

/*****************************************************************************
 *                        拷贝数据
 *  函 数 名： CopyBuf
 *  功    能： 拷贝数据 并调用解析
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-03-07
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IkNetWork::CopyBuf(char *buf,int len)
{
    if(nullptr == buf)
    {
        LOG_ERROR("buf is nullptr,len=%d",len);
    }
	
	int surpluscnt = 0;
    //拆包
	if(_addLen +len >= _dataLen + 12)
	{
		surpluscnt = _dataLen + 12 - _addLen;		// 本包剩余的数据

        LOG_INFO("step1 _bufFrame=%p,buf=%p,_addLen = %d,surpluscnt = %d",_bufFrame,buf,_addLen,surpluscnt);
		memmove(_bufFrame + _addLen,buf,surpluscnt);
		surpluscnt = len - surpluscnt;				// 本次接收的buf有多少没有处理
		unsigned short check;
        if(_dataLen +10>g_maxDataSize)
        {
            LOG_ERROR("_bufFrame's length is no enough");
            return;
        }

        LOG_INFO("step2 _bufFrame=%p,",&_bufFrame[_dataLen +10]);
		memmove(&check,&_bufFrame[_dataLen +10],2);

		if(CheckData::IsCorrect(_bufFrame,_dataLen+10,check))
		{
			std::cout<<"校验成功！"<<std::endl;
            LOG_DEBUG("校验成功！");
		}
		else
		{
			std::cout<<"校验失败！"<<std::endl;
			LOG_ERROR("校验失败, CMD: [%04X]",*((short*)(_bufFrame + 2)));

            // Add by liushu
            char szhex[128] = {0};
            for(int i = 0; i < 12; i ++)
            {
                sprintf(szhex + i * 3,"%02X ",(unsigned char)_bufFrame[i]);
            }

            LOG_INFO("szhex=%s",szhex);

            // 如果数据解析失败，则断开链接并进行重连
            _addLen   = 0;
            _curState = 0;

            _isNetOK = false;
            emit sigNetCut();
            return;
		}
		unsigned short req =0;

		memmove(&req,&_bufFrame[2],2);

		//        ExplainComd(req);
		//采用函数指针方式调用解析函数 --0522
		this->Display(10,this->_bufFrame);//fjf 2014-08-18 display
		std::cout<<"enter ProdessControl..."<<std::endl;      
		ProdessControl(req,req);
		std::cout<<"exit ProdessControl..."<<std::endl;

		_addLen   = 0;
		_curState = 0;
		//FindHeader(buf + _dataLen,len - _dataLen);

	}
	else
	{
        LOG_INFO("step7 ");
		memmove(_bufFrame + _addLen,buf,len);
        LOG_INFO("step8 ");
		_addLen += len;
		_curState = 1;
	}

	LOG_DEBUG("dealdata/recvdata:[%d/%d]",len - surpluscnt,len);

	if(surpluscnt > 0)	// 如果本次还有剩余数据未处理,继续处理
	{
		LOG_DEBUG("deal surplus data:[%d]",surpluscnt);
		DealwithNetWork(buf + len - surpluscnt,surpluscnt);
        LOG_INFO("step10 ");
	}
}

/*****************************************************************************
 *                        初始化函数指针map
 *  函 数 名： InitHandlerMap
 *  功    能： 初始化函数指针
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IkNetWork::InitHandlerMap()
{
	
	HandlerMap[REQ_KEEP_ALIVE] = std::bind(&IkNetWork::KeepALive,this,std::placeholders::_1);//心跳
	HandlerMap[REQ_SYNC_TOKEN] = std::bind(&IkNetWork::Token,this,std::placeholders::_1);//令牌

	HandlerMap[REQ_ADD_PERSON_INFO] = std::bind(&IkNetWork::AddPerson,this,std::placeholders::_1);//增加人员信息
	HandlerMap[REQ_UPDATE_PERSON_INFO] = std::bind(&IkNetWork::UpdPerson,this,std::placeholders::_1);//修改人员信息
	HandlerMap[REQ_DELETE_PERSON_INFO] = std::bind(&IkNetWork::DelPerson,this,std::placeholders::_1);//删除人员信息
	HandlerMap[REQ_GET_PERSON_LIST] = std::bind(&IkNetWork::GetPersonList,this,std::placeholders::_1);//获得人员列表

	HandlerMap[REQ_ADD_PERSON_PHOTO] = std::bind(&IkNetWork::AddPersonPhoto,this,std::placeholders::_1);//增加人员照片
	HandlerMap[REQ_UPDATE_PERSON_PHOTO] = std::bind(&IkNetWork::UpdPersonPhoto,this,std::placeholders::_1);//更新人员照片
	HandlerMap[REQ_DELETE_PERSON_PHOTO] = std::bind(&IkNetWork::DelPersonPhoto,this,std::placeholders::_1);//删除人员照片

	HandlerMap[REQ_ADD_PERSON_IRIS] = std::bind(&IkNetWork::AddPersonIris,this,std::placeholders::_1);//增加虹膜特征从服务器到客户端
	HandlerMap[REQ_DELETE_PERSON_IRIS] = std::bind(&IkNetWork::DelPersonIris,this,std::placeholders::_1);//删除虹膜特征从服务器到客户端

	HandlerMap[REQ_GET_RECOG_RECORD] = std::bind(&IkNetWork::GetRecogRecordCondition,this,std::placeholders::_1);//获得指定的识别记录


	HandlerMap[REQ_SRVCTRL_SHUTDOWN] = std::bind(&IkNetWork::SrvctrlShutDown,this,std::placeholders::_1);//关机
	HandlerMap[REQ_SRVCTRL_RESET] = std::bind(&IkNetWork::SrvctrlReset,this,std::placeholders::_1);//重启
	HandlerMap[REQ_SRVCTRL_ADJUST_TIME] = std::bind(&IkNetWork::SrvCtrlAdjustTime,this,std::placeholders::_1);//调整客户端时间
	HandlerMap[REQ_SRVCTRL_ADJUST_DEVIP] = std::bind(&IkNetWork::SrvCtrlAdjustDevIP,this,std::placeholders::_1);//修改设备IP地址
	HandlerMap[REQ_SRVCTRL_ADJUST_SRVIP] = std::bind(&IkNetWork::SrvCtrlAdjustSrvIP,this,std::placeholders::_1);//修改设备对应的服务器IP地址

	//查询设备信息
	HandlerMap[REQ_GET_DEV_INFO] = std::bind(&IkNetWork::GetDevInfo,this,std::placeholders::_1);

	//获得服务器时间
	HandlerMap[ACK_GET_SRV_TIME] = std::bind(&IkNetWork::AckGetSrvTime,this,std::placeholders::_1);
	//上传虹膜注册数据从客户端到服务器
	HandlerMap[ACK_UPLOAD_PERSON_IRIS] = std::bind(&IkNetWork::AckUploadPersonIris,this,std::placeholders::_1);
	//上传识别记录从客户端到服务器
	HandlerMap[ACK_UPLOAD_RECOG_RECORD] = std::bind(&IkNetWork::AckUploadRecog,this,std::placeholders::_1);
	//实时发送识别记录从客户端到服务器
	HandlerMap[ACK_SEND_RECOG_RECORD] = std::bind(&IkNetWork::AckSendRecog,this,std::placeholders::_1);

	// 普通人员相关
	HandlerMap[ACK_ADD_PERSON_INFO] = std::bind(&IkNetWork::AckAddPersonInfo,this,std::placeholders::_1);// 增加人员信息
	HandlerMap[ACK_UPDATE_PERSON_INFO] = std::bind(&IkNetWork::AckUpdPersonInfo,this,std::placeholders::_1);// 修改人员信息
	HandlerMap[ACK_DELETE_PERSON_INFO] = std::bind(&IkNetWork::AckDelPersonInfo,this,std::placeholders::_1);// 删除人员信息
	HandlerMap[ACK_ADD_PERSON_PHOTO] = std::bind(&IkNetWork::AckAddPhoto,this,std::placeholders::_1); // 增加人员照片
	HandlerMap[ACK_UPDATE_PERSON_PHOTO] = std::bind(&IkNetWork::AckUpdPhoto,this,std::placeholders::_1); // 更新人员照片
	HandlerMap[ACK_DELETE_PERSON_PHOTO] = std::bind(&IkNetWork::AckDelPhoto,this,std::placeholders::_1);// 删除人员照片

	HandlerMap[ACK_ADD_PERSON_IRIS] = std::bind(&IkNetWork::AckAddPersonIris,this,std::placeholders::_1); // 更新人员照片
	HandlerMap[ACK_DELETE_PERSON_IRIS] = std::bind(&IkNetWork::AckDelPersonIris,this,std::placeholders::_1);// 删除人员照片


}

void IkNetWork::SetToken(int personToken,int irisToken,int photoToken)
{
	
	_currentPersonToken = personToken;
	_currentIrisToken = irisToken;
	_currentPhotoToken = photoToken;
}

unsigned short IkNetWork::AckGetSrvTime(unsigned short comd)
{
	
	TimeString time;
	NormalAck ack;
	DecodeResponse::GetSrvTime(_bufFrame,ack,time);
	if(0 != strlen(time.TimeStr))
	{
		SysNetInterface::SetSysTime(std::string(time.TimeStr));
	}

	return comd;
}

unsigned short IkNetWork::AckUploadPersonIris(unsigned short comd)
{
	
	NormalAck ack;
	DecodeResponse::UploadPersonIris(_bufFrame,ack);
	if(ack.ErrorCode == 0)
	{
		//传输成功！
		emit sigAck(true,comd);
	}
	else
	{
		//失败！
		emit sigAck(false,comd);
		LOG_ERROR("emit sigAck failed");
	}

	return comd;
}

unsigned short IkNetWork::AckUploadRecog(unsigned short comd)
{
	
	NormalAck ack;
	DecodeResponse::UploadRecogRecord(_bufFrame,ack);
	if(ack.ErrorCode == 0)
	{
		//传输成功！
		emit sigAck(true,comd);
	}
	else
	{
		//失败！
		emit sigAck(false,comd);
		LOG_ERROR("emit sigAck failed");
	}

	return comd;
}

unsigned short IkNetWork::AckSendRecog(unsigned short comd)
{
	
	NormalAck ack;
	DecodeResponse::SendRecogRecord(_bufFrame,ack);
	if(ack.ErrorCode == 0)
	{
		//传输成功！
		emit sigAck(true,comd);
	}
	else
	{
		//失败！
		emit sigAck(false,comd);
		LOG_ERROR("emit sigAck failed");
	}

	return comd;
}

unsigned short IkNetWork::AckAddPersonInfo(unsigned short comd)
{
	
	NormalAck ack;
	DecodeResponse::AddPerson(_bufFrame,ack);
	if(ack.ErrorCode == 0)
	{
		//传输成功！
		emit sigAck(true,comd);
	}
	else
	{
		//失败！
		emit sigAck(false,comd);
		LOG_ERROR("emit sigAck failed");
	}

	return comd;
}

unsigned short IkNetWork::AckUpdPersonInfo(unsigned short comd)
{
	
	NormalAck ack;
	DecodeResponse::UpdatePerson(_bufFrame,ack);
	if(ack.ErrorCode == 0)
	{
		//传输成功！
		emit sigAck(true,comd);
	}
	else
	{
		//失败！
		emit sigAck(false,comd);
		LOG_ERROR("emit sigAck failed");
	}

	return comd;
}

unsigned short IkNetWork::AckDelPersonInfo(unsigned short comd)
{
	
	NormalAck ack;
	DecodeResponse::DeletePerson(_bufFrame,ack);
	if(ack.ErrorCode == 0)
	{
		//传输成功！
		emit sigAck(true,comd);
	}
	else
	{
		//失败！
		emit sigAck(false,comd);
		LOG_ERROR("emit sigAck failed");
	}

	return comd;
}


unsigned short IkNetWork::AckAddPhoto(unsigned short comd)
{
	
	NormalAck ack;
	DecodeResponse::AddPersonPhoto(_bufFrame,ack);
	if(ack.ErrorCode == 0)
	{
		//传输成功！
		emit sigAck(true,comd);
	}
	else
	{
		//失败！
		emit sigAck(false,comd);
		LOG_ERROR("emit sigAck failed");
	}

	return comd;
}

unsigned short IkNetWork::AckUpdPhoto(unsigned short comd)
{
	
	NormalAck ack;
	DecodeResponse::UpdatePersonPhoto(_bufFrame,ack);
	if(ack.ErrorCode == 0)
	{
		//传输成功！
		emit sigAck(true,comd);
	}
	else
	{
		//失败！
		emit sigAck(false,comd);
		LOG_ERROR("emit sigAck failed");
	}

	return comd;
}

unsigned short IkNetWork::AckDelPhoto(unsigned short comd)
{
	
	NormalAck ack;
	DecodeResponse::DeletePersonPhoto(_bufFrame,ack);
	if(ack.ErrorCode == 0)
	{
		//传输成功！
		emit sigAck(true,comd);
	}
	else
	{
		//失败！
		emit sigAck(false,comd);
		LOG_ERROR("emit sigAck failed");
	}

	return comd;
}

unsigned short IkNetWork::AckAddPersonIris(unsigned short comd)
{
	
	NormalAck ack;
	DecodeResponse::AddPersonIris(_bufFrame,ack);
	if(ack.ErrorCode == 0)
	{
		//传输成功！
		emit sigAck(true,comd);
	}
	else
	{
		//失败！
		emit sigAck(false,comd);
		LOG_ERROR("emit sigAck failed");
	}

	return comd;
}

unsigned short IkNetWork::AckDelPersonIris(unsigned short comd)
{
	
	NormalAck ack;
	DecodeResponse::DeletePersonIris(_bufFrame,ack);
	if(ack.ErrorCode == 0)
	{
		//传输成功！
		emit sigAck(true,comd);
	}
	else
	{
		//失败！
		emit sigAck(false,comd);
		LOG_ERROR("emit sigAck failed");
	}

	return comd;
}


/*****************************************************************************
 *                        调用形影指针函数
 *  函 数 名： ProdessControl
 *  功    能： 通过键值调用相应函数
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int IkNetWork::ProdessControl(unsigned short function_no,  unsigned short comd)
{
	CmdHandMap::iterator it = HandlerMap.find(function_no);
    LOG_DEBUG("enter ProdessControl find over");
	std::cout<<"enter ProdessControl find over..."<<std::endl;
	if(it != HandlerMap.end() && it->second )
	{
		std::cout<<"enter ProdessControl finding...."<<std::endl;
		LOG_DEBUG("----------recv CMD:[%08X]",(int)it->first);
        _aLiveCount ++;
		auto pHandler = it->second;
        LOG_DEBUG("befor pHandler");
        pHandler(comd);
        LOG_DEBUG(" pHandler over");

	}
    std::cout<<"enter ProdessControl find end..."<<std::endl;
    return -1;

}

/*****************************************************************************
 *                        心跳函数
 *  函 数 名： KeepALive
 *  功    能： 与服务端链接情况
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
unsigned short IkNetWork::KeepALive(unsigned short comd)
{
	
	NormalAck ack;
	ack.ErrorCode=0;
	int len = EncodeResponse::KeepAlive(ack,_recAckBuf);

	//2015-01-19 fjf 将心跳与其它数据处理分开，同时处理缓冲区被不同线程冲刷
	//emit sigSendData(_recBuf,len);
    LOG_INFO("_recAckBuf=%016x,len=%d",_recAckBuf,len)
	this->_funBeat(_recAckBuf,len);

    _aLiveCount ++;
	_isNetOK = true;
    std::cout<<"当前时间："<<QDateTime::currentDateTime()
		.toString("yyyy-MM-dd hh:mm:ss").toStdString()<<std::endl;
	std::cout<<"设备收到心跳："<<_aLiveCount<<std::endl;
    LOG_INFO("设备收到心跳");
	return comd;
}

/*****************************************************************************
 *                        关机函数
 *  函 数 名： SrvctrlShutDown
 *  功    能： 关机
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
unsigned short IkNetWork::SrvctrlShutDown(unsigned short comd)
{
	
	NormalAck ack;
	ack.ErrorCode=0;
	int len = EncodeResponse::SrvCtrlShutdown(ack,_recBuf);
	emit sigSendData(_recBuf,len);
	Interaction::GetInteractionInstance()->CloseUsb();
	LOG_INFO("shutdown -hf now");
	system("shutdown -hf now");//设备关机
	return comd;
}

/*****************************************************************************
 *                        重启设备
 *  函 数 名： SrvctrlReset
 *  功    能： 重启设备
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-04
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
unsigned short IkNetWork::SrvctrlReset(unsigned short comd)
{
	
	NormalAck ack;
	ack.ErrorCode=0;
	int len = EncodeResponse::RemoteReset(ack,_recBuf);
	emit sigSendData(_recBuf,len);
	Interaction::GetInteractionInstance()->CloseUsb();
	LOG_INFO("shutdown -rf now");
	system("shutdown -rf now");//重启设备
	return comd;
}

/*****************************************************************************
 *                        设置令牌
 *  函 数 名： Token
 *  功    能： 服务端向设备端设置令牌
 *  说    明：  暂时为启用
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
unsigned short IkNetWork::Token(unsigned short comd)
{
	

}

/*****************************************************************************
 *                        添加人员虹膜函数
 *  函 数 名： AddPersonIris
 *  功    能： 添加人员虹膜
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
unsigned short IkNetWork::AddPersonIris(unsigned short comd)
{
	
	NormalAck ack;
    static std::vector<SyncIrisData> vecNetIrisData;
    static std::vector<IrisDataBase> vecIrisData;

    //memset(_recBuf,0,g_maxAckSize);
	if(!DecodeNetData::AddPersonIris(_bufFrame, vecNetIrisData))
	{
		std::cout<< "虹膜解码失败！"<< std::endl;
		ack.ErrorCode= -1;
		int len = EncodeResponse::AddPersonIris(ack,_recBuf);
		emit sigSendData(_recBuf,len);
	}
	else
    {
		IrisDataBase temp;
		int token =0;
		for(auto var : vecNetIrisData)
		{
			token = TypeConvert::ConvertIrisDown(var,temp);
			vecIrisData.push_back(temp);
		}

        std::vector<irisdataIkUuid>  vecQueryResult;
        int error = DBOperationInterface::AddIrisDataBase(vecIrisData,vecQueryResult);

		if(!error && token > _currentIrisToken)
		{
			_currentIrisToken = token;
			DBOperationInterface::SetToken(token,DBType::DBIrisData);
			LOG_INFO(" if(!error && token > _currentIrisToken)");
		}

		if(!error)
		{
			ack.ErrorCode=0;
			int len = EncodeResponse::AddPersonIris(ack,_recBuf);
			emit sigIrisUpdate();
			LOG_INFO("emit sigIrisUpdate(); error: %d",error);

            //DecodeBuffer(_recBuf,len);
			emit sigSendData(_recBuf,len);
			LOG_INFO("emit sigSendData(_recBuf,len); error: %d",error);

            // Add by liushu add memfeature
            //vecQueryResult[i];
            vector<IkUuid>  vecpersonid;
            bool    bfind = false;
            LOG_INFO("vecIrisData.size() = %d",vecIrisData.size());
            for(int i = 0; i < vecIrisData.size(); i ++)
            {
                bfind = false;
                for(int j = 0; j < vecQueryResult.size(); j++)
                {
                    if(vecQueryResult[j].featureid == vecIrisData[i].irisDataID)
                    {
                        bfind = true;
                        break;
                    }
                }

                if(!bfind)
                {
                    continue;
                }

                {
                    vecpersonid.clear();
                    vecpersonid.push_back(vecIrisData[i].personID);

                    // 从数据库获取person信息
                    static std::vector<Person> persons;  //@获取所有数据可能存在溢出问题
                    LOG_DEBUG("begin GetPersons(vecpersonid,persons)");
                    int nret = DBOperationInterface::GetPersons(vecpersonid,persons);
                    LOG_DEBUG("end GetPersons(vecpersonid,persons)=%d",persons.size());
                    if(!nret)
                    {
                        LOG_ERROR("GetPersons faild");
                    }

                    int nmemret = 0;

                    if(persons.size() > 0)      // 只取一条
                    {
                        InfoData    infodata;
                        vecIrisData[i].irisDataID.Get(infodata.ud);   //特征ID
                        infodata.TypeControl = (IdentifyType)vecIrisData[i].eyeFlag;
                        vecIrisData[i].irisCode.Get(infodata.FeatureData,512);

                        persons[0].personID.Get(infodata.PeopleData.Id);
                        vecIrisData[i].irisDataID.Get(infodata.PeopleData.FeatureID);
                        infodata.PeopleData.CardID = persons[0].cardID;       //卡号  此两个变量是在二期新增 2014-04-15
                        infodata.PeopleData.WorkSn = persons[0].workSn;       //工号
                        infodata.PeopleData.Name = persons[0].name;
                        infodata.PeopleData.Depart = persons[0].departName;
                        infodata.PeopleData.Sex = persons[0].sex;

                        nmemret = SingleControl<IrisManager>::CreateInstance().AddFeatureData(infodata);
                       LOG_DEBUG("AddFeatureData: %s result: %d",persons[0].personID.Unparse().c_str(),nmemret);
                    }
                    persons.clear();
                }
            }
		}
		else
		{
			ack.ErrorCode= error;
			int len = EncodeResponse::AddPersonIris(ack,_recBuf);

			emit sigSendData(_recBuf,len);
			LOG_INFO("emit sigSendData(_recBuf,len); error: %d",error);
		}
	}
    //用完清空
    vecNetIrisData.clear();
    vecIrisData.clear();
	return comd;
}

/*****************************************************************************
 *                        删除人员虹膜
 *  函 数 名： DelPersonIris
 *  功    能： 删除虹膜
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
unsigned short IkNetWork::DelPersonIris(unsigned short comd)
{
	
	NormalAck ack;
	std::vector<Uuid> vecData;
	//NormalAck ack;
	if(!DecodeNetData::DeletePersonIris(_bufFrame, vecData))
	{
		std::cout<< "虹膜解码失败！"<< std::endl;
		ack.ErrorCode= -1;
		int len = EncodeResponse::DeletePersonIris(ack,_recBuf);
		emit sigSendData(_recBuf,len);
	}
	else
	{
		int token = 0;
		int error = DBOperationInterface::DelIrisDataBase(vecData,token);
		if(!error && token>_currentIrisToken)
		{
			_currentIrisToken = token;
			DBOperationInterface::SetToken(token,DBType::DBIrisData);
		}
		if(!error)
		{
			ack.ErrorCode=0;
			int len =  EncodeResponse::DeletePersonIris(ack,_recBuf);
			emit sigSendData(_recBuf,len);
            // Add by liushu del memfeature
            int nmemret = 0;
            for(int i = 0; i < vecData.size(); i ++)
            {
                // 通过虹膜uuid获取人员uuid
                PeopleInfo personinfo;
                bool bret = SingleControl<IrisManager>::CreateInstance().GetPeopleInfoByIrisFeature(vecData[i].uuid,personinfo);
                if(!bret)
                {
                    continue;
                }

                InfoData    infodata;
                uuid_copy(infodata.PeopleData.Id, personinfo.Id);
                uuid_copy(infodata.ud, vecData[i].uuid);
                nmemret = SingleControl<IrisManager>::CreateInstance().DeleteFeatureData(infodata,IdentifyType::All);
                char szuid[40] = {0};
                char szuid1[40] = {0};
                uuid_unparse(infodata.PeopleData.Id,szuid);
                uuid_unparse(infodata.ud,szuid1);
                LOG_INFO("NetWork DelPersonIris: pid: [%s], fid: [%s] result: %d",szuid,szuid1,nmemret);
            }
        }
		else
		{
			ack.ErrorCode= error;
			//ack.ErrorString="";
			int len = EncodeResponse::DeletePersonIris(ack,_recBuf);
			emit sigSendData(_recBuf,len);
		}
		//ack 成功！
	}
	return comd;
}

/*****************************************************************************
 *                        添加人员函数
 *  函 数 名： AddPerson
 *  功    能： 添加人员
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
unsigned short IkNetWork::AddPerson(unsigned short comd)
{
	
	NormalAck ack;
    static std::vector<PersonBase> vecNetPerson;  //@存在多条，可能存在溢出问题
    static std::vector<Person> vecPerson;         //@存在多条，可能存在溢出问题
    //memset(_recBuf,0,g_maxAckSize);
	//NormalAck ack;
	if(!DecodeNetData::AddPerson(_bufFrame, vecNetPerson))
	{
		std::cout<< "虹膜解码失败！"<< std::endl;
		ack.ErrorCode= -1;
		int len = EncodeResponse::AddPerson(ack,_recBuf);

		emit sigSendData(_recBuf,len);
	}
	else
	{

		Person temp ;
        int token = 0;
		for(auto var :vecNetPerson)
		{
			token = TypeConvert::ConvertPersonDown(var,temp);
			vecPerson.push_back(temp);
		}

		std::cout<<vecPerson[0].name<<std::endl;

        int error = DBOperationInterface::AddPersonInfo(vecPerson);

		if(error == dbSuccess && token>_currentPersonToken)
		{
			_currentPersonToken = token;
			DBOperationInterface::SetToken(token,DBType::DBPerson);
		}

		ack.ErrorCode = error;
		int len = EncodeResponse::AddPerson(ack,_recBuf);

        emit sigSendData(_recBuf,len);//@no_break
	}
    vecNetPerson.clear();
    vecPerson.clear();

	return comd;
}

/*****************************************************************************
 *                        删除人员函数
 *  函 数 名： DelPerson
 *  功    能： 删除人员
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
unsigned short IkNetWork::DelPerson(unsigned short comd)
{
	
	NormalAck ack;
	std::vector<Uuid> vecPuid;
	//NormalAck ack;
	if(!DecodeNetData::DeletePerson(_bufFrame, vecPuid))
	{
		std::cout<< "删除人员解码失败！"<< std::endl;
	}
	else
	{
		int token=0;
		bool error = DBOperationInterface::DeletePersonInfo(vecPuid,token);
		if( error && token>_currentPersonToken)
		{
			_currentPersonToken = token;
			DBOperationInterface::SetToken(token,DBType::DBPerson);
		}
		if(error)
		{
			ack.ErrorCode=0;
			int len = EncodeResponse::DeletePerson(ack,_recBuf);
			emit sigSendData(_recBuf,len);
		}
		else
		{
			ack.ErrorCode= error;
            int len = EncodeResponse::DeletePerson(ack,_recBuf);
			emit sigSendData(_recBuf,len);
		}
	}
	return comd;
}

/*****************************************************************************
 *                        更新人员函数
 *  函 数 名： UpdPerson
 *  功    能： 更新人员
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
unsigned short IkNetWork::UpdPerson(unsigned short comd)
{
	
	NormalAck ack;
    static std::vector<PersonBase> vecNetPerson; //@存在多条，可能存在溢出问题
    static std::vector<Person> vecPerson;          //@存在多条，可能存在溢出问题
	//NormalAck ack;
	if(!DecodeNetData::UpdatePerson(_bufFrame, vecNetPerson))
	{
		std::cout<< "虹膜解码失败！"<< std::endl;
	}
	else
	{
		Person temp ;
		int token =0;
		for(auto var : vecNetPerson)
		{
			token = TypeConvert::ConvertPersonDown(var,temp);
			vecPerson.push_back(temp);
		}
        vecNetPerson.clear();

		bool errPerson =DBOperationInterface::UpdatePersonInfo(vecPerson);
		if(errPerson && token>_currentPersonToken)
		{
			_currentPersonToken = token;
			bool errToken = DBOperationInterface::SetToken(token,DBType::DBPerson);
		}
		if(errPerson)
		{
            for(auto var : vecPerson)
            {
                InfoData    infodata;
                infodata.PeopleData.Name = var.name;
                infodata.PeopleData.CardID = var.cardID;
                infodata.PeopleData.WorkSn = var.workSn;
                infodata.PeopleData.Depart = var.departName;

                uuid_t personid;
                var.personID.Get(personid);
                uuid_copy(infodata.PeopleData.Id,personid);

                int nmemret = SingleControl<IrisManager>::CreateInstance().UpdatePersonInfo(infodata);
                LOG_INFO("NetWork ModPersonInfo: %s result: %d",var.personID.Unparse().c_str(),nmemret);
            }

			ack.ErrorCode=0;
			int len = EncodeResponse::UpdatePerson(ack,_recBuf);

			emit sigSendData(_recBuf,len);
		}
		else
		{
			ack.ErrorCode= errPerson;
			int len = EncodeResponse::UpdatePerson(ack,_recBuf);

			emit sigSendData(_recBuf,len);
		}

        vecPerson.clear();
	}
	return comd;
}

/*****************************************************************************
 *                        添加人员图像函数
 *  函 数 名： AddPersonPhoto
 *  功    能： 添加人员图像
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
unsigned short IkNetWork::AddPersonPhoto(unsigned short comd)
{
	
	NormalAck ack;
	//    std::vector<PersonPhoto> vecNetPersonPhoto;
	std::vector<PersonImage> vecPersonPhoto;
	PersonPhoto *netPersonPhoto = NULL;
	PersonImage personPhoto;

	//    PersonPhoto  *netPersonImage = (PersonPhoto*)(new unsigned char[sizeof(PersonPhoto)
	//                                                  +personImage.personImage.Size()]);
	char* tmp = new char[200000];
	netPersonPhoto = (PPersonPhoto)(tmp);
	//NormalAck ack;
	if(!DecodeNetData::AddPersonPhoto(_bufFrame, *netPersonPhoto))
	{
		std::cout<< "虹膜解码失败！"<< std::endl;
		LOG_INFO("虹膜解码失败！");
	}
	else
	{
		//        PersonImage temp;
		//        for(auto var :netPersonPhoto)
		//        {
		//            TypeConvert::ConvertPersonImage(var,temp);
		//            personPhoto.push_back(temp);
		//        }
		int token = TypeConvert::ConvertPersonImageDown(netPersonPhoto,personPhoto);
		vecPersonPhoto.push_back(personPhoto);
		bool error = DBOperationInterface::AddFaceImg(vecPersonPhoto);
		if(error && token>_currentPhotoToken)
		{
			_currentPhotoToken = token;
			DBOperationInterface::SetToken(token,DBType::DBPersonPhoto);
		}
		if(error)
		{
			ack.ErrorCode=0;
			int len = EncodeResponse::AddPersonPhoto(ack,_recBuf);

			emit sigSendData(_recBuf,len);
		}
		else
		{
			ack.ErrorCode= error;
			int len = EncodeResponse::AddPersonPhoto(ack,_recBuf);

			emit sigSendData(_recBuf,len);
		}
	}

	delete[] tmp;
	return comd;
}

/*****************************************************************************
 *                        删除人员图像函数
 *  函 数 名： DelPersonPhoto
 *  功    能： 删除图像
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
unsigned short IkNetWork::DelPersonPhoto(unsigned short comd)
{
	
	NormalAck ack;
	std::vector<Uuid> vecPersonPhotoUid;
	//NormalAck ack;
	if(!DecodeNetData::DeletePersonPhoto(_bufFrame, vecPersonPhotoUid))
	{
		std::cout<< "虹膜解码失败！"<< std::endl;
		LOG_INFO("虹膜解码失败！");
	}
	else
	{

		int token=0;

		bool error = DBOperationInterface::DeleteFaceImg(vecPersonPhotoUid,token);

		if(error && token>_currentPhotoToken)
		{
			_currentPhotoToken = token;
			DBOperationInterface::SetToken(token,DBType::DBPersonPhoto);
		}
		if(error)
		{
			ack.ErrorCode=0;
			int len = EncodeResponse::DeletePersonPhoto(ack,_recBuf);
			emit sigSendData(_recBuf,len);
		}
		else
		{
			ack.ErrorCode= error;
			int len = EncodeResponse::DeletePersonPhoto(ack,_recBuf);
			emit sigSendData(_recBuf,len);
		}
	}
	return comd;
}

/*****************************************************************************
 *                        更新人员图像函数
 *  函 数 名： UpdPersonPhoto
 *  功    能： 更新人员图像
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
unsigned short IkNetWork::UpdPersonPhoto(unsigned short comd)
{
	
	NormalAck ack;
	//    std::vector<PersonPhoto> vecNetPersonPhoto;
	std::vector<PersonImage> vecPersonPhoto;
	PersonPhoto netPersonPhoto;
	PersonImage personPhoto;
	//NormalAck ack;
	if(!DecodeNetData::UpdatePersonPhoto(_bufFrame, netPersonPhoto))
	{
		std::cout<< "虹膜解码失败！"<< std::endl;
		LOG_INFO("虹膜解码失败！");
	}
	else
	{
		int token = TypeConvert::ConvertPersonImageDown(&netPersonPhoto,personPhoto);
		vecPersonPhoto.push_back(personPhoto);
		bool error = DBOperationInterface::UpdateFaceImg(vecPersonPhoto);
		if(error && token>_currentPhotoToken)
		{
			_currentPhotoToken = token;
			DBOperationInterface::SetToken(token,DBType::DBPersonPhoto);
		}
		if(error)
		{
			ack.ErrorCode=0;
			int len = EncodeResponse::UpdatePersonPhoto(ack,_recBuf);
			emit sigSendData(_recBuf,len);
		}
		else
		{
			ack.ErrorCode= error;
			int len = EncodeResponse::UpdatePersonPhoto(ack,_recBuf);
			emit sigSendData(_recBuf,len);
		}
	}
	return comd;
}

/*****************************************************************************
 *                        远程设置时间函数
 *  函 数 名： SrvCtrlAdjustTime
 *  功    能： 远程设置时间
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
unsigned short IkNetWork::SrvCtrlAdjustTime(unsigned short comd)
{
	
	NormalAck ack;
	TimeString time;

	if(!DecodeNetData::SrvCtrlAdjustTime(_bufFrame, time))
	{
		ack.ErrorCode= -2;
		int len = EncodeResponse::SrvCtrlAdjustTime(ack,_recBuf);
		emit sigSendData(_recBuf,len);
		std::cout<< "虹膜解码失败！"<< std::endl;
		LOG_INFO("虹膜解码失败！");
	}
	else
	{
		int error = -1;
		if(0 != strlen(time.TimeStr))
		{
			error = SysNetInterface::SetSysTime(std::string(time.TimeStr))?0:-1;
		}

		if(error)
		{
			ack.ErrorCode=0;
			int len = EncodeResponse::SrvCtrlAdjustTime(ack,_recBuf);
			emit sigSendData(_recBuf,len);
		}
		else
		{
			ack.ErrorCode= error;
			int len = EncodeResponse::SrvCtrlAdjustTime(ack,_recBuf);
			emit sigSendData(_recBuf,len);
		}
	}
}

/*****************************************************************************
 *                        批量开始
 *  函 数 名： SyncMassBegin
 *  功    能： 批量开始
 *  说    明：未使用
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
unsigned short IkNetWork::SyncMassBegin(unsigned short comd)
{
	

}

/*****************************************************************************
 *                        批量结束
 *  函 数 名： SyncMassEnd
 *  功    能： 批量结束
 *  说    明：未使用
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
unsigned short IkNetWork::SyncMassEnd(unsigned short comd)
{
	

}

/*****************************************************************************
 *                        获取人员列表函数
 *  函 数 名： GetPersonList
 *  功    能： 获取人员列表
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
unsigned short IkNetWork::GetPersonList(unsigned short comd)
{

	
    NormalAck ack;
    ControlCmd cmd;
   static std::vector<PersonSimple> lisNetPerson; //@#


    //@存在隐患，上限1w
    if(!DecodeNetData::GetPersonList(_bufFrame, cmd))
    {
        std::cout<< "虹膜解码失败！"<< std::endl;
        LOG_INFO("虹膜解码失败！");
        ack.ErrorCode= -1;
        int len = EncodeResponse::GetPersonList(ack,lisNetPerson,_recBuf);
        emit sigSendData(_recBuf,len);
    }
    else
    {

        static std::vector<Person> lstPerson;  //@存在多条，可能存在溢出问题
        if(DBOperationInterface::GetPersons(lstPerson))
        {
            for(Person ver :lstPerson)
            {
                PersonSimple temp;
                TypeConvert::ConvertSimplePersonUp(ver,temp);
                lisNetPerson.push_back(temp);
            }
        }

        ack.ErrorCode=0;
        int len = EncodeResponse::GetPersonList(ack,lisNetPerson,_recDBBuf);

        emit sigSendData(_recDBBuf,len);
        lstPerson.clear();


    }
     lisNetPerson.clear();
}

/*****************************************************************************
 *                        获取设备IP函数
 *  函 数 名： SrvCtrlAdjustDevIP
 *  功    能： 获取设备IP
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
unsigned short IkNetWork::SrvCtrlAdjustDevIP(unsigned short comd)
{
	

	NormalAck ack;
	IpSetting ip;

	if(!DecodeNetData::SrvCtrlAdjustDevIP(_bufFrame, ip))
	{
		std::cout<< "SrvCtrlAdjustDevIP解码失败！"<< std::endl;
		LOG_INFO("SrvCtrlAdjustDevIP解码失败！");
	}
	else
	{

		std::string devIp = ip.Ip;
		std::string devMask = ip.SubnetMask;
		std::string devGate = ip.GateWay;

		int error = SysNetInterface::SetDevNetInfo(devIp, devMask, devGate);
		if(error)
		{
			ack.ErrorCode=0;
			int len = EncodeResponse::SrvCtrlAdjustDevIP(ack,_recBuf);
			emit sigSendData(_recBuf,len);
		}
		else
		{
			ack.ErrorCode= error;
			int len = EncodeResponse::SrvCtrlAdjustDevIP(ack,_recBuf);
			emit sigSendData(_recBuf,len);
		}
	}

}

/*****************************************************************************
 *                        设置设备连接服务器IP函数
 *  函 数 名： SrvCtrlAdjustSrvIP
 *  功    能： 设置设备连接服务器IP
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
unsigned short IkNetWork::SrvCtrlAdjustSrvIP(unsigned short comd)
{
	
	//设置服务器IP
	NormalAck ack;
	ListenIp listenIp;

	if(!DecodeNetData::SrvCtrlAdjustSrvIP(_bufFrame, listenIp))
	{
		std::cout<< "SrvCtrlAdjustSrvIP解码失败！"<< std::endl;
		LOG_INFO("SrvCtrlAdjustDevIP解码失败！");
	}
	else
	{
		int error = -1;
		if(0 != strlen(listenIp.Ip))
		{
			error = SysNetInterface::SetSrvNetInfo(listenIp.Ip,listenIp.Port)?0:-1;
		}

		if(error)
		{
			ack.ErrorCode=0;
			int len = EncodeResponse::SrvCtrlAdjustSrvIP(ack,_recBuf);
			emit sigSendData(_recBuf,len);
		}
		else
		{
			ack.ErrorCode= error;
			int len = EncodeResponse::SrvCtrlAdjustSrvIP(ack,_recBuf);
			emit sigSendData(_recBuf,len);
		}
	}
}

/*****************************************************************************
 *                        获取设备信息函数
 *  函 数 名： GetDevInfo
 *  功    能： 获取设备信息
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
unsigned short IkNetWork::GetDevInfo(unsigned short comd)
{
	
	NormalAck ack;
	ControlCmd cmd;

    //@获取网络数据并进行解码
    if(!DecodeNetData::GetDevInfo(_bufFrame, cmd))
	{
		std::cout<< "SrvCtrlAdjustSrvIP解码失败！"<< std::endl;
		LOG_INFO("SrvCtrlAdjustDevIP解码失败！");
	}
	else
	{
		DeviceInfo devInfo;
        memset(&devInfo,0,sizeof(DeviceInfo));

        //解码成功以后，向服务发送回应信息
        //之前通过接收buffer发送，造成数据在发送的过程中被改动了。
        //应该属于误写
        //memset(_recBuf,0,g_maxAckSize);
		devInfo.Mode = EnumWorkMode::online;
		devInfo.Status = EnumWorkStatus::identify;
		//devInfo.Version = SysNetInterface::GetSoftVer();
		strncpy(devInfo.Version,
				SysNetInterface::GetSoftVer().c_str(),
				min((size_t)g_ipLen, SysNetInterface::GetSoftVer().length()));

		strncpy(devInfo.Ip,
				SysNetInterface::GetDevIP().c_str(),
				min((size_t)g_snLen, SysNetInterface::GetDevIP().length()));

		strncpy(devInfo.DeviceSn,
				SysNetInterface::GetDevSN().c_str(),
				min((size_t)g_snLen, SysNetInterface::GetDevSN().length()));
		strncpy(devInfo.TimeStr,
				SysNetInterface::GetSysTime().c_str(),
				min((size_t)g_timeLen,SysNetInterface::GetSysTime().length()));

		ack.ErrorCode=0;

        int len = EncodeResponse::GetDevInfo(ack,devInfo,_recBuf);




        char tmp[128] = {0};
        sprintf(tmp ,"%04X",*((short*)(_recBuf + 2)));
        qDebug()<<"DevInfo_cmd="<<QString(QLatin1String(tmp))<<endl;
        //LOG_INFO("DevInfo_cmd=%s",tmp);




        isDevInfoSend = 0;



        //sigSendDevInfo(_recBuf,len);
        emit sigSendData(_recBuf,len);
        isDevInfoSend = 1;



	}
}

/*****************************************************************************
 *                        获取设备指定识别记录据函数
 *  函 数 名： GetRecogRecordCondition
 *  功    能： 获取设备指定识别记录据的条件
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-09
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
unsigned short IkNetWork::GetRecogRecordCondition(unsigned short comd)
{
	
	NormalAck ack;
	//    std::vector<PersonPhoto> vecNetPersonPhoto;
	GetRecogRecord recogCondition;
	//    //NormalAck ack;
	if(!DecodeNetData::GetRecogRecord(_bufFrame, recogCondition))
	{
		std::cout<< "解码获取上传识别记录条件失败！"<< std::endl;
		LOG_INFO("解码获取上传识别记录条件失败！");
	}
	else
	{
		std::vector<PersonRecLog> vecRecog;
		std::vector<RecogRecord> vecNetRecog;
		std::string timeBegin,timeEnd;

		if(recogCondition.startTimeValid)
		{
			timeBegin = QString(recogCondition.startTimeStr).toStdString();
			//            QDateTime dt;
			//            dt.fromString(QString(recogCondition->startTimeStr),"yyyy-MM-dd HH:mm:ss");
		}
		if(recogCondition.endTimeValid)
		{
			//QDateTime dt;
			timeEnd = QString(recogCondition.endTimeStr).toStdString();
			//dt.fromString(QString(recogCondition->startTimeStr),"yyyy-MM-dd HH:mm:ss");
		}

		int error =0;
		error  = DBOperationInterface::GetRecLog(vecRecog,(int)recogCondition.getType,timeBegin,timeEnd);

        LOG_INFO("vecRecog-Length=%d",vecRecog.size());

		for(auto var : vecRecog)
		{
			RecogRecord temp;
			TypeConvert::ConvertRecLogUp(var,temp);
			vecNetRecog.push_back(temp);
		}

        LOG_INFO("vecNetRecog-length=%d",vecNetRecog.size());
        LOG_INFO("vecNetRecogs-size = %d",sizeof(RecogRecord)*vecNetRecog.size());

		//解析条件
		if(error)
		{
			ack.ErrorCode=0;
			int len = EncodeResponse::GetRecogRecord(ack,vecNetRecog,_recBuf);
            emit sigSendData(_recBuf,len);
		}
		else
		{
			ack.ErrorCode= error;
			int len = EncodeResponse::GetRecogRecord(ack,vecNetRecog,_recBuf);
            LOG_INFO("recogRecord's length = %d",len);
            emit sigSendData(_recBuf,len);
		}
	}
	return comd;
}

/*****************************************************************************
 *                        解析执行相应命令并应答
 *  函 数 名： ExplainComd
 *  功    能： 拷贝数据 并调用解析
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-03-07
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IkNetWork::ExplainComd( unsigned short comd)
{
	
	//    NormalAck ack;
	//    switch(comd)
	//    {
	//    case REQ_KEEP_ALIVE://心跳
	//    {
	//        ack.ErrorCode=0;
	//        int len = EncodeResponse::KeepAlive(ack,_recBuf);
	//        emit sigSendData(_recBuf,len);
	//        _aLiveCount ++;
	//        break;
	//    }
	//    case REQ_SRVCTRL_RESET:
	//    {
	//        ack.ErrorCode=0;
	//        int len = EncodeResponse::RemoteReset(ack,_recBuf);
	//        emit sigSendData(_recBuf,len);
	//        system("shutdown -rf now");//重启设备
	//        break;
	//    }
	//    case REQ_SRVCTRL_SHUTDOWN:
	//    {
	//        ack.ErrorCode=0;
	//        EncodeResponse::SrvCtrlShutdown(ack,_recBuf);
	//        emit sigSendData(_recBuf,16);
	//        system("shutdown -hf now");//设备关机
	//        break;
	//    }
	//    case REQ_SRVCTRL_ADJUST_TIME: // 调整客户端时间
	//        break;

	//    case ACK_ADD_PERSON_INFO:// 增加人员信息
	//        break;
	//    case ACK_UPDATE_PERSON_INFO:// 修改人员信息
	//        break;
	//    case ACK_DELETE_PERSON_INFO:// 删除人员信息
	//        break;
	//    case REQ_SYNC_TOKEN: //同步令牌
	//        break;


	//    case REQ_ADD_PERSON_IRIS:
	//    {
	//        //调用何姐接口解析数据结构
	//        std::vector<SyncIrisData> lineData;
	//        //NormalAck ack;
	//        if(!DecodeNetData::AddPersonIris(_bufFrame, lineData))
	//        {
	//            std::cout<< "虹膜解码失败！"<< std::endl;
	//        }
	//        else
	//        {
	//            IrisDataBase temData;

	//            //            temData.devSn = lineData[0].DeviceSn;
	//            temData.irisDataID = lineData[0].Fid;
	//            temData.personID = lineData[0].Pid;
	//            temData.eyeFlag = (int)lineData[0].EyeFlag;
	//            temData.irisCode.Set(&lineData[0].FeatureData[0],512);

	//            //get(_bufFrame,_dataLen);
	//            std::vector<IrisDataBase> vecIrisData;
	//            vecIrisData.clear();
	//            vecIrisData.push_back(temData);

	//            int error = IrisDBOperate::InsertIrisDataBase(vecIrisData);
	//            if(!error)
	//            {
	//                ack.ErrorCode=0;
	//                EncodeResponse::AddPersonIris(ack,_recBuf);
	//                emit sigSendData(_recBuf,16);
	//            }
	//            else
	//            {
	//                ack.ErrorCode= error;
	//                EncodeResponse::AddPersonIris(ack,_recBuf);
	//                emit sigSendData(_recBuf,276);
	//            }
	//        }
	//        break;
	//    }
	//    default:
	//        break;
	//    }

}

/*****************************************************************************
 *                        定时器事件
 *  函 数 名： timerEvent
 *  功    能： 通过定时判断是否联网
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-04
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IkNetWork::timerEvent(QTimerEvent *)
{
	

    if(_isNetOK)
    {
        if(_aLiveCount > _oldALiveCount)
        {
            _oldALiveCount = _aLiveCount;
        }
        else
        {
             _isNetOK = false;
        }
    }
    else
    {
        LOG_ERROR("net is disconnect,and now send sig to reconnect");
        emit sigNetCut();

    }
    LOG_DEBUG("netStatus:_isNetOK=%d;g_bsendreconnect=%d",_isNetOK,g_bsendreconect);
}
/*****************************************************************************
 *                       心跳发送函数－处理心跳与普通数据数据冲突
 *  函 数 名： CallBackFun
 *  功    能： 心跳与普通发送在两个线程里出现互相冲刷数据的现象，所以新增加一个处理
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：fjf
 *  创建时间：2015-01-19
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/

void IkNetWork::CallBackFun(const std::function<void(char*,int)>&fun)
{

    this->_funBeat = fun;
}


/// 解析一段字符串，用于显示调试信息，调试使用
void IkNetWork::DecodeBuffer(char* buffer ,int len)
{
    if(len<11)
    {
        LOG_INFO("in put length is not enough%d",len)
        return ;
    }

    // head 2bit
    char bit1 ;
    int index = 0;
    memcpy(&bit1,buffer+index,1);
    index ++;
    std::cout<<"std-DecodeBuffer-bit1:"<<bit1<<std::endl;


    char bit2 ;
    memcpy(&bit2,buffer+index,1);
    index ++;
    std::cout<<"std-DecodeBuffer-bit2:"<<bit2<<std::endl;


    //askCode 2bit
    unsigned short askCode;
    memcpy(&askCode,buffer+index,2);
    index +=2;
    std::cout<<"std-DecodeBuffer-askCode:"<<askCode<<std::endl;
    if(askCode ==0x0301)
    {
        LOG_INFO("find unmoral cmd:%d",askCode);
    }
    qDebug()<<"DecodeBuffer-askCode:"<<askCode<<"--\n"<<endl;

    //askCodeChild 2bit
    unsigned short askCodeChild;
    memcpy(&askCodeChild,buffer+index,2);
    index +=2;
    qDebug()<<"DecodeBuffer-askCodeChild:"<<askCodeChild<<"--\n"<<endl;

    int dataLength;
    memcpy(&dataLength,buffer+index,4);
    index +=4;
    qDebug()<<"DecodeBuffer-dataLength:"<<dataLength<<"--\n"<<endl;

    char *databuffer = new char[len-10];
    memcpy(databuffer,buffer+index,len-10);



    std::string stdStr = databuffer;
    std::cout << "DecodeBuffer-stdStr:"<<stdStr<<std::endl;

    QString str = QString(QString::fromLatin1(databuffer));
    qDebug()<<"DecodeBuffer-databuffer:"<<str<<endl;

    LOG_INFO("DecodeBuffer-bit1=%x",bit1);
    LOG_INFO("DecodeBuffer-bit2=%x",bit2);
    LOG_INFO("DecodeBuffer-askCode=%x",askCode);
    LOG_INFO("DecodeBuffer-askCodeChild=%x",askCodeChild);
    LOG_INFO("DecodeBuffer-dataLength=%d",dataLength);
    LOG_INFO("DecodeBuffer-databuffer=%s",str.toStdString().c_str());

    delete []databuffer;

}






/*****************************************************************************
 *                       显示数据
 *  函 数 名： Display
 *  功    能： 显示内存中的数据
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：fjf
 *  创建时间：2014-08-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IkNetWork::Display(int num, char *buf)
{
	
	for(int i=0; i<num; i++)
	{
		std::cout<< " ";
		std::cout<<std::hex<<(short)buf[i];//hex<<
	}
	std::cout<<std::endl;
	//-----------------------------------
}

