#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <pthread.h>
#include "../SerialMuxDemux/include/serialmuxdemux.h"

namespace Ui {
class MainWindow;
}


void* GetSensor(void* arg);
void myCallback(const unsigned char* data, const int size);

class MainWindow : public QMainWindow
{
    Q_OBJECT

    friend void* GetSensor(void *arg);
    friend void* OpenRelay(void *arg);
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

//    void myCallback(const unsigned char* data, const int size);
signals:
    void recvData(int);

protected slots:
    void slotRecvSensor();
    void slotStop();
    void slotShowData(int);
    void slotShowSend(int);
    void slotShowData(QString s);
    
private:
    Ui::MainWindow *ui;
    SerialMuxDemux *sm;
    volatile bool runNow;
    volatile bool enableOpt;

    pthread_t pId;
    pthread_t pIdRelay;
};

#endif // MAINWINDOW_H
