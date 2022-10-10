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

    //painter->drawImage(0,0,displayImage.scaled(static_cast<qint32>(this->width()),static_cast<qint32>(this->height()),Qt::IgnoreAspectRatio));

    QImage toDisplay = displayImage.scaled(static_cast<qint32>(this->width()),static_cast<qint32>(this->height()),Qt::KeepAspectRatio);

    // Now that the image is scaled, we compute the values to center the image.
    qint32 x = 0;
    qint32 y = 0;
    if (toDisplay.width() < this->width()){
        x = static_cast<qint32>((this->width() - toDisplay.width())/2);
    }
    if (toDisplay.height() < this->height()){
        y = static_cast<qint32>((this->height() - toDisplay.height())/2);
    }

    painter->drawImage(x,y,toDisplay);
}
