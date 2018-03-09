#ifndef  KeyBoard_H
#define  KeyBoard_H

#include <QtGui>
#include <QSignalMapper>
#include "ui_keyboard.h"

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include "padbase.h"

class FormHanziSelector;
class FormNumPad;
class FormSymbolPad;
class KeyBoardButtons;

namespace Ui {
class KeyBoard;
}
enum KEYBOARD{
    KEBOARD_CHS,
    KEBOARD_ENG
};

class KeyBoard : public QDialog, Ui::KeyBoard
{
    Q_OBJECT

public:
    friend class FormHanziSelector;
    friend class KeyBoardButtons;

    KeyBoard(QWidget * parent = 0, Qt::WindowFlags fl = Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint );
    ~KeyBoard();

private:
    QSignalMapper *signalMapper;

    //QPoint			dragPosition;
    bool			caps_Lock;
    int             m_keyType;

    std::list<QWidget *> _lstHistory;

    FormHanziSelector	*_hanziSelector;
    FormNumPad			*_numPad;
    FormSymbolPad		*_symbolPad;

    QWidget *_lastFocusedWidget;

    // 移动的窗体及其像素数
    //QWidget *movedWidget;
    //int      movedY;

public:
    void gotoPad(QWidget *pad);

protected:
    bool event(QEvent *e);

private:
    void setMapper();
    void connectMapper();
    void setModeInput( int type );

    void sendKeyEvent(QKeyEvent *keyEvent);

signals:
    //void setvalue(const QString &);
    void characterGenerated(QChar);
    void sigUpdatePosition(QWidget *widget);

public slots:
    void setDispText(const QString& text);
    void setHanziText( const QString& text );

    void onCapslock();
    void onBackspace();
    void onEsc();

    void onModeInput();

    virtual void setVisible(bool visible);

    void gotoNumPad();
    void gotoSymbolPad();
    // symbolType: FormSymbolPad::EnumSymbolType
    void gotoSymbolPad(int symbolType);
    void onPrevPad();

    void saveFocusWidget(QWidget *oldFocus, QWidget *newFocus);

public slots:
    // 特殊字符生成
    void generateEnterChar();
    void generateBackSpaceChar();

private slots:
    void updatePosition(QWidget *widget);
};

#endif //  KeyBoard_H
