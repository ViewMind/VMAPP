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

//    QPoint p0(0.05*W,0.05*H);
//    calibrationPoints << p0;
//    QPoint p1(0.95*W,0.05*H);
//    calibrationPoints << p1;
//    QPoint p2(0.95*W,0.95*H);
//    calibrationPoints << p2;
//    QPoint p3(0.05*W,0.95*H);
//    calibrationPoints << p3;
//    R = 0.01*W;

//    calibrationTarget = gview->scene()->addEllipse(0,0,2*R,2*R,QPen(QBrush(Qt::black),2),QBrush(Qt::red));
//    // To hide the target
//    calibrationTarget->setZValue(-10);

}

void CalibrationArea::setCurrentImage(const QImage &toshow){
    toshow.save("00calibration.png");
    QPixmap p = QPixmap::fromImage(toshow);
    QGraphicsPixmapItem *item = gview->scene()->addPixmap(p);
    item->setPos(0,0);
}

//void CalibrationArea::showEvent(QShowEvent *event){
//    Q_UNUSED(event)
//    currentPoint = 0;
//    qDebug() << "Calibration area show event";
//    drawCalibrationPoint();
//}

//void CalibrationArea::drawCalibrationPoint(){
//    calibrationTarget->setZValue(10);
//    QPoint p = calibrationPoints.at(currentPoint);
//    int x = p.x() - R;
//    int y = p.y() - R;
//    calibrationTarget->setPos(x,y);
//}

//bool CalibrationArea::isInCalibrationPoint(int x, int y){

//    if (autoCalibrate) return true;

//    if (currentPoint == calibrationPoints.size()) return true;

//    QPoint center = calibrationPoints.at(currentPoint);
//    qreal diff = qSqrt((x - center.x())*(x - center.x()) + (y - center.y())*(y - center.y()));
//    if (diff < R){
//        currentPoint++;
//        if (currentPoint < calibrationPoints.size()){
//            drawCalibrationPoint();
//            return false;
//        }
//        else return true;
//    }

//    return false;
//}

void CalibrationArea::keyPressEvent(QKeyEvent *event){
    if (event->key() == Qt::Key_Escape){
        this->hide();
    }
}
