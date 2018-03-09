#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <pthread.h>
#include <semaphore.h>

//包含串口API头文件
#include "../../serial/asyncserial.h"
#include "../../serial/syncserial.h"
#include "../../serial/basictypes.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    //显示收到的串口数据
    void showResult(const unsigned char* pBuf, const int dataSize);

private:
    void initUI();
    static void *poll_recv(void* arg);

signals:
    void recvDataSig();

private slots:
    void slotOpen();
    void slotClose();
    void slotSetConfig(int);
    void slotSend();
    void slotWorkModeChanged(int);
    void slotRecv();
    
private:
    Ui::MainWindow *ui;

    AsyncSerial asySerial;
    SyncSerial syncSerial;

    bool runAllowed;
    unsigned char outputBuf[9];   //存储输出数据缓冲区
    sem_t threadSem;
    bool serialOpened;
//    int frameBufSize;
//    unsigned char sendBuf[512];
//    int sendBufSize;
};

#endif // MAINWINDOW_H
