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
#include <unistd.h>
#include "../Common/Exectime.h"


unsigned char g_grayImBuffer[PreviewImageSize + PPMHeaderLength];
unsigned char g_leftImBuffer[PreviewImageSize + PPMHeaderLength];
unsigned char g_rightImBuffer[PreviewImageSize + PPMHeaderLength];
char g_grayPpmHeader[PPMHeaderLength];

int countPer = 0;
static int countBMP = 1;
ImagePaintEvent imPaintEvent;
void ImagePaintEvent::RaisePaintEvent()
{
	Exectime etm(__FUNCTION__);
	emit sigPaint();
}

void ImagePaintEvent::RaisePaintEvent1()
{
	Exectime etm(__FUNCTION__);
	emit sigPaint1();
}
QImage imageBig;
void MainWindow::IKSaveGrayFile(const char* filename,unsigned char *pImgBuffer, int nWidth, int nHeight)
{
	Exectime etm(__FUNCTION__);
	struct tm *ptmNowTime;
	struct timeval tv_NowTime;
	time_t lt;
	char cTimeName[200];
	static int n=0;

	lt =time(NULL);
	ptmNowTime = localtime(&lt);
	gettimeofday(&tv_NowTime,NULL);
	countBMP %= 100;
	//sprintf(cTimeName,"./%02dgray_%02d.ppm",countPer,countBMP++);
	//sprintf(cTimeName,"./%s_%02d_%02d_%02d_%02d_%02d_%04ld.ppm",&filename,ptmNowTime->tm_mon+1,ptmNowTime->tm_mday,ptmNowTime->tm_hour,ptmNowTime->tm_min, ptmNowTime->tm_sec,tv_NowTime.tv_usec);
	sprintf(cTimeName,"./%02dgray_%02d.bmp",countPer,countBMP++);
	imageBig.save( cTimeName,"BMP");
	//    FILE* ff = NULL;
	//    ff=fopen(cTimeName,"wb");
	//    if(ff)
	//    {
	//       // fprintf(ff, "P5\n" "%d %d\n255\n", nWidth, nHeight);
	////        fwrite(pImgBuffer, 1, nWidth * nHeight , ff);
	//        fwrite(g_grayImBuffer, 1, TransBufferSize + PPMHeaderLength , ff);
	//        fclose(ff);
	//    }
}


//void MainWindow::IKSaveGrayFile(const char* filename,unsigned char *pImgBuffer, int nWidth, int nHeight)
//{
////    struct tm *ptmNowTime;
////    struct timeval tv_NowTime;
////    time_t lt;
////    char cTimeName[200];
////    static int n=0;

////    lt =time(NULL);
////    ptmNowTime = localtime(&lt);
////    gettimeofday(&tv_NowTime,NULL);
//    countBMP %= 50;
//    sprintf(cTimeName,"./%02dgray_%02d.ppm",countPer,countBMP++);
//    //sprintf(cTimeName,"./%s_%02d_%02d_%02d_%02d_%02d_%04ld.ppm",&filename,ptmNowTime->tm_mon+1,ptmNowTime->tm_mday,ptmNowTime->tm_hour,ptmNowTime->tm_min, ptmNowTime->tm_sec,tv_NowTime.tv_usec);
//   sprintf(cTimeName,"./%02dgray_%02d.bmp",countPer,countBMP++);
//   imageBig.save( cTimeName,"BMP");
////    FILE* ff = NULL;
////    ff=fopen(cTimeName,"wb");
////    if(ff)
////    {
////       // fprintf(ff, "P5\n" "%d %d\n255\n", nWidth, nHeight);
//////        fwrite(pImgBuffer, 1, nWidth * nHeight , ff);
////        fwrite(g_grayImBuffer, 1, TransBufferSize + PPMHeaderLength , ff);
////        fclose(ff);
////    }
//}

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	Exectime etm(__FUNCTION__);
	ui->setupUi(this);
	_displayButtonClicked=true;
	//隐藏标题栏
	//setWindowFlags(Qt::CustomizeWindowHint);
	//_imageBuf = new unsigned char[TransBufferSize];
	_grayImag = new unsigned char[PreviewImageSize];
	_leftImag = new unsigned char[PreviewImageSize];
	_rightImag = new unsigned char[PreviewImageSize];
	_count =0;

	QTimer *timer = new QTimer(this);
	connect(timer,SIGNAL(timeout()),this,SLOT(ShowCount()));
	timer->start(1000);

	QTimer *timerT = new QTimer(this);
	connect(timerT,SIGNAL(timeout()),this,SLOT(Test()));
	timerT->start(1000 * 5);

	Open();
	//_ikUSBAPI.Close();
	pthread_mutex_init(&_imageMutex,nullptr);

	sprintf(g_grayPpmHeader,"P5\n%d %d\n255\n", ImageWidth/2,ImageHeight);
	memcpy(g_grayImBuffer,g_grayPpmHeader,PPMHeaderLength);
	memset(g_grayImBuffer+PPMHeaderLength,0,PreviewImageSize);
	memcpy(g_leftImBuffer,g_grayPpmHeader,PPMHeaderLength);
	memset(g_leftImBuffer+PPMHeaderLength,0,PreviewImageSize);
	memcpy(g_rightImBuffer,g_grayPpmHeader,PPMHeaderLength);
	memset(g_rightImBuffer+PPMHeaderLength,0,PreviewImageSize);


	connect(&imPaintEvent,SIGNAL(sigPaint()),this,SLOT(imageUpdate()));
	connect(&imPaintEvent,SIGNAL(sigPaint1()),this,SLOT(GetMinPhono()));
	pthread_create(&_pthreadID, 0, ThreadDisplayImage, (void*)this);
	pthread_create(&_pthreadID1, 0, ThreadDisplayImage1, (void*)this);
	imageUpdate();
	GetMinPhono();
}
void MainWindow::Test()
{
	Exectime etm(__FUNCTION__);

	if(_isOpen)
	{
		Close();
	}
	else
	{
		Open();
	}
}

MainWindow::~MainWindow()
{
	Exectime etm(__FUNCTION__);
	Close();
	delete[] _grayImag;
	//    pthread_cancel(_pthreadID);
	//    pthread_join(_pthreadID, nullptr);
	//    pthread_join(_pthreadID1, nullptr);

	if(!pthread_cancel(_pthreadID))
	{
		pthread_join(_pthreadID, nullptr);
	}

	if(!pthread_cancel(_pthreadID1))
	{
		pthread_join(_pthreadID1, nullptr);
	}
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
	Exectime etm(__FUNCTION__);
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
	Exectime etm(__FUNCTION__);
	if(!_isOpen)
	{
		_isOpen = true;
		_ikUSBAPI.Open();

		//        connect(&imPaintEvent,SIGNAL(sigPaint()),this,SLOT(imageUpdate()));
		//        connect(&imPaintEvent,SIGNAL(sigPaint1()),this,SLOT(GetMinPhono()));
		//        pthread_create(&_pthreadID, 0, ThreadDisplayImage, (void*)this);
		//        pthread_create(&_pthreadID1, 0, ThreadDisplayImage1, (void*)this);
		imageUpdate();
		usleep(10000);
		GetMinPhono();

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
	Exectime etm(__FUNCTION__);
	if(_isOpen)
	{
		//        disconnect(&imPaintEvent,SIGNAL(sigPaint()),this,SLOT(imageUpdate()));
		//        disconnect(&imPaintEvent,SIGNAL(sigPaint1()),this,SLOT(GetMinPhono()));
		_ikUSBAPI.Close();
		_isOpen = false;
	}

}

void MainWindow::GetMinPhono()
{
	Exectime etm(__FUNCTION__);
	unsigned int len1=0,len2=0;
	Command cmd(10,10,1000,10);

	if(_ikUSBAPI.GetPictureTaken(cmd,_leftImag,len1,_rightImag,len2) !=0)
	{
		return;
	}


	memcpy(g_leftImBuffer+PPMHeaderLength,_leftImag,PreviewImageSize);
	memcpy(g_rightImBuffer+PPMHeaderLength,_rightImag,PreviewImageSize);

	//IKSaveGrayFile("test",_grayImag,ImageWidth,ImageHeight);

	QImage imageBig1,imageBig2;
	imageBig1 = QImage::fromData(g_leftImBuffer, PreviewImageSize+PPMHeaderLength, "PPM");
	imageBig2 = QImage::fromData(g_rightImBuffer, PreviewImageSize+PPMHeaderLength, "PPM");
	if(!imageBig1.isNull() && !imageBig2.isNull())
	{
		imageBig1 = imageBig1.scaled (ui->labLeft->width()-1,
				ui->labLeft->height()-1,
				Qt::KeepAspectRatio);

		ui->labLeft->setPixmap(QPixmap::fromImage(imageBig1));

		imageBig2 = imageBig2.scaled (ui->labRight->width()-1,
				ui->labRight->height()-1,
				Qt::KeepAspectRatio);

		ui->labRight->setPixmap(QPixmap::fromImage(imageBig2));
		_count++;
		_waiter1.signal();
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
	Exectime etm(__FUNCTION__);
	// SimpleLockForMutex lock(_imageMutex);
	unsigned int length=0;
	if(E_OK !=_ikUSBAPI.GetImage(_grayImag,length))
	{
		return;
	}
	memcpy(g_grayImBuffer+PPMHeaderLength,_grayImag,PreviewImageSize);

	imageBig = QImage::fromData(g_grayImBuffer, PreviewImageSize+PPMHeaderLength, "PPM");
	if(!imageBig.isNull())
	{
		imageBig = imageBig.scaled (ui->labImg->width()-1,
				ui->labImg->height()-1,
				Qt::KeepAspectRatio);

		ui->labImg->setPixmap(QPixmap::fromImage(imageBig)); 
		_count++;
		_waiter.signal();
	}

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
	Exectime etm(__FUNCTION__);
	//    timespec tim;
	//    tim.tv_sec = time(nullptr);
	//    tim.tv_sec +=1;
	MainWindow *pCGDlg = (MainWindow*) pParam;
	while(true)
	{
		//SimpleLockForMutex lock(pCGDlg->_imageMutex);

		pCGDlg->_waiter.wait();
		imPaintEvent.RaisePaintEvent();

	}
	std::cout << "ThreadDisplayImage 进程退出！" << std::endl;
	return ((void*)0);
}


void* MainWindow::ThreadDisplayImage1(void* pParam)
{
	Exectime etm(__FUNCTION__);
	//    timespec tim;
	//    tim.tv_sec = time(nullptr);
	//    tim.tv_sec +=1;
	MainWindow *pCGDlg = (MainWindow*) pParam;
	while(true)
	{
		//SimpleLockForMutex lock(pCGDlg->_imageMutex);

		pCGDlg->_waiter1.wait();
		imPaintEvent.RaisePaintEvent1();

	}
	std::cout << "ThreadDisplayImage1 进程退出！" << std::endl;
	return ((void*)0);
}

