/*****************************************************************************
** 文 件 名：Identity.h
** 主 要 类：Identity
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：刘中昌
** 创建时间：20013-10-13
**
** 修 改 人：
** 修改时间：
** 描  述:   识别对话框
** 主要模块说明: 识别界面的显示
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#ifndef IDENTITY_H
#define IDENTITY_H

#include <QWidget>
#include "../../03 Code/USBAPI/simplelock.h"
#include "../../03 Code/AlgApi/SRC/AlgApi.h"
#include "Common/interface.h"
#include <QVector>

#include <mutex>
#include <thread>
#include <condition_variable>
#include "FeatureManageForDemo.h"
#include "interaction.h"

namespace Ui {
class DialogIdentify;
}
enum IdentResultState
{
    SuccessState=0,
    FailedState,
    ContinueState ,
    KeepState,
    UnKnownState
};

class Person;
class DialogIdentify : public QWidget
{
    Q_OBJECT
    
public:
    explicit DialogIdentify(QWidget *parent = 0);
    ~DialogIdentify();

signals:

   void sigGoBack();              //返回考勤识别
   void sigLoginSucceed(bool isSucceed); //信号，登陆是否成功

public slots:

    void timerEvent(QTimerEvent *event);//界面时间更新
    void ImageUpdate();     //界面图像更新
    void StopIdent();       //停止识别

    void StartAttendIdent();       //开始考勤识别
    void StartLoginIdent();        //开始管理员登陆识别

    void IdentResult(long personIndex, int flag);         //考勤识别结果
    void LoginIdentResult(long personIndex, int flag);                      //管理员登陆识别结果

private:   
    static void * IdentCapImgThread(void* arg); //上传图片数据线程

    //回调函数funResult, recIrisL,recIrisR,lrFlag,pLeftMatchOutputParam,pRightMatchOutputParam
    static int IdentCallBack(int funResult, APIIrisInfo* recIrisL, APIIrisInfo* recIrisR, LRSucFailFlag lrFlag,
                     APIMatchOutput* pLeftMatchOutputParam, APIMatchOutput* pRightMatchOutputParam);

    //管理员识别登陆回调
    static int LoginIdentCallBack(int funResult, APIIrisInfo* recIrisL, APIIrisInfo* recIrisR, LRSucFailFlag lrFlag,
                     APIMatchOutput* pLeftMatchOutputParam, APIMatchOutput* pRightMatchOutputParam);

private:

    Ui::DialogIdentify *ui;
    //正常考勤数据
    unsigned char *_codeListL;//IN
    int _codeNumL;//IN
    unsigned char *_codeListR;//IN
    int _codeNumR;//IN

    //管理员考勤数据
    unsigned char *_loginCodeListL;//IN
    int _loginCodeNumL;//IN
    unsigned char *_loginCodeListR;//IN
    int _loginCodeNumR;//IN

    CAlgApi * _identApi;

    //退出线程
    bool  _exitIdentThread;


    //灰度图像
    unsigned char _grayImgBuffer[g_constIrisImgSize + g_constBMPHeaderLength];


    CFeatureManageForDemo _cFeatureManage;
    QVector<QString>  _personName;             //考勤人员信息
    QVector<QString>  _loginPersonName;        //管理员人员信息

    std::thread _identCapImgThread;            //识别模块图像显示线程
    std::mutex _identCapImgMutex;              //识别模块图像采集mutex
    std::condition_variable _identCapImgCV;    //识别模块图像采集condition_variable
    bool _identCapImgFlag;                     //识别模块图像采集flag

    int _count;                                //帧率
    bool _isClose;                             //是否停止识别
    bool  _isLoginIdent;                      //是否开始识别

    //人机交互类
    static Interaction *_identInteraction;
    LRIrisClearFlag  _irisClearFlag;
    IdentResultState  _identResultState;           //识别结果
    QTimer *_timerStartIdent;                      //识别成功或失败时显示框显示1s钟
    InteractionResultType _interactionResultType;  //绘制什么框

};

#endif // IDENTITY_H
