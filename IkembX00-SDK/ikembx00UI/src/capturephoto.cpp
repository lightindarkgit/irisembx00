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
#include "../include/capturephoto.h"
#include "ui_capturephoto.h"
#include "commononqt.h"
#include "../Common/Logger.h"
#include "buffering.h"


const int IMAGE_WIDTH = 480;
const int IMAGE_HEIGHT = 640;
const int IMAGE_SIZE = IMAGE_WIDTH * IMAGE_HEIGHT;
const long long  bufLen = IMAGE_SIZE * 2 * 30;
unsigned char imgBuf[bufLen];
static int imgCount = 0;

int g_isFaceThreadEnd = 1;


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
    /*
    g_isFaceThreadEnd = 0; //线程正在开始 //@dj
	//TO DO
	if(nullptr == arg)
	{
		return (void *)0;
	}

    CapturePhoto* cp = static_cast<CapturePhoto *>(arg);

	int dropCounts = 0;
	while(cp->enableCapture)
	{
		//TO DO
        if(E_ALGAPI_OK == cp->faceCamer->CapImg(cp->photoBuffer))
		{
			if(dropCounts < 4)
			{
				dropCounts++;
				continue;
			}
			LOG_ERROR("xxx GetFacePhoto");
			emit cp->sigUpdateImage();
		}

		//防止系统误误判死循环
         usleep(30000);
	}

    g_isFaceThreadEnd = 1;
    */

	return (void *)0;
}

CapturePhoto::CapturePhoto(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CapturePhoto),
    //@@@faceCamer(nullptr),
	photoBuffer(nullptr),
	pImage(nullptr),
	enableCapture(false)
{
	ui->setupUi(this);

	//去除对话框标题栏，坐标重定位和填充背景
	setWindowFlags(Qt::FramelessWindowHint);
	setGeometry(QRect(0, 0, 1024, 600));
	setBackgroundColor(QColor(242,241,240));

	//图像缓暂存冲区
	photoBuffer = (unsigned char*)malloc(IMAGE_SIZE * 2 * sizeof(unsigned char));
	pImage = new QImage(photoBuffer, IMAGE_HEIGHT, IMAGE_WIDTH, QImage::Format_RGB16);

	//连接返回和保存信号和对应的槽
	connect(ui->btnBack, SIGNAL(clicked()), this, SLOT(slotBack()));
	connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(slotSave()));
	connect(this, SIGNAL(sigUpdateImage()), this, SLOT(slotGetFaceImage()));

    _isDataEdited =false;




}

CapturePhoto::~CapturePhoto()
{
    enableCapture = false;
//@    free(photoBuffer);

//@    delete faceCamer;

    delete pImage;
	delete ui;

}

bool CapturePhoto::setFileNameInfo(QString personInfo)
{
	persInfo = personInfo;

	return true;
}

bool CapturePhoto::initCamera()
{
    //@@@
    return false;
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
    //@dj 20161226
    if(g_isFaceThreadEnd == 1)
    {
        pthread_create(&thrID, nullptr, GetFacePhoto, this);
        pthread_detach(thrID);
    }
    else
    {
        LOG_ERROR("采集人脸线程正在运行，稍候再试");
    }
//	pthread_create(&thrID, nullptr, GetFacePhoto, this);


	return true;
}

void CapturePhoto::slotGetFaceImage()
{


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

	LOG_ERROR("xxx CapturePhoto::slotGetFaceImage");
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
void CapturePhoto::slotBack(bool isAutoBack)
{
	//TO DO
    stopCapturing(isAutoBack);
	emit sigImgaeCaptured(QImage());
    _isDataEdited = false;
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
    //TO DO
	//发射图像采集结束信号
	emit sigImgaeCaptured(pixMap.toImage());


    _isDataEdited = true;
	stopCapturing();
    //accept();
    //关闭当前窗口
    close();
}


void CapturePhoto::stopCapturing(bool isAutoBack)
{
	enableCapture = false;
    while(g_isFaceThreadEnd != 1 && isAutoBack)
    {
        enableCapture = false;
        LOG_ERROR("采图线程未结束");

        IKMessageBox Note(this, QString::fromUtf8("退出"), QString::fromUtf8("摄像头正在退出请等待"),warningMbx);
        Note.exec();


        if(g_isFaceThreadEnd != 1)
        {
            IKMessageBox errorNote(this, QString::fromUtf8("退出"), QString::fromUtf8("设备繁忙，点击取消等待自动退出，点击确定直接退出"), questionMbx);
            errorNote.exec();
            if(QDialog::Accepted == errorNote.result())
            {
                break;
            }

        }

    }

    //void *rtn_status;
    //@pthread_join(thrID, &rtn_status);
	//memset(photoBuffer, 0, IMAGE_SIZE * 2 * sizeof(unsigned char));
    if(nullptr != faceCamer)
    {
        faceCamer->Release();

    }
        //    delete photoBuffer;
}

bool CapturePhoto::IsPhotoSaved()
{
    return _isDataEdited;
}


