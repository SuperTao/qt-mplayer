#ifndef MYSLIDER_H
#define MYSLIDER_H

#include <QSlider>
#include <QtDebug>

class MySlider : public QSlider
{
    Q_OBJECT
public:
    explicit MySlider(QWidget *parent = 0);

signals:
    void SetValueSignal(void);

public slots:
    void action();

};

#endif // MYSLIDER_H
