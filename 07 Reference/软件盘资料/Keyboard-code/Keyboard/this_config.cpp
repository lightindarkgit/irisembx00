#include "this_config.h"


/////Option/////
Option g_OptionData;
Option Window_show;
int User_admin_flag = 0;
Option::Option()
{

}

Option::~Option()
{

}

void Option::ShowKeyBoard(QWidget *parent, bool num) //�����������һ�����̰�ťʱ�Ի������ʾ�����ء��Լ�������ʾ��λ��
{
    static QWidget *LastParent = 0;

    if (parent != LastParent && k_board != NULL)
    {
        delete k_board;
        k_board = NULL;
    }

    if (k_board == NULL)
    {
        k_board = new Keyboard(parent,num);
        LastParent = parent;
    }

    if (k_board->isVisible())    k_board->hide();
    else
    {

        QApplication::focusWidget();

        k_board->move(SCREEN_WIDTH - k_board->rect().width(), SCREEN_HEIGHT- k_board->rect().height());//�����Ի�����ʾλ��

        k_board->show();
    }
}


void Option::CloseKeyBoard()
{
    if(k_board == NULL) return;
    if(k_board->isVisible())
    {
        k_board->hide();
        delete k_board;
        k_board = NULL;
    }
}


