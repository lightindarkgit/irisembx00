/******************************************************************************************
** 文件名:   __IMAGEMANGER_H__
×× 主要类:   ImageManger
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   刘中昌
** 日  期:   2013-11-26
** 修改人:
** 日  期:
** 描  述:  图像管理，图像的存取的操作
 *×× 主要模块说明:
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/
#ifndef __IMAGEMANGER_H__
#define __IMAGEMANGER_H__
#include "simplelock.h"
#include "usbcommon.h"
#include "thread"

//图片状态
enum ImageStatus {
    NoImage = 0,
    OneImage,
    TwoImage,
    Other
};

class ImageManger
{
public:
    ImageManger();
    ~ImageManger();
    bool GetImage(unsigned char * buf, unsigned int &len);
    unsigned char *SetImage(unsigned char * buf);
private:
    unsigned char *_image1;
    unsigned char *_image2;
    unsigned int    _len;
    ThreadWaiter  _waiter;
    //0没有图片、1——image1图片最新 2——image2图片最新
    ImageStatus             _tag;
    pthread_mutex_t _imageMutex;
};

#endif // IMAGEMANGER_H
