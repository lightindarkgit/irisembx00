/*****************************************************************************
** 文 件 名：FeatureManageForSave.cpp
** 主 要 类：
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
#include "FeatureManageForDemo.h"

QString CFeatureManageForDemo::s_strEnrImagePathLeft;    //左眼注册图像保存路径
QString CFeatureManageForDemo::s_strEnrImagePathRight;   //右眼注册图像保存路径
QString CFeatureManageForDemo::s_strEnrCodePathLeft;     //左眼注册特征载入路径
QString CFeatureManageForDemo::s_strEnrCodePathRight;    //右眼注册特征载入路径
QString CFeatureManageForDemo::s_strUserName;            //用户名

unsigned char CFeatureManageForDemo::s_enrollCodesL[g_constMaxEnrFeatureNum*g_constIKEnrFeatureLen];	//保存左眼注册特征
int CFeatureManageForDemo::s_enrNumL;                                                                 //实际读入系统的左眼注册特征数目
QString CFeatureManageForDemo::s_strNameLeft[g_constMaxEnrFeatureNum];                                //用于存储左眼特征名字

unsigned char CFeatureManageForDemo::s_enrollCodesR[g_constMaxEnrFeatureNum*g_constIKEnrFeatureLen];	//保存右眼注册特征
int CFeatureManageForDemo::s_enrNumR;                                                                 //实际读入系统的右眼注册特征数目
QString CFeatureManageForDemo::s_strNameRight[g_constMaxEnrFeatureNum];

/*****************************************************************************
*                         初始化参数
*  函 数 名：InitParam
*  功    能：初始化特征管理相关参数
*  说    明：由识别模块启动时调用
*  参    数：NULL
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-30
*  修 改 人：
*  修改时间：
*****************************************************************************/
void CFeatureManageForDemo::InitParam()
{
    //创建相关路径
    QDir creatDir;
    QString strPath = APPEnrImagePathLeft;
    if(!creatDir.exists(strPath))
    {
        creatDir.mkpath(strPath);
    }

    strPath = APPEnrImagePathRight;
    if(!creatDir.exists(strPath))
    {
        creatDir.mkpath(strPath);
    }

    strPath = APPEnrCodePathLeft;
    if(!creatDir.exists(strPath))
    {
        creatDir.mkpath(strPath);
    }

    strPath = APPEnrCodePathRight;
    if(!creatDir.exists(strPath))
    {
        creatDir.mkpath(strPath);
    }

    strPath = APPEnrImSavePath;
    if(!creatDir.exists(strPath))
    {
        creatDir.mkpath(strPath);
    }

    strPath = APPRecImSavePath;
    if(!creatDir.exists(strPath))
    {
        creatDir.mkpath(strPath);
    }

    strPath = APPOriImSavePath;
    if(!creatDir.exists(strPath))
    {
        creatDir.mkpath(strPath);
    }

    //应用程序用到的系统空间初始化
    memset(s_enrollCodesL,0,g_constMaxEnrFeatureNum*g_constIKEnrFeatureLen);
    memset(s_enrollCodesR,0,g_constMaxEnrFeatureNum*g_constIKEnrFeatureLen);
    s_enrNumL = 0;
    s_enrNumR = 0;

    s_strEnrImagePathLeft = APPEnrImagePathLeft;
    s_strEnrImagePathRight = APPEnrImagePathRight;
    s_strEnrCodePathLeft = APPEnrCodePathLeft;
    s_strEnrCodePathRight = APPEnrCodePathRight;
}

/*****************************************************************************
*                         载入特征
*  函 数 名：LoadTemplate
*  功    能：从相应文件夹中载入特征到内存
*  说    明：由识别模块启动时调用
*  参    数：NULL
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-30
*  修 改 人：
*  修改时间：
*****************************************************************************/
void CFeatureManageForDemo::LoadTemplate()
{
//    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));
//    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));

    //相关参数初始化
    int enrPosLenLeft=0;
    int enrPosLenRight=0;

    int tempNum;
    int enrCodeLen = g_constIKEnrFeatureLen;

    char* testImName;

    QString strImPathName;
    QByteArray byteAr;
    QString strPath;
    int nSize = 0;
    int nReadTpFlag = -1;
    s_enrNumL = 0;
    s_enrNumR = 0;


    QStringList filters;
    filters << "*.enr";
    QFileInfo fileInfo;

    //搜索左眼虹膜模板
    strPath = APPEnrCodePathLeft;
    QDir dir(strPath);
    if (dir.exists())
    {
        dir.setFilter(QDir::Dirs|QDir::Files);
        dir.setSorting(QDir::DirsFirst);
        dir.setNameFilters(filters);
        QFileInfoList list = dir.entryInfoList();
        nSize = list.size();
        if(nSize>0)
        {
            tempNum=0;
            do
            {
                fileInfo = list.at(tempNum);

                //如果模板文件存在
                if(fileInfo.suffix()=="enr")
                {
                    strImPathName = fileInfo.filePath();
                    byteAr = strImPathName.toLocal8Bit();
                    testImName = byteAr.data();

                    //加入左眼
                    nReadTpFlag = FeatrueLoad(testImName,s_enrollCodesL+enrPosLenLeft,enrCodeLen);
                    if (0 == nReadTpFlag)
                    {
                        s_strNameLeft[s_enrNumL] = fileInfo.baseName();
                        s_enrNumL++;
                        enrPosLenLeft += enrCodeLen;
                    }
                }
                tempNum++;
            }while(tempNum<nSize);
        }
    }

    //搜索右眼虹膜模板
    strPath = APPEnrCodePathRight;
    dir.setPath(strPath);
    if (dir.exists())
    {
        dir.setFilter(QDir::Dirs|QDir::Files);
        dir.setSorting(QDir::DirsFirst);
        dir.setNameFilters(filters);
        QFileInfoList list = dir.entryInfoList();
        nSize = list.size();
        if(nSize>0)
        {
            tempNum=0;
            do
            {
                fileInfo = list.at(tempNum);

                //如果模板文件存在
                if(fileInfo.suffix()=="enr")
                {
                    strImPathName = fileInfo.filePath();
                    byteAr = strImPathName.toLocal8Bit();
                    testImName = byteAr.data();

                    //加入右眼
                    nReadTpFlag = FeatrueLoad(testImName,s_enrollCodesR+enrPosLenRight,enrCodeLen);
                    if (0 == nReadTpFlag)
                    {
                        s_strNameRight[s_enrNumR] = fileInfo.baseName();
                        s_enrNumR++;
                        enrPosLenRight += enrCodeLen;
                    }

                }
                tempNum++;
            }while(tempNum<nSize);
        }
    }
}

/*****************************************************************************
*                         向特征内存中增加一个特征
*  函 数 名：AddTemplate
*  功    能：向特征内存中增加一个特征
*  说    明：由注册模块在保存注册结果时调用
*  参    数：feature：输入参数，待保存特征
*           name：输入参数，用户名称
*           featureFlag：输入参数，标记左眼还是右眼特征
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-30
*  修 改 人：
*  修改时间：
*****************************************************************************/
void CFeatureManageForDemo::AddTemplate(unsigned char *feature, QString name, FeatureFlag featureFlag)
{
    if(FeatureLeft == featureFlag)
    {
        memcpy(s_enrollCodesL+s_enrNumL*g_constIKEnrFeatureLen, feature, g_constIKEnrFeatureLen);
        s_strNameLeft[s_enrNumL] = name;
        s_enrNumL++;
    }
    else if(FeatureRight == featureFlag)
    {
        memcpy(s_enrollCodesR+s_enrNumR*g_constIKEnrFeatureLen, feature, g_constIKEnrFeatureLen);
        s_strNameRight[s_enrNumR] = name;
        s_enrNumR++;
    }
}
