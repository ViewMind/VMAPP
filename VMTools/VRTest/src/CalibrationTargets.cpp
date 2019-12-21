#include "CalibrationTargets.h"

CalibrationTargets::CalibrationTargets()
{
    canvas = nullptr;
    leftEye = nullptr;
    rightEye = nullptr;
}

void CalibrationTargets::initialize(qint32 screenw, qint32 screenh){

    if (canvas != nullptr) return;
    canvas = new QGraphicsScene(0,0,screenw,screenh);
    canvas->setBackgroundBrush(QBrush(Qt::gray));
}

QImage CalibrationTargets::getClearScreen(){
    canvas->clear();
    canvas->setBackgroundBrush(QBrush(Qt::gray));
    QImage image(static_cast<int>(canvas->width()),static_cast<int>(canvas->height()),QImage::Format_RGB888);
    QPainter painter( &image );
    canvas->render( &painter );
    return image;
}

QImage CalibrationTargets::setSingleTarget(qint32 x, qint32 y){
    canvas->clear();

    qreal R = K_LARGE_D*canvas->sceneRect().width()/2;
    r = K_SMALL_D*canvas->sceneRect().width()/2;
    qreal offset = (R-r);

    qreal xf = x - R;
    qreal yf = y - R;

    QGraphicsEllipseItem *circle = canvas->addEllipse(0,0,2*R,2*R,QPen(Qt::black),QBrush(Qt::darkBlue));
    QGraphicsEllipseItem *innerCircle = canvas->addEllipse(0,0,2*r,2*r,QPen(Qt::black),QBrush(Qt::yellow));
    circle->setPos(xf,yf);
    innerCircle->setPos(xf+offset,yf+offset);

    QImage image(static_cast<int>(canvas->width()),static_cast<int>(canvas->height()),QImage::Format_RGB888);
    QPainter painter( &image );
    canvas->render( &painter );
    return image;

}

void CalibrationTargets::setTargetTest(){
    canvas->clear();

    qint32 screenw = static_cast<qint32>(canvas->sceneRect().width());
    qint32 screenh = static_cast<qint32>(canvas->sceneRect().height());

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

    // Appending test text.
    QGraphicsTextItem *testText1 = canvas->addText("This is a very large sentence to test how the eye tracking works",QFont("Mono",40,QFont::Bold));
    QGraphicsTextItem *testText2 = canvas->addText("Esta es una oración larga para ver cuan bien fuciona el seguimiento ocular",QFont("Mono",23,QFont::Bold));
    testText1->setPos((screenw - testText1->boundingRect().width())/2,vertical_margin + R + (R+vertical_target_space)/2 - testText1->boundingRect().height()/2);
    testText2->setPos((screenw - testText2->boundingRect().width())/2,vertical_margin + 3*R + vertical_target_space + (R+vertical_target_space)/2 - testText2->boundingRect().height()/2);

    // Initializing the
    leftEye = canvas->addEllipse(0,0,2*r,2*r,QPen(),QBrush(QColor(0,0,255,100)));
    rightEye = canvas->addEllipse(0,0,2*r,2*r,QPen(),QBrush(QColor(0,255,0,100)));
}

QImage CalibrationTargets::renderCurrentPosition(qint32 rx, qint32 ry, qint32 lx, qint32 ly){
    if (!canvas) return QImage();

    leftEye->setPos(lx-r,ly-r);
    rightEye->setPos(rx-r,ry-r);

    QImage image(static_cast<int>(canvas->width()),static_cast<int>(canvas->height()),QImage::Format_RGB888);
    QPainter painter( &image );
    canvas->render( &painter );
    return image;
}

void CalibrationTargets::saveCanvasToTestImageFile(){
    QImage image(static_cast<int>(canvas->width()),static_cast<int>(canvas->height()),QImage::Format_RGB888);
    QPainter painter( &image );
    canvas->render( &painter );
    image.save("test.png");
}

CalibrationTargets::~CalibrationTargets(){

}

