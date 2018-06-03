#include "monitorscreen.h"

MonitorScreen::MonitorScreen(QWidget *parent, const QRect &screen, qreal SCREEN_W, qreal SCREEN_H) : QWidget(parent)
{

    // Making this window frameless and making sure it stays on top.
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::X11BypassWindowManagerHint|Qt::Window);

    this->setGeometry(screen);

    // Creating a graphics widget and adding it to the layout
    gview = new QGraphicsView(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(gview);

    gview->setScene(new QGraphicsScene(0,0,screen.width(),screen.height(),this));
    gview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // White background
    gview->scene()->setBackgroundBrush(QBrush(Qt::gray));

    // Adding the background
    QPixmap temp(gview->scene()->width(),gview->scene()->height());
    background = gview->scene()->addPixmap(temp);

    // Adding the eye tracking circles
    R = gview->scene()->width()*0.007;
    leftEyeTracker = gview->scene()->addEllipse(0,0,2*R,2*R,QPen(),QBrush(QColor(0,0,255,100)));
    rightEyeTracker = gview->scene()->addEllipse(0,0,2*R,2*R,QPen(),QBrush(QColor(0,255,0,100)));

    // Calculating the values. Will assume that scaling to height is the right option.
    targetHeight = gview->scene()->height();
    targetWidth = gview->scene()->width();
    scaleX = (targetWidth/SCREEN_W);
    scaleY = (targetHeight/SCREEN_H);
    background->setPos(0,0);
}

void MonitorScreen::setBackgroundBrush(const QBrush &brush){
    gview->scene()->setBackgroundBrush(brush);
}

void MonitorScreen::updateBackground(const QPixmap &image){
    QPixmap imagenew = image.scaled(QSize(targetWidth,targetHeight),Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    background->setPixmap(imagenew);
}


void MonitorScreen::updateEyePositions(qint32 rx, qint32 ry, qint32 lx, qint32 ly){
    rx = rx*scaleX;
    lx = lx*scaleX;
    ry = ry*scaleY;
    ly = ly*scaleY;

    rightEyeTracker->setPos(rx-R,ry-R);
    leftEyeTracker->setPos(lx-R,ly-R);
}
