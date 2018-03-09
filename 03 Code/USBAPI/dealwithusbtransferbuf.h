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
#include "imagemanger.h"

class DealWithUSBTransferBuf
{
public:
    DealWithUSBTransferBuf();
    ~DealWithUSBTransferBuf();
    unsigned char* DealBuf(unsigned char* buf);
    bool GetImage(unsigned char * buf, unsigned int &len);
private:
    static void *DealBufThread(void *arg);
    void Bayer2Gray(unsigned char* bayerFrame, unsigned char* grayIm, int imH, int imW);
    //void RepairEndRC(unsigned char* imageBuf, unsigned char* grayIm, int imH, int imW);
    void ReconstructImage();
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
    //图片
    unsigned char *_imageBuf;
    //灰度图片
    unsigned char *_grayImageBuf;

    pthread_t _pollThread;
    //图片管理类
    ImageManger   _imageManager;
    ThreadWaiter  _threadWaiter;

    pthread_mutex_t _mutex;

    //time_t  _timeBegin;
};

#endif // DEALWITHUSBTRANSFERBUF_H
