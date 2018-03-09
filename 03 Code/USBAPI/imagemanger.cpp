/******************************************************************************************
** 文件名:   __IMAGEMANGER_H__
×× 主要类:   ImageManger
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   刘中昌
** 日  期:   2013-11-26
** 修改人:
** 日  期:
** 描  述:  图像管理，图像的存取的操作实现
 *×× 主要模块说明:
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/

#include "imagemanger.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**********************************************************************************************************************************
*                        初始化函数
*  函 数 名：ImageManger
*  功    能：初始化USB的设备
*  说    明：
*  参    数：
*  创 建 人：刘中昌
*  创建时间：2013-11-26
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
ImageManger::ImageManger()
{  
    _len = TransBufferSize;
    _tag = NoImage;
    _image1 = new unsigned char[TransBufferSize];
    _image2 = new unsigned char[TransBufferSize];
     memset(_image1,0,_len);
     memset(_image2,0,_len);
     pthread_mutex_init(&_imageMutex,nullptr);
}
/**********************************************************************************************************************************
*                        析构函数
*  函 数 名：~ImageManger
*  功    能：析构
*  说    明：
*  参    数：
*  返 回 值：错误描述, 异常返回
*  创 建 人：刘中昌
*  创建时间：2013-11-26
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
ImageManger::~ImageManger()
{
    pthread_mutex_destroy(&_imageMutex);
    delete[] _image1;
    delete[] _image2;
}
/**********************************************************************************************************************************
*                        获取图像函数
*  函 数 名：GetImage
*  功    能：获取图像
*  说    明：
*  参    数：
*  返 回 值：错误描述, 异常返回
*  创 建 人：刘中昌
*  创建时间：2013-11-14
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
bool  ImageManger::GetImage(unsigned char * buf, unsigned int &len)
{
    //SimpleLockForMutex lock(_imageMutex);
    timespec tim;
    tim.tv_nsec = 100;
    tim.tv_sec = time(nullptr);
    tim.tv_sec += 5;
    _waiter.wait(tim);
   //_waiter.wait(tim);
    switch(_tag)
    {
    case OneImage://图像1最新
        len = TransBufferSize;
        memmove(buf,_image1,TransBufferSize);
        break;
    case TwoImage://图像2最新
        len = TransBufferSize;
        memmove(buf,_image2,TransBufferSize);
        break;
    case NoImage://无图像
    default:

        return false;
    }
    return true;
}

/**********************************************************************************************************************************
*                        设置图像函数
*  函 数 名：SetImage
*  功    能：设置图像
*  说    明：
*  参    数：
*  返 回 值：错误描述, 异常返回
*  创 建 人：刘中昌
*  创建时间：2013-11-14
*  修 改 人：
*  修改时间：
*********************************************************************************************************************************/
unsigned char* ImageManger::SetImage(unsigned char * buf)
{
    //SimpleLockForMutex lock(_imageMutex);
    unsigned char * ret=buf;
    switch(_tag)
    {
    case NoImage://无图像
    case OneImage://图像1最新
        ret = _image2;
        _image2 = buf;
        _tag = TwoImage;
        break;
    case TwoImage://图像2最新
        ret = _image1;
        _image1 = buf;
        _tag=OneImage;
        break;
    default:
        break;
    }
    _waiter.signal();
    return ret;
}
