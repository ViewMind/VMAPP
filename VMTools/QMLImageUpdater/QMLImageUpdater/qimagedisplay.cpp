#include "qimagedisplay.h"

QImageDisplay::QImageDisplay(QQuickItem *parent):QQuickPaintedItem(parent)
{
    displayImage = QImage(100,100,QImage::Format_RGB32);
    displayImage.fill(QColor(Qt::blue));
}

void QImageDisplay::setImage(const QImage &image){
    displayImage = image;
    this->update();
}

void QImageDisplay::keyPressEvent(QKeyEvent *event){
    QQuickPaintedItem::keyPressEvent(event);
    qDebug() << "ImageDisplayKeyPressed" << event->key();
}


void QImageDisplay::paint(QPainter *painter){
    painter->drawImage(0,0,displayImage.scaled(static_cast<qint32>(this->width()),static_cast<qint32>(this->height()),Qt::IgnoreAspectRatio));
}
