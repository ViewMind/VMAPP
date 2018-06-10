#include "followscreen.h"

FollowScreen::FollowScreen()
{
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::X11BypassWindowManagerHint);

    int W = QApplication::desktop()->screenGeometry(0).width();
    int H = QApplication::desktop()->screenGeometry(0).height();

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

    R = qMin(W,H)*0.01;

    // Adding the follower
    gaze = gview->scene()->addEllipse(0,0,2*R,2*R,QPen(),QBrush(QColor(0,255,255,127)));
    gaze->setPos(-R,-R);

    // Adding the text for the postion
    currentPosition = gview->scene()->addText("0,0",QFont("Sans-Serif",18,QFont::Bold));
    currentPosition->setPos((W - currentPosition->boundingRect().width())/2,(H - currentPosition->boundingRect().height())/2);

    // Adding the text for the freq.
    updateFreq = gview->scene()->addText("000 Hz",QFont("Sans-Serif",10));
    updateFreq->setPos(W - updateFreq->boundingRect().width()-5,updateFreq->boundingRect().height() + 2);
    lastTime = 0;

    this->hide();

}

void FollowScreen::newPosition(EyeTrackerData data){

    gaze->setPos(data.avgX()-R,data.avgY()-R);
    currentPosition->setPlainText(QString::number(data.avgX()) + "," + QString::number(data.avgY()));
    qreal x = (gview->width() -  currentPosition->boundingRect().width())/2;
    qreal y = (gview->height() -  currentPosition->boundingRect().height())/2;
    currentPosition->setPos(x,y);
    if (lastTime != 0){
        qreal T = data.time - lastTime;  // Difference in native unit.
        qreal mult = 1000;
        if (data.getTimeUnit() == EyeTrackerData::TU_US) mult = mult*1000;
        qint32 f = qRound(mult/T);
        updateFreq->setPlainText(QString::number(f) + " Hz");
    }
    lastTime = data.time;

}

void FollowScreen::keyPressEvent(QKeyEvent *e){
    if (e->key() == Qt::Key_Escape) this->hide();
}
