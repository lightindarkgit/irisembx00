/******************************************************************************************
** �ļ���:   CameraBase.cpp
���� ��Ҫ��:   CCameraBase
**
** Copyright (c) �пƺ�����޹�˾
** ������:   lizhl
** ��  ��:   2014-01-15
** �޸���:
** ��  ��:
** ��  ��:   IKEMBX00��Ŀ��������ͷ����ʵ�֣����ⲻ�ɼ�
 *���� ��Ҫģ��˵��: ��������ͷ����ʵ��
**
** ��  ��:   1.0.0
** ��  ע:
**
*****************************************************************************************/
#include "CameraBase.h"
//#include <QDebug>

using namespace std;
mutex CCameraBase::s_instanceCameraMutex;
CCameraBase* CCameraBase::s_pSingletonCCameraBase = NULL;

/*****************************************************************************
*                         CCameraBase���캯��
*  �� �� ����CCameraBase
*  ��    �ܣ����캯��
*  ˵    ����
*  ��    ����NULL
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2014-01-15
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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
*                         CCameraBase��������
*  �� �� ����~CCameraBase
*  ��    �ܣ���������
*  ˵    ����
*  ��    ����NULL
*  �� �� ֵ��NULL
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2014-01-15
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
CCameraBase::~CCameraBase()
{

}

/*****************************************************************************
*                         ��ȡCCameraBaseʵ��
*  �� �� ����GetInstance
*  ��    �ܣ���ȡCCameraBaseʵ��
*  ˵    ����CCameraBase���õ���ģʽ���Ҷ��̰߳�ȫ
*  ��    ����NULL
*
*  �� �� ֵ��!NULL: CAPIBaseʵ��ָ��
*			NULL����ȡʧ��
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2014-01-15
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
CCameraBase* CCameraBase::GetInstance()
{
    // ˫�ؼ�飬�������
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
*                         ��ʼ����������ͷ
*  �� �� ����Init
*  ��    �ܣ���ʼ����������ͷ
*  ˵    ����
*  ��    ����strFunRts��������������ַ�����ʽ��ʾ���κ������ý�������û�����
*
*  �� �� ֵ��0���ɹ�
*			<0��ʧ��
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2014-01-15
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
int CCameraBase::Init(OUT char *strFunRts)
{
    _result = OpenDevice();
    if(-1 == _result)
    {
        //����豸��ʧ����ر��豸
        SetFunRt(strFunRts,E_ALGAPI_CAMERA_INIT_FAILED, "USB Camera open failed");
        CloseDevice();
        return E_ALGAPI_CAMERA_INIT_FAILED;
    }

    _result = InitDevice();
    if(-1 == _result)
    {
        //�����ʼ��ʧ�ܣ�����һ��
        usleep(100);
        _result = InitDevice();
    }

    if(-1 == _result)
    {
        //����豸��ʼ��ʧ����ر��豸
        SetFunRt(strFunRts,E_ALGAPI_CAMERA_INIT_FAILED, "USB Camera init failed");
        UninitDevice();
        CloseDevice();
        return E_ALGAPI_CAMERA_INIT_FAILED;
    }

    _result = StartCapturing();
    if(-1 == _result)
    {
        //����豸��ʼ�ɼ�ʧ����ر��豸
        SetFunRt(strFunRts,E_ALGAPI_CAMERA_INIT_FAILED, "USB Camera start capture failed");
        StopCapturing();
        UninitDevice();
        CloseDevice();
        return E_ALGAPI_CAMERA_INIT_FAILED;
    }

    return E_ALGAPI_OK;
}

/*****************************************************************************
*                         �ɼ�ͼ��
*  �� �� ����CapImg
*  ��    �ܣ��ɼ�����ͼ��
*  ˵    ����
*  ��    ����pFaceImg������������ɼ���ͼ���ָ��
*           strFunRts��������������ַ�����ʽ��ʾ���κ������ý�������û�����
*
*  �� �� ֵ��0���ɹ�
*			<0��ʧ��
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2014-01-15
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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
        //����ɼ��ɹ���ת��ͼ���ʽΪrgb
        ConvertYuv2RgbBuffer(p,pFaceImg,640,480);
        _result = UngetFrame();
        return E_ALGAPI_OK;
    }
    return E_ALGAPI_CAMERA_CAPIMG_FAILED;
}

/*****************************************************************************
*                         �ͷ���������ͷ
*  �� �� ����Release
*  ��    �ܣ��ͷ���������ͷ
*  ˵    ����
*  ��    ����NULL
*
*  �� �� ֵ��NULL
*
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2014-01-15
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
void CCameraBase::Release()
{
    StopCapturing();
    UninitDevice();
    CloseDevice();
}


/*****************************************************************************
*                         ���豸
*  �� �� ����OpenDevice
*  ��    �ܣ����豸
*  ˵    ����
*  ��    ����NULL
*
*  �� �� ֵ��0���ɹ�
*			<0��ʧ��
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2014-01-15
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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
*                         �ر��豸
*  �� �� ����CloseDevice
*  ��    �ܣ����豸
*  ˵    ����
*  ��    ����NULL
*
*  �� �� ֵ��0���ɹ�
*			<0��ʧ��
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2014-01-15
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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
*                         ��ʼ���豸
*  �� �� ����InitDevice
*  ��    �ܣ���ʼ���豸
*  ˵    ����
*  ��    ����NULL
*
*  �� �� ֵ��0���ɹ�
*			<0��ʧ��
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2014-01-15
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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
*                         ��ʼ��mmap
*  �� �� ����InitMmap
*  ��    �ܣ���ʼ��mmap
*  ˵    ����
*  ��    ����NULL
*
*  �� �� ֵ��0���ɹ�
*			<0��ʧ��
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2014-01-15
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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
*                         ��ʼ�ɼ�ͼ��
*  �� �� ����StartCapturing
*  ��    �ܣ���ʼ�ɼ�ͼ��
*  ˵    ����
*  ��    ����NULL
*
*  �� �� ֵ��0���ɹ�
*			<0��ʧ��
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2014-01-15
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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
*                         ֹͣ�ɼ�ͼ��
*  �� �� ����StopCapturing
*  ��    �ܣ�ֹͣ�ɼ�ͼ��
*  ˵    ����
*  ��    ����NULL
*
*  �� �� ֵ��0���ɹ�
*			<0��ʧ��
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2014-01-15
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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
*                         ����ʼ���豸
*  �� �� ����UninitDevice
*  ��    �ܣ�����ʼ���豸
*  ˵    ����
*  ��    ����NULL
*
*  �� �� ֵ��0���ɹ�
*			<0��ʧ��
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2014-01-15
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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
*                         ���������
*  �� �� ����GetFrame
*  ��    �ܣ���ȡһ֡ͼ��
*  ˵    ����
*  ��    ����frameBuf��ͼ��ָ��
*           len��ͼ�񳤶�
*
*  �� �� ֵ��0���ɹ�
*			<0��ʧ��
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2014-01-15
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
int CCameraBase::GetFrame(void **frameBuf, size_t* len)
{
//    v4l2_buffer queueBuf;
    memset(&(_queueBuf), 0, sizeof(_queueBuf));

    _queueBuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    _queueBuf.memory = V4L2_MEMORY_MMAP;
//    qWarning("EAGAIN = %d",EAGAIN);

    //��������ȡ���Ѳɼ����ݵ�֡���壬ȡ��ԭʼ�ɼ�����
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
*                         �������������
*  �� �� ����UngetFrame
*  ��    �ܣ�Ϊ�´β�ͼ��׼��
*  ˵    ����
*  ��    ����NULL
*
*  �� �� ֵ��0���ɹ�
*			<0��ʧ��
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2014-01-15
*  �� �� �ˣ�
*  �޸�ʱ�䣺
*****************************************************************************/
int CCameraBase::UngetFrame()
{
    if(_index != -1)
    {
        memset(&(_queueBuf), 0, sizeof(_queueBuf));

        _queueBuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        _queueBuf.memory = V4L2_MEMORY_MMAP;
        _queueBuf.index = _index;

        //���������������β,��������ѭ���ɼ�
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
*                         ��yuv����תΪrgb����
*  �� �� ����ConvertYuv2RgbPixel
*  ��    �ܣ���yuv����תΪrgb����
*  ˵    ����
*  ��    ����y�����������y����ֵ
*           u�����������u����ֵ
*           v�����������v����ֵ
*
*  �� �� ֵ��rgb����ֵ
*
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2014-01-15
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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
*                         ��yuvͼ��תΪrgbͼ��
*  �� �� ����ConvertYuv2RgbBuffer
*  ��    �ܣ���yuvͼ��תΪrgbͼ��
*  ˵    ����
*  ��    ����yuv�����������yuvͼ��ָ��
*           rgb�����������rgbͼ��ָ��
*           width�����������ͼ����
*           heigh�����������ͼ��߶�
*
*  �� �� ֵ��0���ɹ���
*           <0��ʧ��
*
*  �� �� �ˣ�lizhl
*  ����ʱ�䣺2014-01-15
*  �� �� �ˣ�
*  �޸�ʱ�䣺
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

