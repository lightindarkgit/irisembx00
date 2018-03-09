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
#include "dialogenroll.h"
#include <sys/time.h>

//#include <QDebug>
#include <QDir>
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//为了临时演示加的代码，后期需要删除
#include "FeatureManageForDemo.h"

//VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV


APIIrisInfo *Enroll::s_enrIrisL = NULL;
APIIrisInfo *Enroll::s_enrIrisR = NULL;
LRSucFailFlag Enroll::s_enrLrFlag = EnrRecBothFailed;
Interaction *Enroll::s_enrollInteraction;
LRIrisClearFlag Enroll::s_lrIrisClearFlag;                           //图像是否清晰标志
IrisPositionFlag Enroll::s_irisPositionFlag;                         //图像位置信息标志

//unsigned char _grayCapImgBuffer[g_constIrisImgSize + g_constIrisPPMHeaderLength];   //采集图像buffer
//unsigned char _grayCapImgBuffer[g_constIrisImgSize + g_constBMPHeaderLength];//采集图像buffer，bmp格式

//--------------------------画图事件响应-----------------------------
ImagePaintEvent imPaintEvent;

/*****************************************************************************
*                         注册模块发射绘制采集到的图像信号
*  函 数 名：RaisePaintCapImgEvent
*  功    能：采图
*  说    明：
*  参    数：NULL
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void ImagePaintEvent::RaisePaintCapImgEvent()
{
    emit SigPaintCapImg();
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
*           lrSucFailFlag：输入参数 表明注册结果
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
void ImagePaintEvent::RaisePaintEnrollResultEvent(int result, APIIrisInfo* enrIrisL, int numEnrL, APIIrisInfo* enrIrisR, int numEnrR, LRSucFailFlag lrSucFailFlag)
{
    emit SigPaintEnrollResult(result, enrIrisL, numEnrL, enrIrisR, numEnrR, lrSucFailFlag);
}

//--------------------------画图事件响应-----------------------------

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
    ui(new Ui::Enroll)
{
    ui->setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint);
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(QRect(0, 0, 800, 600));
    setBackgroundColor(QColor(242,241,240));

    qRegisterMetaType<LRSucFailFlag>("LRSucFailFlag");
    qRegisterMetaType<IrisPositionFlag>("IrisPositionFlag");

    _dialogEnroll = new DialogEnroll(this);
    _dialogEnroll->hide();

    connect(&imPaintEvent,SIGNAL(SigPaintCapImg()),this,SLOT(CapImageUpdate()));
    connect(&imPaintEvent,SIGNAL(SigPaintEnrollResult(int, APIIrisInfo* , int , APIIrisInfo* , int, LRSucFailFlag)),
            this, SLOT(EnrollResultImageUpdate(int, APIIrisInfo* , int , APIIrisInfo* , int, LRSucFailFlag)),Qt::AutoConnection);


    _enrollAlgApi = new CAlgApi();
    _enrollAlgApi->Init();
//    int funRet = _enrollAlgApi->Init();
//    if(E_ALGAPI_OK != _enrollAlgApi->Init())
//    {
//        return;
//    }

    _signaledFlag = false;//初始化时先不采图，进入注册界面后调用SetPersonInfo再采图
    //启动采图线程，由于_signaledFlag为false，因此先不采图
    _capImgFlag = true;
    _capImgThread = std::thread(CapImgThread, (void*)this);


//    sprintf(_grayCapPpmHeader,"P5\n%d %d\n255\n", g_constIrisImgWidth,g_constIrisImgHeight);
//    memcpy(_grayCapImgBuffer,_grayCapPpmHeader,g_constIrisPPMHeaderLength);
//    memset(_grayCapImgBuffer+g_constIrisPPMHeaderLength,0,g_constIrisImgSize);

//    sprintf(_grayResultPpmHeader, "P5\n%d %d\n255\n", g_constIKImgWidth, g_constIKImgHeight);
//    memcpy(_grayResultImgBuffer, _grayResultPpmHeader, g_constIKPPMHeaderLength);
//    memset(_grayResultImgBuffer+g_constIKPPMHeaderLength, 0, g_constIKImgSize);

    //^^^^^^^^^^^^^^
    _frameNum = 0;
    _captureImageTimerId = this->startTimer(1000);
    //vvvvvvvvvvvvvv

//    s_enrollInteraction = new Interaction();
    s_enrollInteraction = Interaction::GetInteractionInstance();

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
    _capImgFlag = false;
    _capImgThread.join();
    delete _enrollAlgApi;
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
    //设置界面
    ui->widgetDisplayIm->show();
    ui->widgetEnrollResultIms->hide();
    ui->btnSaveEnrollResult->setEnabled(false);
    ui->btnEnroll->setText(QObject::tr("开始注册"));
    ui->btnEnroll->setIcon(QIcon(":/image/start.ico"));
    ui->btnEnroll->setIconSize(QSize(40,40));

    on_rbtnBothEye_clicked();//默认为双眼模式注册

    _personInfo = *personInfomation;

    CFeatureManageForDemo enrFeatureManage;
    enrFeatureManage.s_strUserName = QString::fromLocal8Bit(_personInfo.name.c_str());
    QString qstrDepartment = QString::fromLocal8Bit(_personInfo.depart.c_str());

    ui->lineEditUserName->setText(enrFeatureManage.s_strUserName);
    ui->lineEditDepartment->setText(qstrDepartment);


    _signaledFlag = true;//采图线程开始采图
    _btnEnrollClicked = false;//启动时注册按钮没有被点击

    on_rbtnBothEye_clicked();//默认为双眼模式注册

    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    _captureImageTimerId = this->startTimer(1000);
    //vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
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
    ui->widgetEnrollResultIms->hide();
    ui->widgetDisplayIm->show();

    ui->btnSaveEnrollResult->setEnabled(false);

    if(_btnEnrollClicked)
    {
        //如果注册按钮被点击过，则停止注册
        _enrollAlgApi->StopEnroll();
        ui->btnEnroll->setText(QObject::tr("开始注册"));
        ui->btnEnroll->setIcon(QIcon(":/image/start.ico"));
        ui->btnEnroll->setIconSize(QSize(40,40));
        _btnEnrollClicked = false;

    }
    else
    {
        //如果注册按钮没有被点击过，则开始注册
        ui->btnEnroll->setText(QObject::tr("停止注册"));
        ui->btnEnroll->setIcon(QIcon(":/image/stop.ico"));
        ui->btnEnroll->setIconSize(QSize(40,40));

        s_enrIrisL = NULL;
        s_enrIrisR = NULL;

//        int funResult = E_ALGAPI_DEFAUL_ERROR;
        _numEnrL = g_constMaxEnrollImNum;
        _numEnrR = g_constMaxEnrollImNum;

        _enrollAlgApi->AsyncStartEnroll(CBAsyncEnroll, _numEnrL, _numEnrR);
//        if(E_ALGAPI_OK != funResult)
//        {
//            std::cout << "AsyncEnroll failed" << std::endl;
//        }
        _btnEnrollClicked = true;
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
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_btnEnrollReturn_clicked()
{
    //不保存注册虹膜图像并退出
    _enrollAlgApi->StopEnroll();
    _signaledFlag = false;//注册界面关闭，不再采图
    close();
    emit SigEnrollResult(false);
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
    //保存注册虹膜图像并退出

    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    //为了临时演示加的代码，后期需要删除

    //注册得到的图像是上下翻转的，因此在保存时需要翻转一下，开辟一个buffer来存储翻转图像
//    unsigned char imgFlip[g_constIKImgSize];
//    char fileName[g_constLengthOfStrFunRts];
    char *fileName;

    FILE* bmpFile;

    CFeatureManageForDemo enrFeatureManage;
//    enrFeatureManage->InitParam();

//    enrFeatureManage->s_strUserName = "jiayan";
    enrFeatureManage.s_strUserName = QString::fromLocal8Bit(_personInfo.name.c_str());
    QString saveName;
    QByteArray byteAr;

    //保存左眼注册图像
    if( (EnrRecLeftSuccess == s_enrLrFlag) || (EnrRecBothSuccess == s_enrLrFlag) )
    {
        for (int count=0; count < _numEnrL; count++)
        {

            saveName = enrFeatureManage.s_strEnrCodePathLeft + "/" + enrFeatureManage.s_strUserName + ".left" + QString::number(count,10) +".enr";
            byteAr = saveName.toLocal8Bit();
            fileName = byteAr.data();
            FeatureSave(fileName, s_enrIrisL[count].IrisEnrTemplate, IR_ENR_FEATURE_LENGTH);

            //增加左眼特征
            enrFeatureManage.AddTemplate(s_enrIrisL[count].IrisEnrTemplate, enrFeatureManage.s_strUserName, FeatureLeft);

//            saveName = enrFeatureManage.s_strEnrImagePathLeft + "/" + enrFeatureManage.s_strUserName + ".left" + QString::number(count,10) +".ppm";
            saveName = enrFeatureManage.s_strEnrImagePathLeft + "/" + enrFeatureManage.s_strUserName + ".left" + QString::number(count,10) +".bmp";
            byteAr = saveName.toLocal8Bit();
            fileName = byteAr.data();
//            ImageMirror(imgFlip, s_enrIrisL[count].ImgData, g_constIKImgHeight, g_constIKImgWidth, false);
////            SaveGrayPPMFile(fileName, imgFlip, g_constIKImgWidth, g_constIKImgHeight);
//            SaveGrayBMPFile(fileName, imgFlip, g_constIKImgHeight, g_constIKImgWidth);

            bmpFile = fopen(fileName, "wb");
            if(bmpFile)
            {
                fwrite(s_enrIrisL[count].ImgData,1, g_constBMPHeaderLength+g_constIKImgSize,bmpFile);
            }
            fclose(bmpFile);

        }
    }


    //保存右眼注册图像
    if( (EnrRecRightSuccess == s_enrLrFlag) || (EnrRecBothSuccess == s_enrLrFlag))
    {
        for (int count=0; count < _numEnrR; count++)
        {
            saveName = enrFeatureManage.s_strEnrCodePathRight + "/" + enrFeatureManage.s_strUserName + ".right" + QString::number(count,10) +".enr";
            byteAr = saveName.toLocal8Bit();
            fileName = byteAr.data();
            FeatureSave(fileName, s_enrIrisR[count].IrisEnrTemplate, IR_ENR_FEATURE_LENGTH);

            //增加右眼特征
            enrFeatureManage.AddTemplate(s_enrIrisR[count].IrisEnrTemplate, enrFeatureManage.s_strUserName, FeatureRight);

//            saveName = enrFeatureManage.s_strEnrImagePathRight + "/" + enrFeatureManage.s_strUserName + ".right" + QString::number(count,10) +".ppm";
            saveName = enrFeatureManage.s_strEnrImagePathRight + "/" + enrFeatureManage.s_strUserName + ".right" + QString::number(count,10) +".bmp";
            byteAr = saveName.toLocal8Bit();
            fileName = byteAr.data();
//            ImageMirror(imgFlip, s_enrIrisR[count].ImgData, g_constIKImgHeight, g_constIKImgWidth, false);
////            SaveGrayPPMFile(fileName, imgFlip, g_constIKImgWidth, g_constIKImgHeight);
//            SaveGrayBMPFile(fileName, imgFlip, g_constIKImgHeight, g_constIKImgWidth);

            bmpFile = fopen(fileName, "wb");
            if(bmpFile)
            {
                fwrite(s_enrIrisR[count].ImgData,1, g_constBMPHeaderLength+g_constIKImgSize,bmpFile);
            }
            fclose(bmpFile);
        }
    }


    //VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV

    _enrollAlgApi->StopEnroll();//由于UI界面对保存按钮做了设置，因此如果能够点击“保存”按钮，则注册已经停止了，这里其实可以不用再停止了
    _signaledFlag = false;//注册界面关闭，不再采图
    close();
    emit SigEnrollResult(true);
}

/*****************************************************************************
*                         注册模块将采集到的图像显示到界面
*  函 数 名：CapImageUpdate
*  功    能：注册模块将采集到的图像显示到界面
*  说    明：
*  参    数：
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::CapImageUpdate()
{
    std::lock_guard<std::mutex> lck (_capImgMutex, std::adopt_lock);
    //^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
    //为了存图用
//    struct tm *ptmNowTime;
//    struct timeval tv_NowTime;
//    time_t lt;
//    char cTimeName[100];
//    static int n=0;

//    if( n < 100 )
//    {
//        lt =time(NULL);
//        ptmNowTime = localtime(&lt);
//        gettimeofday(&tv_NowTime,NULL);
//        sprintf(cTimeName,"./capimg/direct%02d_%02d_%02d_%02d_%02d_%04ld.bmp",ptmNowTime->tm_mon+1,ptmNowTime->tm_mday,ptmNowTime->tm_hour,ptmNowTime->tm_min, ptmNowTime->tm_sec,tv_NowTime.tv_usec);
////        sprintf(cTimeName,"capimg/%02d_%02d_%02d_%02d_%02d_%04ld.ppm",ptmNowTime->tm_mon+1,ptmNowTime->tm_mday,ptmNowTime->tm_hour,ptmNowTime->tm_min, ptmNowTime->tm_sec,tv_NowTime.tv_usec);
////        SaveGrayPPMFile(cTimeName, _grayImgBuffer+g_constIrisPPMHeaderLength,g_constIrisImgWidth,g_constIrisImgHeight);
//     }
//    n++;
    //vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

    QImage imageBig;
    imageBig = QImage::fromData(_grayCapImgBuffer, g_constIrisImgSize + g_constBMPHeaderLength);//, "BMP"

    if(!imageBig.isNull())
    {
        QImage paintImg = imageBig.convertToFormat(QImage::Format_ARGB32_Premultiplied);
        s_enrollInteraction->PlayInteraction(ResultUnknown, s_lrIrisClearFlag, s_irisPositionFlag, paintImg);
//        s_enrollInteraction->DisplayClearBlur(paintImg,_lrIrisClearFlag);

        paintImg = paintImg.scaled (ui->labRealTimeIm->width(),
                             ui->labRealTimeIm->height(),
                             Qt::KeepAspectRatio);

        ui->labRealTimeIm->setPixmap(QPixmap::fromImage(paintImg));


//        imageBig = imageBig.scaled (ui->labRealTimeIm->width(),
//                             ui->labRealTimeIm->height(),
//                             Qt::KeepAspectRatio);

//        ui->labRealTimeIm->setPixmap(QPixmap::fromImage(imageBig));

    }

    //^^^^
    _frameNum++;
    //vvvv
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
*           lrSucFailFlag：输入参数 表明注册结果
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::EnrollResultImageUpdate(int result, APIIrisInfo* enrIrisL, int numEnrL, APIIrisInfo* enrIrisR, int numEnrR, LRSucFailFlag lrEnrSucFailFlag)
{
    _btnEnrollClicked = false;
    ui->btnEnroll->setText(QObject::tr("重新注册"));
    ui->btnEnroll->setIcon(QIcon(":/image/start.ico"));
    ui->btnEnroll->setIconSize(QSize(40,40));

    //用于保存翻转图像
//    unsigned char mirrorImg[g_constIKImgSize+g_constBMPHeaderLength];

    if(E_ALGAPI_OK == result)
    {
        ui->btnSaveEnrollResult->setEnabled(true);

        ui->widgetDisplayIm->hide();
        ui->widgetEnrollResultIms->show();


        QImage resultImg;

        QPushButton *btns[] = {ui->btnIrisImgLeft1, ui->btnIrisImgLeft2, ui->btnIrisImgLeft3,
                               ui->btnIrisImgRight1, ui->btnIrisImgRight2, ui->btnIrisImgRight3};

        QLineEdit * scores[] = {ui->lineEditScoreLeft1,ui->lineEditScoreLeft2,ui->lineEditScoreLeft3,
                                ui->lineEditScoreRight1,ui->lineEditScoreRight2,ui->lineEditScoreRight3};

        QLineEdit * occlusions[] = {ui->lineEditOcclusionLeft1,ui->lineEditOcclusionLeft2,ui->lineEditOcclusionLeft3,
                                    ui->lineEditOcclusionRight1,ui->lineEditOcclusionRight2,ui->lineEditOcclusionRight3};

        int qualityScore = 0;
        int occlusionsScore = 0;

        for(int count = 0; count < numEnrL; count++)
        {
//            ImageMirror(_grayResultImgBuffer+g_constIKPPMHeaderLength, enrIrisL[count].LocImage, g_constIKImgHeight, g_constIKImgWidth, false);
//            resultImg = QImage::fromData(_grayResultImgBuffer, g_constIKImgSize+g_constIKPPMHeaderLength, "PPM");

//            ImageMirror(mirrorImg, enrIrisL[count].LocImage, g_constIKImgHeight, g_constIKImgWidth, false);
//            ConvertRaw2Bmp(_grayResultImgBuffer, mirrorImg, g_constIKImgHeight, g_constIKImgWidth);
//            resultImg = QImage::fromData(_grayResultImgBuffer, g_constIKImgSize+g_constBMPHeaderLength, "BMP");

            resultImg = QImage::fromData(enrIrisL[count].LocImage, g_constIKImgSize+g_constBMPHeaderLength, "BMP");

//            resultImg = resultImg.scaled(320,240,Qt::IgnoreAspectRatio);
            resultImg = resultImg.scaled(btns[count]->width(),btns[count]->height(),Qt::IgnoreAspectRatio);

            btns[count]->setPixmap(QPixmap::fromImage(resultImg));
//            btns[count]->resize(QSize(320,240));
            btns[count]->resize(btns[count]->width(),btns[count]->height());

            qualityScore = enrIrisL[count].QualityScore;
            occlusionsScore = enrIrisL[count].PercentVisible;

            if(occlusionsScore < MinEnrollOcclusion)
            {
                occlusions[count]->setStyleSheet("color: rgb(255, 0, 0);");
            }
            else
            {
                occlusions[count]->setStyleSheet("color: rgb(23, 111, 11);");
            }
            if(qualityScore < MinEnrollQualityScore)
            {
                scores[count]->setStyleSheet("color: rgb(255, 0, 0);");
            }
            else
            {
                scores[count]->setStyleSheet("color: rgb(23, 111, 11);");
            }

            occlusions[count]->setText(QString::number(occlusionsScore,10));
            scores[count]->setText( QString::number(qualityScore,10));
        }

        for(int count = 0; count < numEnrR; count++)
        {
//            ImageMirror(_grayResultImgBuffer+g_constIKPPMHeaderLength, enrIrisR[count].LocImage, g_constIKImgHeight, g_constIKImgWidth, false);
//            resultImg = QImage::fromData(_grayResultImgBuffer, g_constIKImgSize+g_constIKPPMHeaderLength, "PPM");

//            ImageMirror(mirrorImg, enrIrisR[count].LocImage, g_constIKImgHeight, g_constIKImgWidth, false);
//            ConvertRaw2Bmp(_grayResultImgBuffer, mirrorImg, g_constIKImgHeight, g_constIKImgWidth);
//            resultImg = QImage::fromData(_grayResultImgBuffer, g_constIKImgSize+g_constBMPHeaderLength, "BMP");

            resultImg = QImage::fromData(enrIrisR[count].LocImage, g_constIKImgSize+g_constBMPHeaderLength, "BMP");

//            resultImg = resultImg.scaled(320,240,Qt::IgnoreAspectRatio);
            resultImg = resultImg.scaled(btns[count+3]->width(),btns[count+3]->height(),Qt::IgnoreAspectRatio);

            btns[count+3]->setPixmap(QPixmap::fromImage(resultImg));
//            btns[count+3]->resize(QSize(320,240));
//            btns[count+3]->resize(btns[count+3]->width(),btns[count+3]->height());

            qualityScore = enrIrisR[count].QualityScore;
            occlusionsScore = enrIrisR[count].PercentVisible;

            if(occlusionsScore < MinEnrollOcclusion)
            {
                occlusions[count+3]->setStyleSheet("color: rgb(255, 0, 0);");
            }
            else
            {
                occlusions[count+3]->setStyleSheet("color: rgb(23, 111, 11);");
            }
            if(qualityScore < MinEnrollQualityScore)
            {
                scores[count+3]->setStyleSheet("color: rgb(255, 0, 0);");
            }
            else
            {
                scores[count+3]->setStyleSheet("color: rgb(23, 111, 11);");
            }

            occlusions[count+3]->setText(QString::number(occlusionsScore,10));
            scores[count+3]->setText( QString::number(qualityScore,10));
        }


//        ui->btnIrisImgLeft1->setIconSize(QSize(240,320));
//        ui->btnIrisImgLeft1->resize(QSize(240,320));
//        ui->btnIrisImgLeft2->setIconSize(QSize(240,320));
//        ui->btnIrisImgLeft3->setIconSize(QSize(240,320));
//        ui->btnIrisImgRight1->setIconSize(QSize(240,320));
//        ui->btnIrisImgRight2->setIconSize(QSize(240,320));
//        ui->btnIrisImgRight3->setIconSize(QSize(240,320));


    }

}



/*****************************************************************************
*                         注册模块采图线程
*  函 数 名：CapImgThread
*  功    能：采图
*  说    明：
*  参    数：pParam：输入参数
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void* Enroll::CapImgThread(void* pParam)
{
    Enroll *pCGDlg = static_cast <Enroll*> (pParam);
    int funResult = E_ALGAPI_ERROR_BASE;

    while(pCGDlg->_capImgFlag)
    {
        if(pCGDlg->_signaledFlag)
        {
            std::lock_guard<std::mutex> lck (pCGDlg->_capImgMutex, std::adopt_lock);
//            funResult = pCGDlg->_enrollAlgApi->SyncCapIrisImg(pCGDlg->_grayCapImgBuffer + g_constIrisPPMHeaderLength, pCGDlg->_lrIrisClearFlag, pCGDlg->_irisPositionFlag);
//            funResult = pCGDlg->_enrollAlgApi->SyncCapIrisImg(g_infraredDistanceValue, pCGDlg->_grayCapImgBuffer, s_lrIrisClearFlag, s_irisPositionFlag);
            funResult = pCGDlg->_enrollAlgApi->SyncCapIrisImg(pCGDlg->s_enrollInteraction->s_infraredDistanceValue, pCGDlg->_grayCapImgBuffer, s_lrIrisClearFlag, s_irisPositionFlag);
            if(E_ALGAPI_OK == funResult)
            {
                imPaintEvent.RaisePaintCapImgEvent();

                //^^^^^^^^^^^^^^^
//                pCGDlg->_frameNum++;
                //vvvvvvvvvvvvvvv
            }
        }
        else
        {
//        usleep(20);
//            sleep(0);
        }
    }
    return (void*)0;
}

/*****************************************************************************
*                         异步注册回调函数
*  函 数 名：
*  功    能：异步注册回调函数
*  说    明：在回调函数中取异步注册结果
*  参    数：
*  返 回 值：
*  创 建 人：lizhl
*  创建时间：2013-12-26
*  修 改 人：
*  修改时间：
*****************************************************************************/
int Enroll::CBAsyncEnroll(int funResult, APIIrisInfo* enrIrisL, int numEnrL, APIIrisInfo* enrIrisR, int numEnrR, LRSucFailFlag lrFlag)
{
    QImage img;//处理注册结果时目前只是为了适应ProcessResult的接口参数，实际没有使用

    if(E_ALGAPI_OK == funResult)
    {
        //注册成功，处理注册结果
        s_enrIrisL = enrIrisL;
        s_enrIrisR = enrIrisR;
        s_enrLrFlag = lrFlag;

//        s_enrollInteraction->ProcessResult(EnrollSuccess, img);
        s_enrollInteraction->PlayInteraction(EnrollSuccess, s_lrIrisClearFlag, s_irisPositionFlag, img);

    }
    else if(E_ALGAPI_ENR_IDEN_FAILED == funResult)
    {
        //注册失败，在界面显示注册失败
        //TO DO
//        s_enrollInteraction->ProcessResult(EnrollFailed, img);
        s_enrollInteraction->PlayInteraction(EnrollFailed, s_lrIrisClearFlag, s_irisPositionFlag, img);

    }
    else
    {
        //其他情况，如注册过程中注册被人为停止，不再处理，直接返回即可

    }

    s_enrollInteraction->ClearResult();

    imPaintEvent.RaisePaintEnrollResultEvent(funResult, enrIrisL, numEnrL, enrIrisR, numEnrR, lrFlag);
    return 0;
}

/*****************************************************************************
*                         设置注册模式为双眼模式
*  函 数 名：on_rbtnBothEye_clicked
*  功    能：设置注册模式为双眼模式
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：lizhl
*  创建时间：2013-12-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_rbtnBothEye_clicked()
{
    int funResult = E_ALGAPI_DEFAUL_ERROR;
    funResult = _enrollAlgApi->GetParam(&_enrollParamStruct);
    if(E_ALGAPI_OK == funResult)
    {
        _enrollParamStruct.EyeMode = BothEye;
        funResult = _enrollAlgApi->SetParam(&_enrollParamStruct);
    }

    return;
}

/*****************************************************************************
*                         设置注册模式为左眼模式
*  函 数 名：on_rbtnLeftEye_clicked
*  功    能：设置注册模式为左眼模式
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：lizhl
*  创建时间：2013-12-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_rbtnLeftEye_clicked()
{
    int funResult = E_ALGAPI_DEFAUL_ERROR;
    funResult = _enrollAlgApi->GetParam(&_enrollParamStruct);
    if(E_ALGAPI_OK == funResult)
    {
        _enrollParamStruct.EyeMode = LeftEye;
        funResult = _enrollAlgApi->SetParam(&_enrollParamStruct);
    }
    return;
}

/*****************************************************************************
*                         设置注册模式为右眼模式
*  函 数 名：on_rbtnRightEye_clicked
*  功    能：设置注册模式为右眼模式
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：lizhl
*  创建时间：2013-12-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_rbtnRightEye_clicked()
{
    int funResult = E_ALGAPI_DEFAUL_ERROR;
    funResult = _enrollAlgApi->GetParam(&_enrollParamStruct);
    if(E_ALGAPI_OK == funResult)
    {
        _enrollParamStruct.EyeMode = RightEye;
        funResult = _enrollAlgApi->SetParam(&_enrollParamStruct);
    }
    return;
}

/*****************************************************************************
*                         设置注册模式为任意眼模式
*  函 数 名：on_rbtnAnyEye_clicked
*  功    能：设置注册模式为任意眼模式
*  说    明：
*  参    数：
*  返 回 值：
*  创 建 人：lizhl
*  创建时间：2013-12-27
*  修 改 人：
*  修改时间：
*****************************************************************************/
void Enroll::on_rbtnAnyEye_clicked()
{
    int funResult = E_ALGAPI_DEFAUL_ERROR;
    funResult = _enrollAlgApi->GetParam(&_enrollParamStruct);
    if(E_ALGAPI_OK == funResult)
    {
        _enrollParamStruct.EyeMode = AnyEye;
        funResult = _enrollAlgApi->SetParam(&_enrollParamStruct);
    }
    return;
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

//^^^^^^^^^^^^^^^^^^^^^^^^
void Enroll::timerEvent(QTimerEvent *event)
{
    QString strFrameNum;
    if (_captureImageTimerId == event->timerId())
    {
        strFrameNum.setNum(_frameNum);
        ui->lineEditDepartment->setText(strFrameNum);
//        ui->label_18->setText(strFrameNum);
        _frameNum = 0;
    }

}
//vvvvvvvvvvvvvvvvvvvvvvvvvv

