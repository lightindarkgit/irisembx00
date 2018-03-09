#ifndef DRAWTRACGIF_H
#define DRAWTRACGIF_H

#include <QObject>
#include <QWidget>
#include <QMovie>
#include <QLabel>

#include "IKUSBSDK.h"
#include "interaction.h"
//定义注册和识别时的距离范围
#define IKUseDistSensorForSelfPos_OFF
const int maxEnrSelfPosDist = 31;
const int minEnrSelfPosDist = 26;
const int maxRecSelfPosDist = 34;
const int minRecSelfPosDist = 25;

#pragma pack(push)
#pragma pack(1)
//定义bmp格式图像变量
typedef unsigned short WORD;  //2字节
typedef unsigned int   DWORD; //4字节
typedef unsigned long  LONG; //32位 4字节，64位8字节

typedef struct tagBITMAPFILEHEADER
{ // bmfh
   WORD    bfType;           // 文件标识‘BM’
   DWORD   bfSize;           // 用字节表示的整个文件的大小
   WORD    bfReserved1;      // 保留字节，为0
   WORD    bfReserved2;      // 保留字节，为0
   DWORD   bfOffBits;        // 图象数据RGBA的起始地址的偏移值  为54
} BMPFILEHEADER_T;


typedef struct tagBITMAPINFOHEADER
{ // bmih
   DWORD  biSize;               // 位图信息头(Bitmap Info Header)的长度,为40
   DWORD  biWidth;              // 位图的宽度，以象素为单位
   DWORD  biHeight;             // 位图的高度，以象素为单位
   WORD   biPlanes;             // 位图的位面数（注：该值将总是1）
   WORD   biBitCount;           // 每个象素的位数，设为8(表示8Bit位图)
   DWORD  biCompression;        // 压缩说明，设为0(不压缩)
   DWORD  biSizeImage;          // 用字节数表示的位图数据的大小（该数必须是4的倍数）
   DWORD  biXPelsPerMeter;      // 用象素/米表示的水平分辨率(一般设为0)
   DWORD  biYPelsPerMeter;      // 用象素/米表示的垂直分辨率(一般设为0)
   DWORD  biClrUsed;            // 位图使用的颜色数(一般设为0)
   DWORD  biClrImportant;       // 指定重要的颜色数(一般设为0)
} BMPINFOHEADER_T;

typedef BMPFILEHEADER_T  LPBITMAPFILEHEADER;
typedef BMPINFOHEADER_T LPBITMAPINFOHEADER;
typedef BMPFILEHEADER_T  BITMAPFILEHEADER;
typedef BMPINFOHEADER_T  BITMAPINFOHEADER;

typedef struct RGBQUAD
{
    char rgbBlue;
    char rgbGreen;
    char rgbRed;
    char rgbReserved;

}RGBQUAD;

#pragma pack(pop)

class DrawTracGif : public QObject
{

public:
    explicit DrawTracGif(QObject *parent = 0);
    ~DrawTracGif();

public:

    //获得人眼坐标
    static void     GetTracPosition(IKEnrIdenStatus enrIdenStatus,int &Lx,int &Ly,int &R_x,int &Ry);

    //画人眼跟踪图
    static void     ShowTraceGift(IKEnrIdenStatus enrIdenStatus,QMovie *movTrace,QLabel *labTrace,int x,int y,bool &isLastClear );

    //从buffer中获得图片
    static QImage   GetImg(const unsigned char *buffer,unsigned int bufferSize);

    //输入裸数据，输出BMP图像
    static bool     Rgb2BMP( char* bmpFile, unsigned char* rgbData, unsigned int Height, unsigned int Width);

};

#endif // DRAWTRACGIF_H
