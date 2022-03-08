#include "calibrationarea.h"

CalibrationArea::CalibrationArea(QWidget *parent) : QDialog(parent)
{
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::X11BypassWindowManagerHint);

    QList<QScreen*> screens = QGuiApplication::screens();

    int W = screens.at(0)->geometry().width();
    int H = screens.at(0)->geometry().height();

    this->setGeometry(0,0,W,H);

    // Creating a graphics widget and adding it to the layout
    gview = new QGraphicsView(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(gview);

    gview->setScene(new QGraphicsScene(0,0,W,H,this));
    gview->scene()->addRect(0,0,W,H,QPen(),QBrush(Qt::gray));
    gview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

}

void CalibrationArea::setCurrentImage(const QImage &toshow){
    gview->scene()->clear();
    QPixmap p = QPixmap::fromImage(toshow);
    QGraphicsPixmapItem *item = gview->scene()->addPixmap(p);
    item->setPos(0,0);
}

void CalibrationArea::keyPressEvent(QKeyEvent *event){
    if (event->key() == Qt::Key_Escape){
        this->hide();
    }
}
