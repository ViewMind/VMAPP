#include "targettest.h"

TargetTest::TargetTest()
{
    canvas = nullptr;
    painter = nullptr;
    leftEye = nullptr;
    rightEye = nullptr;
}

void TargetTest::initialize(qint32 screenw, qint32 screenh){

    canvas = new QGraphicsScene(0,0,screenw,screenh);
    canvas->setBackgroundBrush(QBrush(Qt::gray));

    qreal R = K_LARGE_D*screenw/2;
    r = K_SMALL_D*screenw/2;

    qreal horizontal_target_space =  (screenw -4*2*R)/2;
    qreal vertical_target_space   =  (screenh -4*2*R)/2;
    qreal horizontal_margin       =  R;
    qreal vertical_margin         =  R;
    qreal offset = (R-r);



    QList<QPointF> largeTargetUpperRight;
    largeTargetUpperRight << QPointF(horizontal_margin                                  ,vertical_margin)
                          << QPointF(horizontal_margin + 2*R + horizontal_target_space  ,vertical_margin)
                          << QPointF(horizontal_margin + 4*R + 2*horizontal_target_space,vertical_margin)
                          << QPointF(horizontal_margin                                  ,vertical_margin + 2*R + vertical_target_space)
                          << QPointF(horizontal_margin + 2*R + horizontal_target_space  ,vertical_margin + 2*R + vertical_target_space)
                          << QPointF(horizontal_margin + 4*R + 2*horizontal_target_space,vertical_margin + 2*R + vertical_target_space)
                          << QPointF(horizontal_margin                                  ,vertical_margin + 4*R + 2*vertical_target_space)
                          << QPointF(horizontal_margin + 2*R + horizontal_target_space  ,vertical_margin + 4*R + 2*vertical_target_space)
                          << QPointF(horizontal_margin + 4*R + 2*horizontal_target_space,vertical_margin + 4*R + 2*vertical_target_space);

    for (qint32 i = 0; i < largeTargetUpperRight.size(); i++){
        QGraphicsEllipseItem *circle = canvas->addEllipse(0,0,2*R,2*R,QPen(Qt::black),QBrush(Qt::darkBlue));
        QGraphicsEllipseItem *innerCircle = canvas->addEllipse(0,0,2*r,2*r,QPen(Qt::black),QBrush(Qt::yellow));
        qreal x = largeTargetUpperRight.at(i).x();
        qreal y = largeTargetUpperRight.at(i).y();
        circle->setPos(x,y);
        innerCircle->setPos(x+offset,y+offset);
    }

    // Initializing the
    leftEye = canvas->addEllipse(0,0,2*r,2*r,QPen(),QBrush(QColor(0,0,255,100)));
    rightEye = canvas->addEllipse(0,0,2*r,2*r,QPen(),QBrush(QColor(0,255,0,100)));

    currentRender = QImage(screenw,screenh,QImage::Format_ARGB32);
    painter = new QPainter(&currentRender);
    painter->setRenderHint(QPainter::Antialiasing);

}

QImage TargetTest::renderCurrentPosition(qint32 rx, qint32 ry, qint32 lx, qint32 ly){
    Q_UNUSED(rx);
    Q_UNUSED(lx);
    Q_UNUSED(ry);
    Q_UNUSED(ly);

    if (!canvas) return currentRender;
    if (!painter) return currentRender;

    leftEye->setPos(lx-r,ly-r);
    rightEye->setPos(rx-r,ry-r);

    canvas->render(painter);
    return currentRender;
}

TargetTest::~TargetTest(){
    finalize();
}

void TargetTest::finalize(){
    if (canvas){
        delete canvas;
        canvas = nullptr;
    }
    leftEye = nullptr;
    rightEye = nullptr;
    if (painter){
        delete painter;
        painter = nullptr;
    }
}
