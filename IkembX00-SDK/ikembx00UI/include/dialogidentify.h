/*****************************************************************************
** 文 件 名：Identity.h
** 主 要 类：Identity
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：刘中昌
** 创建时间：20013-10-13
**
** 修 改 人：
** 修改时间：
** 描  述:   识别对话框
** 主要模块说明: 识别界面的显示
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#ifndef IDENTITY_H
#define IDENTITY_H

#include <QWidget>
#include <QVector>
#include <QString>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <QMovie>
#include <QLabel>
#include <QProgressBar>

#include "../../USBAPI/simplelock.h"
#include "Common/interface.h"
#include "FeatureManageForDemo.h"
#include "interaction.h"
#include "../IrisAppLib/IrisManager/irisManager.h"
#include "../IrisAppLib/IrisManager/singleControl.h"
#include "hbweather.h"
#include "IKUSBSDK.h"
#include "CCameraApi.h"



namespace Ui {
class DialogIdentify;
}
enum IdentResultState
{
    SuccessState=0,
    FailedState,
    ContinueState ,
    KeepState,
    UnKnownState
};

enum class WiganOutMode : unsigned int
{
    OutWorkSN = 0,
    OutCard,
    NoOut
};

class DialogIdentify : public QWidget
{
    Q_OBJECT
    
public:
    explicit DialogIdentify(QWidget *parent = 0);
    ~DialogIdentify();
    void SetReLoadIris(bool isReLoad);
    // void IrisStatusInfoCallback(IKEnrIdenStatus* enrIdenStatus);

signals:
    void sigGoBack();              //返回考勤识别
    void sigLoginSucceed(bool isSucceed); //信号，登陆是否成功
    void sigNetworkStatusChanged(bool isOK);
    void sigShowEnrollCamera(IKEnrIdenStatus enrIdenStatus);
    //重新初始化SDK返回结果信号
    void sigReInitSDKResult(bool result);

    //20141217 lizhl
private slots:
    void slotUIofIden();
    void slotUIofWait();
    void slotUIofSleep();
    //重新初始化SDK返回结果处理槽函数
    void slotReInitSDKResult(bool result);

public slots:

    void sendBell();   //门铃
    void closeBell();  //关闭门铃
    void selectLogin();

    void AccountLogin();
    void IrisLogin();
    void CancelLogin();

    void timerEvent(QTimerEvent *event);//界面时间更新
    void imageUpdate();     //界面图像更新
    void quitIdent();       //停止识别

    void isStartIdent();       //是否开启识别

    void identResult(long personIndex, int flag);         //考勤识别结果
    void loginIdentResult(long personIndex, int flag);    //管理员登陆识别结果

    void interactionStatus(bool flag);
    void StartIdentCardID(QString personCardID);

    void slotInitSdkOuttime();

    //20140822lizhl
    void SetUIImg(bool isSleep);

    //模拟门铃振铃，播放门铃动画
    void PlayDoorbell();
    //更新网络状态
    void slotUpdateNetWorkState(bool);
    //设置时间控件坐标
    void SetTimecoordinate();
    void slotInitLeftWidget(bool);     //Init left widget to select weather or logog which to show?

    //add by hatao 2015.4.22
    void slotTimerWeather();

    //For LiYan's Test@20150803
    void slotSaveImg();

    void slotUpdateCityName(bool isFromNet,QString cityName);

    void slotWakeDevice();

    void slotReleaseDevice();
public:
    void initNetStateDisplay();

    void WakeInteraction();
    void InitIdent();//识别相关初始化

    void StartIdent();             //开始识别

    void StartAttendIdent();       //开始考勤识别
    void StartLoginIdent();        //开始管理员登陆识别

    int  SetIdentParam();           //设置识别模式
    void StopIdent();
    bool        _m_isSDKWake;      //sdk 是否唤醒状态

    //人机交互类
    static Interaction *_identInteraction;
    static bool g_isQuitIdent;                      //是否退出识别
    static bool g_isStopIdent;
    QTimer *_timerStartIdent;       //识别成功或失败时显示框显示1s钟
    void StartIdentTimer(int msec=2000);

    //发送重新初始化SDK结果信号函数
    void sendReInitSDKResult(bool result);


private:
    void updateLogo();
    void SetStatus(bool isStop);
    void InitSerialMuxDemux();
    void SetSerialCallBack(bool isAdd);
    void ReleaseSerialMuxDemux();
    void DellSeriseCallback();
    void ShowLeftWgt(bool isShow);
    void ShowOnLeftWgt(bool isNetOK);   //Added BY: Wang.L @20160218
    void ShowLogo();     //Added BY: Wang.L @20160218
    void ShowWeather();  //Added BY: Wang.L @20160218
//    void CheckCPUTemp();  //检测cpu温度
//    void CheckCPUAndMem();  //检测cpu使用率和内存使用率
    void ShowClock(); //dj
    void ShowIrisLoginCount(int &count,bool isIrisLogin); //显示虹膜登录计数UI
    void ShowIrisAddCardUI();       //显示刷卡与虹膜识别模式UI
    void ShowLoadProcess();         //显示特征加载进度





    void KeepSdk();
    void ReInitSDK();
    void ReIdent();
    void GetConfig();

    void TestWakeAndSleep();

    void clearSysLog();

    QString GetFacePhotoPath(const QString& subDir);   //Added By: Wang.l @2015-03-23
    QString GetFacePhotoBackUpPath();    //Added By: Wang.l @2015-03-23
    bool BackUpFacePhoto();     //Added By: Wang.l @2015-03-23

    QString GetFacePhotoPath();
    QString GetFacePhotoSonPath(int index);


    //根据配置设置识别模式
    void AddSeriseCallback();

    void LoadCodeList(bool isLoadSuperDate = false);
    void ReleseCodeList();

    //图像回调函数

    int ShowIdentResult   (int funResult,IKResultFlag lrFlag,IKSDKMatchOutput matchOutputL,IKSDKMatchOutput matchOutputR);


    //检测是当前否有人
    static void IdentInteractionStatusCallBack(IfHasPerson hasPerson);

    //刷卡事件回调
    static void RecvCardCallBack(const unsigned char* data, const int size);
    //刷卡事件回调
    static void RecvRalayCallBack(const unsigned char* data, const int size);
    //刷卡事件回调
    static void RecvWiganCallBack(const unsigned char* data, const int size);


    //将识别记录存储到数据库
    void SaveRecogToDB(PeopleInfo& person,int recType = 1);

    //向串口发送韦根、继电器信号
    void SendSingnalToSerial(PeopleInfo& person);

    //20141212 lizhl
    //20141208
    void InitLabelMask();//初始化_labMask
    void InitLabelStatus();//初始化_labStatus
    void InitTraceGif();//初始化_labTraceL, _labTraceR
    void InitIdentWindow();


    void SetTraceVisible(bool isvisible);

    //20141212 lizhl
    void DrawStatusLabel(StatusLabelType type, QString name = "");

    void DrawTraceGif(IKEnrIdenStatus enrIdenStatus);


    void SetOnSleep();
    void WakeFromSleep();

    void InitTimers();
    //检查网络状态是否发生变化，如果变化，则发信号通知等待该信号的处理函数
    void CheckNetworkStatus();


private:
    //处理刷卡通过的函数 2015-08-26
    void AllowAccess(QString personCardID);
    long long GetPersonWorkSn(PeopleInfo &person);//获得人员的工号 2015-08-26 fjf
    void InterationRegCB();                  //抽出人机交互函数 fjf 2015-08-26

    //将发送卡的消息函数抽出来 2015-08-26 fjf
    static void SendFormatCardNum(const unsigned char* data, const int size);
    QString WorkSnQueryFromIris(PeopleInfo &person);  //通过虹膜人员获取工号
    QString WorkSnQueryFromCard();                    //通过卡号获取工号
    bool SendWigan26(std::string cardID);
    bool SendWigan34(std::string cardID);
private:

    Ui::DialogIdentify *ui;
    //正常考勤数据
    unsigned char *_codeListL;//IN
    int            _codeNumL; //IN
    unsigned char *_codeListR;//IN
    int            _codeNumR; //IN

    //管理员考勤数据
    unsigned char *_loginCodeListL;//IN
    int             _loginCodeNumL;//IN
    unsigned char *_loginCodeListR;//IN
    int             _loginCodeNumR;//IN
    int            _countDown;

    int             _noticeCount;

    IKIrisDistFlag _distEstimation;



   // static unsigned char _grayImgBuffer[IK_DISPLAY_IMG_SIZE + IK_BMP_BMP_HEADER_SIZE];

    QVector<QString>  _personName;                  //考勤人员信息
    QVector<QString>  _loginPersonName;             //管理员人员信息

    std::mutex _identCapImgMutex;                   //识别模块图像采集mutex
    std::condition_variable _identCapImgCV;         //识别模块图像采集condition_variable

    int         _count;                             //帧率
    static bool _isLoginIdent;                       //是否登录识别
    bool        _isInSelectLogin;

    //串口类
    SerialMuxDemux*         _sm;
    IdentResultState        _identResultState;       //识别结果
    InteractionResultType   _interactionResultType;  //绘制什么框
    static IfHasPerson      _hasPerson;

    //20141205 lizhl
    static IfHasPerson _lastPersonStatus;           //上一次是否有人的状态
    bool ifUpdatePosition;                          //是否更新位置信息的标志


    //虹膜管理
    IrisManager& _irisManager;
    //数据库操作类
    DatabaseAccess _dbAccess;

    //人脸API
   ///@@@ static CCameraApi* _faceCamer;
    ImgSizeInfo  _facephotoSize;
    unsigned char* _facephotoBuf;

    //门铃定时关闭
    QTimer _bellTimer;
    QTimer _bellRingCtrlTimer;      //控制门铃振铃定时器
    int _bellRingTimeLen;           //门铃振铃时间
    bool  _isOutWigan;                //韦根输出
//    bool  _isOutWigan26;              //韦根输出
//    bool  _isOutWigan34;              //韦根输出
    WiganOutMode  _outMode;         //输出类型---韦根
    bool  _isOutRelay;              //继电器输出
    int   _secOutRalay;
    int   _minOutRalay;
    static bool  _defaultIdentMode; //默认为  非刷卡识别
    static bool  _cardoririsMode;   //用来处理新增加的刷卡或者虹膜 2015-08-26 fjf
    QString _personCardID;
    QString _curCardID;             //fjf 2015-08-26 处理保存当前刷卡通过的人员卡号
    int   _countCardMode;           //刷卡识别倒计时时长  0： 需要重新刷卡；-1：显示“请刷卡”标签；大于0 ：不需要刷卡
    std::string _mac;               //本机mac地址   ---作为设备SN号使用


    //将人脸采集独立成一个线程进行操作，李志林修改
private:
    std::mutex _capFaceImgMutex;                     //采集人脸图像mutex
    std::condition_variable _capFaceImgCV;           //采集人脸图像condition_variable
    bool _capFaceFlag;                               //采集人脸图像flag
    std::thread _capFaceThread;                      //采集人脸图像thread的handle
    QString _idenName;                                //识别出的用户名
    bool _isIdenSuccess;                              //是否识别成功
    bool _isLoginSuccess;                             //是否登录成功
    bool _isSlept;                                    //主要用户刷卡模式时，判断当前是否时睡眠状态@dj

    //20140822lizhl
    QImage *_uiImg;
    static QMovie *_loadMovie;
    static QLabel *_loadLabel;
    QLabel *_tipLabel;

    //guowen添加2018-01-23
    QMovie *_waitSdkInitMovie;
    QLabel *_waitSdkInitLable;

    //20141204lizhl
    QMovie *_waitMovie;//在红外测距未检测到人时显示一个gif
    QLabel *_waitLabel;//
    QProgressBar *_loadDataBar;

private:
    static void* ThreadCapFaceImg(DialogIdentify* pParam);               //采集人脸图像线程
    bool _isReloadiris;

    //The private vaiable was added by wang.l 2014-09-23
private:
    std::map<std::string, std::string> _weekDay;
    QMovie *_bellRing;
    QTimer _timeCoordinateTimer;
    QMovie _netStatePlayer;

    QPoint _imGlobalPoint;
    static QLabel *_labMask;
    QMovie *_movTraceL;
    QLabel *_labTraceL;

    QMovie *_movTraceR;
    QLabel *_labTraceR;

    bool _isClearL, _isClearR, _isLastClearL, _isLastClearR;

    std::string _strCityName;
    int rebootTime;
   // QTimer *_ptimerUpate;   // 气象接口定时器

    HBWeather *m_weather;
    pthread_t idenThread;
    pthread_t loadDataThread;
    static void *ThreadLoadData(void *arg);
    static void *ThreadIden(void *arg);
	
    int Lx,Ly,Lr,Rx,Ry,Rr;
    int _varStartY;
    QString _identDate;    //用户识别时候的时间
};

#endif // IDENTITY_H
