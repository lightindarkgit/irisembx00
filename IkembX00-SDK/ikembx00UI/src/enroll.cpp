/*****************************************************************************
** 文 件 名：enroll.cpp
** 主 要 类：Enroll
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：lizhl
** 创建时间：2013-10-15
**
** 修 改 人：
** 修改时间：
** 描  述:   注册模块
** 主要模块说明: 注册类
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include "enroll.h"
#include "ui_enroll.h"

#include <sys/time.h>
#include "QDateTime"
#include <QDir>

#include "dialogenroll.h"
#include "drawtracgif.h"
#include "../XML/xml/IKXmlApi.h"
#include "../Common/Exectime.h"
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//增加数据库操作相关头文件
#include <postgresql/soci-postgresql.h>
#include "IKBackendSession.h"
#include "IKDatabaseLib.h"
#include "IKDatabaseType.h"
#include "dboperationinterface.h"
#include "../Common/Logger.h"
#include "iksocket.h"
#include "interaction.h"
#include "ikmessagebox.h"

#include <signal.h>

#define DIS_FRAMERATE_NO
#define FEATURE_CHECK_EXIST

#define g_constMaxEnrollImNum   3

const int G_FeatrueMaxNum = 6;
const int G_enrollFeatureNum = 2; //目前x00设备注册时左右眼各只存储2个特征，原有设计没特殊要求先不改动

int g_isEnrollThreadEnd = 1;

//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv


//@for sdk 最后将该数组改为成员变量
IKSDKIrisInfo enrIrisL[g_constMaxEnrollImNum];
IKSDKIrisInfo enrIrisR[g_constMaxEnrollImNum];

//比对过程中的输出结构
typedef struct IrisMatchIndexStruct
{
    int matchIndex; //表示当前已经符合注册要求的图像数目
} IrisMatchIndex, *pIrisMatchIndex;
extern "C" int IKIR_IdenMatch(unsigned char* codeList, int codeNum, unsigned char* irisRecTemplate, pIrisMatchIndex pMatchResult);

IKSDKIrisInfo *Enroll::s_enrIrisL = NULL;
IKSDKIrisInfo *Enroll::s_enrIrisR = NULL;
IKResultFlag Enroll::s_enrLrFlag = EnrRecBothFailed;
Interaction *Enroll::s_enrollInteraction;
bool Enroll::s_isEnrollStoped = true;
IKIrisMode  Enroll::_irisMode = IrisModeBoth;//识别或注册时眼睛模式
//图像位置信息标志

//20140930
int Enroll::s_leftEnrNum;
int Enroll::s_rightEnrNum;

unsigned char Enroll::_grayCapImgBuffer[IK_DISPLAY_IMG_SIZE + g_ppmHeaderLength];//采集图像buffer，ppm格式
bool Enroll::_signaledFlag;


static IKSDKIrisInfo        g_enrIrisL[3],g_enrIrisR[3];


//--------------------------画图事件响应-----------------------------
ImagePaintEvent imPaintEvent;

/*****************************************************************************
*                         注册模块发射绘制采集到的图像信号
*  函 数 名：RaisePaintCapImgEvent
*  功    能：采图
*  说    明：
*  参    数：result：采集图像成功与否的标志
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void ImagePaintEvent::RaisePaintCapImgEvent(int result)
{
    Exectime etm(__FUNCTION__);
    emit SigPaintCapImg(result);
}

/*****************************************************************************
*                         注册模块发射绘制注册结果信号
*  函 数 名：RaisePaintEnrollResultEvent
*  功    能：注册模块发射绘制注册结果信号
*  说    明：
*  参    数：result：输入参数，注册函数结果
*           enrIrisL：输入参数，左眼注册结果虹膜信息结构体
*           numEnrL：输入参数，左眼注册结果虹膜个数
*           enrIrisR：输入参数,右眼注册结果虹膜信息结构体
*           numEnrR：输入参数，右眼注册结果虹膜个数
*           IKResultFlag：输入参数 表明注册结果
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
void ImagePaintEvent::RaisePaintEnrollResultEvent(int result, IKSDKIrisInfo* enrIrisL, int numEnrL, IKSDKIrisInfo* enrIrisR, int numEnrR, IKResultFlag IKflag)
{
    Exectime etm(__FUNCTION__);

    emit SigPaintEnrollResult(result, enrIrisL, numEnrL, enrIrisR, numEnrR, IKflag);

}


/*****************************************************************************
*                         Enroll构造函数
*  函 数 名：Enroll
*  功    能：分配Enroll类资源
*  说    明：
*  参    数：
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
Enroll::Enroll(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Enroll),
    _enrIrisManager(SingleControl<IrisManager>::CreateInstance())
{
    Exectime etm(__FUNCTION__);
    ui->setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint);
    setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::WindowStaysOnTopHint);
    setGeometry(QRect(0, 0, 1024, 600));
    setBackgroundColor(QColor(242,241,240,0));

    //this->setStyleSheet("background-image: url(:/image/ui_background.png);");
    qRegisterMetaType<IKResultFlag>("IKResultFlag");
    //@ FOR SDK qRegisterMetaType<IrisPositionFlag>("IrisPositionFlag");

    _dialogEnroll = new DialogEnroll(this);
    connect(this,SIGNAL(sigCloseDialogFeatureExist()),_dialogEnroll,SLOT(Slot_CloseWindow()));
    _dialogEnroll->hide();


    connect(&imPaintEvent,SIGNAL(SigPaintCapImg(int)),this,SLOT(CapImageUpdate(int)));

    connect(&imPaintEvent,SIGNAL(SigPaintEnrollResult(int, IKSDKIrisInfo* , int , IKSDKIrisInfo* , int, IKResultFlag)),
            this, SLOT(EnrollResultImageUpdate(int, IKSDKIrisInfo* , int , IKSDKIrisInfo* , int, IKResultFlag)),
            Qt::AutoConnection);

    connect(ui->rbtnBothEye , SIGNAL(clicked()), this, SLOT(on_rbtn_clicked()));
    connect(ui->rbtnAnyEye  , SIGNAL(clicked()), this, SLOT(on_rbtn_clicked()));
    connect(ui->rbtnLeftEye , SIGNAL(clicked()), this, SLOT(on_rbtn_clicked()));
    connect(ui->rbtnRightEye, SIGNAL(clicked()), this, SLOT(on_rbtn_clicked()));

    s_enrollInteraction = Interaction::GetInteractionInstance();

    InitTraceGif();

    InitLabelMask();


    InitEnrollWindow();

    InitLabelStatus();






    _irisMode = IrisModeBoth;
    IsDataSaved = true;

    ui->rbtnBothEye->setChecked(true);  //默认为双眼模式注册
    _noticeCount = 0;

    _isEnrollThreadCreated = false;

    isFeatureCheckUIOpen = false;

    SetTraceVisible(false);
    //this->hide();
}

/*****************************************************************************
*                         Enroll析构函数
*  函 数 名：~Enroll
*  功    能：释放Enroll类资源
*  说    明：
*  参    数：
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
Enroll::~Enroll()
{
    delete IdentWindow;
    IdentWindow = nullptr;

    delete _dialogEnroll;
    _dialogEnroll = nullptr;

    delete _movTraceL;
    _movTraceL = nullptr;

    delete _movTraceR;
    _movTraceR = nullptr;

    delete ui;
}

/*****************************************************************************
*                         设置人员信息，并对界面作相应初始化，由人员管理模块调用
*  函 数 名：SetPersonInfo
*  功    能：设置人员信息，并对界面作相应初始化，由人员管理模块调用
*  说    明：
*  参    数：personInfomation：人员信息
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-31
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::SetPersonInfo(PersonInfo *personInfomation)
{
    Exectime etm(__FUNCTION__);
    //设置界面
    ui->widgetDisplayIm->show();
    ui->widgetEnrollResultIms->hide();
    ui->btnSaveEnrollResult->setEnabled(false);
    ui->btnEnroll->setText(QObject::tr("开始注册"));
    LOG_INFO("显示开始注册");
    ui->btnEnroll->setIcon(QIcon(":/image/start.ico"));
    ui->btnEnroll->setIconSize(QSize(40,40));
    ui->btnEnrollReturn->setText(QObject::tr("返回"));
    ui->btnEnrollReturn->setIcon(QIcon(":/image/jtbq_022.ico"));
    ui->btnEnrollReturn->setIconSize(QSize(40,40));

    //使能眼睛模式按钮
    EnableEyeModeRbtn();

    _personInfo = *personInfomation;
    QString qstrUserName = QString::fromLocal8Bit(_personInfo.name.c_str());
    ui->labUserName->setText(qstrUserName);
    QString qstrDepartment = QString::fromLocal8Bit(_personInfo.depart.c_str());
    ui->labDepartment->setText(qstrDepartment);
    _btnEnrollClicked = false;          //启动时注册按钮没有被点击

    //@ modify by dj ，应该将默认模式放入构造函数中，否则用户想修改默认模式时，每次都的修改
    //ui->rbtnBothEye->setChecked(true);  //默认为双眼模式注册

    QDateTime nowTime = QDateTime::currentDateTime();
    //设置系统时间显示格式
    QString strTime = nowTime.toString("hh:mm:ss");
    QString strDate = nowTime.toString("yyyy") + tr("年") + nowTime.toString("MM") + tr("月") + nowTime.toString("dd") + tr("日");

    DrawStatusLabel(StatusEnrNotStart);

    //设置背景
    QPalette palette;

    this->setPalette(palette);

    _labMask->setVisible(true);

}

/*****************************************************************************
*                         开始注册
*  函 数 名：on_btnEnroll_clicked
*  功    能：开始注册
*  说    明：
*  参    数：
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_btnEnroll_clicked()
{
    Exectime etm(__FUNCTION__);
    ui->widgetEnrollResultIms->hide();
    ui->widgetDisplayIm->show();

    ui->btnSaveEnrollResult->setEnabled(false);
    _isLeftFeatureExisted = false;//点击注册时，将虹膜特征是否注册过的标志置为false
    _isRightFeatureExisted = false;

    _indexOfLeftExistedFeature = -1;//点击注册时，已存在特征的序号为-1，表示没有重复特征
    _indexOfRightExistedFeature = -1;

    ui->btnEnrollReturn->setText(QObject::tr("返回"));
    ui->btnEnrollReturn->setIcon(QIcon(":/image/jtbq_022.ico"));
    ui->btnEnrollReturn->setIconSize(QSize(40,40));

    bool ifSaveImg = false;

    //@dj 这个变量是静态变量，防止上次的注册数据用在下次，每次注册前先置0；
    s_leftEnrNum = 0;
    s_rightEnrNum = 0;

    //  停止注册 //如果注册按钮被点击过，则停止注册
    if(_btnEnrollClicked)
    {  
        EnableEyeModeRbtn();

        //@dj
        if(!StopEnroll())
        {
            LOG_ERROR("点击按钮停止注册失败");
            //for test
            ui->btnEnroll->setText(QObject::tr("停止失败"));
            return;
        }

        ui->btnEnroll->setText(QObject::tr("开始注册"));
        LOG_INFO("按钮显示开始注册");
        ui->btnEnroll->setIcon(QIcon(":/image/start.ico"));
        ui->btnEnroll->setIconSize(QSize(40,40));

        _btnEnrollClicked = false;

        DrawStatusLabel(StatusEnrStop);

        //20141210
        SetTraceVisible(false);


    }
    else
    {
        if(g_isEnrollThreadEnd == 0)
        {
            IKMessageBox errNote(this, QString::fromUtf8("开始注册"), QString::fromUtf8("注册线程未停止，稍候再试"), errorMbx);
            errNote.exec();
            return false;
        }

        CheckAndSetEyeMode();
        //如果注册按钮没有被点击过，则开始注册
        DisableEyeModeRbtn();
        ui->btnEnroll->setText(QObject::tr("停止注册"));
        LOG_INFO("按钮显示 停止注册");
        ui->btnEnroll->setIcon(QIcon(":/image/stop.ico"));
        ui->btnEnroll->setIconSize(QSize(40,40));
        _btnEnrollClicked =true;


        s_enrIrisL = NULL;
        s_enrIrisR = NULL;

        int funResult = E_USBSDK_DEFAULT_ERROR;
        _numEnrL = g_constMaxEnrollImNum;
        _numEnrR = g_constMaxEnrollImNum;


        pthread_create(&enrollThread, NULL, ThreadEnroll, this);
        _isEnrollThreadCreated = true;
        pthread_detach(enrollThread);


        _labMask->setVisible(true);

        SetTraceVisible(true);

        DrawStatusLabel(StatusEnrStart);
    }

}

/*****************************************************************************
*                         注册模块返回，不保存结果
*  函 数 名：on_btnEnrollReturn_clicked
*  功    能：注册模块返回，不保存结果
*  说    明：
*  参    数：
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-23
*  修 改 人：guodj
*  修改时间：
*****************************************************************************/
void Enroll::on_btnEnrollReturn_clicked()
{
    Exectime etm(__FUNCTION__);
//    if(!IsDataSaved)
//    {
//        IKMessageBox Note(this, QString::fromUtf8("提示"), QString::fromUtf8("注册数据未保存，请确认是否退出"), questionMbx);
//        Note.exec();
//        if(QDialog::Rejected == Note.result())
//        {
//            return;
//        }

//    }
    //不保存注册虹膜图像并退出
    ExitEnrollUI();

}


/*****************************************************************************
*                         退出注册界面
*  函 数 名：ExitEnrollUI
*  功    能：退出注册界面，由其上一层调用，用于系统强行退出注册界面，而不是通过界面操作退出
*  说    明：
*  参    数：
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2014-06-13
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::ExitEnrollUI(bool isAutoExit)
{
    Exectime etm(__FUNCTION__);
    //@dj 自动退出注册界面前，先发信号保证特征对比界面先退出，以防止下次注册无法进入。
   // if(isFeatureCheckUIOpen)
    {
        emit sigCloseDialogFeatureExist();
    }




    StopEnroll();

    while(g_isEnrollThreadEnd == 0)
    {
        IKMessageBox errNote(this, QString::fromUtf8("开始注册"), QString::fromUtf8("注册线程未停止，稍候再试"), errorMbx);
        errNote.exec();

        if(!isAutoExit)
        {
            return;

        }

    }


    //@ dj 正在注册状态退出时将按钮点击状态恢复原值
    //_btnEnrollClicked = false;
    emit sigEnrollResult(false);
    close();
}

bool Enroll::GetEnrollStatus()
{
    return s_isEnrollStoped;
}

void Enroll::SetEnrollStatus(bool isStoped)
{
    s_isEnrollStoped = isStoped;
}

int Enroll::IsPthreadAlive(pthread_t tid)
{
    int pthread_kill_err;
    if(tid == 0)
    {
        return 0;
    }
    pthread_kill_err = pthread_kill(tid,0);

    if(pthread_kill_err == ESRCH)
    {
       LOG_INFO("ID为0x%x的线程不存在或者已经退出",(unsigned int)tid);
       return 0;
    }
    else if(pthread_kill_err == EINVAL)
    {
        LOG_ERROR("发送信号非法。");
        return -1;
    }
    else
    {
        LOG_INFO("ID为0x%x的线程目前仍然存活。",(unsigned int)tid);
        IKMessageBox errNote(this, QString::fromUtf8("开始注册"), QString::fromUtf8("注册线程未停止，稍候再试"), errorMbx);
        errNote.exec();
        return 1;
    }

}

/*****************************************************************************
*                         保存结果，注册模块返回
*  函 数 名：on_btnSaveEnrollResult_clicked
*  功    能：保存结果，注册模块返回
*  说    明：
*  参    数：
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_btnSaveEnrollResult_clicked()
{
    Exectime etm(__FUNCTION__);

    IsDataSaved = true;
#ifdef FEATURE_CHECK_EXIST

    DialogFeatureExist dialogFeatureExist(this);
    connect(this,SIGNAL(sigCloseDialogFeatureExist()),&dialogFeatureExist,SLOT(on_btnReturn_clicked()));


    //如果特征已存在，即重复注册，弹出对话框让用户选择
    PeopleInfo personLeft;
    PeopleInfo personRight;

    if(_isLeftFeatureExisted || _isRightFeatureExisted || _personInfo.hasEnrolledIris)
    {
        Exectime etm2(__FUNCTION__);
        LOG_INFO("Exist Iris.Left: %d,Right: %d,HasIris: %d",_isLeftFeatureExisted , _isRightFeatureExisted , _personInfo.hasEnrolledIris)
                //这种方式需要修改，反复load特征反复release特征耗时太长
                LoadExistedFeature();
        if(_isLeftFeatureExisted)
        {
            _enrIrisManager.SelectInfoFromFeature(_indexOfLeftExistedFeature,personLeft,IdentifyType::Left);
        }

        if(_isRightFeatureExisted)
        {
            _enrIrisManager.SelectInfoFromFeature(_indexOfRightExistedFeature,personRight,IdentifyType::Right);
        }


        if(_isLeftFeatureExisted || _isRightFeatureExisted)
        {
            dialogFeatureExist.XferExistedPersonInfo(personLeft.Depart, personLeft.Name,
                                                     personRight.Depart, personRight.Name);
        }
        else if(_personInfo.hasEnrolledIris)
        {
            dialogFeatureExist.XferExistedPersonInfo(_personInfo.depart, _personInfo.name,
                                                     _personInfo.depart, _personInfo.name,1);
        }

        isFeatureCheckUIOpen = true;
        if(QDialog::Rejected == dialogFeatureExist.exec())
        {
            return;
        }
        isFeatureCheckUIOpen = false;

    }
#endif
    if(AddFeature == dialogFeatureExist.returnType)
    {
        std::string name = _personInfo.name;
        int leftNum =  0;
        int rightNum = 0;
        _enrIrisManager.GetPersonFeatureCount(name,leftNum,rightNum);
        LOG_DEBUG("该同志的左右眼特征数量为：leftNum:%d; rightNum:%d",leftNum,rightNum);

        if (leftNum + G_enrollFeatureNum > G_FeatrueMaxNum  || rightNum + G_enrollFeatureNum> G_FeatrueMaxNum )
        {
            IKMessageBox warnNote(this, QString("保存注册数据"), QString::fromUtf8("该人员左右眼特征数量超过限制，请删除重试"), warningMbx);
            warnNote.exec();
            return ;
        }

    }


    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    //保存数据到数据库
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //需要对返回类型_typeOfFeatureExistReturn进行判断，是增加特征还是覆盖特征

#ifdef FEATURE_CHECK_EXIST

    if(CoverFeature == dialogFeatureExist.returnType)
    {
        Exectime etm2(__FUNCTION__);

        //如果是覆盖，则先删除原来personID下的特征
        std::vector<IkUuid> vecUuid;
        IkUuid personDelId;

        //        UuidControl uuidDelCtrl;
        uuid_t uuidT;

        if(_isLeftFeatureExisted && _isRightFeatureExisted)
        {
            if(uuid_compare(personLeft.Id, personRight.Id))
            {
                //如果两者不相等
                uuid_copy(uuidT, personLeft.Id);
                personDelId.Set(uuidT);
                vecUuid.push_back(personDelId);

                uuid_copy(uuidT, personRight.Id);
                personDelId.Set(uuidT);
                vecUuid.push_back(personDelId);

            }
            else
            {
                uuid_copy(uuidT, personLeft.Id);
                personDelId.Set(uuidT);
                vecUuid.push_back(personDelId);
            }
        }
        else if(_isLeftFeatureExisted)
        {
            uuid_copy(uuidT, personLeft.Id);
            personDelId.Set(uuidT);
            vecUuid.push_back(personDelId);
        }
        else if(_isRightFeatureExisted)
        {
            uuid_copy(uuidT, personRight.Id);
            personDelId.Set(uuidT);
            vecUuid.push_back(personDelId);
        }
        else if(_personInfo.hasEnrolledIris)
        {
            UuidControl uuidCtrlEnrolled;
            uuid_t personIdEnrolled;
            uuidCtrlEnrolled.UuidParse(_personInfo.id.c_str(),personIdEnrolled);
            personDelId.Set(personIdEnrolled);
            vecUuid.push_back(personDelId);
        }

        DBOperationInterface::DevDelIrisDataBaseAndInsTempIris(vecUuid);
        // Add by liushu  del memfeature
        int nmemret = 0;
        for(int i = 0; i < vecUuid.size(); i ++)
        {
            InfoData    infodata;
            vecUuid[i].Get(infodata.PeopleData.Id);
            nmemret = SingleControl<IrisManager>::CreateInstance().DeletePersonAndFeatureData(infodata);
            char szuid[40] = {0};
            uuid_unparse(infodata.PeopleData.Id,szuid);
            LOG_INFO("Enroll DeletePersonAndFeatureData: pid: [%s] result: %d",szuid,nmemret);
        }
    }

#endif
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


    //打开数据库
    //DatabaseAccess dbAccess;
    std::vector<IrisDataBase> vecIrisDataBase;
    IrisDataBase irisDataBase;
    std::vector<TempIrisDataBase> vecTempIrisDataBase;
    TempIrisDataBase tempIrisDataBase;

    /////////////////////////////////////////////////////////////////
    //生成uuid
    //    Person person;
    // 生成人员UUID
    UuidControl uuidCtrl;
    uuid_t irisDataId;
    uuid_t personId;//使用王磊传入的id

    //保存左眼注册信息到数据库
    if( (EnrRecLeftSuccess == s_enrLrFlag) || (EnrRecBothSuccess == s_enrLrFlag) )
    {
        Exectime etm2(__FUNCTION__);
        for (int count=0; count < G_enrollFeatureNum/*_numEnrL*/; count++)
        {
            uuidCtrl.GetUUID(irisDataId);
            irisDataBase.irisDataID.Set(irisDataId);//"sassleft" +count;//待修改
            tempIrisDataBase.irisDataID.Set(irisDataId);

            uuidCtrl.UuidParse(_personInfo.id.c_str(),personId);
            irisDataBase.personID.Set(personId);//"671e1657-cc34-4b0b-a1fd-1ced7ecba225";//"zhangsan";//待修改
            tempIrisDataBase.personID.Set(personId);
            tempIrisDataBase.operation = 0;//增加虹膜数据操作

            irisDataBase.irisCode.Set(s_enrIrisL[count].irisEnrTemplate, IK_ENROLL_FEATURE_SIZE);//与matchIrisCode有什么区别

            LOG_DEBUG("convert from rgb to bmp left");
            //转换为bmp
            unsigned char BmpImgBuffer[IK_IRIS_IMG_SIZE + 1078];
            DrawTracGif::Rgb2BMP(BmpImgBuffer,s_enrIrisL[count].imgData,IK_IRIS_IMG_HEIGHT,IK_IRIS_IMG_WIDTH);
            irisDataBase.eyePic.Set(BmpImgBuffer, IK_IRIS_IMG_SIZE + 1078);

            //irisDataBase.eyePic.Set(s_enrIrisL[count].imgData, IK_IRIS_IMG_SIZE);
            irisDataBase.matchIrisCode.Set(s_enrIrisL[count].irisRecTemplate, IK_IDEN_FEATURE_SIZE);

            irisDataBase.devSn = "";//待修改
            irisDataBase.eyeFlag = int(IdentifyType::Left);//左眼标记
            irisDataBase.regTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();//待修改
            //            irisDataBase.regPalce = 0;//待修改
            irisDataBase.createTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();//待修改
            irisDataBase.updateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();//待修改
            irisDataBase.memo = "";//待修改

            //@dj 这些值没有 暂初始化为0，不然在数据库中显示为随机数
            irisDataBase.pupilRow = 0;
            irisDataBase.pupilCol = 0;
            irisDataBase.pupilRadius = 0;


            irisDataBase.irisRow = s_enrIrisL[count].IrisRow;
            irisDataBase.irisCol = s_enrIrisL[count].IrisCol;
            irisDataBase.irisRadius = s_enrIrisL[count].IrisRadius;

            irisDataBase.focusScore = s_enrIrisL[count].FocusScore;
            irisDataBase.percentVisible = s_enrIrisL[count].PercentVisible;
            //20161104 lizhl 新SDK没有这些值了
            //			irisDataBase.spoofValue = s_enrIrisL[count].SpoofValue;
            irisDataBase.interlaceValue = s_enrIrisL[count].InterlaceValue;
            //20161104 lizhl 新SDK没有这些值了
            //			irisDataBase.qualityLevel = s_enrIrisL[count].QualityLevel;
            irisDataBase.qualityScore = s_enrIrisL[count].QualityScore;

            vecIrisDataBase.push_back(irisDataBase);
            vecTempIrisDataBase.push_back(tempIrisDataBase);
        }
    }


    //保存右眼注册图像
    if( (EnrRecRightSuccess == s_enrLrFlag) || (EnrRecBothSuccess == s_enrLrFlag))
    {
        Exectime etm2(__FUNCTION__);
        for (int count=0; count < G_enrollFeatureNum; count++)
        {
            uuidCtrl.GetUUID(irisDataId);
            irisDataBase.irisDataID.Set(irisDataId);//"sassright" +count;//待修改
            tempIrisDataBase.irisDataID.Set(irisDataId);

            uuidCtrl.UuidParse(_personInfo.id.c_str(),personId);
            irisDataBase.personID.Set(personId);//"671e1657-cc34-4b0b-a1fd-1ced7ecba225";//"zhangsan";//待修改
            tempIrisDataBase.personID.Set(personId);
            tempIrisDataBase.operation = 0;//增加虹膜数据操作

            irisDataBase.irisCode.Set(s_enrIrisR[count].irisEnrTemplate, IK_ENROLL_FEATURE_SIZE);//与matchIrisCode有什么区别



            LOG_DEBUG("convert from rgb to bmp right");
            //转换为bmp
            unsigned char BmpImgBuffer[IK_IRIS_IMG_SIZE + 1078];
            DrawTracGif::Rgb2BMP(BmpImgBuffer,s_enrIrisR[count].imgData,(unsigned int)IK_IRIS_IMG_HEIGHT, (unsigned int)IK_IRIS_IMG_WIDTH);
            irisDataBase.eyePic.Set(BmpImgBuffer, IK_IRIS_IMG_SIZE + 1078);
//            QImage tmp;
//            bool bT;
//            bT = tmp.loadFromData(s_enrIrisR[count].imgData, IK_IRIS_IMG_SIZE);
//            if(bT)
//                LOG_DEBUG("1111 true");
//            bT = tmp.save("11.bmp");
//            if(bT)
//                LOG_DEBUG("222  true");

            //irisDataBase.eyePic.Set(s_enrIrisR[count].imgData, IK_IRIS_IMG_SIZE);
            irisDataBase.matchIrisCode.Set(s_enrIrisR[count].irisRecTemplate, IK_IDEN_FEATURE_SIZE);

            irisDataBase.devSn = "";//待修改
            irisDataBase.eyeFlag = int(IdentifyType::Right);//右眼标记
            irisDataBase.regTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();//待修改
            //            irisDataBase.regPalce = 0;//待修改
            irisDataBase.createTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();//待修改
            irisDataBase.updateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toStdString();//待修改
            irisDataBase.memo = "";//待修改
            //20161104 lizhl 新SDK没有这些值了
            //			irisDataBase.pupilRow = s_enrIrisR[count].PupilRow;
            //			irisDataBase.pupilCol = s_enrIrisR[count].PupilCol;
            //			irisDataBase.pupilRadius = s_enrIrisR[count].PupilRadius;

            irisDataBase.irisRow = s_enrIrisR[count].IrisRow;
            irisDataBase.irisCol = s_enrIrisR[count].IrisCol;
            irisDataBase.irisRadius = s_enrIrisR[count].IrisRadius;

            irisDataBase.focusScore = s_enrIrisR[count].FocusScore;
            irisDataBase.percentVisible = s_enrIrisR[count].PercentVisible;
            //20161104 lizhl 新SDK没有这些值了
            //			irisDataBase.spoofValue = s_enrIrisR[count].SpoofValue;
            irisDataBase.interlaceValue = s_enrIrisR[count].InterlaceValue;
            //20161104 lizhl 新SDK没有这些值了
            //			irisDataBase.qualityLevel = s_enrIrisR[count].QualityLevel;
            irisDataBase.qualityScore = s_enrIrisR[count].QualityScore;

            vecIrisDataBase.push_back(irisDataBase);
            vecTempIrisDataBase.push_back(tempIrisDataBase);
        }
    }


    DBOperationInterface::DevAddIrisDataBase(vecIrisDataBase);
    DBOperationInterface::DevAddTempIrisDataBase(vecTempIrisDataBase);
    // Add by liushu  add memfeature
    vector<IkUuid>  vecpersonid;
    for(int i = 0; i < vecIrisDataBase.size(); i ++)
    {
        vecpersonid.clear();
        vecpersonid.push_back(vecIrisDataBase[i].personID);

        // 从数据库获取person信息
        static std::vector<Person> persons;    //@获取所有数据 可能存在内存溢出问题
        DBOperationInterface::GetPersons(vecpersonid,persons);

        int nmemret = 0;
        if(persons.size() > 0)      // 只取一条
        {
            InfoData    infodata;
            vecIrisDataBase[i].irisDataID.Get(infodata.ud);   //特征ID
            infodata.TypeControl = (IdentifyType)vecIrisDataBase[i].eyeFlag;
            vecIrisDataBase[i].irisCode.Get(infodata.FeatureData,512);

            persons[0].personID.Get(infodata.PeopleData.Id);
            vecIrisDataBase[i].irisDataID.Get(infodata.PeopleData.FeatureID);
            infodata.PeopleData.CardID = persons[0].cardID;       //卡号  此两个变量是在二期新增 2014-04-15
            infodata.PeopleData.WorkSn = persons[0].workSn;       //工号
            infodata.PeopleData.Name = persons[0].name;
            infodata.PeopleData.Depart = persons[0].departName;
            infodata.PeopleData.Sex = persons[0].sex;

            nmemret = SingleControl<IrisManager>::CreateInstance().AddFeatureData(infodata);
            char sztemp[40] = {0};
            uuid_unparse(infodata.ud,sztemp);
            LOG_INFO("AddFeatureData: pid: [%s], fid: [%s] result: %d",persons[0].personID.Unparse().c_str(),sztemp,nmemret);
        }
        persons.clear();
    }

    if(IkSocket::GetIkSocketInstance()->isSocketConnet())
    {
            IkSocket::GetIkSocketInstance()->UploadTemp();
    }
    else
    {
        LOG_INFO("网络未连接到服务器");
    }

    StopEnroll();





#ifdef DIS_FRAMERATE
    // _showTime->stop();
#endif

    close();
    emit sigEnrollResult(true);

}

//获取摄像头数据
IKEnrIdenStatus Enroll::GetCameraData()
{
    return m_enrIdenStatus;
}

//将外部摄像头采集数据赋值给成员变量，供内部使用
void Enroll::SetCameraData(const IKEnrIdenStatus enrIdenStatus)
{
    m_enrIdenStatus = enrIdenStatus;
    return ;
}
//@add for sdk  ---为使用sdk重载
//通過消息 将初始化摄像头采集的数据发送到注册窗口
void Enroll::CapImageUpdate(IKEnrIdenStatus enrIdenStatus)
{
    Exectime etm(__FUNCTION__);

    SetCameraData(enrIdenStatus);
    //@将sdk采集的摄像头数据人言坐标赋值给成员变量
    IKIrisDistFlag distEstimation;
    distEstimation =   enrIdenStatus.distEstimation;
    s_leftEnrNum   =   enrIdenStatus.leftIrisProgress;
    s_rightEnrNum  =   enrIdenStatus.rightIrisProgress;

    //s_enrollInteraction->noticeCount++;
    ifUpdatePosition = (_noticeCount++ % g_noticeNum == 0) ? true : false;

    //只有采集图像成功才更新界面
    //@dj for sdk-------------------------------------------------------------------------------
    QImage imageBig = DrawTracGif::GetImg(enrIdenStatus.imageData, IK_DISPLAY_IMG_SIZE);
    if(!imageBig.isNull())
    {
        //如果处于注册期间则进行人机交互
        if(_btnEnrollClicked)
        {
            //@ 重载函数替换旧版 for sdk
            s_enrollInteraction->PlayInteraction(ResultUnknown,distEstimation,ifUpdatePosition);
            DrawTraceGif(enrIdenStatus);

            if(ifUpdatePosition)
            {
                DrawStatusLabel(StatusAlgIsPerform);
            }
        }
        ui->labRealTimeIm->setPixmap(QPixmap::fromImage(imageBig));

    }


    //20140930
    if(!_btnEnrollClicked)
    {
        s_leftEnrNum = 0;
        s_rightEnrNum = 0;
    }

    QString leftStr = "左眼已注册\n图像：" + QString::number(s_leftEnrNum,10);
    QString rightStr = "右眼已注册\n图像：" + QString::number(s_rightEnrNum,10);
    ui->labLeftStr->setText(leftStr);
    ui->labRightStr->setText(rightStr);

    QLabel * labLeft[] = {ui->labLeft1,ui->labLeft2,ui->labLeft3};
    QLabel * labRight[] = {ui->labRight1,ui->labRight2,ui->labRight3};

    for(int num = 0; num < 3; num++)
    {
        if(num < s_leftEnrNum)
        {
            labLeft[num]->setPixmap(QPixmap(":/image/greenEye.png"));
        }
        else
        {
            labLeft[num]->setPixmap(QPixmap(":/image/grayEye.png"));//redEye.png
        }

        if(num < s_rightEnrNum)
        {
            labRight[num]->setPixmap(QPixmap(":/image/greenEye.png"));
        }
        else
        {
            labRight[num]->setPixmap(QPixmap(":/image/grayEye.png"));//redEye.png
        }

#ifdef DIS_FRAMERATE
        _frameNum++;
#endif
        //vvvv
    }

}

/*****************************************************************************
*                         从数据库中导入已注册虹膜特征
*  函 数 名：LoadExistedFeature
*  功    能：从数据库中导入已注册虹膜特征，供验证虹膜是否重复注册使用
*  说    明：
*  参    数：NULL
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2014-03-10
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::LoadExistedFeature()
{
     Exectime etm(__FUNCTION__);

    // common by liushu
    //初始化
    //_enrIrisManager.Init();

    //加载普通人员特征
    _enrIrisManager.GetIrisFeature(IdentifyType::Left,_enrCodeListL,_enrCodeNumL);
    _enrIrisManager.GetIrisFeature(IdentifyType::Right,_enrCodeListR,_enrCodeNumR);


    //加载管理员特征
    _enrIrisManager.GetSuperFeature(IdentifyType::Left,_enrAdminCodeListL,_enrAdminCodeNumL);
    _enrIrisManager.GetSuperFeature(IdentifyType::Right,_enrAdminCodeListR,_enrAdminCodeNumR);
}


/*****************************************************************************
*                         注册模块将注册结果图像显示到界面
*  函 数 名：EnrollResultImageUpdate
*  功    能：注册模块将注册结果图像显示到界面
*  说    明：
*  参    数：result：函数调用结果
*           enrIrisL：输入参数，左眼注册结果虹膜信息结构体
*           numEnrL：输入参数，左眼注册结果虹膜个数
*           enrIrisR：输入参数,右眼注册结果虹膜信息结构体
*           numEnrR：输入参数，右眼注册结果虹膜个数
*           IKResultFlag：输入参数 表明注册结果
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::EnrollResultImageUpdate(int result, IKSDKIrisInfo* enrIrisL, int numEnrL, IKSDKIrisInfo* enrIrisR, int numEnrR, IKResultFlag lrEnrSucFailFlag)
{
    Exectime etm(__FUNCTION__);

    IsDataSaved = false;
    _btnEnrollClicked = false;

    EnableEyeModeRbtn();

    if(E_USBSDK_OK == result)
    {
        ui->widgetDisplayIm->hide();

        DrawStatusLabel(StatusEnrSuccess);

        //20141202
        _labMask->setVisible(false);

        ui->widgetEnrollResultIms->show();

#ifdef FEATURE_CHECK_EXIST
        //加载虹膜特征
        LoadExistedFeature();
#endif

        QPushButton *btns[]     = { ui->btnIrisImgLeft1, ui->btnIrisImgLeft2, ui->btnIrisImgLeft3,
                                    ui->btnIrisImgRight1, ui->btnIrisImgRight2, ui->btnIrisImgRight3};

        QLineEdit * scores[]    = { ui->lineEditScoreLeft1,ui->lineEditScoreLeft2,ui->lineEditScoreLeft3,
                                    ui->lineEditScoreRight1,ui->lineEditScoreRight2,ui->lineEditScoreRight3};

        QLineEdit * occlusions[]= { ui->lineEditOcclusionLeft1,ui->lineEditOcclusionLeft2,ui->lineEditOcclusionLeft3,
                                    ui->lineEditOcclusionRight1,ui->lineEditOcclusionRight2,ui->lineEditOcclusionRight3};

        for(int count = 0; count < 6; count++)
        {
            btns[count]->setPixmap(NULL);
            btns[count]->resize(btns[count]->width(),btns[count]->height());

            occlusions[count]->setStyleSheet("background-color: rgb(255, 255, 255);  border:2px groove gray;  padding: 4px 4px;  border-radius: 8px;color: rgb(255, 0, 0);");
            scores[count]->setStyleSheet("background-color: rgb(255, 255, 255);  border:2px groove gray;  padding: 4px 4px;  border-radius: 8px;color: rgb(255, 0, 0);");
            occlusions[count]->setText(QString::number(0,10));
            scores[count]->setText( QString::number(0,10));
        }

        int qualityScore = 0;
        int occlusionsScore = 0;
        QImage resultImg;

        if((EnrRecLeftSuccess == lrEnrSucFailFlag) || (EnrRecBothSuccess == lrEnrSucFailFlag))
        {

            for(int count = 0; count < numEnrL; count++)
            {
                resultImg = DrawTracGif::GetImg(enrIrisL[count].imgData, IK_IRIS_IMG_SIZE);
                resultImg = resultImg.scaled(btns[count]->width(),btns[count]->height(),Qt::IgnoreAspectRatio);

                btns[count]->setPixmap(QPixmap::fromImage(resultImg));

                btns[count]->resize(btns[count]->width(),btns[count]->height());

                qualityScore = enrIrisL[count].QualityScore;
                occlusionsScore = enrIrisL[count].PercentVisible;

                if(occlusionsScore < g_MinEnrollOcclusion)
                {
                    occlusions[count]->setStyleSheet("background-color: rgb(255, 255, 255);  border:2px groove gray;  padding: 4px 4px;  border-radius: 8px;color: rgb(255, 0, 0);");
                }
                else
                {
                    occlusions[count]->setStyleSheet("background-color: rgb(255, 255, 255);  border:2px groove gray;  padding: 4px 4px;  border-radius: 8px;color: rgb(23, 111, 11);");
                }
                if(qualityScore < g_MinEnrollQualityScore)
                {
                    scores[count]->setStyleSheet("background-color: rgb(255, 255, 255);  border:2px groove gray;  padding: 4px 4px;  border-radius: 8px;color: rgb(255, 0, 0);");
                }
                else
                {
                    scores[count]->setStyleSheet("background-color: rgb(255, 255, 255);  border:2px groove gray;  padding: 4px 4px;  border-radius: 8px;color: rgb(23, 111, 11);");
                }

                occlusions[count]->setText(QString::number(occlusionsScore,10));
                scores[count]->setText( QString::number(qualityScore,10));

#ifdef FEATURE_CHECK_EXIST

                //注册后检查该虹膜特征是否已经保存
                //@ for sdk
                IrisMatchIndex pMatchResult[IK_MAX_NUM_OF_MATCH];
                StopEnroll();

                int result = IKIR_IdenMatch(_enrCodeListL, _enrCodeNumL,enrIrisL[count].irisRecTemplate, pMatchResult);
                if(result == 0)
                {
                    _isLeftFeatureExisted = true;
                    _indexOfLeftExistedFeature = pMatchResult[0].matchIndex;
                }

#endif
            }

        }

        if((EnrRecRightSuccess == lrEnrSucFailFlag) || (EnrRecBothSuccess == lrEnrSucFailFlag))
        {
            for(int count = 0; count < numEnrR; count++)
            {
                resultImg = DrawTracGif::GetImg(enrIrisR[count].imgData, IK_IRIS_IMG_SIZE);
                resultImg = resultImg.scaled(btns[count+3]->width(),btns[count+3]->height(),Qt::IgnoreAspectRatio);

                btns[count+3]->setPixmap(QPixmap::fromImage(resultImg));
                btns[count+3]->resize(btns[count+3]->width(),btns[count+3]->height());

                qualityScore = enrIrisR[count].QualityScore;
                occlusionsScore = enrIrisR[count].PercentVisible;

                if(occlusionsScore < g_MinEnrollOcclusion)
                {
                    occlusions[count+3]->setStyleSheet("background-color: rgb(255, 255, 255);  border:2px groove gray;  padding: 4px 4px;  border-radius: 8px;color: rgb(255, 0, 0);");
                }
                else
                {
                    occlusions[count+3]->setStyleSheet("background-color: rgb(255, 255, 255);  border:2px groove gray;  padding: 4px 4px;  border-radius: 8px;color: rgb(23, 111, 11);");
                }
                if(qualityScore < g_MinEnrollQualityScore)
                {
                    scores[count+3]->setStyleSheet("background-color: rgb(255, 255, 255);  border:2px groove gray;  padding: 4px 4px;  border-radius: 8px;color: rgb(255, 0, 0);");
                }
                else
                {
                    scores[count+3]->setStyleSheet("background-color: rgb(255, 255, 255);  border:2px groove gray;  padding: 4px 4px;  border-radius: 8px;color: rgb(23, 111, 11);");
                }

                occlusions[count+3]->setText(QString::number(occlusionsScore,10));
                scores[count+3]->setText( QString::number(qualityScore,10));

#ifdef FEATURE_CHECK_EXIST
                //注册后检查该虹膜特征是否已经保存
                //@ for sdk
                IrisMatchIndex pMatchResult[IK_MAX_NUM_OF_MATCH];
                StopEnroll();
                LOG_INFO("begin IKIR_IdenMatch");
                int result = IKIR_IdenMatch(_enrCodeListR, _enrCodeNumR,enrIrisR[count].irisRecTemplate, pMatchResult);
                LOG_INFO("end IKIR_IdenMatch");
                if(result == 0)
                {
                    _isRightFeatureExisted = true;
                    _indexOfRightExistedFeature = pMatchResult[0].matchIndex;
                }

#endif
            }
        }


        ui->btnEnroll->setText(QObject::tr("重新注册"));
        LOG_INFO("重新注册--EnrollResult返回结果result = %d",result);
        ui->btnEnroll->setIcon(QIcon(":/image/start.ico"));
        ui->btnEnroll->setIconSize(QSize(40,40));
        ui->btnSaveEnrollResult->setEnabled(true);
    }
    else
    {
        //超时的时候不再显示图像
        SetEnrollStatus(true);

        DrawStatusLabel(StatusEnrFailed);
        ui->btnEnroll->setText(QObject::tr("重新注册"));
        LOG_INFO("重新注册--EnrollResult返回结果result = %d",result);
        ui->btnEnroll->setIcon(QIcon(":/image/start.ico"));
        ui->btnEnroll->setIconSize(QSize(40,40));
    }

    //20141209
    SetTraceVisible(false);


}

/*****************************************************************************
 *                         检查并设置眼睛模式
 *  函 数 名：CheckAndSetEyeMode
 *  功    能：扫描哪个radiobutton被选中，并设置相应的眼睛模式
 *  说    明：
 *  参    数：
 *  返 回 值：true：成功
 *           false：失败
 *  创 建 人：lizhl
 *  创建时间：2014-03-13
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
bool Enroll::CheckAndSetEyeMode()
{

    if(ui->rbtnAnyEye->isChecked())
    {
        _irisMode = IrisModeUndef;
    }
    else if(ui->rbtnBothEye->isChecked())
    {
        _irisMode = IrisModeBoth;
    }
    else if(ui->rbtnLeftEye->isChecked())
    {
        _irisMode = IrisModeLeft;
    }
    else if(ui->rbtnRightEye->isChecked())
    {
        _irisMode = IrisModeRight;
    }

    return true;
}


/*****************************************************************************
*                         去使能眼睛模式选择按钮
*  函 数 名：DisableEyeModeRbtn
*  功    能：去使能眼睛模式选择按钮
*  说    明：
*  参    数：
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2014-03-13
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::DisableEyeModeRbtn()
{
    ui->rbtnAnyEye->setEnabled(false);
    ui->rbtnBothEye->setEnabled(false);
    ui->rbtnLeftEye->setEnabled(false);
    ui->rbtnRightEye->setEnabled(false);
}

/*****************************************************************************
*                         使能眼睛模式选择按钮
*  函 数 名：EnableEyeModeRbtn
*  功    能：使能眼睛模式选择按钮
*  说    明：
*  参    数：
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2014-03-13
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::EnableEyeModeRbtn()
{
    ui->rbtnAnyEye->setEnabled(true);
    ui->rbtnBothEye->setEnabled(true);
    ui->rbtnLeftEye->setEnabled(true);
    ui->rbtnRightEye->setEnabled(true);
}

/*****************************************************************************
*                         点击注册结果左眼第一幅小图浏览大图
*  函 数 名：on_btnIrisImgLeft1_clicked
*  功    能：点击注册结果左眼第一幅小图浏览大图
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：lizhl
*  创建时间：2013-12-28
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_btnIrisImgLeft1_clicked()
{
    _dialogEnroll->XferIrisInfo(&s_enrIrisL[0]);
    _dialogEnroll->exec();
}

/*****************************************************************************
*                         点击注册结果左眼第二幅小图浏览大图
*  函 数 名：on_btnIrisImgLeft2_clicked
*  功    能：点击注册结果左眼第二幅小图浏览大图
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：lizhl
*  创建时间：2013-12-28
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_btnIrisImgLeft2_clicked()
{
    _dialogEnroll->XferIrisInfo(&s_enrIrisL[1]);
    _dialogEnroll->exec();
}

/*****************************************************************************
*                         点击注册结果左眼第三幅小图浏览大图
*  函 数 名：on_btnIrisImgLeft3_clicked
*  功    能：点击注册结果左眼第三幅小图浏览大图
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：lizhl
*  创建时间：2013-12-28
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_btnIrisImgLeft3_clicked()
{
    _dialogEnroll->XferIrisInfo(&s_enrIrisL[2]);
    _dialogEnroll->exec();
}

/*****************************************************************************
*                         点击注册结果右眼第一幅小图浏览大图
*  函 数 名：on_btnIrisImgRight1_clicked
*  功    能：点击注册结果右眼第一幅小图浏览大图
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：lizhl
*  创建时间：2013-12-28
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_btnIrisImgRight1_clicked()
{
    _dialogEnroll->XferIrisInfo(&s_enrIrisR[0]);
    _dialogEnroll->exec();
}

/*****************************************************************************
*                         点击注册结果右眼第二幅小图浏览大图
*  函 数 名：on_btnIrisImgRight2_clicked
*  功    能：点击注册结果右眼第二幅小图浏览大图
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：lizhl
*  创建时间：2013-12-28
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_btnIrisImgRight2_clicked()
{
    _dialogEnroll->XferIrisInfo(&s_enrIrisR[1]);
    _dialogEnroll->exec();
}

/*****************************************************************************
*                         点击注册结果右眼第三幅小图浏览大图
*  函 数 名：on_btnIrisImgRight3_clicked
*  功    能：点击注册结果右眼第三幅小图浏览大图
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：lizhl
*  创建时间：2013-12-28
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_btnIrisImgRight3_clicked()
{
    _dialogEnroll->XferIrisInfo(&s_enrIrisR[2]);
    _dialogEnroll->exec();
}



void Enroll::on_rbtn_clicked()
{
    Exectime etm(__FUNCTION__);
    QRadioButton *rbtns[] = {ui->rbtnAnyEye, ui->rbtnBothEye, ui->rbtnLeftEye, ui->rbtnRightEye};
    for(int index = 0; index < 4; index++)
    {
        if(rbtns[index]->isChecked())
        {
            rbtns[index]->setStyleSheet("QRadioButton{  font: bold;spacing: 10px;color:rgb(0, 0, 255);    };");
        }
        else
        {
            rbtns[index]->setStyleSheet("QRadioButton{  font: normal;spacing: 10px;color:rgb(0, 0, 0);    };");
        }
    }
}

void Enroll::InitLabelStatus()
{
    Exectime etm(__FUNCTION__);
    _labStatus = new QLabel(this,Qt::WindowStaysOnTopHint);
    //    _labStatus->setGeometry((1024-668)/2, 600-80, 668, 80);
    _labStatus->setGeometry(g_imgBeginX + g_statusBeginX, g_imgBeginY + g_statusBeginY, g_statusWidth, g_statusHeight);

    QFont serifFont("浪漫雅圆", 26, QFont::Bold);
    _labStatus->setFont(serifFont);
    _labStatus->setAlignment(Qt::AlignCenter);

    DrawStatusLabel(StatusEnrNotStart);
}

void Enroll::InitLabelMask()
{
    Exectime etm(__FUNCTION__);
    _labMask = new QLabel(this,/*Qt::SplashScreen | */Qt::WindowStaysOnTopHint/* | Qt::X11BypassWindowManagerHint*/ );
    _labMask->setGeometry(g_imgBeginX, g_imgBeginY, g_imgWidth, g_imgHeight);//178,104,664,498

    QImage image(":/image/ui_mask_green.png");
    image.scaled(g_imgWidth, g_imgHeight);
    QPixmap pic1(QPixmap::fromImage(image));

    _labMask->setPixmap(pic1);
}

void Enroll::InitTraceGif()
{
    Exectime etm(__FUNCTION__);
    Lx = g_defaultTraceLeftBeginX + g_defaultTraceSize/2;                   //170
    Ly = g_imgHeight - (g_defaultTraceLeftBeginY + g_defaultTraceSize/2)-30; //268
    Lr = 80;
    Rx = g_defaultTraceRightBeginX + g_defaultTraceSize/2;
    Ry = g_imgHeight - (g_defaultTraceRightBeginY + g_defaultTraceSize/2)-30; //424
    Rr = 80;

    _movTraceL = new QMovie(":/image/ui_trace_r.gif");
    _movTraceL->setScaledSize(QSize(g_defaultTraceSize, g_defaultTraceSize));

    _labTraceL = new QLabel(this);
    _labTraceL->setMovie(_movTraceL);
    _labTraceL->setGeometry(g_defaultTraceLeftBeginX + g_imgBeginX, g_defaultTraceLeftBeginY + g_imgBeginY, g_defaultTraceSize, g_defaultTraceSize);

    _movTraceR = new QMovie(":/image/ui_trace_r.gif");
    _movTraceR->setScaledSize(QSize(g_defaultTraceSize, g_defaultTraceSize));

    _labTraceR = new QLabel(this);
    _labTraceR->setMovie(_movTraceR);
    _labTraceR->setGeometry(g_defaultTraceRightBeginX + g_imgBeginX, g_defaultTraceRightBeginY + g_imgBeginY, g_defaultTraceSize, g_defaultTraceSize);

    //20141210
    _isClearL = _isClearR = _isLastClearL = _isLastClearR = false;

}

//20141208
void Enroll::SetTraceVisible(bool isvisible)
{
    if(isvisible)
    {
        _movTraceL->setFileName(":/image/ui_trace_r.gif");
        _movTraceL->restart();
        _labTraceL->setVisible(true);

        _movTraceR->setFileName(":/image/ui_trace_r.gif");
        _movTraceR->restart();
        _labTraceR->setVisible(true);
    }
    else
    {
        _movTraceL->stop();
        _labTraceL->setVisible(false);
        _movTraceR->stop();
        _labTraceR->setVisible(false);
    }

    _enrollWindowLabel->setVisible(isvisible);
}

void Enroll::DrawStatusLabel(StatusLabelType statusType)
{
    IKIrisDistFlag irisPositionFlg = GetCameraData().distEstimation;
    switch(statusType)
    {

    case StatusAlgIsPerform:
        if(DistTooFar == irisPositionFlg)

        {
            _labStatus->setVisible(true);
            _labStatus->setStyleSheet("QLabel{border-image:url(:/image/ui_bar_get_closer.png)}");
            _labStatus->setText("请靠近一点");

        }
        else if(DistTooClose == irisPositionFlg)
            // else if(UIUE_MsgID_EYE_TOO_CLOSE == irisPositionFlg)
        {
            _labStatus->setVisible(true);
            _labStatus->setStyleSheet("QLabel{border-image:url(:/image/ui_bar_get_further.png)}");
            _labStatus->setText("请远一点");
        }
        else
        {
            _labStatus->setVisible(false);
        }
        break;
    case StatusEnrStop:
    case StatusEnrNotStart:
        _labStatus->setStyleSheet("QLabel{border-image:url(:/image/ui_bar_wait.png)}");
        _labStatus->setText("请点击\"开始注册\"");
        _labStatus->setVisible(true);
        break;

    case StatusEnrFailed:
        _labStatus->setStyleSheet("QLabel{border-image:url(:/image/ui_bar_wait.png)}");
        _labStatus->setVisible(true);
        _labStatus->setText("请点击\"重新注册\"");
        break;

    case StatusEnrStart:
    case StatusEnrSuccess:
    case StatusCardAndIri:

    default:
        _labStatus->setVisible(false);
        break;
    }


}

//@for sdk 重写 人眼跟踪
void Enroll::DrawTraceGif(IKEnrIdenStatus enrIdenStatus)
{
    int Lx,Ly,Rx,Ry;

    //获取左右眼虹膜坐标
    DrawTracGif::GetTracPosition(enrIdenStatus,Lx,Ly,Rx,Ry);
    //显示左眼跟踪
    DrawTracGif::ShowTraceGift(enrIdenStatus,_movTraceL,_labTraceL,Lx,Ly,_isLastClearL);
    //显示右眼跟踪
    DrawTracGif::ShowTraceGift(enrIdenStatus,_movTraceR,_labTraceR,Rx,Ry,_isLastClearR);

}


void * Enroll::ThreadEnroll(void *arg)
{
    LOG_INFO("进入注册线程");
    g_isEnrollThreadEnd = 0;
    //检查窗口句柄是否为无效值
    if(NULL == arg)
    {
        return (void *)0;
        LOG_ERROR("传入窗口句柄无效");
    }
    //提取出当前窗口类对象指针
    Enroll* pWnd = static_cast<Enroll *>(arg);
    IKResultFlag  flag;
    IKAlgConfigStruct algConfig;

    algConfig.minEnrIdenTime = IK_MIN_ENR_TIME;
    algConfig.maxEnrIdenTime = IK_MAX_ENR_TIME;
    algConfig.reservedPara[IK_RESERVEDPARA_BLINK] = 0;
    algConfig.reservedPara[IK_RESERVEDPARA_I2FMODE] = I2FModeIris;
    s_isEnrollStoped = false;
    for(int i=0;i<g_constMaxEnrollImNum;i++)
    {
        memset(&enrIrisL[i],0,sizeof(IKSDKIrisInfo));
        memset(&enrIrisR[i],0,sizeof(IKSDKIrisInfo));
    }

    int funResult=IKUSBSDK_StartIrisEnroll(enrIrisL, 3, enrIrisR, 3, _irisMode, flag, &algConfig);

    if(E_USBSDK_OK == funResult)
    {
        //注册成功，处理注册结果
        s_enrIrisL    = enrIrisL;
        s_enrIrisR    = enrIrisR;
        s_enrLrFlag   = flag;

        if(_irisMode == IrisModeLeft)
        {
            s_leftEnrNum  = 3;
            s_rightEnrNum = 0;

        }
        else if(_irisMode == IrisModeRight)
        {
            s_leftEnrNum  = 0;
            s_rightEnrNum = 3;
        }
        else
        {
            s_leftEnrNum  = 3;
            s_rightEnrNum = 3;
        }

        s_enrollInteraction->PlayInteraction(EnrollSuccess, DistSuitable ,false);
        LOG_INFO("注册成功");
    }
    else if(E_USBSDK_DEFAULT_ERROR == funResult)
    {
        //注册失败，在界面显示注册失败
        s_enrollInteraction->PlayInteraction(EnrollFailed, DistSuitable ,false);
        LOG_INFO("注册失败");
    }
    else if(funResult == E_USBSDK_ENR_IDEN_INTERRUPT)
    {
        LOG_WARN("注册被终止");
    }
    s_enrollInteraction->ClearResult();
    LOG_INFO("注册结果 funResult=%d",funResult);
    imPaintEvent.RaisePaintEnrollResultEvent(funResult, enrIrisL, 3, enrIrisR, 3, flag);
    g_isEnrollThreadEnd = 1;
}

bool Enroll::StopEnroll()
{
    Exectime etm(__FUNCTION__);
    if(!_btnEnrollClicked)
    {
        LOG_DEBUG("注册没有开始，直接返回");
        return false;
    }

    if(s_isEnrollStoped)
    {
        LOG_WARN("注册已经被停止s_isEnrollStoped%d:",s_isEnrollStoped);
        return false;
    }

    int funResult = IKUSBSDK_StopEnroll();
    if(funResult < 0)
    {
        LOG_ERROR("停止注册失败，返回错误码：%d",funResult);
        return false;
    }

    int join =  0;//pthread_join(enrollThread, NULL);
    if(join == 0)
    {
        LOG_INFO("停止注册成功");
        SetEnrollStatus(true);
        SetTraceVisible(false);
        return true;
    }
    else
    {
        LOG_ERROR("停止注册时，回收注册线程失败,错误码：%d",join);
        return false;
    }
}


void Enroll::InitEnrollResultPic()
{


}

void Enroll::InitEnrollWindow()
{

    Exectime etm(__FUNCTION__);
    IdentWindow =new QPixmap(g_imgWidth,g_imgHeight);

    _enrollWindowLabel = new QLabel(this,Qt::WindowStaysOnTopHint );
    _enrollWindowLabel->setGeometry(g_imgBeginX, g_imgBeginY, g_imgWidth, g_imgHeight);//178,104,664,498
    IdentWindow->fill(QColor(255,255,255,0));
    QPainter painter(IdentWindow);

    QPen pen(QColor(255,255,255,0), 6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);

//    QPoint centL(int(Lx*g_scaling) ,int(Ly*g_scaling) );
//    QPoint centR(int(Rx*g_scaling) ,int(Ry*g_scaling) );
    QPoint centL(Lx+10 ,Ly );
    QPoint centR(Rx+10 ,Ry );

    QPainterPath path;
    path.addEllipse(centL,Lr,Lr);
    path.addEllipse(centR,Rr,Rr);
    path.addRect(0,0,g_imgWidth,g_imgHeight);

    painter.setBrush(QBrush(QColor(230,243,206,80),Qt::Dense2Pattern));
    path.setFillRule(Qt::OddEvenFill);
    painter.drawPath(path);
    painter.end();
    _enrollWindowLabel->setPixmap(*IdentWindow);

}
