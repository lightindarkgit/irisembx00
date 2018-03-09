/*****************************************************************************
 ** 文 件 名： iksocket.cpp
 ** 主 要 类： IkSocket
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
#include "iksocket.h"
#include "../XML/xml/IKXmlApi.h"
#include <mutex>
#include <iostream>
#include "sysnetinterface.h"
#include "QDateTime"
#include <signal.h>
#include "../Common/Logger.h"
#include <iostream>
#include "../Common/Exectime.h"

#include <QDebug>

const static long s_sleepTimespec = 2*1000;
const static long s_sleepUploadTimespec = 10 *1000 *1000;//定时更新暂定20秒为测试
const static long s_sleepReStart = 5 * 1000*1000;

ISocketAPI*  IkSocket::s_socketAPI = nullptr;
IkSocket*  IkSocket::s_scoket = nullptr;
std::mutex s_mutex;
std::mutex s_reConMutex;   //fjf 2015-02-05

extern bool g_bsendreconect;

IkSocket::IkSocket()
{
    Exectime etm(__FUNCTION__);
    //设置定时器 定时向服务器上传设备数据
    //    _uploadTimer = new QTimer();
    //    connect(_uploadTimer,SIGNAL(timeout()),this,SLOT(UploadTemp()));
    SyncLock::Init(&_mutex);
    memset(_sendBuf,0,g_maxDataSize);
    _isUpLoading = false ;
    _startUpdateThread = std::thread(StartUpdateThread, (void*)this);
    _isConnOK= false;
    //网络通信类初始化
    ControlFunPtr();   //初始化函数指针控制
    StartScoket();
}

IkSocket::~IkSocket()
{
    Exectime etm(__FUNCTION__);
    _startUpdateThread.join();

    SyncLock::Destroy(&_mutex);
}
/*****************************************************************************
 *                         处理函数指针异常
 *  函 数 名：ControlFunPtr
 *  功    能：处理BIND函数指针为空
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：fjf
 *  创建时间：2015-07-23
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int sig_set()
{
    Exectime etm(__FUNCTION__);
    return 1;
}
/*****************************************************************************
 *                         处理函数指针异常
 *  函 数 名：ControlFunPtr
 *  功    能：处理BIND函数指针为空
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：fjf
 *  创建时间：2015-01-23
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IkSocket::ControlFunPtr()
{
    Exectime etm(__FUNCTION__);
    signal(SIGHUP,sig_set);
}
/*****************************************************************************
 *                         重连控制函数
 *  函 数 ReConnect
 *  功    能：设置网络重新连接的过程
 *  说    明：将中昌原来的重连进行了修正
 *  参    数：
 *  返 回 值：
 *  创 建 人：fjf
 *  创建时间：2014-12-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IkSocket::ReConnect()
{
    Exectime etm(__FUNCTION__);
    std::lock_guard<std::mutex> lock(s_reConMutex);

    //启动重连
    _reStartWaiter.signal();
    this->_isConnOK = false;//在重连线程启动前设置，防止线程切换引起错误。注意：中昌在重连线程里也做了这个处理，但为安全，这里再设置一次。
}
/*****************************************************************************
 *                         开始函数
 *  函 数 名：StartScoket
 *  功    能：通过配置获取服务器IP
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-03-10
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IkSocket::StartScoket()
{
    Exectime etm(__FUNCTION__);
    if(CreateSocketAPI())
    {
        //将心跳和其它数据拆开
        this->_netWork.CallBackFun([&](char*buf,int len){
                                   SendData(buf,len);
    });
    //开启线程执行启动
    _startScoletThread = std::thread(StartSocketThread, (void*)this);
    //设置信号槽
    _startScoletThread.detach();

    SetConn();
}
}


bool IkSocket::CreateSocketAPI()
{
    Exectime etm(__FUNCTION__);
    IKXml::IKXmlDocument doc;

    if(doc.LoadFromFile("ikembx00.xml"))
    {
        if(s_socketAPI)
        {
            s_socketAPI->Close();
            delete s_socketAPI;
            s_socketAPI =nullptr;
        }

        ClientData cd;
        strcpy(cd.Addr,doc.GetElementValue("/IKEMBX00/configures/server/ip").c_str());
        cd.Port = atoi(doc.GetElementValue("/IKEMBX00/configures/server/port").c_str());
        //std::lock_guard<std::mutex> lock(s_mutex);
        s_socketAPI = ISocketAPI::CreateInstance(std::bind(&IkNetWork::DealwithNetWork,&_netWork,std::placeholders::_1,std::placeholders::_2),cd);
        assert(s_socketAPI!=nullptr);
        return true;
    }
    LOG_ERROR("load ikembx00.xml  failed");
    return false;
}

bool IkSocket::AsyncDevTime()
{
    Exectime etm(__FUNCTION__);
    int size = EncodeNetData::GetSrvTime(_sendBuf);
    return size == SendData(_sendBuf,size);
}

/*****************************************************************************
 *                         设置槽连接函数
 *  函 数 名：SetConn
 *  功    能：设置槽连接
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-03-10
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IkSocket::SetConn()
{
    Exectime etm(__FUNCTION__);
    connect(&_netWork,SIGNAL(sigSendData(char*,int)),this,SLOT(SendData(char*,int)));
    connect(&_netWork,SIGNAL(sigAck(bool,unsigned short)),this,SLOT(SendDataAck(bool,unsigned short)));
    //    connect(&_netWork,SIGNAL(sigNetConn()),this,SLOT());
    connect(&_netWork,SIGNAL(sigNetCut()),this,SLOT(SendWaiterSig()));
    connect(&_netWork,SIGNAL(sigIrisUpdate()),this,SIGNAL(sigIrisUpdate()));
}


void IkSocket::SendWaiterSig()
{
    Exectime etm(__FUNCTION__);
    // _reStartWaiter.signal();
    this->ReConnect();
}

/*****************************************************************************
 *                         取消设置槽连接函数2
 *  函 数 名：DisConn
 *  功    能：取消设置槽连接
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-03-10
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IkSocket::DisConn()
{
    Exectime etm(__FUNCTION__);
    disconnect(&_netWork,SIGNAL(sigSendData(char*,int)),this,SLOT(SendData(char*,int)));
    disconnect(&_netWork,SIGNAL(sigAck(bool,unsigned short)),this,SLOT(SendDataAck(bool,unsigned short)));
    disconnect(&_netWork,SIGNAL(sigNetCut()),this,SLOT(CreateSocketAPI()));
}

/*****************************************************************************
 *                         释放函数
 *  函 数 名：ReleaseScoket
 *  功    能：
 *  说    明：暂时不开放
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-03-10
 *  修 改 人：liuzhch
 *  修改时间：2014-05-08
 *****************************************************************************/
void IkSocket::ReleaseScoket()
{
    Exectime etm(__FUNCTION__);
    if(s_scoket->s_refCount == 0)
    {
        s_scoket->Close();
    }
    else
    {
        s_scoket->s_refCount--;
    }
}

/*****************************************************************************
 *                        关闭socket函数
 *  函 数 名：Close
 *  功    能：关闭socket
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-03-10
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IkSocket::Close()
{
    Exectime etm(__FUNCTION__);
    s_scoket->DisConn();
    if(s_socketAPI)
    {
        std::lock_guard<std::mutex> lock(s_mutex);
        s_socketAPI->Close();
        s_socketAPI = nullptr;
    }
    //delete s_scoket;
    // s_scoket = nullptr;
}

/*****************************************************************************
 *                         获取socket回应函数
 *  函 数 名：GetSocket
 *  功    能：回应
 *  说    明：本函数未应用，本打算ISocketAPI为一个静态类应用
 *  参    数：
 *  返 回 值：ISocketAPI
 *  创 建 人：liuzhch
 *  创建时间：2014-03-10
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
IkSocket* IkSocket::GetIkSocketInstance()
{
    Exectime etm(__FUNCTION__);
    if(nullptr == s_scoket )
    {
        std::lock_guard<std::mutex> lock(s_mutex);
        if(nullptr == s_scoket)
        {
            s_scoket = new IkSocket();
            assert(s_scoket != nullptr);
        }
    }
    //暂时不应用计数
    //s_scoket->s_refCount++;

    return s_scoket;
}

/*****************************************************************************
 *                        重新连接socket函数
 *  函 数 名：ReSetNetConn
 *  功    能：重新连接socket
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-4
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IkSocket::ReSetNetConn()
{
    Exectime etm(__FUNCTION__);
    s_scoket->Close();
    std::lock_guard<std::mutex> lock(s_mutex);
    if(nullptr == s_scoket)
    {
        s_scoket = new IkSocket();
    }
}

/*****************************************************************************
 *                        添加人员函数
 *  函 数 名：AddPerson
 *  功    能：添加人员
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-6
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int  IkSocket::AddPerson(const std::vector<Person>& lstPerson)
{
    Exectime etm(__FUNCTION__);
    if(_personUid.size()>0)
    {
        LOG_ERROR("_personUid.size()>0");
        return -2;
    }

    memset(_sendBuf,0,g_maxDataSize);
    std::vector<PersonBase> lstNetPerson;
    for(auto& p : lstPerson)
    {
        PersonBase per;
        _personUid.push_back(p.personID.Unparse());
        TypeConvert::ConvertPersonUp(p,per);
        lstNetPerson.push_back(per);
    }

    int len = EncodeNetData::AddPerson(lstNetPerson,_sendBuf);
    int result = SendData(_sendBuf,len);

    return result;
}

/*****************************************************************************
 *                        删除人员函数
 *  函 数 名：DelPerson
 *  功    能：删除人员
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-6
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int  IkSocket::DelPerson(const std::vector<Uuid>& lstPerson)
{
    Exectime etm(__FUNCTION__);
    if(_personUid.size()>0)
    {
        LOG_ERROR("_personUid.size()>0");
        return -2;
    }

    for(Uuid var : lstPerson)
    {
        IkUuid ikUid;
        ikUid.Set(var.uuid);

        _personUid.push_back(ikUid.Unparse());
    }

    memset(_sendBuf,0,g_maxDataSize);
    int len = EncodeNetData::DeletePerson(lstPerson,_sendBuf);
    int result = SendData(_sendBuf,len);

    return result;
}

/*****************************************************************************
 *                       更新人员信息函数
 *  函 数 名：UpdPerson
 *  功    能：更新人员信息
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-6
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int  IkSocket::UpdPerson(const std::vector<Person>& lstPerson)
{
    Exectime etm(__FUNCTION__);
    if(_personUid.size()>0)
    {
        LOG_ERROR("_personUid.size()>0");
        return -2;
    }
    std::vector<PersonBase> lstNetPerson;
    for(auto& p : lstPerson)
    {
        PersonBase per;

        _personUid.push_back(p.personID.Unparse());
        TypeConvert::ConvertPersonUp(p,per);
        lstNetPerson.push_back(per);
    }
    memset(_sendBuf,0,g_maxDataSize);
    int len = EncodeNetData::UpdatePerson(lstNetPerson,_sendBuf);
    int result = SendData(_sendBuf,len);

    return result;
}

/*****************************************************************************
 *                        添加人员图像函数
 *  函 数 名：AddPersonImage
 *  功    能：添加人员图像
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-6
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int  IkSocket::AddPersonImage(const PersonImage& personImage)
{
    Exectime etm(__FUNCTION__);
    if(_personImageUid.size()>0)
    {
        LOG_ERROR("_personUid.size()>0");
        return -2;
    }
    _personImageUid.push_back(personImage.imageID.Unparse());
    PersonPhoto  *netPersonImage = (PersonPhoto*)(new unsigned char[sizeof(PersonPhoto)
                                                  +personImage.personImage.Size()]); //= (PersonPhoto)personImage;
    TypeConvert::ConvertPersonImageUp(personImage,netPersonImage);
    //    netPersonImage.Photo = image;
    memset(_sendBuf,0,g_maxDataSize);
    int len = EncodeNetData::AddPersonPhoto(*netPersonImage,_sendBuf);
    int result = SendData(_sendBuf,len);

    delete netPersonImage;
    return result;
}

/*****************************************************************************
 *                        删除人员图像函数
 *  函 数 名：DelPersonImage
 *  功    能：删除人员图像
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-6
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int  IkSocket::DelPersonImage(const vector<Uuid>& lstPersonImage)
{
    Exectime etm(__FUNCTION__);
    if(_personImageUid.size()>0 || lstPersonImage.size()<1)
    {
        LOG_ERROR("_personImageUid.size()>0 || lstPersonImage.size()<1");
        return -2;
    }

    for(Uuid var : lstPersonImage)
    {
        IkUuid ar;
        ar.Set(var.uuid);
        _personImageUid.push_back(ar.Unparse());
    }

    memset(_sendBuf,0,g_maxDataSize);
    int len = EncodeNetData::DeletePersonPhoto(lstPersonImage,_sendBuf);
    int result = SendData(_sendBuf,len);
    return result;
}

/*****************************************************************************
 *                        更新人员图像函数
 *  函 数 名：UpdPersonImage
 *  功    能：更新人员图像
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-6
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int  IkSocket::UpdPersonImage(const PersonImage& personImage)
{
    Exectime etm(__FUNCTION__);
    if(_personImageUid.size()>0)
    {
        LOG_ERROR("_personImageUid.size()>0");
        return -2;
    }

    _personImageUid.push_back(personImage.imageID.Unparse());
    PersonPhoto  *netPersonImage = (PersonPhoto*)(new unsigned char[sizeof(PersonPhoto)
                                                  +personImage.personImage.Size()]);
    TypeConvert::ConvertPersonImageUp(personImage,netPersonImage);

    memset(_sendBuf,0,g_maxDataSize);
    int len = EncodeNetData::UpdatePersonPhoto(*netPersonImage,_sendBuf);
    int result = SendData(_sendBuf,len);

    delete netPersonImage;
    return result;
}

/*****************************************************************************
 *                        增加虹膜函数
 *  函 数 名：AddIrisData
 *  功    能：增加虹膜
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-6
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int  IkSocket::AddIrisData(const std::vector<IrisDataBase>& lstIrisData)
{
    Exectime etm(__FUNCTION__);
    if(_irisUid.size()>0)
    {
        LOG_ERROR("_irisUid.size()>0");
        return -2;
    }
    std::vector<UploadIrisData> lstNetIrisData;
    for(auto& p : lstIrisData)
    {
        UploadIrisData per;
        _irisUid.push_back(p.irisDataID.Unparse());
        TypeConvert::ConvertIrisUp(p,per);
        lstNetIrisData.push_back(per);
    }

    memset(_sendBuf,0,g_maxDataSize);
    int len = EncodeNetData::UploadPersonIris(lstNetIrisData,_sendBuf);
    int result = SendData(_sendBuf,len);
    return result;
}

/*****************************************************************************
 *                        删除虹膜函数
 *  函 数 名：DelIrisData
 *  功    能：删除虹膜
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-6
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int  IkSocket::DelIrisData(const std::vector<Uuid>& lstIrisData)
{
    Exectime etm(__FUNCTION__);
    if(_irisUid.size()>0 || lstIrisData.size()<1)
    {
        LOG_ERROR("_irisUid.size()>0 || lstIrisData.size()<1");
        return -2;
    }

    std::cout<<"thread enter DelIrisData for circle..."<<std::endl;
    for(Uuid var : lstIrisData)
    {
        IkUuid ar;
        ar.Set(var.uuid);
        _irisUid.push_back(ar.Unparse());
    }

    std::cout<<"发送的删除虹膜特征个数为："<<lstIrisData.size()<<std::endl;
    std::cout<<"_irisUid vector len is:"<<_irisUid.size()<<std::endl;
    std::cout<<"thread exit DelIrisData for circle..."<<std::endl;
    memset(_sendBuf,0,g_maxDataSize);
    int len = EncodeNetData::DeletePersonIris(lstIrisData,_sendBuf);
    std::cout<<"thread DelIrisData for send Data...len is:"<<len<<std::endl;
    int result = SendData(_sendBuf,len);
    std::cout<<"thread exit DelIrisData for send Data..."<<std::endl;
    return result;
}

/*****************************************************************************
 *                        增加管理员函数
 *  函 数 名：AddAdmin
 *  功    能：增加管理员
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-7
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int  IkSocket::AddAdmin(const std::vector<Person>& lstPerson)
{
    Exectime etm(__FUNCTION__);
    return AddPerson(lstPerson);
}

/*****************************************************************************
 *                        删除管理员函数
 *  函 数 名：DelAdmin
 *  功    能：删除管理员
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-7
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int  IkSocket::DelAdmin(const std::vector<Uuid>& lstPerson)
{
    Exectime etm(__FUNCTION__);
    return DelPerson(lstPerson);
}

/*****************************************************************************
 *                        更新管理员信息函数
 *  函 数 名：UpdAdmin
 *  功    能：更新管理员信息
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-7
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int  IkSocket::UpdAdmin(const std::vector<Person>& lstPerson)
{
    Exectime etm(__FUNCTION__);
    return UpdPerson(lstPerson);
}

/*****************************************************************************
 *                       增加管理员图像函数
 *  函 数 名：AddAdminImage
 *  功    能：增加管理员图像
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-7
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int  IkSocket::AddAdminImage(const PersonImage& personImage)
{
    Exectime etm(__FUNCTION__);
    return AddPersonImage(personImage);
}

/*****************************************************************************
 *                      删除管理员图像函数
 *  函 数 名：DelAdminImage
 *  功    能：删除管理员图像
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-7
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int  IkSocket::DelAdminImage(const vector<Uuid>& lstPersonImage)
{
    Exectime etm(__FUNCTION__);
    return DelPersonImage(lstPersonImage);
}

/*****************************************************************************
 *                        更新管理员图像函数
 *  函 数 名：UpdAdminImage
 *  功    能：更新管理员图像
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-7
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int  IkSocket::UpdAdminImage(const PersonImage& personImage)
{
    Exectime etm(__FUNCTION__);
    return UpdPersonImage(personImage);
}

/*****************************************************************************
 *                        增加管理员虹膜函数
 *  函 数 名：AddAdminIrisData
 *  功    能：增加管理员虹膜
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-7
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int  IkSocket::AddAdminIrisData(const std::vector<IrisDataBase>& lstIrisData)
{
    Exectime etm(__FUNCTION__);
    return AddIrisData(lstIrisData);
}

/*****************************************************************************
 *                        删除管理员虹膜函数
 *  函 数 名：DelAdminIrisData
 *  功    能：删除管理员虹膜
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-7
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int  IkSocket::DelAdminIrisData(const std::vector<Uuid>& lstIrisData)
{
    Exectime etm(__FUNCTION__);
    return DelIrisData(lstIrisData);
}

/*****************************************************************************
 *                        上传识别记录函数
 *  函 数 名：UpLoadRecLog
 *  功    能：上传识别记录
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-7
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int  IkSocket::UpLoadRecLog(const std::vector<PersonRecLog>& lstRecLog)
{
    Exectime etm(__FUNCTION__);
    std::vector<RecogRecord> vecNetRecLog;
    RecogRecord temp;

    for(auto var : lstRecLog)
    {
        TypeConvert::ConvertRecLogUp(var,temp);
        vecNetRecLog.push_back(temp);
        _recogId.push_back(var.recLogID);
    }

    memset(_sendBuf,0,g_maxDataSize);
    int len = EncodeNetData::SendRecogRecord(vecNetRecLog,_sendBuf);
    int result = SendData(_sendBuf,len);
    LOG_INFO("UpLoadRecLog");
    return result;
}

/*****************************************************************************
 *                        上传令牌函数
 *
 *  函 数 名：UploadToken
 *  功    能：上传令牌
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-05-8
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int IkSocket::UploadToken()
{
    Exectime etm(__FUNCTION__);
    DevConnInfo devConn;
    memset(&devConn,0,sizeof(DevConnInfo));

    DBOperationInterface::GetToken(devConn.Tokens);
    strncpy(devConn.Ip,
            SysNetInterface::GetDevIP().c_str(),
            min((size_t)g_ipLen, SysNetInterface::GetDevIP().length()));

    strncpy(devConn.DeviceSn,
            SysNetInterface::GetDevSN().c_str(),
            min((size_t)g_snLen, SysNetInterface::GetDevSN().length()));

    memset(_sendBuf,0,g_maxDataSize);

    _netWork.SetToken(devConn.Tokens[0],devConn.Tokens[1],devConn.Tokens[2]);
    int size = EncodeNetData::DevConnect(devConn,_sendBuf);
    int result = SendData(_sendBuf,size);
    LOG_INFO("UploadToken");
    return result;
}

/*****************************************************************************
 *                         上传数据修改函数
 *
 *  函 数 名：UploadTemp
 *  功    能：
 *  说    明：
 *  参    数：
 *  返 回 值：NULL
 *  创 建 人：liuzhch
 *  创建时间：2014-05-21
 *  修 改 人：fjf
 *  修改时间：2014-08-27
 *****************************************************************************/
void IkSocket::UploadTemp()
{
    Exectime etm(__FUNCTION__);
    //	LOG_INFO("UploadTemp use time %llu us",usec);
    LOG_INFO("UploadTemp");
    SyncLock	lock(&_mutex);
    //人员曾删改 虹膜数据增删、图像增删 识别记录上传
    if(_isConnOK)
    {
        std::cout<<"thread enter _isConnOK"<<std::endl;

        //如果未收到回应则跳过
        if(_personUid.size()<1)
        {
            LOG_INFO("thread enter 人员更新");
            ////人员更新
            std::vector<Person> vecAddPerson;
            std::vector<Person> vecUpdaPerson;
            std::vector<std::string> vecDelPerson;

            //@dj usleep(s_sleepTimespec);
            if(DBOperationInterface::GetPersonRecentAdded(vecAddPerson)
                    && vecAddPerson.size()>0)
            {
                std::cout<<"info="<<vecAddPerson[0].memo<<"  length="<<vecAddPerson[0].memo.length()<<endl;

                if(AddPerson(vecAddPerson)<0)
                {
                    return;
                }

            }

            LOG_INFO("thread enter GetPersonRecentEdited...");
            //@dj usleep(s_sleepTimespec);
            if(DBOperationInterface::GetPersonRecentEdited(vecUpdaPerson)
                    && vecUpdaPerson.size()>0)
            {
                if(UpdPerson(vecUpdaPerson)<0)
                {
                    return;
                }
            }

            std::cout<<"thread enter GetPersonRecentDeleted..."<<std::endl;
            LOG_INFO("thread enter GetPersonRecentDeleted...");
            //@dj usleep(s_sleepTimespec);
            if(DBOperationInterface::GetPersonRecentDeleted(vecDelPerson)
                    && vecDelPerson.size()>0)
            {
                std::vector<Uuid> vecDelUuid;
                TypeConvert::ConvertUuidfromStr(vecDelPerson,vecDelUuid);
                DelPerson(vecDelUuid);
            }
        }

        if(_recogId.size()<1)
        {
            std::vector<PersonRecLog> vecReclog;
            DBOperationInterface::GetRecLog(vecReclog,2,"","");//1为已发送，2为wei发送
            if(vecReclog.size()>0)
            {
                std::cout<<"thread enter UpLoadRecLog"<<std::endl;
               // LOG_INFO("UpLoadRecLog start");
                UpLoadRecLog(vecReclog);
                LOG_INFO("UpLoadRecLog over");
            }
        }

//        std::cout<<"thread enter 人员图像更新..."<<std::endl;
//        LOG_INFO("thread enter 人员图像更新...");
        //如果未收到回应则跳过
        if(_personImageUid.size()<1)
        {
            ///人员图像更新
            std::vector<PersonImage> vecAddPersonImage;
            std::vector<std::string> vecDelPersonImage;

           //@dj usleep(s_sleepTimespec);
            if(DBOperationInterface::GetPersonImgRecentAdded(vecAddPersonImage))
            {
                for(PersonImage var : vecAddPersonImage)
                {
                   // LOG_INFO("AddPersonImage start");
                    AddPersonImage(var);
                    LOG_INFO("AddPersonImage over");
                    //                    usleep(100*1000);
                }
            }

           //@dj usleep(s_sleepTimespec);
            if(DBOperationInterface::GetPersonImgRecentDeleted(vecDelPersonImage)
                    && vecDelPersonImage.size()>0)
            {
                std::vector<Uuid> vecDelUuid;
                TypeConvert::ConvertUuidfromStr(vecDelPersonImage,vecDelUuid);
                DelPersonImage(vecDelUuid);
            }
        }

//        std::cout<<"thread enter 人员虹膜更新..."<<std::endl;
//        LOG_INFO("thread enter 人员虹膜更新...");
        //如果未收到回应则跳过
        if(_irisUid.size()<1)
        {
            ///人员虹膜更新
            std::vector<IrisDataBase> vecAddPersonIrisDataBase;
            std::vector<std::string> vecDelPersonIrisData;

            //@dj usleep(s_sleepTimespec);
            if(DBOperationInterface::GetIrisDataBaseRecentAdd(vecAddPersonIrisDataBase)
                    && vecAddPersonIrisDataBase.size()>0)
            {
                //LOG_INFO("AddIrisData start");
                AddIrisData(vecAddPersonIrisDataBase);
                LOG_INFO("AddIrisData over");
            }


            LOG_DEBUG("thread enter GetIrisDataBaseRecentDel...");
            //@dj usleep(s_sleepTimespec);
            if(DBOperationInterface::GetIrisDataBaseRecentDel(vecDelPersonIrisData)
                    && vecDelPersonIrisData.size()>0)
            {
                std::vector<Uuid> vecDelUuid;
                TypeConvert::ConvertUuidfromStr(vecDelPersonIrisData,vecDelUuid);
                std::cout<<"thread enter DelIrisData..."<<std::endl;
                DelIrisData(vecDelUuid);
            }


            LOG_DEBUG("thread exit GetIrisDataBaseRecentDel...");
        }
        else
        {
            LOG_INFO("no  ack been receviced _irisUid.size(): %d",_irisUid.size());
        }

    }
    else
    {
        //@LOG_ERROR("_isConnOK 为false ：断网！,reconnect");
        //_reStartWaiter.signal();
        this->ReConnect();
    }

}

void IkSocket::UploadTemp(UploatType type)
{



}

bool IkSocket::IsUpLoading()
{
    return _isUpLoading;
}


/*****************************************************************************
 *                         socket回应函数
 *
 *  函 数 名：SendData
 *  功    能：回应
 *  说    明：
 *  参    数：buf  :回应数据
 *           len  ： 回应数据长度
 *  返 回 值：NULL
 *  创 建 人：liuzhch
 *  创建时间：2014-03-10
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
int IkSocket::SendData(char *buf,int len)
{
    Exectime etm(__FUNCTION__);
    if(!_isConnOK)
    {
        LOG_ERROR("_isConnOK 为false");
        return -1;
    }

    char tmp[128] = {0};
    //	for(int i = 0; i < 32 && i < len; i++)
    //	{
    //		sprintf(tmp + i * 2,"%02X",*(buf +i));
    //	}

    sprintf(tmp ,"%04X",*((short*)(buf + 2)));

    if(nullptr != s_socketAPI)
    {
        std::cout<<"SendData开始！"<<std::endl;
        LOG_INFO("SendData开始！cmd:[%s]",tmp);
        std::lock_guard<std::mutex> lock(s_mutex);
        if(s_socketAPI)
        {
            std::cout<<"sendData时间："<<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString()<<std::endl;

            //类似单例中双重检查机制，如果已经错误不再发送，控制SENDDATA 2015－01－23 fjf
            if (!_isConnOK)
            {
                LOG_ERROR("_isConnOK 为false");
                return -1;
            }

            errno = 0;
            int err = s_socketAPI->SendData(buf,len);
            std::cout<<"send msg error ："<< strerror(errno)<< "  errno:"<<errno<<std::endl;
            LOG_INFO("++++++++++send msg cmd:[%s],len:[%d / %d], ecode ：%s,%d",tmp,err,len,strerror(errno),errno);

            //            std::cout<<"sendData数据前30:";
            //            for(int i=0; i<30; i++)
            //            {
            //               std::cout<<buf[i];//hex<<
            //            }
            std::cout<<std::endl<<"sendData数据长度:"<<len<<std::endl;
            //断网
            if(err<=0)
            {
                std::cout<<"断网sendData()失败！"<<std::endl;
                LOG_ERROR("断网sendData()失败！,reconnect");
                this->ReConnect();
                //_reStartWaiter.signal();
                //_isConnOK = false;
            }

            std::cout<<"SendData结束！"<<std::endl;
            LOG_INFO("SendData结束！cmd:[%s]",tmp);
            return err;
        }
    }
    LOG_ERROR("s_socketAPI is null");
    return -1;
}

/*****************************************************************************
 *                         清空上保存传信息列表函数
 *
 *  函 数 名：ClearUid
 *  功    能：列表清空
 *  说    明：
 *  参    数：
 *  返 回 值：NULL
 *  创 建 人：liuzhch
 *  创建时间：2014-05-30
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void IkSocket::ClearUid()
{
    Exectime etm(__FUNCTION__);
    _personUid.clear();
    _personImageUid.clear();
    _irisUid.clear();
    _recogId.clear();
}
/*****************************************************************************
 *                         处理不同的回应命令对UUID的操作
 *
 *  函 数 名：SendDataAck
 *  功    能：防止误删除不同操作的相同的UUID
 *  说    明：
 *  参    数：
 *  返 回 值：NULL
 *  创 建 人：liuzhch
 *  创建时间：2014-09-09
 *  修 改 人：fjf
 *  修改时间：
 *****************************************************************************/
int IkSocket::SendDataAck(bool isSucceed,unsigned short comd)
{
    Exectime etm(__FUNCTION__);
    if(isSucceed)
    {
        //fjf 2014-09-09 修改不同的操作使用不同的方法标记，防止TEMP表中误删除不同操作相同UUID
        switch(comd)
        {
        case ACK_UPLOAD_RECOG_RECORD:
        case ACK_SEND_RECOG_RECORD:
            DBOperationInterface::SetRecogStatue(_recogId);
            _recogId.clear();
            break;
        case ACK_ADD_PERSON_INFO:
            DBOperationInterface::DelPersonTemp(_personUid,0);
            _personUid.clear();
            break;
        case ACK_UPDATE_PERSON_INFO:
            DBOperationInterface::DelPersonTemp(_personUid,2);
            _personUid.clear();
            break;
        case ACK_DELETE_PERSON_INFO:
            DBOperationInterface::DelPersonTemp(_personUid,1);
            _personUid.clear();
            break;
        case ACK_ADD_PERSON_PHOTO:
            DBOperationInterface::DelPersonImageTemp(_personImageUid,0);
            _personImageUid.clear();
            break;
        case ACK_UPDATE_PERSON_PHOTO:
            DBOperationInterface::DelPersonImageTemp(_personImageUid,2);
            _personImageUid.clear();
            break;
        case ACK_DELETE_PERSON_PHOTO:
            DBOperationInterface::DelPersonImageTemp(_personImageUid,1);
            _personImageUid.clear();
            break;
        case ACK_UPLOAD_PERSON_IRIS:
            DBOperationInterface::DelIrisdataTemp(_irisUid,0);
            _irisUid.clear();
            break;
        case ACK_DELETE_PERSON_IRIS:
            DBOperationInterface::DelIrisdataTemp(_irisUid,1);
            _irisUid.clear();
            break;
        default:
            break;
        }

    }
    else
    {
        switch(comd)
        {
        case ACK_UPLOAD_PERSON_IRIS:
        case ACK_UPLOAD_RECOG_RECORD:
        case ACK_SEND_RECOG_RECORD:
            _recogId.clear();
            break;
        case ACK_ADD_PERSON_INFO:
        case ACK_UPDATE_PERSON_INFO:
        case ACK_DELETE_PERSON_INFO:
            _personUid.clear();
            break;
        case ACK_ADD_PERSON_PHOTO:
        case ACK_UPDATE_PERSON_PHOTO:
        case ACK_DELETE_PERSON_PHOTO:
            _personImageUid.clear();
            break;
        case ACK_ADD_PERSON_IRIS:
        case ACK_DELETE_PERSON_IRIS:
            _irisUid.clear();
            break;
        default:
            break;
        }
    }
}

/*****************************************************************************
 *                         StartScoletThread
 *
 *  函 数 名：StartScoletThread
 *  功    能：
 *  说    明：由于服务端未开启时 Statr()函数相应时间较长，因此将其执行单启一个线程
 *  参    数：IkSocket类
 *
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-03-13
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void * IkSocket::StartSocketThread(void *arg)
{
    Exectime etm(__FUNCTION__);
    IkSocket *soc = static_cast <IkSocket*> (arg);

    int err = soc->s_socketAPI->Start();
    while(true)
    {
        if(err==0)
        {
            std::cout<<"网络连接失败！"<<std::endl;
            LOG_ERROR("网络连接失败,CreateSocketAPI");
            soc->_isConnOK = false;
            usleep(s_sleepReStart);//链接失败定时重连
            soc->CreateSocketAPI();
            err = soc->s_socketAPI->Start();
            if(err != 0)
            {
                g_bsendreconect = false;
                LOG_ERROR("Net Reconnect Success.result: %d",err);
            }
            else
            {
                LOG_ERROR("Net Reconnect Failed.result: %d",err);
            }
        }
        else
        {

            std::cout<<"网络连接成功！"<<std::endl;
            soc->_isConnOK = true;
            soc->ClearUid();
            soc->UploadToken();
            soc->_waiter.signal();

            soc->_reStartWaiter.wait();
            err = 0;
            std::cout<<"开始重联 当前时间："<<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString()<<std::endl;
        }

    }
}

/*****************************************************************************
 *                         StartUpdateThread
 *
 *  函 数 StartUpdateThread
 *  功    能：
 *  说    明：由于服务端未开启时 Statr()函数相应时间较长，因此将其执行单启一个线程
 *  参    数：IkSocket类
 *
 *  返 回 值：
 *  创 建 人：liuzhch
 *  创建时间：2014-03-13
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void * IkSocket::StartUpdateThread(void *arg)
{
    Exectime etm(__FUNCTION__);

    IkSocket *soc = static_cast <IkSocket*> (arg);
    soc->_waiter.wait();
    while(true)
    {
        //fjf 2014-08-27
        std::cout<<"thread is keep alive"<<std::endl;
        if( soc->_isConnOK )
        {
            std::cout<<"thread enter UploadTemp"<<std::endl;
            LOG_INFO("thread enter UpLoadTemp");
            soc->_isUpLoading = true;
            usleep(0.1*1000*1000 );//缓冲0.1s
            soc->UploadTemp();
            soc->_isUpLoading = false;
        }
        else
        {
            std::cout<<"thread enter waiting..."<<std::endl;
            LOG_ERROR("thread enter waiting...");
            soc->_waiter.wait();
        }

        usleep(s_sleepUploadTimespec );
    }
}
