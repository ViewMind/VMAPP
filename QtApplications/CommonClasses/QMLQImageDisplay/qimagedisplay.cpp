#include "qimagedisplay.h"

QImageDisplay::QImageDisplay(QQuickItem *parent):QQuickPaintedItem(parent)
{
    displayImage = QImage(100,100,QImage::Format_RGB32);
    displayImage.fill(QColor(Qt::white));
}

void QImageDisplay::setImage(const QImage &image){
    displayImage = image;
    this->update();
}

void QImageDisplay::keyPressEvent(QKeyEvent *event){
    QQuickPaintedItem::keyPressEvent(event);
    //qDebug() << "ImageDisplayKeyPressed" << event->key();
}

QImage QImageDisplay::getImage() const{
    return displayImage;
}

void QImageDisplay::paint(QPainter *painter){
    //this->setWidth(displayImage.width());
    //this->setHeight(displayImage.height());
    if (displayImage.isNull()) {
        qDebug() << "Tying to paint a null image in the QImageDisplay";
        return;
    }
    painter->drawImage(0,0,displayImage.scaled(static_cast<qint32>(this->width()),static_cast<qint32>(this->height()),Qt::IgnoreAspectRatio));
}
