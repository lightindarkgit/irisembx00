/*****************************************************************************
** 文 件 名：dialogenroll.cpp
** 主 要 类：DialogEnroll
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：lizhl
** 创建时间：2013-10-15
**
** 修 改 人：
** 修改时间：
** 描  述:   注册对话框
** 主要模块说明: 注册过程中的对话框处理
**
** 版  本:   1.0.0
** 备  注:
**
*****************************************************************************/
#include "dialogenroll.h"
#include "ui_dialogenroll.h"

/*****************************************************************************
*                         DialogEnroll构造函数
*  函 数 名：DialogEnroll
*  功    能：分配DialogEnroll类资源
*  说    明：
*  参    数：
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
DialogEnroll::DialogEnroll(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogEnroll)
{
    ui->setupUi(this);

    setWindowFlags(Qt::CustomizeWindowHint);
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(QRect(0, 0, 800, 600));
    setBackgroundColor(QColor(242,241,240));
}

/*****************************************************************************
*                         DialogEnroll析构函数
*  函 数 名：~DialogEnroll
*  功    能：释放DialogEnroll类资源
*  说    明：
*  参    数：
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
DialogEnroll::~DialogEnroll()
{
    delete ui;
}

/*****************************************************************************
*                         DialogEnroll返回
*  函 数 名：on_btnDlgEnrollReturn_clicked
*  功    能：从DialogEnroll界面返回到Enroll界面
*  说    明：
*  参    数：
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogEnroll::on_btnDlgEnrollReturn_clicked()
{
    close();
}

/*****************************************************************************
*                         传递注册结果信息
*  函 数 名：XferIrisInfo
*  功    能：从Enroll界面传递注册结果信息给DialogEnroll界面
*  说    明：
*  参    数：
*
*  返 回 值：NULL
*  创 建 人：lizhl
*  创建时间：2013-12-23
*  修 改 人：
*  修改时间：
*****************************************************************************/
void DialogEnroll::XferIrisInfo(APIIrisInfo *pResultIrisInfo)
{
    _resultIrisInfo = pResultIrisInfo;

    QImage resultImg;

//    unsigned char mirrorImg[g_constIKImgSize];

//    sprintf(_grayResultPpmHeader, "P5\n%d %d\n255\n", g_constIKImgWidth, g_constIKImgHeight);
//    memcpy(_grayResultImgBuffer, _grayResultPpmHeader, g_constIKPPMHeaderLength);
//    ImageMirror(_grayResultImgBuffer+g_constIKPPMHeaderLength, _resultIrisInfo->LocImage, g_constIKImgHeight, g_constIKImgWidth, false);
//    resultImg = QImage::fromData(_grayResultImgBuffer, g_constIKImgSize+g_constIKPPMHeaderLength, "PPM");
//    resultImg = resultImg.scaled(640,480,Qt::KeepAspectRatio);

//    ImageMirror(mirrorImg,  _resultIrisInfo->LocImage, g_constIKImgHeight, g_constIKImgWidth, false);
//    ConvertRaw2Bmp(_grayResultImgBuffer, mirrorImg, g_constIKImgHeight, g_constIKImgWidth);
//    resultImg = QImage::fromData(_grayResultImgBuffer, g_constIKImgSize+g_constBMPHeaderLength, "BMP");

    resultImg = QImage::fromData(_resultIrisInfo->LocImage, g_constIKImgSize+g_constBMPHeaderLength, "BMP");
    resultImg = resultImg.scaled(640,480,Qt::KeepAspectRatio);


    int qualityScore = _resultIrisInfo->QualityScore;
    int occlusionsScore = _resultIrisInfo->PercentVisible;

    if(occlusionsScore < MinEnrollOcclusion)
    {
        ui->lineEditOcclusion->setStyleSheet("color: rgb(255, 0, 0);");
    }
    else
    {
        ui->lineEditOcclusion->setStyleSheet("color: rgb(23, 111, 11);");
    }
    if(qualityScore < MinEnrollQualityScore)
    {
        ui->lineEditQualityScore->setStyleSheet("color: rgb(255, 0, 0);");
    }
    else
    {
        ui->lineEditQualityScore->setStyleSheet("color: rgb(23, 111, 11);");
    }

    ui->lineEditOcclusion->setText(QString::number(occlusionsScore,10));
    ui->lineEditQualityScore->setText( QString::number(qualityScore,10));

    ui->labIrisIm->setPixmap(QPixmap::fromImage(resultImg));

}
