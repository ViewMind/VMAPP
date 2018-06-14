#include "eventdetection.h"

EventDetection::EventDetection(QWidget *parent) : QWidget(parent)
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    btnDone = new QPushButton(this);
    connect(btnDone,SIGNAL(clicked()),this,SLOT(onButtonPressed()));
}

void EventDetection::onButtonPressed(){
    emit(buttonPressed());
}

void EventDetection::configure(qint32 x, qint32 y, qreal w, qreal h, const QString &message){
    this->setGeometry(x,y,w,h);
    btnDone->setGeometry(this->geometry());
    btnDone->setStyleSheet("background-color: rgb(0, 0, 0);\ncolor: rgb(255, 255, 255);\nfont: 72pt \"Arial\";");
    btnDone->setText(message);
}
