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
    QPixmap temp(static_cast<qint32>(gview->scene()->width()),static_cast<qint32>(gview->scene()->height()));
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

    // Messages
    QFont messageFont("Mono",12,QFont::Bold,false);
    messages = gview->scene()->addSimpleText("",messageFont);
    messages->setBrush(QBrush(QColor("#e79a00")));
    messages->setPos(0,0);
}

void MonitorScreen::setBackgroundBrush(const QBrush &brush){
    gview->scene()->setBackgroundBrush(brush);
}

void MonitorScreen::updateBackground(const QPixmap &image){
    QPixmap imagenew = image.scaled(QSize(static_cast<qint32>(targetWidth),static_cast<qint32>(targetHeight)),
                                    Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
    background->setPixmap(imagenew);
    // Clearing fixations and messages.
    for (qint32 i = 0; i < rightEyeFixations.size(); i++){
        gview->scene()->removeItem(rightEyeFixations.at(i));
    }
    for (qint32 i = 0; i < leftEyeFixations.size(); i++){
        gview->scene()->removeItem(leftEyeFixations.at(i));
    }
    rightEyeFixations.clear();
    leftEyeFixations.clear();
    messages->setText("");
}

void MonitorScreen::addFixations(qint32 rx, qint32 ry, qint32 lx, qint32 ly){
    if ((rx >= 0) && (ry >= 0)){
        QGraphicsEllipseItem *f = gview->scene()->addEllipse(0,0,2*R,2*R,QPen(),QBrush(QColor(0,255,0)));
        f->setPos(static_cast<qint32>(rx*scaleX) - R,static_cast<qint32>(ry*scaleY) - R);
        rightEyeFixations << f;
    }
    if ((lx >= 0) && (ly >= 0)){
        QGraphicsEllipseItem *f = gview->scene()->addEllipse(0,0,2*R,2*R,QPen(),QBrush(QColor(0,0,255)));
        f->setPos(static_cast<qint32>(lx*scaleX) - R,static_cast<qint32>(ly*scaleY) - R);
        leftEyeFixations << f;
    }
}


void MonitorScreen::addMessages(const QString &message, bool append){
    if (append){
        if (messages->text().isEmpty()) messages->setText(message);
        else messages->setText(messages->text() + "\n" + message);
    }
    else{
        messages->setText(message);
    }
}

void MonitorScreen::updateEyePositions(qint32 rx, qint32 ry, qint32 lx, qint32 ly){

    rx = static_cast<qint32>(rx*scaleX);
    lx = static_cast<qint32>(lx*scaleX);
    ry = static_cast<qint32>(ry*scaleY);
    ly = static_cast<qint32>(ly*scaleY);

    rightEyeTracker->setPos(rx-R,ry-R);
    leftEyeTracker->setPos(lx-R,ly-R);
}
