#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "usbcommon.h"
#include <QTimer>

//unsigned char g_imBuffer[TransBufferSize + PPMHeaderLength];
unsigned char g_grayImBuffer[TransBufferSize + PPMHeaderLength];
char g_grayPpmHeader[PPMHeaderLength];

ImagePaintEvent imPaintEvent;
void ImagePaintEvent::RaisePaintEvent()
{
    emit sigPaint();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _displayButtonClicked=true;
    //隐藏标题栏
    //setWindowFlags(Qt::CustomizeWindowHint);
    //_imageBuf = new unsigned char[TransBufferSize];
    _grayImag = new unsigned char[TransBufferSize];
    _count =0;

    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(ShowCount()));
    timer->start(1000);

   _ikUSBAPI.Open();
    pthread_mutex_init(&_imageMutex,nullptr);

    sprintf(g_grayPpmHeader,"P5\n%d %d\n255\n", ImageWidth,ImageHeight);
    memcpy(g_grayImBuffer,g_grayPpmHeader,PPMHeaderLength);
    memset(g_grayImBuffer+PPMHeaderLength,0,TransBufferSize);


    connect(&imPaintEvent,SIGNAL(sigPaint()),this,SLOT(imageUpdate()));
    pthread_t pthreadID;
    pthread_create(&pthreadID, 0, ThreadDisplayImage, (void*)this);
    imageUpdate();
}

MainWindow::~MainWindow()
{
    Close();
    //delete[] _imageBuf;
    delete[] _grayImag;
    pthread_mutex_destroy(&_imageMutex);

    delete ui;
}

void MainWindow::ShowCount()
{
    ui->labCount->setText(QString::number(_count));
    _count=0;
}
void MainWindow::Open()
{
     _ikUSBAPI.Open();
     _waiter.broadcast();
}

void MainWindow::Close()
{
    _ikUSBAPI.Close();
}

void MainWindow::imageUpdate()
{
    SimpleLockForMutex lock(_imageMutex);
    unsigned int length=0;

    if(E_OK !=_ikUSBAPI.GetImage(_grayImag,length))
    {
        _waiter.broadcast();
        return;
    }
    memcpy(g_grayImBuffer+PPMHeaderLength,_grayImag,TransBufferSize);


    _imageBig = QImage::fromData(g_grayImBuffer, TransBufferSize+PPMHeaderLength, "PPM");
    if(!_imageBig.isNull())
    {
        _imageBig = _imageBig.scaled (ui->labImg->width()-1,
                                      ui->labImg->height()-1,
                                      Qt::KeepAspectRatio);

        ui->labImg->setPixmap(QPixmap::fromImage(_imageBig));
        _count++;
    }
    _waiter.broadcast();
}

void* MainWindow::ThreadDisplayImage(void* pParam)
{
    MainWindow *pCGDlg = (MainWindow*) pParam;
    while(pCGDlg->_displayButtonClicked)
    {
        pCGDlg->_waiter.wait();
        imPaintEvent.RaisePaintEvent();
    }

    return ((void*)0);
}
