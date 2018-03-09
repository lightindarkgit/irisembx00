/*****************************************************************************
 ** 文 件 名：dialogfeatureexist.cpp
 ** 主 要 类：DialogFeatureExist
 **
 ** Copyright (c) 中科虹霸有限公司
 **
 ** 创 建 人：lizhl
 ** 创建时间：2014-02-26
 **
 ** 修 改 人：
 ** 修改时间：
 ** 描  述:   注册查重后，如果注册特征与已有特征相似，将弹出特征存在对话框，由用户选择处理方式
 ** 主要模块说明: 特征存在类
 **
 ** 版  本:   1.0.0
 ** 备  注:
 **
 *****************************************************************************/
#include "dialogfeatureexist.h"
#include "ui_dialogfeatureexist.h"
#include "../Common/Exectime.h"

/*****************************************************************************
 *                         DialogFeatureExist构造函数
 *  函 数 名：DialogFeatureExist
 *  功    能：分配DialogFeatureExist类资源
 *  说    明：
 *  参    数：
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2014-02-26
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
DialogFeatureExist::DialogFeatureExist(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DialogFeatureExist)
{
	Exectime etm(__FUNCTION__);
	ui->setupUi(this);

	setWindowFlags(Qt::CustomizeWindowHint);
	setWindowFlags(Qt::FramelessWindowHint);
	setGeometry(QRect(0, 0, 1024, 600));
	setBackgroundColor(QColor(242,241,240));

	//设置背景
	//    QPalette palette;
	////    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/image/back3.jpg")));
	//    palette.setBrush(QPalette::Background, QBrush(QPixmap(":/image/menus_bkground.jpg")));//bkground.jpg
	//    this->setPalette(palette);
}

/*****************************************************************************
 *                         DialogFeatureExist析构函数
 *  函 数 名：~DialogFeatureExist
 *  功    能：释放DialogFeatureExist类资源
 *  说    明：
 *  参    数：
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2014-02-26
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
DialogFeatureExist::~DialogFeatureExist()
{
	Exectime etm(__FUNCTION__);
	delete ui;
}

/*****************************************************************************
 *                         传递已存在特征人员信息函数
 *  函 数 名：XferExistedPersonInfo
 *  功    能：传递已存在特征人员的信息给特征存在对话框并显示
 *  说    明：
 *  参    数：leftFeatureIndex：输入参数，左眼特征索引
 *           leftName：输入参数，左眼特征对应用户名
 *           rightFeatureIndex：输入参数，右眼特征索引
 *           rightName：输入参数，右眼特征对应用户名
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2014-02-26
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DialogFeatureExist::XferExistedPersonInfo(std::string leftDepart,
                                               std::string leftName, std::string rightDepart,
                                               std::string rightName,int personexist)
{
	Exectime etm(__FUNCTION__);
	if(leftName != "")
	{
		ui->lineEditLeftDepart->setText(QString::fromStdString(leftDepart));
		ui->lineEditLeftName->setText(QString::fromStdString(leftName));
	}

	if(rightName != "")
	{
		ui->lineEditRightDepart->setText(QString::fromStdString(rightDepart));
		ui->lineEditRightName->setText(QString::fromStdString(rightName));
	}

	ui->lineEditLeftDepart->setCursorPosition(1);
	ui->lineEditLeftName->setCursorPosition(1);
	ui->lineEditRightDepart->setCursorPosition(1);
	ui->lineEditRightName->setCursorPosition(1);

    if(personexist == 1)
    {
        ui->textBrowser->setHtml(QApplication::translate("DialogFeatureExist", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
                                                         "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
                                                         "p, li { white-space: pre-wrap; }\n"
                                                         "</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
                                                         "<p align=\"center\" style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>\n"
                                                         "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:16pt; color:#ff0000;\">\350\257\267\346\263\250\346\204\217\357\274\201</span></p>\n"
                                                         "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:16pt; color:#ff0000;\">\346\234\254\346\254\241\346\263\250\345\206\214\344\272\272\345\221\230\345\220\215\344\270"
                                                                                 "\213\345\267\262\345\255\230\345\234\250\350\231\271\350\206\234\347\211\271\345\276\201\357\274\214\350\257\267\347\256\241\347\220\206\345\221\230\350\260\250\346\205\216\346\223\215\344\275\234\357\274\201</span></p></body></html>", 0, QApplication::UnicodeUTF8));

    }
    else
    {
        ui->textBrowser->setHtml(QApplication::translate("DialogFeatureExist", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
                                                         "<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
                                                         "p, li { white-space: pre-wrap; }\n"
                                                         "</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
                                                         "<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:16pt; color:#ff0000;\">\350\257\267\346\263\250\346\204\217\357\274\201</span></p>\n"
                                                         "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:16pt; color:#ff0000;\">\346\234\254\346\254\241\351\207\207\351\233\206\347\232\204\350\231\271\350\206\234\344\270\216\344\270\213\345\210\227\344\272\272\345\221\230\344\273\245\345\211\215\351\207\207\351\233\206\350\277\207\347\232\204\350\231\271\350\206\234\345\215\201\345\210\206\347\233"
                                                                                 "\270\344\274\274\357\274\214\350\257\267\347\256\241\347\220\206\345\221\230\350\260\250\346\205\216\346\223\215\344\275\234\357\274\201</span></p></body></html>", 0, QApplication::UnicodeUTF8));
    }

}

/*****************************************************************************
 *                         “重新注册”按钮点击函数
 *  函 数 名：on_btnReturn_clicked
 *  功    能：放弃保存特征，返回注册界面
 *  说    明：
 *  参    数：
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2014-02-26
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DialogFeatureExist::on_btnReturn_clicked()
{
	Exectime etm(__FUNCTION__);
	returnType = QuitFeature;
	QDialog::reject();
}

/*****************************************************************************
 *                         “增加特征”按钮点击函数
 *  函 数 名：on_btnAddFeature_clicked
 *  功    能：已有特征不变，增加本次注册特征
 *  说    明：
 *  参    数：
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2014-02-26
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DialogFeatureExist::on_btnAddFeature_clicked()
{
	Exectime etm(__FUNCTION__);
	returnType = AddFeature;
	QDialog::accept();
}

/*****************************************************************************
 *                         “覆盖特征”按钮点击函数
 *  函 数 名：on_btnCoverFeature_clicked
 *  功    能：将本次注册特征覆盖已有相似特征
 *  说    明：
 *  参    数：
 *
 *  返 回 值：NULL
 *  创 建 人：lizhl
 *  创建时间：2014-02-26
 *  修 改 人：
 *  修改时间：
 *****************************************************************************/
void DialogFeatureExist::on_btnCoverFeature_clicked()
{
	Exectime etm(__FUNCTION__);
	returnType = CoverFeature;
	QDialog::accept();
}

