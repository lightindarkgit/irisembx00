/******************************************************************************************
** 文件名:   buffering.cpp
×× 主要类:   (无)
**
** Copyright (c) 中科虹霸有限公司
** 创建人:   Wang.L
** 日  期:   2015-01-28
** 修改人:
** 日  期:
** 描  述:   等待动画实现
** ×× 主要模块说明：
**
** 版  本:   1.0.0
** 备  注:
**
******************************************************************************************/
#pragma once

#include <QDialog>
#include <QMovie>

namespace Ui {
class Buffering;
}

class Buffering : public QDialog
{
    Q_OBJECT
    
public:
    explicit Buffering(QWidget *parent = 0,QString ShowText ="");
    ~Buffering();

    void SetNotionText(const QString &info);
private:
    void InitPlayer();

    
private:
    Ui::Buffering *ui;
    QMovie gifPlayer;
};
