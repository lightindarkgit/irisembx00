#include "keyboard.h"
#include "ui_keyboard.h"
#include <Qt>
#include <QtGui>
#include <QPalette>
#include <qtextedit.h>
#include <QLineEdit>
extern bool n;
Keyboard::Keyboard(QWidget *parent, bool num) :
    QMainWindow(parent),
    ui(new Ui::Keyboard)
{
    ui->setupUi(this);
    Flag_InputMode = LETTER;   //�����ʽ����
    Flag_Mid_Mode  = LETTER;   //�����ʽ��ת����

    m_MouseDown = false;     //����¼���ʼ��

    m_Page     =0;
    m_Page_A   =0;//��ʼ����ҳ���͵�ǰҳ��
    k_letter[0]=0;
    k_letter[1]=0;
    k_letter[2]=0;
    k_letter[3]=0;
    k_letter[4]=0;
    k_letter[5]=0;
    k_letter[6]=0;
    k_letter_index=0;

    m_Display[0] = ui->hz_1;
    m_Display[1] = ui->hz_2;
    m_Display[2] = ui->hz_3;
    m_Display[3] = ui->hz_4;
    m_Display[4] = ui->hz_5;
    m_Display[5] = ui->hz_6;
    m_Display[6] = ui->hz_7;
    m_Display[7] = ui->hz_8;
    m_Display[8] = ui->hz_9;
    m_Display[9] = ui->hz_10;//����1-10�Ķ��壬����ͳһ����һ�������У�����������롣

    m_letter_a_z[0]=ui->pushButton_a;
    m_letter_a_z[1]=ui->pushButton_b;
    m_letter_a_z[2]=ui->pushButton_c;
    m_letter_a_z[3]=ui->pushButton_d;
    m_letter_a_z[4]=ui->pushButton_e;
    m_letter_a_z[5]=ui->pushButton_f;
    m_letter_a_z[6]=ui->pushButton_g;
    m_letter_a_z[7]=ui->pushButton_h;
    m_letter_a_z[8]=ui->pushButton_i;
    m_letter_a_z[9]=ui->pushButton_j;
    m_letter_a_z[10]=ui->pushButton_k;
    m_letter_a_z[11]=ui->pushButton_l;
    m_letter_a_z[12]=ui->pushButton_m;
    m_letter_a_z[13]=ui->pushButton_n;
    m_letter_a_z[14]=ui->pushButton_o;
    m_letter_a_z[15]=ui->pushButton_p;
    m_letter_a_z[16]=ui->pushButton_q;
    m_letter_a_z[17]=ui->pushButton_r;
    m_letter_a_z[18]=ui->pushButton_s;
    m_letter_a_z[19]=ui->pushButton_t;
    m_letter_a_z[20]=ui->pushButton_u;
    m_letter_a_z[21]=ui->pushButton_v;
    m_letter_a_z[22]=ui->pushButton_w;
    m_letter_a_z[23]=ui->pushButton_x;
    m_letter_a_z[24]=ui->pushButton_y;
    m_letter_a_z[25]=ui->pushButton_z;



    setWindowFlags(Qt::Tool|Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);/*���ڷ�����ã�������ȥ������������ɫ����*/

    if(num)
    {
        Flag_InputMode = NUMBER;   //�����ʽ����
        Flag_Mid_Mode  = NUMBER;   //�����ʽ��ת����
        KeyBoardStartMode(NUMBER);
        ui->pushButton_en_ch->setEnabled(false);
        ui->pushButton_cap->setEnabled(false);
    }
    else
    {
        Flag_InputMode = LETTER;   //�����ʽ����
        Flag_Mid_Mode  = LETTER;   //�����ʽ��ת����
        KeyBoardStartMode(LETTER);  //HANZI
        ui->pushButton_en_ch->setEnabled(true);
        ui->pushButton_cap->setEnabled(true);
    }

    QString srf;
    QFile file(FILE_ZI_KU);//��FILE_ZI_KU�ļ�
    if (file.open(QFile::ReadOnly))
    {
        while (true)
        {
            char buf[1024] = {0};
            qint64 Len = file.readLine(buf, sizeof(buf));//��ȡ�ļ�һ��
            if (Len <= 0) break; //�ж��Ƿ��ȡ�ɹ���ʧ�ܼ�����
            srf += QString(buf);//����ȡ���ַ�����ӵ�srf��
        }
    }
    m_srf = srf.split("\r\n");      //��QString��split�������ֿ��ļ����ֳ�һ��һ�Σ�ÿ��ƴ����Ӧһ���ֿ�
    InitKeyBoard();
}
void Keyboard::InitKeyBoard()
{

}

void Keyboard::paintEvent(QPaintEvent *)
{
    QPainter dc(this);
    QRect r = QRect(0, 0, this->width(), this->height());

    Draw3dFrame(r, dc);
}

/*********ͨ��ƴ���Һ���*************
**QString PinYin �����ƴ��
**QString &Chinese ƴ������Ӧ�ĺ���
**********************************/
bool Keyboard::FindChinese(QString PinYin, QString &Chinese)
{
    QStringList lst;
    QString Line;

    for(int i=0; i<m_srf.count(); i++)//m_srf.count()QStringList�ж��ٶ�
    {
        Line = m_srf.at(i);
        if (Line.isEmpty() || PinYin.isEmpty()) continue;

        lst = Line.split(",");
        if (QString::compare(lst.at(0), PinYin, Qt::CaseInsensitive) == 0)
        {
            Chinese = lst.at(1);
            return true;
        }
    }
    return false;
}

void Keyboard::hanzi_zk_set()   //�˺�������ȡ�õ��ַ�����Ӧ�ĺ��ְ��ַ��ں�����ʾ���С�
{
    QString HzList;
    QString Temp;

    for (int i=0; i<10; i++)
    {
        m_Display[i]->setEnabled(false);
        m_Display[i]->setText("");
    }

    if (!FindChinese(m_CurPY, HzList))
    {
        m_Page_A=0;
        ui->pushButton_next->setEnabled(false);
        ui->pushButton_pre->setEnabled(false);
        return;
    }

    m_Page_A=(HzList.length()-1)/10;   //�����ҳ��
    //�˴�����if-else�������ǵ���ҳbuttonû�й����ǣ�hide��
    if (m_Page_A == m_Page)
    {
        ui->pushButton_next->setEnabled(false);
    }
    else
    {
        ui->pushButton_next->setEnabled(true);
    }
    if ( m_Page_A== 0||m_Page==0)
    {
        ui->pushButton_pre->setEnabled(false);
    }
    else
    {
        ui->pushButton_pre->setEnabled(true);
    }

    for (int i=m_Page*10; i<HzList.length(); i++)
    {
        Temp = HzList.at(i);
      //  Temp = QStringLiteral(Temp);

        m_Display[i%10]->setText(Temp);
        m_Display[i%10]->setEnabled(true);
        if ((i+1) % 10 == 0 && i != 0) break;
    }

}
/******************����϶����̴�����*******************************/
void Keyboard::mouseMoveEvent(QMouseEvent *event)  //�����϶�
{
    if (event->buttons() == Qt::LeftButton && m_MouseDown)
    {
        move(event->globalPos() - m_DragPos);
        event->accept();
    }
}

void Keyboard::mousePressEvent(QMouseEvent *event) //��ȡ�������λ�ã�
{
    if (event->button() == Qt::LeftButton)
    {
        m_DragPos = event->globalPos() - frameGeometry().topLeft();
        event->accept();

        m_MouseDown = true;
    }
}

void Keyboard::mouseReleaseEvent(QMouseEvent *event) //����ͷš�
{
    Q_UNUSED(event);

    m_MouseDown = false;
}
//------------------

/*********************ͨ�����������ַ�QLineEdit********************************/
void Keyboard::Input_letter_to_linEdit(const QString letter)
{
//    if(Flag_InputMode==LETTER)
    QWidget *q = QApplication::focusWidget();
    QLineEdit *e;
    QTextEdit *t;

    if (q != NULL)
    {
        e = dynamic_cast<QLineEdit *>(q);//dynamic_cast�����ж�q�������Ƿ�ΪQLineEdit����ǣ���ôe��ΪQLineEdit��
        if (e != NULL) e->insert(letter);

        t = dynamic_cast<QTextEdit *>(q);
        if (t != NULL) t->insertPlainText(letter);
    }
}
//Input_letter_to_linEdit--end

/*******�洢������ַ���ƴ������********************************/
void Keyboard::get_key_letter(QString letter)
{
    m_Page=0;
    if(k_letter_index++<6)
    {
        m_CurPY = m_CurPY + letter;
    }
    ui-> pinyin ->setText(m_CurPY);//Ҫ��uiָ��ǰ���������ָ��
    hanzi_zk_set();
                         //�嵱ǰҳΪ0
}
//get_key_letter____end


void Keyboard::on_pushButton_esc_clicked()
{
//    ui-> pushButton_en_ch->setText("����");
//    Flag_InputMode=HANZI;

//    m_CurPY="";
//    ui-> pinyin ->setText(m_CurPY);//Ҫ��uiָ��ǰ���������ָ��
//    hanzi_zk_set();

//    //�޸�һЩbutton
//    ui->pushButton_sign_tan->setText("��");
//    ui->pushButton_sign_wen->setText("��");
//    ui->pushButton_sign_dou->setText("��");
//    ui->pushButton_sign_ju->setText("��");
//    ui->pushButton_sign_fen->setText("��");
//    for(int i=0;i<26;i++)
//    {
//        m_letter_a_z[i]->setEnabled(true);
//    }
    close();
}

void Keyboard::on_pushButton_en_ch_clicked()//��Ӣ�л�button
{
    if(Flag_InputMode==HANZI)
    {
        KeyBoardLetterMode();
    }
    else if(Flag_InputMode==LETTER)
    {
        KeyBoardNumMode();
    }
    else if(Flag_InputMode==NUMBER)
    {
        KeyBoardHanziMode();
    }
}

void Keyboard::on_pushButton_cap_clicked()//��Сд��ĸ�����л�
{
    //��ʼ�����ֲ��ֵ�button
    clean_piy_and_hanzi_while_clicked();

    char flag=Flag_InputMode;
    if(flag==CAP_LETTER)
    {
        Flag_InputMode=Flag_Mid_Mode;
        ui->pushButton_cap->setText("cap");
        for(int i=0; i<26; i++)
        {
            QString Letter_Set="a";
            Letter_Set.fill('a'+i,1);
            m_letter_a_z[i]->setEnabled(true);
            m_letter_a_z[i]->setText(Letter_Set);
        }
    }
    else
    {
        Flag_Mid_Mode=Flag_InputMode;
        Flag_InputMode=CAP_LETTER;
        ui->pushButton_cap->setText("CAP");
        for(int i=0; i<26; i++)
        {
            QString Letter_Set="A";          //ֱ�ӡ�a��+i�ǲ���ʵ�ֵģ�����fill��������ʵ�֡�
            Letter_Set.fill('A'+i,1);
            m_letter_a_z[i]->setEnabled(true);
            m_letter_a_z[i]->setText(Letter_Set);
        }
    }
}

void Keyboard::clean_piy_and_hanzi_while_clicked(void) //��ѡ���ֺ����ƴ�����ͺ�������ִ�д˺������
{
    m_Page=0;//�嵱ǰҳ��
    k_letter_index=0;
    m_CurPY="";
    ui-> pinyin ->setText(m_CurPY);//Ҫ��uiָ��ǰ���������ָ��
    hanzi_zk_set();
}

void Keyboard::on_pushButton_q_clicked()
{
    if(Flag_InputMode == HANZI)
    {
        get_key_letter("q");
    }
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("q");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("Q");
    }

}
void Keyboard::on_pushButton_w_clicked()
{
    if(Flag_InputMode == HANZI)
    {
        get_key_letter("w");
    }
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("w");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("W");
    }
}
void Keyboard::on_pushButton_e_clicked()
{

    if(Flag_InputMode == HANZI)
    {
        get_key_letter("e");
    }
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("e");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("E");
    }
}
void Keyboard::on_pushButton_r_clicked()
{
    if(Flag_InputMode == HANZI)
    {
        get_key_letter("r");
    }
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("r");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("R");
    }
}
void Keyboard::on_pushButton_t_clicked()
{
    if(Flag_InputMode == HANZI)
        get_key_letter("t");
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("t");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("T");
    }
}
void Keyboard::on_pushButton_y_clicked()
{
    if(Flag_InputMode == HANZI)
        get_key_letter("y");
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("y");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("Y");
    }
}
void Keyboard::on_pushButton_u_clicked()
{
    if(Flag_InputMode == HANZI)
        get_key_letter("u");
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("u");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("U");
    }
}
void Keyboard::on_pushButton_i_clicked()
{
    if(Flag_InputMode == HANZI)
        get_key_letter("i");
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("i");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("I");
    }
}
void Keyboard::on_pushButton_o_clicked()
{
    if(Flag_InputMode == HANZI)
        get_key_letter("o");
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("o");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("O");
    }
}
void Keyboard::on_pushButton_p_clicked()
{
    if(Flag_InputMode == HANZI)
        get_key_letter("p");
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("p");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("P");
    }
}
void Keyboard::on_pushButton_a_clicked()
{
    if(Flag_InputMode == HANZI)
    {
        get_key_letter("a");
    }
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("a");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("A");
    }
}
void Keyboard::on_pushButton_s_clicked()
{
    if(Flag_InputMode == HANZI)
        get_key_letter("s");
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("s");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("S");
    }
}
void Keyboard::on_pushButton_d_clicked()
{
    if(Flag_InputMode == HANZI)
     {
        get_key_letter("d");
     }
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("d");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("D");
    }
}
void Keyboard::on_pushButton_f_clicked()
{
    if(Flag_InputMode == HANZI)
        get_key_letter("f");
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("f");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("F");
    }
}
void Keyboard::on_pushButton_g_clicked()
{
    if(Flag_InputMode == HANZI)
        get_key_letter("g");
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("g");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("G");
    }
}
void Keyboard::on_pushButton_h_clicked()
{
    if(Flag_InputMode == HANZI)
        get_key_letter("h");
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("h");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("H");
    }
}
void Keyboard::on_pushButton_j_clicked()
{
    if(Flag_InputMode == HANZI)
        get_key_letter("j");
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("j");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("J");
    }
}
void Keyboard::on_pushButton_k_clicked()
{
    if(Flag_InputMode == HANZI)
        get_key_letter("k");
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("k");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("K");
    }
}
void Keyboard::on_pushButton_l_clicked()
{
    if(Flag_InputMode == HANZI)
        get_key_letter("l");
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("l");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("L");
    }
}
void Keyboard::on_pushButton_z_clicked()
{
    if(Flag_InputMode == HANZI)
        get_key_letter("z");
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("z");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("Z");
    }
}
void Keyboard::on_pushButton_x_clicked()
{
    if(Flag_InputMode == HANZI)
        get_key_letter("x");
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("x");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("X");
    }
}
void Keyboard::on_pushButton_c_clicked()
{
    if(Flag_InputMode == HANZI)
        get_key_letter("c");
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("c");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("C");
    }
}
void Keyboard::on_pushButton_v_clicked()
{
    if(Flag_InputMode == HANZI)
        get_key_letter("v");
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("v");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("V");
    }
}
void Keyboard::on_pushButton_b_clicked()
{
    if(Flag_InputMode == HANZI)
        get_key_letter("b");
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("b");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("B");
    }
}
void Keyboard::on_pushButton_n_clicked()
{
    if(Flag_InputMode == HANZI)
        get_key_letter("n");
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("n");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("N");
    }
}
void Keyboard::on_pushButton_m_clicked()
{
    if(Flag_InputMode == HANZI)
        get_key_letter("m");
    else if(Flag_InputMode == LETTER)
    {
        Input_letter_to_linEdit("m");
    }
    else if(Flag_InputMode ==CAP_LETTER)
    {
        Input_letter_to_linEdit("M");
    }
}
void Keyboard::on_hz_1_clicked()
{
    m_Page=0;
    QString hz_n=ui->hz_1->text();
    Input_letter_to_linEdit(hz_n);
    if(Flag_InputMode == HANZI)
    {
        clean_piy_and_hanzi_while_clicked();
    }

}
void Keyboard::on_hz_2_clicked()
{
    m_Page=0;
    QString hz_n=ui->hz_2->text();
    Input_letter_to_linEdit(hz_n);
    if(Flag_InputMode == HANZI)
    {
        clean_piy_and_hanzi_while_clicked();
    }
}
void Keyboard::on_hz_3_clicked()
{
    m_Page=0;
    QString hz_n=ui->hz_3->text();
    Input_letter_to_linEdit(hz_n);
    if(Flag_InputMode == HANZI)
    {
        clean_piy_and_hanzi_while_clicked();
    }
}
void Keyboard::on_hz_4_clicked()
{
    m_Page=0;
    QString hz_n=ui->hz_4->text();
    Input_letter_to_linEdit(hz_n);
    if(Flag_InputMode == HANZI)
    {
        clean_piy_and_hanzi_while_clicked();
    }
}
void Keyboard::on_hz_5_clicked()
{
    m_Page=0;
    QString hz_n=ui->hz_5->text();
    Input_letter_to_linEdit(hz_n);
    if(Flag_InputMode == HANZI)
    {
        clean_piy_and_hanzi_while_clicked();
    }
}
void Keyboard::on_hz_6_clicked()
{
    m_Page=0;
    QString hz_n=ui->hz_6->text();
    Input_letter_to_linEdit(hz_n);
    if(Flag_InputMode == HANZI)
    {
        clean_piy_and_hanzi_while_clicked();
    }
}
void Keyboard::on_hz_7_clicked()
{
    QString hz_n=ui->hz_7->text();
    Input_letter_to_linEdit(hz_n);
    if(Flag_InputMode == HANZI)
    {
        clean_piy_and_hanzi_while_clicked();
    }
}
void Keyboard::on_hz_8_clicked()
{
    QString hz_n=ui->hz_8->text();
    Input_letter_to_linEdit(hz_n);
    if(Flag_InputMode == HANZI)
    {
        clean_piy_and_hanzi_while_clicked();
    }
}
void Keyboard::on_hz_9_clicked()
{
    QString hz_n=ui->hz_9->text();
    Input_letter_to_linEdit(hz_n);
    if(Flag_InputMode == HANZI)
    {
        clean_piy_and_hanzi_while_clicked();
    }
}
void Keyboard::on_hz_10_clicked()
{
    QString hz_n=ui->hz_10->text();
    Input_letter_to_linEdit(hz_n);
    if(Flag_InputMode == HANZI)
    {
        clean_piy_and_hanzi_while_clicked();
    }
}

void Keyboard::on_pushButton_next_clicked()//��һҳ
{
    if(Flag_InputMode == HANZI)
    {
        if(m_Page<m_Page_A)
        {
            m_Page++;
            hanzi_zk_set();
        }
    }

}
void Keyboard::on_pushButton_pre_clicked()//��һҳ
{
    if(m_Page==0) return;
    m_Page--;
    hanzi_zk_set();
}

void Keyboard::on_pushButton_del_clicked()//ɾ��button
{
    if(m_CurPY=="")//�����ǰƴ��Ϊ��
    {
        QWidget *q = QApplication::focusWidget();
        QLineEdit *e;
//        QTextEdit *t;

        if (q != NULL)
        {
            e = dynamic_cast<QLineEdit *>(q);//dynamic_cast�����ж�q�������Ƿ�ΪQLineEdit����ǣ���ôe��ΪQLineEdit��
            if (e != NULL) e->backspace ();  //ɾ��
    //        t = dynamic_cast<QTextEdit *>(q);
    //        if (t != NULL) t->insertPlainText(letter);
        }
    }
    else
    {
        if(k_letter_index--!=0)
        {
            m_CurPY.remove(m_CurPY.size()-1,1);  //remove ( int position, int n )position:Ҫɾ���ַ�����ʼλ�ã�n��Ҫɾ������
        }
        ui-> pinyin ->setText(m_CurPY);//Ҫ��uiָ��ǰ���������ָ��
        hanzi_zk_set();
    }

}


void Keyboard::on_pushButton_sign_tan_clicked()//��̾�ţ����߼Ӻţ����������ʱΪ�Ӻ�+
{
    Input_letter_to_linEdit(ui->pushButton_sign_tan->text());
}
void Keyboard::on_pushButton_sign_wen_clicked()//�ʺţ����߼��ţ����������ʱΪ-��
{
    Input_letter_to_linEdit(ui->pushButton_sign_wen->text());
}
void Keyboard::on_pushButton_sign_dou_clicked()//���ţ� ����*�����������ʱΪ*
{
    Input_letter_to_linEdit(ui->pushButton_sign_dou->text());
}
void Keyboard::on_pushButton_sign_ju_clicked()//��š�����/�����������ʱΪ/
{
    Input_letter_to_linEdit(ui->pushButton_sign_ju->text());
}
void Keyboard::on_pushButton_sign_fen_clicked()//�ֺţ����ߵȺţ����������ʱΪ=
{
    Input_letter_to_linEdit(ui->pushButton_sign_fen->text());
}

void Keyboard::on_pushButton_space_clicked()//�ո�
{
    if(m_CurPY=="")//�����ǰƴ��Ϊ��
    {
        Input_letter_to_linEdit(" ");
    }
    else
    {
        on_hz_1_clicked();
    }

}

void Keyboard::on_pushButton_Enter_clicked()
{
    if(m_CurPY=="")//�����ǰƴ��Ϊ��
    {
        Input_letter_to_linEdit("\r\n");
    }
    else
    {
        Input_letter_to_linEdit(m_CurPY);
        clean_piy_and_hanzi_while_clicked();
    }
}

void Keyboard::KeyBoardStartMode(int Mode)   //������̳�ʼ���뷨
{
    if(Mode == CAP_LETTER)
    {
        on_pushButton_cap_clicked();   //�����Сд��ĸ
    }
    else if(Mode == HANZI)
    {
        KeyBoardHanziMode(); //����
    }
    else if(Mode == NUMBER)
    {
        KeyBoardNumMode();   //����
    }
    else
    {
        KeyBoardLetterMode(); //����Сд��ĸ
    }

}

void Keyboard::KeyBoardHanziMode()           //�������뺺��ģʽ
{
    ui-> pushButton_en_ch->setText(tr("Chinese"));
    Flag_InputMode=HANZI;

    m_CurPY="";
    ui-> pinyin ->setText(m_CurPY);//Ҫ��uiָ��ǰ���������ָ��
    hanzi_zk_set();

    //�޸�һЩbutton
    ui->pushButton_sign_tan->setText(tr("!"));
    ui->pushButton_sign_wen->setText(tr("?"));
    ui->pushButton_sign_dou->setText(tr(","));
    ui->pushButton_sign_ju->setText(tr("."));
    ui->pushButton_sign_fen->setText(tr(";"));
    for(int i=0;i<26;i++)
    {
        m_letter_a_z[i]->setEnabled(true);
    }
}

void Keyboard::KeyBoardNumMode()            //������������ģʽ
{
    ui-> pushButton_en_ch->setText("NUM");
    Flag_InputMode=NUMBER;
    for (int i=0; i<10; i++)
    {
        QString num;
        num.setNum(i);
        m_Display[i]->setEnabled(true);
        m_Display[i]->setText(num);
    }
    //�޸�һЩbutton
    ui->pushButton_sign_tan->setText(tr("+"));
    ui->pushButton_sign_wen->setText(tr("-"));
    ui->pushButton_sign_dou->setText(tr("*"));
    ui->pushButton_sign_ju->setText(tr("."));
    ui->pushButton_sign_fen->setText(tr("="));
    for(int i=0;i<26;i++)
    {
        m_letter_a_z[i]->setEnabled(false);

    }
}

void Keyboard::KeyBoardLetterMode()              //��������Сд��ĸģʽ
{
    Flag_InputMode=LETTER;
    k_letter_index=0;
    m_CurPY="";
    ui-> pinyin ->setText(m_CurPY);//Ҫ��uiָ��ǰ���������ָ��
    hanzi_zk_set();
    ui-> pushButton_en_ch->setText(tr("English"));

    //�޸�һЩbutton
    ui->pushButton_sign_tan->setText(tr("!"));
    ui->pushButton_sign_wen->setText(tr("?"));
    ui->pushButton_sign_dou->setText(tr(","));
    ui->pushButton_sign_ju->setText(tr("/"));
    ui->pushButton_sign_fen->setText(tr(";"));
    for(int i=0;i<26;i++)
    {
        m_letter_a_z[i]->setEnabled(true);
    }
}


void Keyboard::Draw3dFrame(QRect &r, QPainter &dc)//��һ�����ο�
{
    QRect rect = r;

    dc.setPen(QPen(QColor::fromRgb(241,239,226), 1, Qt::SolidLine));
    dc.drawLine(rect.left(), rect.top(), rect.right(), rect.top());
    dc.drawLine(rect.left(), rect.top(), rect.left(), rect.bottom());

    dc.setPen(QPen(QColor::fromRgb(113,111,100), 1, Qt::SolidLine));
    dc.drawLine(rect.left(), rect.bottom(), rect.right(), rect.bottom());
    dc.drawLine(rect.right(), rect.top(), rect.right(), rect.bottom());

    rect.adjust(1,1,-1,-1);
    dc.setPen(QPen(QColor::fromRgb(255,255,255), 1, Qt::SolidLine));
    dc.drawLine(rect.left(), rect.top(), rect.right(), rect.top());
    dc.drawLine(rect.left(), rect.top(), rect.left(), rect.bottom());

    dc.setPen(QPen(QColor::fromRgb(172,168,153), 1, Qt::SolidLine));
    dc.drawLine(rect.left(), rect.bottom(), rect.right(), rect.bottom());
    dc.drawLine(rect.right(), rect.top(), rect.right(), rect.bottom());

    rect.adjust(1,1,-1,-1);
    dc.setPen(QPen(QColor::fromRgb(212,208,200), 1, Qt::SolidLine));
    dc.drawLine(rect.left(), rect.top(), rect.right(), rect.top());
    dc.drawLine(rect.left(), rect.top(), rect.left(), rect.bottom());

    dc.setPen(QPen(QColor::fromRgb(212,208,200), 1, Qt::SolidLine));
    dc.drawLine(rect.left(), rect.bottom(), rect.right(), rect.bottom());
    dc.drawLine(rect.right(), rect.top(), rect.right(), rect.bottom());
}


Keyboard::~Keyboard()
{
    delete ui;
}
