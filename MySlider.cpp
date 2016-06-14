#include "MySlider.h"

MySlider::MySlider(QWidget *parent) :
    QSlider(parent)
{
    this->setGeometry(20,420,200,200);
    this->setRange(0,100);
    this->setOrientation(Qt::Horizontal);
    this->show();
}
void MySlider::action()
{
    qDebug() << "slider be pressed";
    int value = this->sliderPosition();
 //   emit this->SetValueSignal();
    emit this->valueChanged(value);
}
