#ifndef __DEALWITHUSBTRANSFERBUF_H__
#define __DEALWITHUSBTRANSFERBUF_H__
/******************************************************************************************
** 文件名:   DEALWITHUSBTRANSFERBUF_H
×× 主要类:   DealWithUSBTransferBuf
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   刘中昌
** 日  期:   2013-11-22
** 修改人:
** 日  期:
** 描  述:  数据处理函数--根据USB 定义的传输协议 重组所需要的数据
 *×× 主要模块说明:
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/

#include "usbcommon.h"
#include "simplelock.h"
//#include "imagemanger.h"
#include "blockingqueue.h"
#include <string.h>

//
class EyeImage
{
public:
    EyeImage():_offset(0){

        pthread_mutex_init(&_mutex,nullptr);
    }

    ~EyeImage()
    {
        pthread_mutex_destroy(&_mutex);

    }

    //是否存储图片 ---为使用
    bool IsFull()
    {
        return (_offset == PreviewImageSize);
    }
//    获取图像
    bool GetImage(unsigned char * buf, unsigned int &len)
    {
        SimpleLockForMutex lock(_mutex);
        len = _offset;
        memmove(buf,&_image[0],_offset);
        return _offset == PreviewImageSize;
    }

//    unsigned char* SetImage(unsigned char * buf, unsigned int len)
//    {
//        if(len != PreviewImageSize)
//        {
//            return buf;
//        }

//        SimpleLockForMutex lock(_mutex);
////        pthread_mutex_lock(&_mutex);
////        unsigned char * ret=_image;
////        _image = buf;
////        _offset = PreviewImageSize;
////        pthread_mutex_unlock(&_mutex);
//        return ret;
//    }

    //设置图像
    bool SetImageCopy(unsigned char * buf, unsigned int len)
    {
        SimpleLockForMutex lock(_mutex);
        _offset = len;
        memmove(&_image[0],buf,len);
        return _offset == PreviewImageSize;
    }

private:
    pthread_mutex_t _mutex;
    unsigned char _image[PreviewImageSize];

    int _offset;
};


class PictureImage
{

public:
    //获取图像
    bool GetImage(unsigned char * buf1,unsigned int &len1, unsigned char * buf2,unsigned int &len2)
    {
       return  leftImage.GetImage(buf1,len1) && rightImage.GetImage(buf2,len2);
    }
    //设置图像
    bool SetImage(unsigned char * buf1,unsigned int len1, unsigned char * buf2,unsigned int len2)
    {
        return leftImage.SetImageCopy(buf1,len1) && rightImage.SetImageCopy(buf2,len2);
    }

private:
    EyeImage leftImage,rightImage;

};


//枚举类---Preview；为降采样数据；Picture:为扣取小图
enum class TransBufType
{
    Preview,
    Picture,
    UnKnow,
};

class DealWithUSBTransferBuf
{
public:
    DealWithUSBTransferBuf();
    ~DealWithUSBTransferBuf();
    unsigned char* DealBuf(unsigned char* buf);
    bool GetPreview(unsigned char * buf, unsigned int &len);
    bool GetPictureTaken(unsigned char * buf1, unsigned int &len1, unsigned char *buf2, unsigned int &len2);
private:
    static void *DealBufThread(void *arg);
    //转灰度
    void Bayer2Gray(unsigned char* bayerFrame, unsigned char* grayIm, int imH, int imW);
    //void RepairEndRC(unsigned char* imageBuf, unsigned char* grayIm, int imH, int imW);
    //组帧
    void ReconstructImage(const unsigned char *messBuf, int imageW,int imageH,unsigned char *imageBuf );
    //非降采样组帧
    void ReconstructTwoImage(const unsigned char *messBuf,int imageH,int imageW,unsigned char* leftImageBuf ,unsigned char* rightImageBuf);
    //将采样
    void DecodeReduceSampledImage(const unsigned char *buf,int width,int height, unsigned char *image);

//    template<typename T>
    void CopyBuf();
    void FindHead();
private:

    //中间数据
    unsigned char *_mediBuf;
    //中间数据
    unsigned char *_messBuf;


    //中间数据偏移量，当前数据大小
    unsigned int  _mediBufOffset;
    unsigned int  _index;
    unsigned int  _offset;

    //降采样组帧图片
    unsigned char *_previewImageBuf;
    //降采样灰度图片
    unsigned char *_grayImageBuf;

    //左眼组帧图片
    unsigned char *_previewLeftImageBuf;
    //左眼灰度图片
    unsigned char *_grayLeftImageBuf;
    //右眼组帧图片
    unsigned char *_previewRightImageBuf;
    //右眼灰度图片
    unsigned char *_grayRightImageBuf;

    pthread_t _pollThread;
    //图片管理类
    //ImageManger   _imageManager;
    ThreadWaiter  _threadWaiter;

    pthread_mutex_t _mutex;
    pthread_mutex_t _copyMutex;

    TransBufType   _transBufType;

    BlockingQueue<EyeImage> _queuePreview;
    BlockingQueue<PictureImage> _queuePictureTaken;
};

#endif // DEALWITHUSBTRANSFERBUF_H
