#include <QString>
#include <unistd.h>
#include <stdio.h>
#include <math.h>


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../../serial/serial/include/basictypes.h"
//#include "../../serial/serial/include/asyncserial.h"
#include "event.h"



//QString sensorVal;

MyEvent e;

void myCallback(const unsigned char* data, const int size)
{
    int val = 0;
    int factor = 16;
    static int counter = 0;
    if(NULL != data)
    {
//        for(int i = 0; i < size; i++)
//        {
//            //TO DO
//            val += data[i] * pow(factor, i);
//        }

        //printf("[%04d]当前距离: %dcm\n", counter++, val);
        printf("[%08d]adc____\n", counter++);
//        e.onRecvData(val);
    }   
}


void cardCallback(const unsigned char* data, const int size)
{
    QString val, s;
    static int counter = 0;
    if(NULL != data)
    {
//        for(int i = 0; i < size; i++)
//        {
//            //TO DO
//            val += s.sprintf("%02x", data[i]);
//        }

        //printf("卡号: %s\n", val.toUtf8().data());
        printf("[%08d]IC card____\n", counter++);
        //e.onRecvCard(val);
    }
}

void* OpenRelay(void* arg)
{
    MainWindow* pwnd = (MainWindow *)arg;
    unsigned char data[4] = {0x0D, 0x00, 0x00, 0x00};
    int rtn = 0;
    static int counter = 0;
    while(pwnd->enableOpt)
    {
        rtn = pwnd->sm->Send(data, 4);
        printf("[%d]sending(%d): 0x%02X\n", counter++, rtn, 0x0D000000);
        //printf("[%08d]openRelay____\n", counter++);
        usleep(20000);
    }

    return (void *)0;
}

void* GetSensor(void* arg)
{
    MainWindow* pwnd = (MainWindow *)arg;
    unsigned char data[4] = {0x0C, 0x00, 0x00, 0x00};
    int rtn = 0;
    static int counter = 0;
    while(pwnd->runNow)
    {
        rtn = pwnd->sm->Send(data, 4);
        printf("[%d]sending(%d): 0x%02X\n", counter++, rtn, 0x0c000000);
        rtn = pwnd->sm->Send(data, 4);
        printf("[%d]sending(%d): 0x%02X\n", counter++, rtn, 0x0c000000);
//        emit e.onSendData(0x0c000000);
        //printf("[%08d]getAdc____\n", counter++);
        usleep(20000);
    }

    return (void *)0;
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    sm(SerialMuxDemux::GetInstance()),
    runNow(false),
    enableOpt(false)
{
    ui->setupUi(this);

    int rtnVal = sm->Init("ttys1");
    if(E_SERIAL_OK != rtnVal)
    {
        ui->txtResult->append(QString("串口(%1)打开失败:%2").arg("ttys1").arg(rtnVal));
    }

    sm->AddCallback(0x0c, myCallback);
    sm->AddCallback(0x0f, cardCallback);

    connect(ui->btnRecvSensor, SIGNAL(clicked()), this, SLOT(slotRecvSensor()));
    connect(ui->btnStop, SIGNAL(clicked()), this, SLOT(slotStop()));
    connect(ui->btnExit, SIGNAL(clicked()), this, SLOT(close()));

    connect(&e, SIGNAL(SendData(int)), this, SLOT(slotShowSend(int)));
    connect(&e, SIGNAL(RecvData(int)), this, SLOT(slotShowData(int)));
    connect(&e, SIGNAL(ReadCard(QString)), this, SLOT(slotShowData(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
    sm->ReleaseInstance();
}

void MainWindow::slotRecvSensor()
{
    ui->txtResult->append(QString("开始接收："));

    runNow = true;
    pthread_create(&pId, NULL, GetSensor, this);

    enableOpt = true;
    pthread_create(&pIdRelay, NULL, OpenRelay, this);
}

void MainWindow::slotStop()
{
    if(true == runNow)
    {
        runNow = false;
        pthread_join(pId, NULL);
    }

    if(true == enableOpt)
    {
        enableOpt = false;
        pthread_join(pIdRelay, NULL);
    }


    ui->txtResult->setText(QString("发送线程结束"));
}

void MainWindow::slotShowData(int adcVal)
{
    //TO DO
    static int i = 0;
    ui->txtResult->setText(QString("当前距离：%1cm").arg(adcVal));
    printf("[%d]ADC: %d\n", i++, adcVal);
}


void MainWindow::slotShowData(QString s)
{
    //TO DO
    static int i = 0;
    ui->txtResult->setText(QString("卡号：%1").arg(s));
    printf("[%d]卡号: %s\n", i++, s.toUtf8().data());
}


void MainWindow::slotShowSend(int adcVal)
{
//    static int i = 0;
//    ui->txtResult->append(QString.sprintf("正在发送：0x%X", adcVal));
}
