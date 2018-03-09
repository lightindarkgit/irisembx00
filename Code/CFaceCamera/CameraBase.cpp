/******************************************************************************************
** 文件名:   CameraBase.cpp
×× 主要类:   CCameraBase
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   lizhl
** 日  期:   2014-01-15
** 修改人:
** 日  期:
** 描  述:   IKEMBX00项目人脸摄像头具体实现，对外不可见
 *×× 主要模块说明: 人脸摄像头具体实现
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
#include "CameraBase.h"
//#include <QDebug>

using namespace std;
mutex CCameraBase::s_instanceCameraMutex;
CCameraBase* CCameraBase::s_pSingletonCCameraBase = NULL;

int tryCount = 0; //全局变量，记录尝试次数
int restartCount = 0;//全局变量，记录重启次数

/*****************************************************************************
*                         CCameraBase构造函数
*  函 数 名：CCameraBase
*  功    能：构造函数
*  说    明：
*  参    数：NULL
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*****************************************************************************/
CCameraBase::CCameraBase()
{    
    _fd = -1;
    _buffers = NULL;
    _nBuffers = 0;
    _index = -1;
    _result = -1;
}

/*****************************************************************************
*                         CCameraBase析构函数
*  函 数 名：~CCameraBase
*  功    能：析构函数
*  说    明：
*  参    数：NULL
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*****************************************************************************/
CCameraBase::~CCameraBase()
{

}

/*****************************************************************************
*                         获取CCameraBase实例
*  函 数 名：GetInstance
*  功    能：获取CCameraBase实例
*  说    明：CCameraBase采用单件模式，且多线程安全
*  参    数：NULL
*
*  返 回 值：!NULL: CAPIBase实例指针
*			NULL：获取失败
*  创 建 人：lizhl
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*****************************************************************************/
CCameraBase* CCameraBase::GetInstance()
{
    // 双重检查，提高性能
    if (NULL == s_pSingletonCCameraBase)
    {
        lock_guard<mutex> lck (s_instanceCameraMutex);

        if (NULL == s_pSingletonCCameraBase)
        {
            s_pSingletonCCameraBase = new CCameraBase();
            s_pSingletonCCameraBase->_instanceCamBaseNum = 0;
        }
    }

    return s_pSingletonCCameraBase;
}


/*****************************************************************************
*                         初始化人脸摄像头
*  函 数 名：Init
*  功    能：初始化人脸摄像头
*  说    明：
*  参    数：strFunRts：输出参数，以字符串形式表示本次函数调用结果，由用户分配
*
*  返 回 值：0：成功
*			<0：失败
*  创 建 人：lizhl
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CCameraBase::Init(OUT char *strFunRts)
{
    if(0 == _instanceCamBaseNum)
    {
        //如果是第一次初始化，则初始化摄像头
        _result = OpenDevice();
        if(-1 == _result)
        {
            //如果设备打开失败则关闭设备
            sprintf(strFunRts,"%6d:%s", E_ALGAPI_CAMERA_INIT_FAILED, "USB Camera open failed");
            CloseDevice();
            return E_ALGAPI_CAMERA_INIT_FAILED;
        }

        _result = InitDevice();
        if(-1 == _result)
        {
            //如果初始化失败，再试一次
            usleep(100);
            _result = InitDevice();
        }

        if(-1 == _result)
        {
            //如果设备初始化失败则关闭设备
            sprintf(strFunRts,"%6d:%s", E_ALGAPI_CAMERA_INIT_FAILED, "USB Camera init failed");
            UninitDevice();
            CloseDevice();
            return E_ALGAPI_CAMERA_INIT_FAILED;
        }

        _result = StartCapturing();
        if(-1 == _result)
        {
            //如果设备开始采集失败则关闭设备
            sprintf(strFunRts,"%6d:%s", E_ALGAPI_CAMERA_INIT_FAILED, "USB Camera start capture failed");
            StopCapturing();
            UninitDevice();
            CloseDevice();
            return E_ALGAPI_CAMERA_INIT_FAILED;
        }

    }
    _instanceCamBaseNum++;
    return E_ALGAPI_OK;
}

/*****************************************************************************
*                         采集图像
*  函 数 名：CapImg
*  功    能：采集人脸图像
*  说    明：
*  参    数：pFaceImg：输出参数，采集到图像的指针
*           strFunRts：输出参数，以字符串形式表示本次函数调用结果，由用户分配
*
*  返 回 值：0：成功
*			<0：失败
*  创 建 人：lizhl
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*****************************************************************************/
int	CCameraBase::CapImg(OUT unsigned char *pFaceImg, OUT char *strFunRts)
{
    int len = 0;
    unsigned char *p;

    //需要保证图像采集成功,20140605
    struct timeval tvCapStart, tvCapEnd;
    gettimeofday(&tvCapStart,NULL);
    while(0 != GetFrame((void **)&p,&len))
    {
        usleep(200*1000);
        gettimeofday(&tvCapEnd,NULL);
        if((tvCapEnd.tv_sec - tvCapStart.tv_sec + (tvCapEnd.tv_usec - tvCapStart.tv_usec)/1000000) > 2)
        {
            UngetFrame();
            return E_ALGAPI_CAMERA_CAPIMG_FAILED;
        }
    }
    ConvertYUYVtoRGB565(p,pFaceImg,640,480);//rgb16
    _result = UngetFrame();
//        std::cout << "Capture face image success" << std::endl;
    return 0;


////    qWarning("^^^getframe in");
//    _result = GetFrame((void **)&p,&len);
////    qWarning("vvvgetframe out");
//    if(0 == _result)
//    {
//        //如果采集成功则转换图像格式为rgb
////        ConvertYuv2RgbBuffer(p,pFaceImg,640,480);//rgb888
//        ConvertYUYVtoRGB565(p,pFaceImg,640,480);//rgb16
//        _result = UngetFrame();
////        std::cout << "Capture face image success" << std::endl;
//        return E_ALGAPI_OK;
//    }
//    //20140417
//    UngetFrame();
//    return E_ALGAPI_CAMERA_CAPIMG_FAILED;
}

/*****************************************************************************
*                         释放人脸摄像头
*  函 数 名：Release
*  功    能：释放人脸摄像头
*  说    明：
*  参    数：NULL
*
*  返 回 值：NULL
*
*  创 建 人：lizhl
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*****************************************************************************/
void CCameraBase::Release()
{
    StopCapturing();
    UninitDevice();
    CloseDevice();
    _instanceCamBaseNum = 0;
    tryCount = 0;
}


/*****************************************************************************
*                         打开设备
*  函 数 名：OpenDevice
*  功    能：打开设备
*  说    明：
*  参    数：NULL
*
*  返 回 值：0：成功
*			<0：失败
*  创 建 人：lizhl
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CCameraBase::OpenDevice()
{
//    _fd = open("/dev/video0", O_RDWR/*|O_NONBLOCK*/, 0);
    _fd = open("/dev/video0", O_RDWR|O_NONBLOCK, 0);

    if(-1 == _fd)
    {
        return -1;
    }
    return 0;
}

/*****************************************************************************
*                         关闭设备
*  函 数 名：CloseDevice
*  功    能：打开设备
*  说    明：
*  参    数：NULL
*
*  返 回 值：0：成功
*			<0：失败
*  创 建 人：lizhl
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CCameraBase::CloseDevice()
{
    if(-1 == close(_fd))
    {
        return -1;
    }
    return 0;
}

/*****************************************************************************
*                         初始化设备
*  函 数 名：InitDevice
*  功    能：初始化设备
*  说    明：
*  参    数：NULL
*
*  返 回 值：0：成功
*			<0：失败
*  创 建 人：lizhl
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CCameraBase::InitDevice()
{
    v4l2_capability cap;
//    v4l2_cropcap cropcap;
    v4l2_crop crop;
    v4l2_format fmt;

    if(-1 == ioctl(_fd, VIDIOC_QUERYCAP, &cap))
    {
        if(EINVAL == errno)
        {
//            emit display_error(tr("%1 is no V4l2 device").arg(dev_name));
        }
        else
        {
//            emit display_error(tr("VIDIOC_QUERYCAP: %1").arg(QString(strerror(errno))));
        }
        return -1;
    }

    if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
    {
//        emit display_error(tr("%1 is no video capture device").arg(dev_name));
        return -1;
    }

    if(!(cap.capabilities & V4L2_CAP_STREAMING))
    {
//        emit display_error(tr("%1 does not support streaming i/o").arg(dev_name));
        return -1;
    }

//    memset(&(cropcap), 0, sizeof(cropcap));

//    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

//    if(0 == ioctl(_fd, VIDIOC_CROPCAP, &cropcap))
//    {
//        memset(&(crop), 0, sizeof(crop));
//        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
//        crop.c = cropcap.defrect;

//        if(-1 == ioctl(_fd, VIDIOC_S_CROP, &crop))
//        {
//            if(EINVAL == errno)
//            {
////                emit display_error(tr("VIDIOC_S_CROP not supported"));
//            }
//            else
//            {
////                emit display_error(tr("VIDIOC_S_CROP: %1").arg(QString(strerror(errno))));
//                return -1;
//            }
//        }
//    }
//    else
//    {
////        emit display_error(tr("VIDIOC_CROPCAP: %1").arg(QString(strerror(errno))));
//        return -1;
//    }

    memset(&(fmt), 0, sizeof(fmt));

    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 640;
    fmt.fmt.pix.height = 480;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    if(-1 == ioctl(_fd, VIDIOC_S_FMT, &fmt))
    {
//        emit display_error(tr("VIDIOC_S_FMT").arg(QString(strerror(errno))));
        return -1;
    }

    if(-1 == InitMmap())
    {
        return -1;
    }

    //20140429设置帧率
    struct v4l2_streamparm *parm = new struct v4l2_streamparm;
    memset(parm,0,sizeof(struct v4l2_streamparm));
    parm->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    //  parm->parm.capture.capturemode = V4L2_MODE_HIGHQUALITY;
    parm->parm.capture.capability = V4L2_CAP_TIMEPERFRAME;
    parm->parm.capture.timeperframe.denominator = 400 ;//时间间隔分母
    parm->parm.capture.timeperframe.numerator = 1;//分子
    if(-1 == ioctl(_fd,VIDIOC_S_PARM,parm))
    {
        return -1;
    }

    std::cout << "the frame rate:" << (parm->parm.capture.timeperframe.numerator*1000)/(parm->parm.capture.timeperframe.denominator) << std::endl;
    /////////////////

    return 0;
}

/*****************************************************************************
*                         初始化mmap
*  函 数 名：InitMmap
*  功    能：初始化mmap
*  说    明：
*  参    数：NULL
*
*  返 回 值：0：成功
*			<0：失败
*  创 建 人：lizhl
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CCameraBase::InitMmap()
{
    v4l2_requestbuffers req;
    memset(&(req), 0, sizeof(req));

    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if(-1 == ioctl(_fd, VIDIOC_REQBUFS, &req))
    {
        if(EINVAL == errno)
        {
//            emit display_error(tr("%1 does not support memory mapping").arg(dev_name));
            return -1;
        }
        else
        {
//            emit display_error(tr("VIDIOC_REQBUFS %1").arg(QString(strerror(errno))));
            return -1;
        }
    }

    if(req.count < 2)
    {
//        emit display_error(tr("Insufficient buffer memory on %1").arg(dev_name));
        return -1;
    }

    _buffers = (buffer*)calloc(req.count, sizeof(*_buffers));

    if(!_buffers)
    {
//        emit display_error(tr("out of memory"));
        return -1;
    }

    for(_nBuffers = 0; _nBuffers < req.count; ++_nBuffers)
    {
        v4l2_buffer buf;
        memset(&(buf), 0, sizeof(buf));

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = _nBuffers;

        if(-1 == ioctl(_fd, VIDIOC_QUERYBUF, &buf))
        {
//            emit display_error(tr("VIDIOC_QUERYBUF: %1").arg(QString(strerror(errno))));
            return -1;
        }

        _buffers[_nBuffers].length = buf.length;
        _buffers[_nBuffers].start =
                mmap(NULL, // start anywhere
                     buf.length,
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED,
                     _fd, buf.m.offset);

        if(MAP_FAILED == _buffers[_nBuffers].start)
        {
//            emit display_error(tr("mmap %1").arg(QString(strerror(errno))));
            return -1;
        }
    }
    return 0;

}

/*****************************************************************************
*                         开始采集图像
*  函 数 名：StartCapturing
*  功    能：开始采集图像
*  说    明：
*  参    数：NULL
*
*  返 回 值：0：成功
*			<0：失败
*  创 建 人：lizhl
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CCameraBase::StartCapturing()
{
    unsigned int count;
    for(count = 0; count < _nBuffers; ++count)
    {
        v4l2_buffer buf;
        memset(&(buf), 0, sizeof(buf));

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory =V4L2_MEMORY_MMAP;
        buf.index = count;
//        fprintf(stderr, "n_buffers: %d\n", count);

        if(-1 == ioctl(_fd, VIDIOC_QBUF, &buf))
        {
//            emit display_error(tr("VIDIOC_QBUF: %1").arg(QString(strerror(errno))));
            return -1;
        }
    }

    v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if(-1 == ioctl(_fd, VIDIOC_STREAMON, &type))
    {
//        emit display_error(tr("VIDIOC_STREAMON: %1").arg(QString(strerror(errno))));
        return -1;
    }
    return 0;
}

/*****************************************************************************
*                         停止采集图像
*  函 数 名：StopCapturing
*  功    能：停止采集图像
*  说    明：
*  参    数：NULL
*
*  返 回 值：0：成功
*			<0：失败
*  创 建 人：lizhl
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CCameraBase::StopCapturing()
{
    v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if(-1 == ioctl(_fd, VIDIOC_STREAMOFF, &type))
    {
//        emit display_error(tr("VIDIOC_STREAMOFF: %1").arg(QString(strerror(errno))));
        return -1;
    }
    return 0;
}

/*****************************************************************************
*                         反初始化设备
*  函 数 名：UninitDevice
*  功    能：反初始化设备
*  说    明：
*  参    数：NULL
*
*  返 回 值：0：成功
*			<0：失败
*  创 建 人：lizhl
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CCameraBase::UninitDevice()
{
    unsigned int count;
    for(count = 0; count < _nBuffers; ++count)
    {
        if(-1 == munmap(_buffers[count].start, _buffers[count].length))
        {
//            emit display_error(tr("munmap: %1").arg(QString(strerror(errno))));
            return -1;
        }

    }
    free(_buffers);
    return 0;
}

/*****************************************************************************
*                         缓冲出队列
*  函 数 名：GetFrame
*  功    能：获取一帧图像
*  说    明：
*  参    数：frameBuf：图像指针
*           len：图像长度
*
*  返 回 值：0：成功
*			<0：失败
*  创 建 人：lizhl
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*****************************************************************************/
void CCameraBase::Restart()
{
    char resultOfInit[260];
    struct timeval tvRestart;
    struct tm *ptmNowTime;
    time_t lt;
    FILE *logFile;

    Release();
    _instanceCamBaseNum = 0;
    Init(resultOfInit);
    tryCount = 0;
    restartCount++;
    logFile = fopen("log.txt","ab");;//记录摄像头重启
    lt =time(NULL);
    ptmNowTime = localtime(&lt);
    gettimeofday(&tvRestart,NULL);
    fprintf(logFile, "Time: %02d_%02d_%02d_%02d_%02d_%04ld \r\n",
            ptmNowTime->tm_mon+1,ptmNowTime->tm_mday,ptmNowTime->tm_hour,ptmNowTime->tm_min, ptmNowTime->tm_sec,tvRestart.tv_usec);

    fprintf(logFile, "ReStart Device, total restart count = %d \r\n", restartCount);
    fclose(logFile);
//    std::cout << "ReStart Device, total restart count = " << restartCount << std::endl;
}

int CCameraBase::GetFrame(void **frameBuf, size_t* len)
{
//    v4l2_buffer queueBuf;
    memset(&(_queueBuf), 0, sizeof(_queueBuf));

    _queueBuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    _queueBuf.memory = V4L2_MEMORY_MMAP;
//    qWarning("EAGAIN = %d",EAGAIN);

    //出队列以取得已采集数据的帧缓冲，取得原始采集数据
    if(-1 == ioctl(_fd, VIDIOC_DQBUF, &_queueBuf))
    {
        switch(errno)
        {        
        case EAGAIN:
            tryCount++;
//            std::cout<< "cannot get frame, EAGAIN: try again, try count = " << tryCount << std::endl;
            if(tryCount > 30)
            {
                Restart();
            }
            return -1;

        case EIO:
            tryCount++;
//            std::cout<< "cannot get frame, EIO: try again, try count = " << tryCount << std::endl;
            if(tryCount > 30)
            {
                Restart();
            }
            return -1;

        default:
            tryCount++;
//            std::cout<< "cannot get frame, default: error = " << errno << "try again, try count" << tryCount << std::endl;
            if(tryCount > 30)
            {
                Restart();
            }
            return -1;
        }
    }

    tryCount = 0;
    *frameBuf = _buffers[_queueBuf.index].start;
    *len = _buffers[_queueBuf.index].length;
    _index = _queueBuf.index;

    return 0;

}

/*****************************************************************************
*                         缓冲重新入队列
*  函 数 名：UngetFrame
*  功    能：为下次采图做准备
*  说    明：
*  参    数：NULL
*
*  返 回 值：0：成功
*			<0：失败
*  创 建 人：lizhl
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CCameraBase::UngetFrame()
{
    if(_index != -1)
    {
        memset(&(_queueBuf), 0, sizeof(_queueBuf));

        _queueBuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        _queueBuf.memory = V4L2_MEMORY_MMAP;
        _queueBuf.index = _index;

        //将缓冲重新入队列尾,这样可以循环采集
        if(-1 == ioctl(_fd, VIDIOC_QBUF, &_queueBuf))
        {
//            emit display_error(tr("VIDIOC_QBUF: %1").arg(QString(strerror(errno))));
            return -1;
        }
        return 0;
    }
    return -1;
}

/*****************************************************************************
*                         将yuv像素转为rgb像素
*  函 数 名：ConvertYuv2RgbPixel
*  功    能：将yuv像素转为rgb像素
*  说    明：
*  参    数：y：输入参数，y分量值
*           u：输入参数，u分量值
*           v：输入参数，v分量值
*
*  返 回 值：rgb像素值
*
*  创 建 人：lizhl
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CCameraBase::ConvertYuv2RgbPixel(int y, int u, int v)
{
    unsigned int pixel32 = 0;
    unsigned char *pixel = (unsigned char *)&pixel32;
    int r, g, b;

    r = y + (1.370705 * (v-128));
    g = y - (0.698001 * (v-128)) - (0.337633 * (u-128));
    b = y + (1.732446 * (u-128));

    if(r > 255)
    {
        r = 255;
    }

    if(g > 255)
    {
        g = 255;
    }

    if(b > 255)
    {
        b = 255;
    }

    if(r < 0)
    {
        r = 0;
    }

    if(g < 0)
    {
        g = 0;
    }

    if(b < 0)
    {
        b = 0;
    }

    pixel[0] = r * 220 / 256;
    pixel[1] = g * 220 / 256;
    pixel[2] = b * 220 / 256;
    return pixel32;
}

/*****************************************************************************
*                         将yuv图像转为rgb图像
*  函 数 名：ConvertYuv2RgbBuffer
*  功    能：将yuv图像转为rgb图像
*  说    明：
*  参    数：yuv：输入参数，yuv图像指针
*           rgb：输入参数，rgb图像指针
*           width：输入参数，图像宽度
*           heigh：输入参数，图像高度
*
*  返 回 值：0：成功；
*           <0：失败
*
*  创 建 人：lizhl
*  创建时间：2014-01-15
*  修 改 人：
*  修改时间：
*****************************************************************************/
int CCameraBase::ConvertYuv2RgbBuffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
    unsigned int in, out = 0;
    unsigned int pixel_16;
    unsigned char pixel_24[3];
    unsigned int pixel32;
    int y0, u, y1, v;
    for(in = 0; in < width * height * 2; in += 4)
    {
        pixel_16 =
                yuv[in + 3] << 24 |
                               yuv[in + 2] << 16 |
                               yuv[in + 1] <<  8 |
                               yuv[in + 0];
        y0 = (pixel_16 & 0x000000ff);
        u  = (pixel_16 & 0x0000ff00) >>  8;
        y1 = (pixel_16 & 0x00ff0000) >> 16;
        v  = (pixel_16 & 0xff000000) >> 24;

        pixel32 = ConvertYuv2RgbPixel(y0, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;

        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];

        pixel32 = ConvertYuv2RgbPixel(y1, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;

        rgb[out++] = pixel_24[0];
        rgb[out++] = pixel_24[1];
        rgb[out++] = pixel_24[2];
    }
    return 0;
}



static const signed short redAdjust[] =
{
-161,-160,-159,-158,-157,-156,-155,-153,
-152,-151,-150,-149,-148,-147,-145,-144,
-143,-142,-141,-140,-139,-137,-136,-135,
-134,-133,-132,-131,-129,-128,-127,-126,
-125,-124,-123,-122,-120,-119,-118,-117,
-116,-115,-114,-112,-111,-110,-109,-108,
-107,-106,-104,-103,-102,-101,-100, -99,
 -98, -96, -95, -94, -93, -92, -91, -90,
 -88, -87, -86, -85, -84, -83, -82, -80,
 -79, -78, -77, -76, -75, -74, -72, -71,
 -70, -69, -68, -67, -66, -65, -63, -62,
 -61, -60, -59, -58, -57, -55, -54, -53,
 -52, -51, -50, -49, -47, -46, -45, -44,
 -43, -42, -41, -39, -38, -37, -36, -35,
 -34, -33, -31, -30, -29, -28, -27, -26,
 -25, -23, -22, -21, -20, -19, -18, -17,
 -16, -14, -13, -12, -11, -10,  -9,  -8,
  -6,  -5,  -4,  -3,  -2,  -1,   0,   1,
   2,   3,   4,   5,   6,   7,   9,  10,
  11,  12,  13,  14,  15,  17,  18,  19,
  20,  21,  22,  23,  25,  26,  27,  28,
  29,  30,  31,  33,  34,  35,  36,  37,
  38,  39,  40,  42,  43,  44,  45,  46,
  47,  48,  50,  51,  52,  53,  54,  55,
  56,  58,  59,  60,  61,  62,  63,  64,
  66,  67,  68,  69,  70,  71,  72,  74,
  75,  76,  77,  78,  79,  80,  82,  83,
  84,  85,  86,  87,  88,  90,  91,  92,
  93,  94,  95,  96,  97,  99, 100, 101,
 102, 103, 104, 105, 107, 108, 109, 110,
 111, 112, 113, 115, 116, 117, 118, 119,
 120, 121, 123, 124, 125, 126, 127, 128,
};
static const signed short greenAdjust1[] =
{
  34,  34,  33,  33,  32,  32,  32,  31,
  31,  30,  30,  30,  29,  29,  28,  28,
  28,  27,  27,  27,  26,  26,  25,  25,
  25,  24,  24,  23,  23,  23,  22,  22,
  21,  21,  21,  20,  20,  19,  19,  19,
  18,  18,  17,  17,  17,  16,  16,  15,
  15,  15,  14,  14,  13,  13,  13,  12,
  12,  12,  11,  11,  10,  10,  10,   9,
   9,   8,   8,   8,   7,   7,   6,   6,
   6,   5,   5,   4,   4,   4,   3,   3,
   2,   2,   2,   1,   1,   0,   0,   0,
   0,   0,  -1,  -1,  -1,  -2,  -2,  -2,
  -3,  -3,  -4,  -4,  -4,  -5,  -5,  -6,
  -6,  -6,  -7,  -7,  -8,  -8,  -8,  -9,
  -9, -10, -10, -10, -11, -11, -12, -12,
 -12, -13, -13, -14, -14, -14, -15, -15,
 -16, -16, -16, -17, -17, -17, -18, -18,
 -19, -19, -19, -20, -20, -21, -21, -21,
 -22, -22, -23, -23, -23, -24, -24, -25,
 -25, -25, -26, -26, -27, -27, -27, -28,
 -28, -29, -29, -29, -30, -30, -30, -31,
 -31, -32, -32, -32, -33, -33, -34, -34,
 -34, -35, -35, -36, -36, -36, -37, -37,
 -38, -38, -38, -39, -39, -40, -40, -40,
 -41, -41, -42, -42, -42, -43, -43, -44,
 -44, -44, -45, -45, -45, -46, -46, -47,
 -47, -47, -48, -48, -49, -49, -49, -50,
 -50, -51, -51, -51, -52, -52, -53, -53,
 -53, -54, -54, -55, -55, -55, -56, -56,
 -57, -57, -57, -58, -58, -59, -59, -59,
 -60, -60, -60, -61, -61, -62, -62, -62,
 -63, -63, -64, -64, -64, -65, -65, -66,
};
static const signed short greenAdjust2[] =
{
  74,  73,  73,  72,  71,  71,  70,  70,
  69,  69,  68,  67,  67,  66,  66,  65,
  65,  64,  63,  63,  62,  62,  61,  60,
  60,  59,  59,  58,  58,  57,  56,  56,
  55,  55,  54,  53,  53,  52,  52,  51,
  51,  50,  49,  49,  48,  48,  47,  47,
  46,  45,  45,  44,  44,  43,  42,  42,
  41,  41,  40,  40,  39,  38,  38,  37,
  37,  36,  35,  35,  34,  34,  33,  33,
  32,  31,  31,  30,  30,  29,  29,  28,
  27,  27,  26,  26,  25,  24,  24,  23,
  23,  22,  22,  21,  20,  20,  19,  19,
  18,  17,  17,  16,  16,  15,  15,  14,
  13,  13,  12,  12,  11,  11,  10,   9,
   9,   8,   8,   7,   6,   6,   5,   5,
   4,   4,   3,   2,   2,   1,   1,   0,
   0,   0,  -1,  -1,  -2,  -2,  -3,  -4,
  -4,  -5,  -5,  -6,  -6,  -7,  -8,  -8,
  -9,  -9, -10, -11, -11, -12, -12, -13,
 -13, -14, -15, -15, -16, -16, -17, -17,
 -18, -19, -19, -20, -20, -21, -22, -22,
 -23, -23, -24, -24, -25, -26, -26, -27,
 -27, -28, -29, -29, -30, -30, -31, -31,
 -32, -33, -33, -34, -34, -35, -35, -36,
 -37, -37, -38, -38, -39, -40, -40, -41,
 -41, -42, -42, -43, -44, -44, -45, -45,
 -46, -47, -47, -48, -48, -49, -49, -50,
 -51, -51, -52, -52, -53, -53, -54, -55,
 -55, -56, -56, -57, -58, -58, -59, -59,
 -60, -60, -61, -62, -62, -63, -63, -64,
 -65, -65, -66, -66, -67, -67, -68, -69,
 -69, -70, -70, -71, -71, -72, -73, -73,
};
static const signed short blueAdjust[] = {
-276,-274,-272,-270,-267,-265,-263,-261,
-259,-257,-255,-253,-251,-249,-247,-245,
-243,-241,-239,-237,-235,-233,-231,-229,
-227,-225,-223,-221,-219,-217,-215,-213,
-211,-209,-207,-204,-202,-200,-198,-196,
-194,-192,-190,-188,-186,-184,-182,-180,
-178,-176,-174,-172,-170,-168,-166,-164,
-162,-160,-158,-156,-154,-152,-150,-148,
-146,-144,-141,-139,-137,-135,-133,-131,
-129,-127,-125,-123,-121,-119,-117,-115,
-113,-111,-109,-107,-105,-103,-101, -99,
 -97, -95, -93, -91, -89, -87, -85, -83,
 -81, -78, -76, -74, -72, -70, -68, -66,
 -64, -62, -60, -58, -56, -54, -52, -50,
 -48, -46, -44, -42, -40, -38, -36, -34,
 -32, -30, -28, -26, -24, -22, -20, -18,
 -16, -13, -11,  -9,  -7,  -5,  -3,  -1,
   0,   2,   4,   6,   8,  10,  12,  14,
  16,  18,  20,  22,  24,  26,  28,  30,
  32,  34,  36,  38,  40,  42,  44,  46,
  49,  51,  53,  55,  57,  59,  61,  63,
  65,  67,  69,  71,  73,  75,  77,  79,
  81,  83,  85,  87,  89,  91,  93,  95,
  97,  99, 101, 103, 105, 107, 109, 112,
 114, 116, 118, 120, 122, 124, 126, 128,
 130, 132, 134, 136, 138, 140, 142, 144,
 146, 148, 150, 152, 154, 156, 158, 160,
 162, 164, 166, 168, 170, 172, 175, 177,
 179, 181, 183, 185, 187, 189, 191, 193,
 195, 197, 199, 201, 203, 205, 207, 209,
 211, 213, 215, 217, 219, 221, 223, 225,
 227, 229, 231, 233, 235, 238, 240, 242,
};

#define CLAMP(x) if (x < 0) x = 0 ; else if (x > 255) x = 255

void CCameraBase::yuv2rgb565(const int y, const int u, const int v, unsigned short &rgb565)
{
    int r, g, b;
    r = y + redAdjust[v];
    g = y + greenAdjust1[u] + greenAdjust2[v];
    b = y + blueAdjust[u];

//    static int n = 0;
//    n++;
//    if(n%3 == 0)
//    {
//        r = 0;
//        g = 0;
//        b = 0;
//    }
//    else if(n%3 == 1)
//    {
//        r = 128;
//        g = 128;
//        b = 128;
//    }
//    else
//    {
//        r = 256;
//        g = 256;
//        b = 256;
//    }


    CLAMP(r);
    CLAMP(g);
    CLAMP(b);

    rgb565 = (unsigned short)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
}

void CCameraBase::ConvertYUYVtoRGB565(const void *yuyv_data, void *rgb565_data, const unsigned int w, const unsigned int h)
{
    const unsigned char *src = (unsigned char *)yuyv_data;
    unsigned short *dst = (unsigned short *)rgb565_data;
    for (unsigned int i = 0, j = 0; i < w * h * 2; i += 4, j += 2)
    {
        int y, u, v;
        unsigned short rgb565;
        y = src[i + 0];
        u = src[i + 1];
        v = src[i + 3];
        yuv2rgb565(y, u, v, rgb565);
        dst[j + 0] = rgb565;

        y = src[i + 2];
        yuv2rgb565(y, u, v, rgb565);
        dst[j + 1] = rgb565;
    }
}

