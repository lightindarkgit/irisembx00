/*****************************************************************************
** 文 件 名：FeatureManageForDemo.h
** 主 要 类：CFeatureManageForDemo
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：lizhl
** 创建时间：2013-12-30
**
** 修 改 人：
** 修改时间：
** 描  述:   临时文件，用于第一期演示时的特征管理
** 主要模块说明:
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#ifndef FEATUREMANAGEFORDEMO_H
#define FEATUREMANAGEFORDEMO_H
#include <QDir>
#include <QTextCodec>
#include "AlgApi.h"
#include "BRCCommon.h"

//对注册特征个数及用户名长度进行设置
const int g_constMaxEnrFeatureNum = 100;
const int g_constUserNameLen = 20;

enum FeatureFlag
{
    FeatureLeft = 0,
    FeatureRight = 1
};

//设置路径
#define APPEnrImagePathLeft ("./IrisPath/EnrollImage/LeftEye")
#define APPEnrImagePathRight ("./IrisPath/EnrollImage/RightEye")
#define APPEnrCodePathLeft ("./IrisPath/EnrollTemplate/LeftEye")
#define APPEnrCodePathRight ("./IrisPath/EnrollTemplate/RightEye")

#define APPEnrImSavePath	("./IrisPath/SavedEnrIm")
#define APPRecImSavePath	("./IrisPath/SavedRecIm")
#define APPOriImSavePath	("./IrisPath/SavedOriIm")
#define APPEnrCodeFormat (".enr")
#define APPEnrImFormat (".ppm")

//临时特征管理类
class CFeatureManageForDemo
{
public:
    static QString s_strEnrImagePathLeft;    //左眼注册图像保存路径
    static QString s_strEnrImagePathRight;   //右眼注册图像保存路径
    static QString s_strEnrCodePathLeft;     //左眼注册特征保存路径
    static QString s_strEnrCodePathRight;    //右眼注册特征保存路径
    static QString s_strUserName;             //用户名

    static unsigned char s_enrollCodesL[g_constMaxEnrFeatureNum*g_constIKEnrFeatureLen];	//保存左眼注册特征
    static int s_enrNumL;                                                                 //实际读入系统的左眼注册特征数目
    static QString s_strNameLeft[g_constMaxEnrFeatureNum];                                //用于存储左眼特征名字

    static unsigned char s_enrollCodesR[g_constMaxEnrFeatureNum*g_constIKEnrFeatureLen];	//保存右眼注册特征
    static int s_enrNumR;                                                                 //实际读入系统的右眼注册特征数目
    static QString s_strNameRight[g_constMaxEnrFeatureNum];                               //用于存储右眼特征名字


public:
    void InitParam();   //初始化参数
    void LoadTemplate();//载入特征
    void AddTemplate(unsigned char *feature, QString name, FeatureFlag featureFlag);//向特征内存中增加一个特征
};


#endif // FEATUREMANAGEFORDEMO_H
