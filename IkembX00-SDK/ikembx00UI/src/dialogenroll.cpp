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
#include "interaction.h"
#include "drawtracgif.h"

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
    setGeometry(QRect(0, 0, 1024, 600));
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

//用于自动返回识别界面时，关闭本窗体，防止本窗体没有关闭，下次阻塞住，无法显示注册界面
void DialogEnroll::Slot_CloseWindow()
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
void DialogEnroll::XferIrisInfo(IKSDKIrisInfo *pResultIrisInfo)
{
//    QPalette palette;
////    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/image/back3.jpg")));
//    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/image/menus_bkground.jpg")));//bkground.jpg
//    this->setPalette(palette);
    if(nullptr == pResultIrisInfo)
    {
        LOG_ERROR("input pResultIrisInfo is null");
    }

    _resultIrisInfo = pResultIrisInfo;

    QImage resultImg;

    resultImg = DrawTracGif::GetImg(_resultIrisInfo->imgData, IK_IRIS_IMG_SIZE);
    resultImg = resultImg.scaled(640,480,Qt::KeepAspectRatio);


    int qualityScore = _resultIrisInfo->QualityScore;
    int occlusionsScore = _resultIrisInfo->PercentVisible;

    if(occlusionsScore < g_MinEnrollOcclusion)
    {
        ui->lineEditOcclusion->setStyleSheet("background-color: rgb(255, 255, 255);  border:2px groove gray;  padding: 4px 4px;  border-radius: 8px;color: rgb(255, 0, 0); background-image: url();");
    }
    else
    {
        ui->lineEditOcclusion->setStyleSheet("background-color: rgb(255, 255, 255);  border:2px groove gray;  padding: 4px 4px;  border-radius: 8px;color: rgb(23, 111, 11); background-image: url();");
    }
    if(qualityScore < g_MinEnrollQualityScore)
    {
        ui->lineEditQualityScore->setStyleSheet("background-color: rgb(255, 255, 255);  border:2px groove gray;  padding: 4px 4px;  border-radius: 8px;color: rgb(255, 0, 0); background-image: url();");
    }
    else
    {
        ui->lineEditQualityScore->setStyleSheet("background-color: rgb(255, 255, 255);  border:2px groove gray;  padding: 4px 4px;  border-radius: 8px;color: rgb(23, 111, 11); background-image: url();");
    }

    ui->lineEditOcclusion->setText(QString::number(occlusionsScore,10));
    ui->lineEditQualityScore->setText( QString::number(qualityScore,10));

    ui->labIrisIm->setPixmap(QPixmap::fromImage(resultImg));

}
