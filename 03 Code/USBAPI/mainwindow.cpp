/******************************************************************************************
** 文件名:   MAINWINDOW_CPP
×× 主要类:   MainWindow
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   刘中昌
** 日  期:   2013-12-20
** 修改人:
** 日  期:
** 描  述:  主界面实现
 *×× 主要模块说明:
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "usbcommon.h"
#include <QTimer>



unsigned char g_grayImBuffer[TransBufferSize + PPMHeaderLength];
char g_grayPpmHeader[PPMHeaderLength];

ImagePaintEvent imPaintEvent;
void ImagePaintEvent::RaisePaintEvent()
{
    emit sigPaint();
}

void MainWindow::IKSaveGrayFile(const char* filename,unsigned char *pImgBuffer, int nWidth, int nHeight)
{
    struct tm *ptmNowTime;
    struct timeval tv_NowTime;
    time_t lt;
    char cTimeName[200];
    static int n=0;

    lt =time(NULL);
    ptmNowTime = localtime(&lt);
    gettimeofday(&tv_NowTime,NULL);
    sprintf(cTimeName,"./%s_%02d_%02d_%02d_%02d_%02d_%04ld.ppm",&filename,ptmNowTime->tm_mon+1,ptmNowTime->tm_mday,ptmNowTime->tm_hour,ptmNowTime->tm_min, ptmNowTime->tm_sec,tv_NowTime.tv_usec);

    FILE* ff = NULL;
    ff=fopen(cTimeName,"wb");
    if(ff)
    {
        fprintf(ff, "P5\n" "%d %d\n255\n", nWidth, nHeight);
        fwrite(pImgBuffer, 1, nWidth * nHeight , ff);
        fclose(ff);
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _displayButtonClicked=true;
    //隐藏标题栏
    //setWindowFlags(Qt::CustomizeWindowHint);
    //_imageBuf = new unsigned char[TransBufferSize];
    _grayImag = new unsigned char[TransBufferSize];
    _count =0;

    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(ShowCount()));
    timer->start(1000);
    Open();
  //_ikUSBAPI.Close();
    pthread_mutex_init(&_imageMutex,nullptr);

    sprintf(g_grayPpmHeader,"P5\n%d %d\n255\n", ImageWidth,ImageHeight);
    memcpy(g_grayImBuffer,g_grayPpmHeader,PPMHeaderLength);
    memset(g_grayImBuffer+PPMHeaderLength,0,TransBufferSize);


    connect(&imPaintEvent,SIGNAL(sigPaint()),this,SLOT(imageUpdate()));
    pthread_create(&_pthreadID, 0, ThreadDisplayImage, (void*)this);

    imageUpdate();
}

MainWindow::~MainWindow()
{
    Close();
    delete[] _grayImag;
    pthread_cancel(_pthreadID);
    pthread_join(_pthreadID, nullptr);
    pthread_mutex_destroy(&_imageMutex);
    delete ui;
}
/**********************************************************************************************************************************
  *                        显示帧率
  *  函 数 名：ShowCount
  *  功    能：显示帧率
  *  说    明：
  *  参    数：
  *  返 回 值：
  *  创 建 人：刘中昌
  *  创建时间：2013-12-23
  *  修 改 人：
  *  修改时间：
  *********************************************************************************************************************************/
void MainWindow::ShowCount()
{
    ui->labCount->setText(QString::number(_count,10));
    _count=0;
}
/**********************************************************************************************************************************
  *                        打开USB传输
  *  函 数 名：Open
  *  功    能：打开USB传输
  *  说    明：
  *  参    数：
  *  返 回 值：
  *  创 建 人：刘中昌
  *  创建时间：2013-12-23
  *  修 改 人：
  *  修改时间：
  *********************************************************************************************************************************/
void MainWindow::Open()
{
    if(!_isOpen)
    {
     //SimpleLockForMutex lock(_imageMutex);
     _ikUSBAPI.Open();
     _waiter.signal();
     _isOpen = true;
    }

}
/**********************************************************************************************************************************
  *                      关闭USB
  *  函 数 名：Close
  *  功    能：关闭USB接口
  *  说    明：
  *  参    数：
  *  返 回 值：
  *  创 建 人：刘中昌
  *  创建时间：2013-12-23
  *  修 改 人：
  *  修改时间：
  *********************************************************************************************************************************/
void MainWindow::Close()
{
    //SimpleLockForMutex lock(_imageMutex);
    if(_isOpen)
    {
        _ikUSBAPI.Close();
        _isOpen = false;
    }
}
/**********************************************************************************************************************************
  *                        更新界面图像函数
  *  函 数 名：imageUpdate
  *  功    能：更新界面
  *  说    明：
  *  参    数：
  *  返 回 值：
  *  创 建 人：刘中昌
  *  创建时间：2013-12-23
  *  修 改 人：
  *  修改时间：
  *********************************************************************************************************************************/
void MainWindow::imageUpdate()
{
   // SimpleLockForMutex lock(_imageMutex);
    unsigned int length=0;

    if(E_OK !=_ikUSBAPI.GetImage(_grayImag,length))
    {
        return;
    }
    memcpy(g_grayImBuffer+PPMHeaderLength,_grayImag,TransBufferSize);

    //IKSaveGrayFile("test",_grayImag,ImageWidth,ImageHeight);

    _imageBig = QImage::fromData(g_grayImBuffer, TransBufferSize+PPMHeaderLength, "PPM");
    if(!_imageBig.isNull())
    {
        _imageBig = _imageBig.scaled (ui->labImg->width()-1,
                                      ui->labImg->height()-1,
                                      Qt::KeepAspectRatio);

        ui->labImg->setPixmap(QPixmap::fromImage(_imageBig));
        _count++;
    }
    _waiter.signal();
}
/**********************************************************************************************************************************
  *                       线程获取图像
  *  函 数 名：ThreadDisplayImage
  *  功    能：获得当前图像
  *  说    明：未获得数据阻塞
  *  参    数：MainWindow
  *  返 回 值：
  *  创 建 人：刘中昌
  *  创建时间：2013-12-23
  *  修 改 人：
  *  修改时间：
  *********************************************************************************************************************************/
void* MainWindow::ThreadDisplayImage(void* pParam)
{
   MainWindow *pCGDlg = (MainWindow*) pParam;
    while(pCGDlg->_displayButtonClicked)
    {
         //SimpleLockForMutex lock(pCGDlg->_imageMutex);

        pCGDlg->_waiter.wait();
        imPaintEvent.RaisePaintEvent();

    }

    return ((void*)0);
}
