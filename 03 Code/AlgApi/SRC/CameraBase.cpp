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
    _result = OpenDevice();
    if(-1 == _result)
    {
        //如果设备打开失败则关闭设备
        SetFunRt(strFunRts,E_ALGAPI_CAMERA_INIT_FAILED, "USB Camera open failed");
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
        SetFunRt(strFunRts,E_ALGAPI_CAMERA_INIT_FAILED, "USB Camera init failed");
        UninitDevice();
        CloseDevice();
        return E_ALGAPI_CAMERA_INIT_FAILED;
    }

    _result = StartCapturing();
    if(-1 == _result)
    {
        //如果设备开始采集失败则关闭设备
        SetFunRt(strFunRts,E_ALGAPI_CAMERA_INIT_FAILED, "USB Camera start capture failed");
        StopCapturing();
        UninitDevice();
        CloseDevice();
        return E_ALGAPI_CAMERA_INIT_FAILED;
    }

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
//    qWarning("^^^getframe in");
    _result = GetFrame((void **)&p,&len);
//    qWarning("vvvgetframe out");
    if(0 == _result)
    {
        //如果采集成功则转换图像格式为rgb
        ConvertYuv2RgbBuffer(p,pFaceImg,640,480);
        _result = UngetFrame();
        return E_ALGAPI_OK;
    }
    return E_ALGAPI_CAMERA_CAPIMG_FAILED;
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
    v4l2_cropcap cropcap;
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

    memset(&(cropcap), 0, sizeof(cropcap));

    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if(0 == ioctl(_fd, VIDIOC_CROPCAP, &cropcap))
    {
        memset(&(crop), 0, sizeof(crop));
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c = cropcap.defrect;

        if(-1 == ioctl(_fd, VIDIOC_S_CROP, &crop))
        {
            if(EINVAL == errno)
            {
//                emit display_error(tr("VIDIOC_S_CROP not supported"));
            }
            else
            {
//                emit display_error(tr("VIDIOC_S_CROP: %1").arg(QString(strerror(errno))));
                return -1;
            }
        }
    }
    else
    {
//        emit display_error(tr("VIDIOC_CROPCAP: %1").arg(QString(strerror(errno))));
        return -1;
    }

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
//            perror("dqbuf");
            return -1;
        case EIO:
            return -1 ;
        default:
//            emit display_error(tr("VIDIOC_DQBUF: %1").arg(QString(strerror(errno))));
            return -1;
        }
    }

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

