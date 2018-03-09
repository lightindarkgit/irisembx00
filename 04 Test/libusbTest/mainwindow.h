#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"simplelock.h"
#include "syncubsapi.h"

namespace Ui {
class MainWindow;
}

class ImagePaintEvent : public QObject
{
    Q_OBJECT
public:
    void RaisePaintEvent();
signals:
    void sigPaint();
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
private slots:
    void imageUpdate();
    void Close();
    void ShowCount();
    void Open();

private:
    //unsigned char *_imageBuf;
    unsigned char *_grayImag;
    Ui::MainWindow *ui;
    SyncUSBAPI _ikUSBAPI;
    pthread_mutex_t _imageMutex;
    ThreadWaiter  _waiter;
    int _count;
};



#endif // MAINWINDOW_H
