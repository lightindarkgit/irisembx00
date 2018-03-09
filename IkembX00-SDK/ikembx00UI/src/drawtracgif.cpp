#include "drawtracgif.h"

DrawTracGif::DrawTracGif(QObject *parent):
    QObject(parent)
{
}
DrawTracGif::~DrawTracGif()
{}
void DrawTracGif::GetTracPosition(IKEnrIdenStatus enrIdenStatus,int &Lx,int &Ly,int &Rx,int &Ry)
{
    //找不到眼睛用默认坐标
    if( enrIdenStatus.uiueID == UIUE_MsgID_EYE_NOT_FOUND )
    {
        Lx  = g_defaultTraceLeftBeginX + g_defaultTraceSize/2;
        Ly = g_imgHeight/2 ;
        Rx = g_defaultTraceRightBeginX + g_defaultTraceSize/2;
        Ry = g_imgHeight /2;

    }
    else
    {

        if(enrIdenStatus.irisPos.leftIrisPos.xPixel + enrIdenStatus.irisPos.leftIrisPos.yPixel> 0 )
        {
            //左眼坐标
            Lx = enrIdenStatus.irisPos.leftIrisPos.xPixel/4 + 80;
            Ly = IK_DISPLAY_IMG_HEIGHT - enrIdenStatus.irisPos.leftIrisPos.yPixel/4 - 60;

        }
        else
        {
            Lx = 0;
            Ly = 0;
        }

        if(enrIdenStatus.irisPos.rightIrisPos.xPixel + enrIdenStatus.irisPos.rightIrisPos.yPixel > 0 )
        {
            //右眼坐标
            Rx = enrIdenStatus.irisPos.rightIrisPos.xPixel/4 +80;
            Ry = IK_DISPLAY_IMG_HEIGHT - enrIdenStatus.irisPos.rightIrisPos.yPixel/4 -60;
        }
        else
        {
            Rx = 0;
            Ry = 0;
        }
    }

}
void DrawTracGif::ShowTraceGift(IKEnrIdenStatus enrIdenStatus,QMovie *movTrace,QLabel *labTrace,int x,int y,bool &isLastClear)
{
    if(nullptr == movTrace || nullptr==labTrace)
    {
        LOG_ERROR("in put ui ptr is null");
        return ;
    }
    bool isClear = false ;

    if(x + y > 0)
    {

        if(enrIdenStatus.distEstimation == DistSuitable && enrIdenStatus.uiueID == UIUE_MsgID_SUITABLE)
        {
            movTrace->setFileName(":/image/ui_trace_g.gif");
        }
        else
        {
            movTrace->setFileName(":/image/ui_trace_r.gif");
        }
        movTrace->restart();
        labTrace->setMovie(movTrace);


        labTrace->setGeometry((int)(x * g_scaling) - g_defaultTraceSize/2 + g_imgBeginX,
                              (int)(y * g_scaling) - g_defaultTraceSize/2 + g_imgBeginY,
                              g_defaultTraceSize,
                              g_defaultTraceSize);

        if(!labTrace->isVisible())
        {
            labTrace->setVisible(true);
        }
    }
    else
    {
        //未检测到眼睛则不显示追踪gif
        labTrace->setVisible(false);
    }
    isLastClear = isClear;

    return;
}
/*****************************************************************************
*                         从摄像头中获取图片
*  函 数 名：GetImg
*  功    能：从摄像头采集的buffer中获取图片数据形成图片
*  说    明：
*  参    数：buffer
*
*  返 回 值：QImage
*  创 建 人：guodj
*  创建时间：2016-10-12
*  修 改 人：lizhl
*  修改时间：20161026
*****************************************************************************/
QImage DrawTracGif::GetImg(const unsigned char *buffer, unsigned int bufferSize)
{
    QImage imageBig;
    if(nullptr == buffer)
    {
        LOG_ERROR("input ptr buffer is null");
        return imageBig;
    }
    if(bufferSize< 0||bufferSize> 20*1024*1024)
    {
        LOG_ERROR("inputlent is erro %d",bufferSize);
        return imageBig;
    }


    unsigned char grayImgBuffer[bufferSize + g_ppmHeaderLength];
    sprintf(grayImgBuffer, "P5\n 640 480\n255\n");
    memmove(grayImgBuffer + g_ppmHeaderLength , buffer , bufferSize);
    imageBig = QImage::fromData(grayImgBuffer,
                                       bufferSize + g_ppmHeaderLength,
                                       "PGM"
                                       ).mirrored(false, true).convertToFormat(QImage::Format_RGB32);
    return imageBig;
}



bool DrawTracGif::Rgb2BMP(  char* bmpFile, unsigned char* rpgData, unsigned int Height, unsigned int Width)
{
    if(nullptr == bmpFile)
    {
        LOG_ERROR("input ptr bmpFile is null");
        return false;

    }

    if(nullptr == rpgData)
    {
        LOG_ERROR("input ptr rpgData is null");
        return false;
    }

    int height      =   Height;
    int width       =   Width;
    int imagesize   =   height*width;

    BITMAPFILEHEADER         bmpfileheader;     //定义文件头
    BITMAPINFOHEADER         bmpinfoheader;     //定义信息头
    RGBQUAD                  bmprgb[256];       //定义8bit灰度颜色表

    bmpfileheader.bfType          =      (WORD)('M' << 8 | 'B');
    bmpfileheader.bfReserved1     =      (WORD)0;
    bmpfileheader.bfReserved2     =      (WORD)0;
    bmpfileheader.bfOffBits       =      (DWORD)(sizeof(BITMAPFILEHEADER)+ sizeof(BITMAPINFOHEADER)+ (256* 4));
    bmpfileheader.bfSize          =      (DWORD)(bmpfileheader.bfOffBits + (DWORD)imagesize);

    bmpinfoheader.biSize          =      (DWORD)sizeof(BITMAPINFOHEADER);
    bmpinfoheader.biWidth         =      (long)width;
    bmpinfoheader.biHeight        =      (long)height;
    bmpinfoheader.biPlanes        =      (WORD)1;
    bmpinfoheader.biBitCount      =      (WORD)8;
    bmpinfoheader.biCompression   =      (DWORD)0;
    bmpinfoheader.biSizeImage     =      (DWORD)imagesize;
    bmpinfoheader.biXPelsPerMeter =      (LONG)0;
    bmpinfoheader.biYPelsPerMeter =      (LONG)0;
    bmpinfoheader.biClrUsed       =      (DWORD)256;
    bmpinfoheader.biClrImportant  =      (DWORD)0;

    for(int i= 0;i< 256; i++)
    {
        bmprgb[i].rgbBlue       =   (char)i;
        bmprgb[i].rgbGreen      =   (char)i;
        bmprgb[i].rgbRed        =   (char)i;
        bmprgb[i].rgbReserved   =   (char)0;
    }

    int index =0;
   memmove(bmpFile,(char*)&bmpfileheader,sizeof(BITMAPFILEHEADER));
    index += sizeof(BITMAPFILEHEADER);
    memmove(bmpFile+index,(char*)&bmpinfoheader,sizeof(BITMAPINFOHEADER));
    index +=sizeof(BITMAPINFOHEADER);
    memmove(bmpFile+index, bmprgb, sizeof(RGBQUAD) * 256);
    index += sizeof(RGBQUAD) * 256;
    memmove(bmpFile +index,rpgData,imagesize);

////for test
//    QFile file("testEyeBmp.bmp");
//    file.open(QIODevice::WriteOnly);
//    file.write(bmpFile,bmpfileheader.bfSize);
//    file.close();

}
