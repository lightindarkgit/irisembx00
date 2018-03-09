/*****************************************************************************
** 文 件 名：dialogenroll.h
** 主 要 类：DialogEnroll
**
** Copyright (c) 中科虹霸有限公司
**
** 创 建 人：lizhl
** 创建时间：2013-10-18
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
#ifndef DIALOGENROLL_H
#define DIALOGENROLL_H

#include <QDialog>

#include "IKUSBSDK.h"


namespace Ui {
class DialogEnroll;
}

class DialogEnroll : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogEnroll(QWidget *parent = 0);
    ~DialogEnroll();
    //传递注册结果信息
    void XferIrisInfo(IKSDKIrisInfo *pResultIrisInfo);

    
private slots:
   void on_btnDlgEnrollReturn_clicked();
public slots:
   void Slot_CloseWindow();

private:
    Ui::DialogEnroll *ui;
//    unsigned char _grayResultImgBuffer[g_constIKImgSize + g_constIKPPMHeaderLength];//注册图像buffer
  //  unsigned char _grayResultImgBuffer[g_constIKImgSize + g_constBMPHeaderLength];
//    char _grayResultPpmHeader[g_constIKPPMHeaderLength];                            //存储注册图像ppm头buffer
    IKSDKIrisInfo *_resultIrisInfo;                                                   //注册结果
};

#endif // DIALOGENROLL_H
