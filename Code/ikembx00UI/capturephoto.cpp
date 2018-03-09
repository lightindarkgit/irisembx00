/*****************************************************************************
 ** 文 件 名：capturephoto.cpp
 ** 主 要 类：CapturePhoto
 **
 ** Copyright (c) 中科虹霸有限公司
 **
 ** 创 建 人：L.Wang
 ** 创建时间：20013-10-18
 **
 ** 修 改 人：
 ** 修改时间：
 ** 描  述:   人脸图像采集界面
 ** 主要模块说明: 人脸图像采集过程中实时显示， 采集完成， 返回
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 *****************************************************************************/
#include <QImage>
#include <QPixmap>
#include <QMessageBox>
#include <unistd.h>
#include <pthread.h>
#include <QFile>

#include "ikmessagebox.h"
#include "capturephoto.h"
#include "ui_capturephoto.h"
#include "commononqt.h"
#include "../Common/Logger.h"
#include "../Common/Exectime.h"

//人脸图像API
#include "AlgApi.h"

const int IMAGE_WIDTH = 480;
const int IMAGE_HEIGHT = 640;
const int IMAGE_SIZE = IMAGE_WIDTH * IMAGE_HEIGHT;
const long long  bufLen = IMAGE_SIZE * 2 * 30;
unsigned char imgBuf[bufLen];
static int imgCount = 0;


/*****************************************************************************
 *                        人脸图像采集处理函数
 *  函 数 名：GetFacePhoto
 *  功    能：调用人脸采集图像接口，采集到图像后发射信号，供图像处理类使用
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2014-01-20
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void *GetFacePhoto(void* arg)
{
	Exectime etm(__FUNCTION__);
	//TO DO
	if(nullptr == arg)
	{
		return (void *)0;
	}

	CapturePhoto* cp = static_cast<CapturePhoto *>(arg);

	while(cp->enableCapture)
	{
		//TO DO
		if(E_ALGAPI_OK == cp->faceCamer->CapImg(cp->photoBuffer))
		{
			emit cp->sigUpdateImage();
		}

		//防止系统误误判死循环
        usleep(1000);
	}

	return (void *)0;
}

CapturePhoto::CapturePhoto(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CapturePhoto),
	faceCamer(nullptr),
	photoBuffer(nullptr),
	pImage(nullptr),
	enableCapture(false)
{
	Exectime etm(__FUNCTION__);
	ui->setupUi(this);

	//去除对话框标题栏，坐标重定位和填充背景
	setWindowFlags(Qt::FramelessWindowHint);
	setGeometry(QRect(0, 0, 1024, 600));
	setBackgroundColor(QColor(242,241,240));


	//图像缓暂存冲区
	photoBuffer = (unsigned char*)malloc(IMAGE_SIZE * 2 * sizeof(unsigned char));
	pImage = new QImage(photoBuffer, IMAGE_HEIGHT, IMAGE_WIDTH, QImage::Format_RGB16);


	//    pp = (unsigned char *)malloc(640 * 480* 2 * sizeof(char));//采图前分配空间,rgb16
	//    frame = new QImage(pp,640,480,QImage::Format_RGB16);
	//    frame->loadFromData(pp, 640 * 480 * 2 * sizeof(char));//rgb16
	//                label->setPixmap(QPixmap::fromImage(*frame,Qt::AutoColor));



	//连接返回和保存信号和对应的槽
	connect(ui->btnBack, SIGNAL(clicked()), this, SLOT(slotBack()));
	connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(slotSave()));
	connect(this, SIGNAL(sigUpdateImage()), this, SLOT(slotGetFaceImage()));
}

CapturePhoto::~CapturePhoto()
{
	Exectime etm(__FUNCTION__);
    enableCapture = false;
    free(photoBuffer);
    delete pImage;
	delete ui;
    delete faceCamer;
}

bool CapturePhoto::setFileNameInfo(QString personInfo)
{
	Exectime etm(__FUNCTION__);
	persInfo = personInfo;

	return true;
}

bool CapturePhoto::initCamera()
{
	Exectime etm(__FUNCTION__);
	faceCamer = new CCameraApi();
	if(E_ALGAPI_OK !=  faceCamer->Init())
	{
		//        QMessageBox::critical(this, QString::fromUtf8("初始化人脸图像采集设备"), QString::fromUtf8("API初始化失败或未初始化！"));
		//        IKMessageBox errorNote(this, QString::fromUtf8("初始化人脸图像采集设备"), QString::fromUtf8("API初始化失败或未初始化！"), errorMbx);

		//        errorNote.exec();
		std::cout << "[ERROR@" << __FUNCTION__ << "]:" << "init facecamer failed" << std::endl;
		LOG_ERROR("init facecamer failed");
		return false;
	}


	enableCapture = true;
	pthread_create(&thrID, nullptr, GetFacePhoto, this);

	return true;
}

void CapturePhoto::slotGetFaceImage()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	//FOR TEST CAMERA
	//    if(imgCount >= 0)
	//    {
	//        if(imgCount < 5)
	//        {
	//            memcpy(imgBuf + IMAGE_SIZE * 2 * imgCount++, photoBuffer, IMAGE_SIZE * 2);
	//            std::cout << "[" << __FUNCTION__ << "]image count: " << imgCount << std::endl;
	//        }
	//        else
	//        {
	//            FILE* fp = fopen("rawFaceImageData", "wb+");
	//            if(fp != NULL)
	//            {
	//                for(long long i = 0; i < imgCount * IMAGE_SIZE * 2; )
	//                {
	//                    std::cout << "[" << __FUNCTION__ << "]image buffer offset: " << std::dec << i << std::endl;
	//                    if(IMAGE_WIDTH != fwrite(imgBuf + i, sizeof(unsigned char), IMAGE_WIDTH, fp))
	//                    {
	//                        std::cout << "write image data to file error!" << std::endl;
	//                        imgCount = -1;
	//                        break;
	//                    }

	//                    i += IMAGE_WIDTH;
	//                }

	//                fclose(fp);
	//            }
	//            else
	//            {
	//                std::cout << "can not open file for logging face image data!" << std::endl;
	//            }
	//        }
	//    }


	//    QString fileName("rawFaceImgData%1_%2");
	//    fileName = fileName.arg(GetCurrentDateTime().c_str(), imgCount++);
	//    FILE* fp = fopen(fileName.toStdString().c_str(), "wb+");
	//    if(fp != NULL)
	//    {
	//        for(long i = 0; i < IMAGE_SIZE * 2; )
	//        {
	//            std::cout << "[" << __FUNCTION__ << "]image buffer offset: " << std::dec << i << std::endl;
	//            if(IMAGE_SIZE != fwrite(photoBuffer + i, sizeof(unsigned char), IMAGE_SIZE, fp))
	//            {
	//                std::cout << "write image data to file error!" << std::endl;
	//                break;
	//            }

	//            i += IMAGE_SIZE;
	//        }

	//        fclose(fp);
	//    }
	//    else
	//    {
	//        std::cout << "can not open file for logging face image data!" << std::endl;
	//    }
	//    static std::string oldTime = "0";
	//    std::string curTime;
	//    if(oldTime == (curTime = GetCurrentDateTime()))
	//    {
	//        imgCount++;
	//    }
	//    else
	//    {
	//        oldTime = curTime;
	////        std::cout << GetLogHead(__FUNCTION__) << "frame rate: "  << std::dec << imgCount << std::endl;
	//        ui->labFrameCnt->setText(QString("帧率: %1").arg(imgCount));
	//        imgCount = 0;
	//    }

	pImage->loadFromData(photoBuffer, IMAGE_SIZE * 2 * sizeof(unsigned char));
	QImage img(IMAGE_WIDTH, IMAGE_HEIGHT, QImage::Format_RGB16);
	int height = ui->labFaceImageStream->height();
	int width = ui->labFaceImageStream->width();

	if(pImage->height() > pImage->width())
	{
		img = pImage->copy(0, pImage->height() / 4, pImage->width(), pImage->width() * height / width);
	}
	else
	{
		img = pImage->copy(pImage->width() / 4, 0, pImage->height() * width / height, pImage->height());
	}

	//    QMatrix matrix;
	//    matrix.rotate(180);
	//    img = img.transformed(QMatrix().rotate(180));
	img = img.mirror(false, true);
	img = img.scaled(width, height, Qt::KeepAspectRatio);

	pixMap = QPixmap::fromImage(img, Qt::AutoColor);

	ui->labFaceImageStream->setPixmap(pixMap);
}

/*****************************************************************************
 *                        返回操作处理函数
 *  函 数 名：slotBack
 *  功    能：槽函数，处理返回操作
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void CapturePhoto::slotBack()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	stopCapturing();
	emit sigImgaeCaptured(QImage());
	close();
}

/*****************************************************************************
 *                        保存采集到的当前人脸图像
 *  函 数 名：slotSave
 *  功    能：槽函数，存储当前采集的人脸图像
 *  说    明：
 *  参    数：
 *  返 回 值：
 *  创 建 人：L.Wang
 *  创建时间：2013-10-18
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void CapturePhoto::slotSave()
{
	Exectime etm(__FUNCTION__);
	//TO DO
	//发射图像采集结束信号
	emit sigImgaeCaptured(pixMap.toImage());

	stopCapturing();
	//关闭当前窗口
	close();
}


void CapturePhoto::stopCapturing()
{
	Exectime etm(__FUNCTION__);
	enableCapture = false;

	void *rtn_status;
	pthread_join(thrID, &rtn_status);
	faceCamer->Release();
	//    delete photoBuffer;
}

