#ifndef MYLABEL_H
#define MYLABEL_H

#include <QObject>
#include <QLabel>
#include <QWidget>
class MyLabel : public QLabel
{
    Q_OBJECT
public:
    explicit MyLabel(QWidget *parent = 0);
    
signals:
    public:
    void paintEvent(QPaintEvent *);
    
public slots:
    
};

#endif // MYLABEL_H
