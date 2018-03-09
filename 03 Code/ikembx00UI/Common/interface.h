#ifndef __INTERFACE_H__
#define __INTERFACE_H__

///*****************************************************************************
//*  文 件 名：interface.h
//*  主 要 类：
//*  说    明：
//*  创 建 人：刘中昌
//*  创建时间：20013-12-24
//*  修 改 人：
//*  修改时间：
//*****************************************************************************/
#include "../../03 Code/AlgApi/SRC/AlgApi.h"
#include "QObject"
#include "FeatureManageForDemo.h"
//#include "QThread"

///*****************************
//*
//*类 名：MyThread
//*
//*作 用：开启新的进程处理并发事件
//*1，主要处理设备并行（人员信息加载、设备初始化）和备份/恢复数据库
//*2，run函数需要重写
//*
//*******************************/
////#include "QThread"

////class  MyThread :  public  QThread {
////    Q_OBJECT
////public :
////    virtual   void  run();
////};



////新进程
//class IDialogThreadCallback
//{
//public:
//	bool sign;
//public:
//	IDialogThreadCallback(){sign = false;}

//	virtual ~IDialogThreadCallback()
//	{}
//	virtual void run() = 0;
//};


//class DialogThread: public QThread
//{
//	Q_OBJECT
//public:
//	IDialogThreadCallback *callback;

//public:
//	DialogThread()
//		:QThread(NULL)
//	{}
//    DialogThread( IDialogThreadCallback *_callback)
//        :QThread(parent)
//    {
//		callback = _callback;
//    }

//signals:
//    void over();
//public:
//    virtual void run()
//    {
//		callback->run();
//		if(callback->sign)
//		{
//			emit over();
//		}

//    }
//};

//inline DialogThread * startDialogThread( IDialogThreadCallback *callback)
//{
//    DialogThread *t = new DialogThread(callback);
//    t->start();
//	return t;
//}

//class CReadyEnroll: public IDialogThreadCallback
//{
//	DialogEnroll *_dlg;
//public:
//	CReadyEnroll(DialogEnroll *dlg)
//		:_dlg(dlg)
//	{}

//	virtual void run()
//	{
//		_dlg->readyEnrollInfo();
//		sign = true;
//	}
//};


///*****************************
//*
//*类 名：QIdentifyCallback
//*作 用：回调函数 1，回调返回识别结果 2， 返回提示信息
//*
//*******************************/
class ReDrawEvent :public QObject
{
    Q_OBJECT
public:
//    ReDrawEvent(){}
//    ~ReDrawEvent(){}
    void onResult()//返回识别结果
    {
        emit sigResult();
    }
    void onMsg(QString str)//返回提示信息
    {
        emit sigMsg(str);
    }
    //识别界面重绘
    void OnDrawIdent()
    {
        emit sigDrawIdent();
    }

signals:
    void sigDrawIdent();
    void sigResult();
    void sigMsg(QString str);
};

///*****************************
//*
//*类 名：QIdentifyCallback
//*作 用：回调函数 1，回调返回识别结果 2， 返回提示信息
//*
//*******************************/
class IdentEvent :public QObject
{
    Q_OBJECT
public:

    void onResult(long index,int flag)//返回识别结果
    {
        emit sigResult(index,flag);
    }
    void onLoginResult(long index,int flag)//返回管理员识别结果
    {
        emit sigLoginResult(index,flag);
    }
    void onMsg(QString str)//返回提示信息
    {
        emit sigMsg(str);
    }
    //识别界面重绘
    void OnDrawIdent()
    {
        emit sigDrawIdent();
    }

signals:
    void sigDrawIdent();
    void sigResult(long index,int flag);
    void sigLoginResult(long index,int flag);
    void sigMsg(QString str);
};

#include <mutex>
#include <sys/time.h>
#include <QPainter>
#include <QPen>
#include <QBrush>
class CommonFunction
{
public:
    /*****************************************************************************
    *                        设置图像的头
    *  函 数 名： SetImageHeader
    *  功    能： 给图像数据设在头
    *  说    明：
    *  参    数：将要设在的图像地址
    *  返 回 值：
    *  创 建 人：刘中昌
    *  创建时间：2013-12-23
    *  修 改 人：
    *  修改时间：
    *****************************************************************************/
  inline bool  SetShowImageHeader(unsigned char *grayImgBuffer,const int length)
    {
       std::lock_guard<std::mutex> lock(_mutex, std::adopt_lock);
        if(length>=g_constIrisImgSize)
        {
            char grayPpmHeader[g_constIrisPPMHeaderLength];
            sprintf(grayPpmHeader,"P5\n%d %d\n255\n", g_constIrisImgWidth,g_constIrisImgHeight);
            memcpy(grayImgBuffer,grayPpmHeader,g_constIrisPPMHeaderLength);
            memset(grayImgBuffer+g_constIrisPPMHeaderLength,0,g_constIrisImgSize);
            return true;
        }
        return false;
    }
  /*****************************************************************************
  *                        保存需要保存的图像
  *  函 数 名： IKSaveGrayFile
  *  功    能： 保存图像
  *  说    明：
  *  参    数：将要设在的图像地址
  *  返 回 值：
  *  创 建 人：刘中昌
  *  创建时间：2013-12-26
  *  修 改 人：
  *  修改时间：
  *****************************************************************************/
  void IKSaveGrayFile(const char* filename,unsigned char *pImgBuffer, int nWidth, int nHeight)
  {
      struct tm *ptmNowTime;
      struct timeval tv_NowTime;
      time_t lt;
      char cTimeName[200];
      static int n=0;

      lt =time(NULL);
      ptmNowTime = localtime(&lt);
      gettimeofday(&tv_NowTime,NULL);
      sprintf(cTimeName,"./%s_%02d_%02d_%02d_%02d_%02d_%04ld.ppm",filename,ptmNowTime->tm_mon+1,ptmNowTime->tm_mday,ptmNowTime->tm_hour,ptmNowTime->tm_min, ptmNowTime->tm_sec,tv_NowTime.tv_usec);


      std::lock_guard<std::mutex> lock(_mutex, std::adopt_lock);
      FILE* ff = NULL;
      ff=fopen(filename,"wb");
      if(ff)
      {
          fprintf(ff, "P5\n" "%d %d\n255\n", nWidth, nHeight);
          fwrite(pImgBuffer, 1, nWidth * nHeight , ff);
          fclose(ff);
      }
  }
  void DrawIdentResult(QPaintDevice* w,int xPos,int yPos,int witchPic)
  {
      std::lock_guard<std::mutex> lock(_mutex, std::adopt_lock);
      QPainter painter(w);
      QPen pen;
      pen.setColor(Qt::red);
      pen.setWidth(5);
      painter.setPen(pen);
      QPainterPath path;
      path.addRect(xPos,yPos,200,150);
      path.moveTo(xPos+20,yPos+40);
      path.lineTo(xPos+60,yPos+140);
      path.lineTo(xPos+180,yPos+10);
      painter.drawPath(path);
      //painter.drawRect(xPos,yPos,200,150);
     // painter.end();
  }

private:
  std::mutex _mutex;
};



#endif //__INTERFACE_H__
