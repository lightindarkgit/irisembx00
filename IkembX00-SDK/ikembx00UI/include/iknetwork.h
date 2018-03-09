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

#ifndef IKNETWORK_H
#define IKNETWORK_H
#include <QObject>
#include <unordered_map>
#include <thread>

//数据库操作
#include "dboperationinterface.h"
#include <functional>
#include "../USBAPI/simplelock.h"

const static int g_maxDataSize = 1024*1024* 20;
const static int g_maxAckSize = 300;

const static int g_netDataHandleLen = 12;
const static int g_netVecSize = 4;
const static int g_reConnTime = 15*1000;//服务发送心跳频率10s 30*1000;/*7*60*1000;*/

class IkNetWork;
//class ThreadWaiter;

typedef unsigned short (*ProcessFuncPtr)(unsigned short comd);
//typedef std::unordered_map<unsigned short, ProcessFuncPtr> CmdHandMap;
typedef std::unordered_map<unsigned short, std::function<unsigned short(unsigned short)>> CmdHandMap;

class DealWithData
{
public:
    unsigned short _tags;
    bool           _isPerson;
//    std::vector<PersonBase>      _lstPerson;
//    std::vector<SyncIrisData>    _lstIrisDatabase;
};

class IkNetWork :public QObject
{

        Q_OBJECT
public:
    IkNetWork(QObject *parent = 0);
    ~IkNetWork();

public:
    void DealwithNetWork(char *buf, int len);

    inline bool GetIsNetOK()
    {
        return _isNetOK;
    }
    inline void SetNetOK()
    {
        _isNetOK = true;
        this->startTimer(g_reConnTime);
    }

    void SetToken(int personToken,int irisToken,int photoToken);
private :

    static void *ThreadDealWith(void *arg);
    void FindHeader(char *buf,int len);
    void CopyBuf(char *buf,int len);
    void ExplainComd( unsigned short comd);

    //初始化映射map
    void InitHandlerMap();

    //网络请求分发函数
    int ProdessControl(unsigned short function_no,  unsigned short comd);

    //网络请求处理函数
    unsigned short KeepALive(unsigned short comd);
    unsigned short SrvctrlShutDown(unsigned short comd);

    unsigned short Token(unsigned short comd);

    unsigned short AddPersonIris(unsigned short comd);
    unsigned short DelPersonIris(unsigned short comd);
    unsigned short AddPerson(unsigned short comd);
    unsigned short DelPerson(unsigned short comd);
    unsigned short UpdPerson(unsigned short comd);
    unsigned short AddPersonPhoto(unsigned short comd);
    unsigned short DelPersonPhoto(unsigned short comd);
    unsigned short UpdPersonPhoto(unsigned short comd);


    //unsigned short UploadRecogRecord(unsigned short comd);

    unsigned short SrvCtrlAdjustTime(unsigned short comd);
    unsigned short SyncMassBegin(unsigned short comd);
    unsigned short SyncMassEnd(unsigned short comd);

    unsigned short SrvCtrlAdjustDevIP(unsigned short comd);
    unsigned short SrvCtrlAdjustSrvIP(unsigned short comd);

    unsigned short GetDevInfo(unsigned short comd);
    unsigned short GetRecogRecordCondition(unsigned short comd);
    unsigned short GetPersonList(unsigned short comd);

    unsigned short SrvctrlReset(unsigned short comd);

    unsigned short AckGetSrvTime(unsigned short comd);
    unsigned short AckUploadPersonIris(unsigned short comd);
    unsigned short AckUploadRecog(unsigned short comd);
    unsigned short AckSendRecog(unsigned short comd);

    unsigned short AckAddPersonInfo(unsigned short comd);
    unsigned short AckUpdPersonInfo(unsigned short comd);
    unsigned short AckDelPersonInfo(unsigned short comd);
    unsigned short AckAddPhoto(unsigned short comd);
    unsigned short AckUpdPhoto(unsigned short comd);
    unsigned short AckDelPhoto(unsigned short comd);

    unsigned short AckAddPersonIris(unsigned short comd);
    unsigned short AckDelPersonIris(unsigned short comd);




signals:
    void sigSendData(char *buf,int len);
    void sigSendDevInfo(const char *buf,int len);
    void sigAck(bool isSucceed ,unsigned short comd);
    void sigNetCut();//网络断开
    void sigNetConn();//网络连接
    void sigIrisUpdate();//网络虹膜更新

public slots:
    void timerEvent(QTimerEvent *);
public:
    void Display(int num,char *buf);//2014-08-18 fjf add display data fun
    //2015-1-19 fjf
    inline void SetBeatNum(){this->_aLiveCount = 0;this->_oldALiveCount = 0;this->_isNetOK = false;}
    void CallBackFun(const std::function<void(char*,int)>&fun);

    ThreadWaiter  _threadWaiter;
private:
    std::function<void(char*,int)> _funBeat;   //心跳回调  fjf 2015-1-19
    int _dataLen;
    int _curState;

    int _addLen;
    char _bufFrame[g_maxDataSize];
    char _recBuf[g_maxAckSize];
    char _recDBBuf[g_maxDataSize];
    //char *_recDBBuf;
    char _recAckBuf[g_maxAckSize];

    bool _isNetOK;
    int  _oldALiveCount;
    int  _aLiveCount;

    pthread_t _pthreadID;


    CmdHandMap HandlerMap;

    int _currentPersonToken;
    int _currentIrisToken;
    int _currentPhotoToken;

    void DecodeBuffer(char * buffer,int num);

};




#endif // IKNETWORK_H
