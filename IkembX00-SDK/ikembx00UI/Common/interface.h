#ifndef __INTERFACE_H__
#define __INTERFACE_H__

///*****************************************************************************
//*  文 件 名：interface.h
//*  主 要 类：
//*  说    明：
//*  创 建 人：刘中昌
//*  创建时间：20013-12-24
//*  修 改 人：gdj 添加sdk 删除无用代码
//*  修改时间： 20161015
//*****************************************************************************/

#include "QObject"
#include "FeatureManageForDemo.h"
//#include "QThread"



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

    //@dj
    void onResult(long index,int flag,bool isLoginIden = false)//返回识别结果
    {
        if(isLoginIden)
        {
            emit sigLoginResult(index,flag);
        }
        else
        {
            emit sigResult(index,flag);
        }

    }

    void onInteractionStatus(bool flag)
    {
        emit sigInteractionStatus(flag);
    }

    void onMsg(QString str)//返回提示信息
    {
        emit sigMsg(str);
    }
    //识别界面重绘
    void onDrawIdent()
    {
        emit sigDrawIdent();
    }

    void onIdentCardID(QString cardID)
    {
        emit sigIdentCardID(cardID);
    }

    //20140822lizhl
    void onSetUIImg(bool isSleep)
    {
        emit sigSetUIImg(isSleep);
    }


    void onWakeSDK()
    {
        emit sigWakeSDK();

    }

    void onSleepSDK()
    {
        emit sigSleepSDK();

    }

    void onInitSDKOutTime()
    {
        emit sigInitSdkOutTime();
    }

    //20141217 lizhl
    void onUIofIden()
    {
        emit sigUIofIden();
    }

    void onUIofWait()
    {
        emit sigUIofWait();
    }

    void onUIofSleep()
    {
        emit sigUIofSleep();
    }

signals:
    void sigInteractionStatus(bool);
    void sigIdentCardID(QString cardID);
    void sigDrawIdent();
    void sigResult(long index,int flag);
    void sigLoginResult(long index,int flag);
    void sigMsg(QString str);
    //20140822lizhl
    void sigSetUIImg(bool isSleep);

    //sleep sdk
    void sigSleepSDK();
    //wake SDK
    void sigWakeSDK();

    // 尝试多次初始化sdk超时信号dj
    void sigInitSdkOutTime();

    //20141217 lizhl
    void sigUIofIden();
    void sigUIofWait();
    void sigUIofSleep();
};

#endif //__INTERFACE_H__
