#include "control.h"

Control::Control(QObject *parent) : QObject(parent)
{
    canvas = new QGraphicsScene(0,0,500,200);
    currentImage = QImage(static_cast<qint32>(canvas->width()),static_cast<qint32>(canvas->height()),QImage::Format_RGB32);
    counter = 0;
}

QImage Control::image() const{
    return currentImage;
}

void Control::goToNextImage(){
    canvas->clear();
    canvas->setBackgroundBrush(QBrush(Qt::cyan));
    QGraphicsTextItem *number = canvas->addText(QString::number(counter),QFont("Courier",50,QFont::Bold));
    counter++;
    number->setPos((canvas->width() - number->boundingRect().width())/2,(canvas->height() - number->boundingRect().height())/2);
    QPainter painter( &currentImage );
    canvas->render( &painter );
    emit(newImageAvailable());
}
