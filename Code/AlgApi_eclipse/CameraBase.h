/******************************************************************************************
** 文件名:   CameraBase.h
×× 主要类:   CCameraBase
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   lizhl
** 日  期:   2014-01-15
** 修改人:
** 日  期:
** 描  述:   IKEMBX00项目人脸摄像头具体实现，对外不可见
 *×× 主要模块说明: CameraBase.h
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
#pragma once
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <asm/types.h>
#include <linux/videodev2.h>
#include <mutex>
#include <string.h>

#include "AlgApi.h"
#include "BRCCommon.h"

//人脸摄像头基类
class CCameraBase
{
public:
    ~CCameraBase();
    //获取单实例指针
    static CCameraBase* GetInstance();

    //初始化采集人脸图像摄像头
    int		Init(OUT char *strFunRts);
    //采集人脸图像
    int		CapImg(OUT unsigned char *pFaceImg, OUT char *strFunRts);
    //释放采集人脸图像摄像头相关资源
    void	Release();

private:
    CCameraBase();
    CCameraBase(const CCameraBase&);//禁止拷贝赋值
    CCameraBase& operator=(const CCameraBase&);

    int OpenDevice();       //打开设备
    int CloseDevice();      //关闭设备
    int InitDevice();       //初始化设备
    int StartCapturing();   //开始采集
    int StopCapturing();    //停止采集

    int UninitDevice();     //反初始化设备
    int GetFrame(void **, size_t*);//缓冲出队列
    int UngetFrame();       //缓冲重新入队列
    int InitMmap();

    void Restart();

    int ConvertYuv2RgbPixel(int y, int u, int v);//像素转换成rgb格式
    int ConvertYuv2RgbBuffer(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height);//将采集到的yuv图像转换成rgb格式图像

    void yuv2rgb565(const int y, const int u, const int v, unsigned short &rgb565);
    void ConvertYUYVtoRGB565(const void *yuyv_data, void *rgb565_data, const unsigned int w, const unsigned int h);

private:
//    static shared_ptr<CAPIBase> s_pSingletonCAPIBase;
    static CCameraBase *s_pSingletonCCameraBase;                //单实例对象指针
    static std::mutex s_instanceCameraMutex;                   //获取实例mutex

    struct buffer
    {
        void * start;
        size_t length;
    };

    int _instanceCamBaseNum;    //实例计数
    int _fd;                    //设备文件索引？？
    buffer* _buffers;
    unsigned int _nBuffers;     //buffer数量
    int _index;                 //v4l2_buffer的索引
    int _result;                //返回值
    v4l2_buffer _queueBuf;      //用于接收图像的buffer
};
