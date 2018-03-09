/******************************************************************************************
** 文件名:   MAINWINDOW_H
×× 主要类:   MainWindow
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   刘中昌
** 日  期:   2013-12-20
** 修改人:
** 日  期:
** 描  述:  主界面
 *×× 主要模块说明:
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "syncusbapi.h"
#include "simplelock.h"
#include <mutex>
#include <sys/time.h>
namespace Ui {
class MainWindow;
}

class ImagePaintEvent : public QObject
{
    Q_OBJECT
public:
    void RaisePaintEvent();
    void RaisePaintEvent1();

signals:
    void sigPaint();
    void sigPaint1();
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool _displayButtonClicked;

    int _threadDisId;
    QImage _imageBig;
private :
    void Bayer2Gray(unsigned char* bayerFrame, unsigned char* grayIm, int imH, int imW);
    static void* ThreadDisplayImage(void* pParam);
    static void* ThreadDisplayImage1(void* pParam);
    void IKSaveGrayFile(const char* filename,unsigned char *pImgBuffer, int nWidth, int nHeight);
private slots:
    void imageUpdate();
    void Close();
    void ShowCount();
    void Open();
    void GetMinPhono();
    void Test();

private:
    //unsigned char *_imageBuf;
    unsigned char *_grayImag;
    unsigned char *_leftImag;
    unsigned char *_rightImag;

    Ui::MainWindow *ui;

    pthread_t _pthreadID;
    pthread_t _pthreadID1;
    SyncUSBAPI _ikUSBAPI;
    pthread_mutex_t _imageMutex;
    ThreadWaiter  _waiter;
    ThreadWaiter  _waiter1;
    int _count;

    bool _isOpen;
};


////新进程
//class IDialogThreadCallback
//{
//public:
//    bool sign;
//public:
//    IDialogThreadCallback(){sign = false;}

//    virtual ~IDialogThreadCallback()
//    {}
//    virtual void run() = 0;
//};


//class MyThread: public QThread
//{
//    Q_OBJECT
//public:
//    IDialogThreadCallback *callback;

//public:
//    MyThread()
//        :QThread(nullptr)
//    {}
//    MyThread( IDialogThreadCallback *_callback)
//        :QThread(parent)
//    {
//        callback = _callback;
//    }

//signals:
//    void over();
//public:
//    virtual void run()
//    {
//        callback->run();
//        if(callback->sign)
//        {
//            emit over();
//        }

//    }
//};

//inline MyThread * startDialogThread( IDialogThreadCallback *callback)
//{
//    MyThread *t = new MyThread(callback);
//    t->start();
//    return t;
//}

////class CReadyEnroll: public IDialogThreadCallback
////{
////	DialogEnroll *_dlg;
////public:
////	CReadyEnroll(DialogEnroll *dlg)
////		:_dlg(dlg)
////	{}

////	virtual void run()
////	{
////		_dlg->readyEnrollInfo();
////		sign = true;
////	}
////};


//class Callback :public QObject
//{
//    Q_OBJECT
//public:
//    void  onGetImage()
//    {
//        emit sigGetImage();
//    }
//    void  onClose()
//    {
//        emit sigClose();
//    }
//signals:
//    void sigGetImage();
//    void sigClose();
//};


#endif // MAINWINDOW_H
