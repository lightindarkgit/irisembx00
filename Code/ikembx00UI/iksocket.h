/*****************************************************************************
** 文 件 名： iksocket.h
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

#ifndef IKSOCKET_H
#define IKSOCKET_H
#include "../SocketComm/socketAPI.h"
#include "../SocketComm/socketInterfaceAPI.h"
#include "QString"
#include "QObject"
#include "iknetwork.h"
#include <thread>
#include "typeconvert.h"
#include "../USBAPI/simplelock.h"
//#include "../USBAPI/blockingqueue.h"
#include "QTimer"
#include "../Common/SyncLock.h"


enum UploatType
{
    AddPerson  = 0,
    EditPerson = 1,
    DelPerson  = 2,
    RecLog     = 3

};


class IkSocket : public QObject
{
    Q_OBJECT

public:
    static IkSocket* GetIkSocketInstance();

signals:
    void sigIrisUpdate();
public slots:
    void SendWaiterSig();
    int SendData(char *buf,int len);
    int SendDataAck(bool isSucceed,unsigned short comd);


public :
    inline bool IsNetOK()
    {
        return _isConnOK;
    }
    void ReSetNetConn();
    int  AddPerson(const std::vector<Person>& lstPerson);
    int  DelPerson(const vector<Uuid>& lstPerson);
    int  UpdPerson(const std::vector<Person>& lstPerson);
    int  AddPersonImage(const PersonImage& personImage);
    int  DelPersonImage(const std::vector<Uuid>& lstPersonImage);
    int  UpdPersonImage(const PersonImage& personImage);
    int  AddIrisData(const std::vector<IrisDataBase>& lstIrisData);
    int  DelIrisData(const std::vector<Uuid>& lstIrisData);

    int  AddAdmin(const std::vector<Person>& lstPerson);
    int  DelAdmin(const vector<Uuid>& lstPerson);
    int  UpdAdmin(const std::vector<Person>& lstPerson);
    int  AddAdminImage(const PersonImage& personImage);
    int  DelAdminImage(const std::vector<Uuid>& lstPersonImage);
    int  UpdAdminImage(const PersonImage& personImage);
    int  AddAdminIrisData(const std::vector<IrisDataBase>& lstIrisData);
    int  DelAdminIrisData(const std::vector<Uuid>& lstIrisData);
    bool AsyncDevTime();

    int UpLoadRecLog(const std::vector<PersonRecLog>& lstRecLog);

    //上传当前token
    int UploadToken();
    //定时上传数据
    void UploadTemp();
    void UploadTemp(UploatType type );
    bool IsUpLoading();
public:
    void ReConnect();
private:
    void ControlFunPtr();
private:
    ~IkSocket();
    static void ReleaseScoket();
    IkSocket();
    void StartScoket();
    bool CreateSocketAPI();
    void Close();
    void SetConn();
    void DisConn();
    void ClearUid();
    static void * StartSocketThread(void* arg); //开始启动网络通信

    static void * StartUpdateThread(void* arg); //开始启动网络通信

private:
    static IkSocket   *s_scoket;
    static ISocketAPI *s_socketAPI;
    IkNetWork  _netWork;
    std::thread _startScoletThread;
    std::thread _startUpdateThread;
    std::string _ip;
    int s_refCount;
    bool _isConnOK;
    bool _isUpLoading ; //@dj 上传状态标志 是否正在上传识别记录，人员信息，虹膜信息等数据

    //断网重联等待
    ThreadWaiter _reStartWaiter;
    ThreadWaiter _waiter;
    char _sendBuf[g_maxDataSize];//上传数据buf
    std::vector<std::string> _irisUid;
    std::vector<std::string> _personUid;
    std::vector<std::string> _personImageUid;
    std::vector<int> _recogId;

	IRISK_MUTEX		_mutex;


};

#endif // IKSOCKET_H
