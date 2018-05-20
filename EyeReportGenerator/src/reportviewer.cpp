#include "reportviewer.h"

ReportViewer::ReportViewer(QWidget *parent): QWidget(parent)
{

    // Making this window frameless and making sure it stays on top.
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::X11BypassWindowManagerHint|Qt::Window);
    qreal width = SCREEN_H/1.414; // Max height and width proportionate to screen width.
    this->setGeometry(0,0,width,SCREEN_H);

    // Creating a graphics widget and adding it to the layout
    gview = new QGraphicsView(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(gview);
    gview->setScene(new QGraphicsScene(0,0,width,SCREEN_H));

    gview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

}

void ReportViewer::loadReport(const QString &report){
    QImage image(report);
    if (image.isNull()) return;

    image = image.scaled(QSize(this->width(),this->height()),Qt::KeepAspectRatio,Qt::SmoothTransformation);

    gview->scene()->clear();
    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap::fromImage(image));
    gview->scene()->addItem(item);
}
