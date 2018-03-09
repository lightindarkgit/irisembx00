#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <semaphore.h>
#include <QString>
#include <sys/time.h>
#include <unistd.h>


const int CMP_EQUAL = 0;
const int CMP_LESS = -1;
const int CMP_GREATER = 1;

const int frameBufSize = 9;
unsigned char frameBuf[frameBufSize];   //存储接收到的帧数据

int recvDataSize = 0;
const int sendBufSize = 512;
unsigned char sendBuf[sendBufSize];

bool haveRecvNewData = false;
pthread_cond_t recvedNewDataCon = PTHREAD_COND_INITIALIZER;
pthread_cond_t procDataCon = PTHREAD_COND_INITIALIZER;
pthread_mutex_t recvDataMutex = PTHREAD_MUTEX_INITIALIZER;

bool allowRun = false;

void callback(const unsigned char* pBuf, const int dataSize)
{
    //TO DO
    int cpySize = dataSize > frameBufSize ? frameBufSize : dataSize;

    //static int frameCounter = 0;
    recvDataSize = cpySize;

    while(haveRecvNewData && allowRun)
    {
        sleep(0);
    }

    pthread_mutex_lock(&recvDataMutex);
    memcpy(frameBuf, pBuf, cpySize);
    haveRecvNewData = true;
    pthread_mutex_unlock(&recvDataMutex);

    //printf("[%2d]Get frame: %x\n", frameCounter++,frameBuf[0]);
    //pthread_cond_signal(&recvedNewDataCon);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
//    runAllowed(false),
    serialOpened(false)
{
    ui->setupUi(this);

    initUI();
//    ui->btnClose->setDisabled(true);
//    ui->btnOpen->setDisabled(false);

    if(CMP_EQUAL == ui->cmbWokMode->currentText().compare("异步"))
    {
        sem_init(&threadSem, 0, 0);
    }

    connect(ui->btnOpen, SIGNAL(clicked()), this, SLOT(slotOpen()));
    connect(ui->btnClose, SIGNAL(clicked()), this, SLOT(slotClose()));
    connect(ui->btnSend, SIGNAL(clicked()), this, SLOT(slotSend()));
    connect(ui->cmbBaudRate, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSetConfig(int)));
    connect(ui->cmbParity, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSetConfig(int)));
    connect(ui->cmbDataBits, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSetConfig(int)));
    connect(ui->cmbStopBits, SIGNAL(currentIndexChanged(int)), this, SLOT(slotSetConfig(int)));
    connect(ui->cmbWokMode,SIGNAL(currentIndexChanged(int)), this, SLOT(slotWorkModeChanged(int)));
    connect(ui->btnRecv, SIGNAL(clicked()), this, SLOT(slotRecv()));
}

MainWindow::~MainWindow()
{
    //销毁信号量
    if(serialOpened)
    {
        slotClose();
    }

    sem_destroy(&threadSem);

    //销毁UI
    delete ui;
}

void MainWindow::initUI()
{
    //TO DO
    QStringList cmbVal;

    cmbVal << "9600" << "19200" << "4800";
    ui->cmbBaudRate->addItems(cmbVal);

    cmbVal.clear();
    cmbVal << "notparity" << "even" << "odd" << "space";
    ui->cmbParity->addItems(cmbVal);

    cmbVal.clear();
    cmbVal << "8" << "7" << "6";
    ui->cmbDataBits->addItems(cmbVal);

    cmbVal.clear();
    cmbVal << "1" << "2";
    ui->cmbStopBits->addItems(cmbVal);

    cmbVal.clear();
    cmbVal << "异步" << "同步";
    ui->cmbWokMode->addItems(cmbVal);
    ui->btnRecv->hide();
}

void MainWindow::slotOpen()
{
    //TO DO
    if(serialOpened)
    {
        QMessageBox::information(this, tr("打开串口"), tr("串口已经打开!"), QMessageBox::Ok);
        return;
    }

    QByteArray serialName = ui->ledtSerialName->text().toLatin1();
//    const char* serialName = "ttys1";
    if(serialName.isEmpty())
    {
        QMessageBox::critical(this, tr("打开串口"), tr("或许您太着急了，忘记指定串口!"), QMessageBox::Ok);
        return;
    }

    //异步工作模式
    if(CMP_EQUAL == ui->cmbWokMode->currentText().compare("异步"))
    {
        if(E_SERIAL_OK != asySerial.OpenSerial(serialName.data(), callback))
        {
            QMessageBox::critical(this, tr("打开串口"), tr("串口打开失败，请稍后重试!"), QMessageBox::Ok);
            return;
        }
        //设置数据接收和处理线程允许运行
        allowRun = true;

        //创建接收线程
        pthread_t threadID;
        pthread_create(&threadID, NULL, poll_recv, static_cast<void *>(this));

        asySerial.StartRecv();  //启动接收线程
    }
    else //同步工作模式
    {
        if(E_SERIAL_OK != syncSerial.OpenSerial(serialName.data()))
        {
            QMessageBox::critical(this, tr("打开串口"), tr("串口打开失败，请稍后重试!"), QMessageBox::Ok);
            return;
        }
    }


    //设置串口打开标志
    serialOpened = true;
    slotSetConfig(0);

    //禁用串口工作模式修改
    ui->cmbWokMode->setEnabled(false);


    //串口打开提示
    QString s;
    s = QString("串口(%1)打开：操作成功").arg(serialName.data());

    ui->Output->append(s/*"串口打开：操作成功"*/);

//    ui->btnOpen->setDisabled(true);
//    ui->btnClose->setDisabled(false);
}

void MainWindow::slotClose()
{
    //TO DO
    if(!serialOpened)
    {
        QMessageBox::information(this, tr("关闭串口"), tr("串口已经关闭或尚未打开!"), QMessageBox::Ok);
        return;
    }

    if(CMP_EQUAL == ui->cmbWokMode->currentText().compare("异步"))
    {
        //runAllowed = false;
        //停止应用接收数据
        allowRun = false;
        sem_wait(&threadSem);


        //停止串口API数据接收
        asySerial.StopRecv();
        asySerial.CloseSerial();
    }
    else
    {
        syncSerial.CloseSerial();
    }

    //设置串口关闭标志
    serialOpened = false;
    ui->cmbWokMode->setEnabled(true);

//    ui->btnOpen->setDisabled(false);
//    ui->btnClose->setDisabled(true);
    ui->Output->append("关闭串口：操作成功");
}

void MainWindow::slotSetConfig(int)
{
    //TO DO
    if(!serialOpened)
    {
        QMessageBox::critical(this, tr("设置串口"), tr("请先打开串口!"), QMessageBox::Ok);
        return;
    }

    SerialConfig sc;

    //获取原配置信息
    if(CMP_EQUAL == ui->cmbWokMode->currentText().compare("异步"))
    {
        asySerial.GetConfig(sc);
    }
    else
    {
        syncSerial.GetConfig(sc);
    }


    bool res;
    int value;

    //设置波特率
    value = ui->cmbBaudRate->currentText().toInt(&res);
    sc.SerialBaudRate = value;

    //设置数据位
    value = ui->cmbDataBits->currentText().toInt(&res);
    sc.SerialDataBits = (DataBits)value;

    //设置停止位
    value = ui->cmbStopBits->currentText().toInt(&res);
    sc.SerialStopBits = (StopBits)value;

    //设置校验方式
    QString parityMode = ui->cmbParity->currentText();
    parityMode = parityMode.toLower();
    if(CMP_EQUAL == parityMode.compare("notparity"))
    {
        sc.SerialParityMode = noParity;
    }
    else if(CMP_EQUAL == parityMode.compare("even"))
    {
        sc.SerialParityMode = evenParity;
    }
    else if(CMP_EQUAL == parityMode.compare("odd"))
    {
        sc.SerialParityMode = oddParity;
    }
    else
    {
        sc.SerialParityMode = spaceParity;
    }


//    sc.SerialBaudRate = 19200;
//    sc.SerialParityMode = noParity;
//    sc.SerialDataBits = eightBits;
//    sc.SerialStopBits = zeroBit;

    if(CMP_EQUAL == ui->cmbWokMode->currentText().compare("异步"))
    {
        asySerial.SetConfig(sc);
    }
    else
    {
        syncSerial.SetConfig(sc);
    }

}

int str2hex(unsigned char* pBuf, int bufSize, QString &s)
{
    //TO DO
    int totalSize = s.length();
    unsigned char c;

    memset(pBuf, 0, bufSize);
    for(int i = 0 , j = 0; i < totalSize; )
    {
        //取高四位
        c = s[i++].toAscii();
        if('0' <= c && c <= '9')
        {
            c = c - '0';
        }
        else if( 'a' <= c && c <= 'z')
        {
            c = 10 + c - 'a';
        }
        else if('A' <= c && c <= 'Z')
        {
            c = 10 + c - 'Z';
        }
        else
        {
            return -1;
        }
        pBuf[j] = c << 4;

        //取低四位
        c = s[i++].toAscii();
        if('0' <= c && c <= '9')
        {
            c = c - '0';
        }
        else if( 'a' <= c && c <= 'z')
        {
            c = 10 + c - 'a';
        }
        else if('A' <= c && c <= 'Z')
        {
            c = 10 + c - 'Z';
        }
        else
        {
            return -1;
        }
        pBuf[j++] += c;
    }

    return totalSize / 2;
}

void MainWindow::slotSend()
{
    //TO DO
    if(!serialOpened)
    {
        QMessageBox::critical(this, tr("发送数据"), tr("请先打开串口!"), QMessageBox::Ok);
        return;
    }

    QString userData = ui->ledtData->text();

    int userDataSize = str2hex(sendBuf, 9, userData);

    if(CMP_EQUAL == ui->cmbWokMode->currentText().compare("异步"))
    {
        asySerial.Send(sendBuf, userDataSize);
    }
    else
    {
        syncSerial.Send(sendBuf, userDataSize);
    }
}

void MainWindow::slotWorkModeChanged(int)
{
    //TO DO
    if(CMP_EQUAL == ui->cmbWokMode->currentText().compare("同步"))
    {
        ui->btnRecv->show();
        ui->grpOpt->setTitle("收发数据");
    }
    else //if(CMP_EQUAL == ui->cmbWokMode->currentText().compare("异步"))
    {
        ui->grpOpt->setTitle("发送数据");
        ui->btnRecv->hide();
    }
}

void MainWindow::slotRecv()
{
    //TO DO
    recvDataSize = syncSerial.Recv(frameBuf, frameBufSize);

    showResult(frameBuf, recvDataSize);
}

void MainWindow::showResult(const unsigned char *pBuf, const int dataSize)
{
    //TO DO
    if(dataSize <= 0)
    {
        return;
    }

    QString output = "接收到:  ";
    QString s;

    for(int i = 0; i < dataSize; i++)
    {
        output += s.sprintf("%02X", pBuf[i]);
    }

    ui->Output->append(output);
}

void makeTimeout(struct timespec *tsp, int seconds)
{
    struct timeval now;

    gettimeofday(&now, NULL);
    tsp->tv_sec = now.tv_sec;
    tsp->tv_nsec = now.tv_usec * 1000;
    tsp->tv_sec += seconds;
}

void* MainWindow::poll_recv(void *arg)
{
    //TO DO
    MainWindow* pMainWnd = static_cast<MainWindow*>(arg);


    struct timespec timeout;
    timeout.tv_nsec = 10;
    timeout.tv_sec = 0;

    pthread_cond_signal(&procDataCon);

    while(allowRun)
    {
        //makeTimeout(&timeout, 1);
        pthread_mutex_lock(&recvDataMutex);
        pthread_cond_timedwait(&recvedNewDataCon, &recvDataMutex, &timeout);
        if(haveRecvNewData)
        {
//            memcpy(pMainWnd->outputBuf, frameBuf, recvDataSize);
            haveRecvNewData = false;
//            pthread_mutex_unlock(&recvDataMutex);

            pMainWnd->showResult(frameBuf, recvDataSize);
            //emit pMainWnd->recvDataSig();
//            pMainWnd->ui->Output->append("recvThread!");
        }

        pthread_mutex_unlock(&recvDataMutex);

    }

    sem_post(&pMainWnd->threadSem);

    return (void *)0;
}
