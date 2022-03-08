#include "calibrationtargets.h"

CalibrationTargets::CalibrationTargets()
{
    canvas = nullptr;
    leftEye = nullptr;
    rightEye = nullptr;
    vrScale = 1;
}

void CalibrationTargets::initialize(qint32 screenw, qint32 screenh, bool useBorderTargetsAsCalibration){

    if (canvas != nullptr) return;

    canvas = new QGraphicsScene(0,0,screenw,screenh);
    canvas->setBackgroundBrush(QBrush(Qt::gray));

    // Setting up all the circles and the Radii
    R = K_LARGE_D*screenw/2;
    r = K_SMALL_D*screenw/2;

    qreal horizontal_target_space =  (screenw -4*2*R)/2;
    qreal vertical_target_space   =  (screenh -4*2*R)/2;
    qreal horizontal_margin       =  R;
    qreal vertical_margin         =  R;

    borderTargets.clear();
    borderTargets         << QPointF(horizontal_margin                                  ,vertical_margin)
                          << QPointF(horizontal_margin + 2*R + horizontal_target_space  ,vertical_margin)
                          << QPointF(horizontal_margin + 4*R + 2*horizontal_target_space,vertical_margin)
                          << QPointF(horizontal_margin                                  ,vertical_margin + 2*R + vertical_target_space)
                          << QPointF(horizontal_margin + 2*R + horizontal_target_space  ,vertical_margin + 2*R + vertical_target_space)
                          << QPointF(horizontal_margin + 4*R + 2*horizontal_target_space,vertical_margin + 2*R + vertical_target_space)
                          << QPointF(horizontal_margin                                  ,vertical_margin + 4*R + 2*vertical_target_space)
                          << QPointF(horizontal_margin + 2*R + horizontal_target_space  ,vertical_margin + 4*R + 2*vertical_target_space)
                          << QPointF(horizontal_margin + 4*R + 2*horizontal_target_space,vertical_margin + 4*R + 2*vertical_target_space);

    calibrationTargets    << QPointF(screenw*K_CALIBRATION_LT-R,screenh*K_CALIBRATION_LT-R)
                          << QPointF(screenw*K_CALIBRATION_MC-R,screenh*K_CALIBRATION_LT-R)
                          << QPointF(screenw*K_CALIBRATION_RB-R,screenh*K_CALIBRATION_LT-R)

                          << QPointF(screenw*K_CALIBRATION_LT-R,screenh*K_CALIBRATION_MC-R)
                          << QPointF(screenw*K_CALIBRATION_MC-R,screenh*K_CALIBRATION_MC-R)
                          << QPointF(screenw*K_CALIBRATION_RB-R,screenh*K_CALIBRATION_MC-R)

                          << QPointF(screenw*K_CALIBRATION_LT-R,screenh*K_CALIBRATION_RB-R)
                          << QPointF(screenw*K_CALIBRATION_MC-R,screenh*K_CALIBRATION_RB-R)
                          << QPointF(screenw*K_CALIBRATION_RB-R,screenh*K_CALIBRATION_RB-R);

    if (useBorderTargetsAsCalibration){
        calibrationTargets.clear();
        calibrationTargets = borderTargets;
    }

    calibrationSequenceIndex.clear();
    indexInCalibrationSequence = 0;

}

QImage CalibrationTargets::getClearScreen(){
    canvas->clear();
    canvas->setBackgroundBrush(QBrush(Qt::gray));
    QImage image(static_cast<int>(canvas->width()),static_cast<int>(canvas->height()),QImage::Format_RGB888);
    QPainter painter( &image );
    canvas->render( &painter );
    return image;
}

void CalibrationTargets::setTestTargetFontScale(qreal scale){
    vrScale = scale;
}


QList<QPointF> CalibrationTargets::setupCalibrationSequence(qint32 npoints){

    QList<QPointF> targetCenters;
    calibrationSequenceIndex.clear();
    if (npoints == 9){
        calibrationSequenceIndex << 0 << 1 << 2
                                 << 3 << 4 << 5
                                 << 6 << 7 << 8;
    }
    else{
        calibrationSequenceIndex << 0      << 2
                                 << 4
                                 << 6      << 8;
    }

    for (qint32 i = 0; i < calibrationSequenceIndex.size(); i++){
        QPointF p = calibrationTargets.at(calibrationSequenceIndex.at(i));
        // The list hast the upper left corner of the circle. It's center is +R lower and to the right.
        p.setX(p.x()+R);
        p.setY(p.y()+R);
        targetCenters << p;
    }

    indexInCalibrationSequence = -1;

    return targetCenters;
}

QImage CalibrationTargets::nextSingleTarget(){
    canvas->clear();

    if (indexInCalibrationSequence <= (calibrationSequenceIndex.size()-1)){
        indexInCalibrationSequence++;

        qreal offset = (R-r);

        qreal x = calibrationTargets.at(calibrationSequenceIndex.at(indexInCalibrationSequence)).x();
        qreal y = calibrationTargets.at(calibrationSequenceIndex.at(indexInCalibrationSequence)).y();

        QGraphicsEllipseItem *circle = canvas->addEllipse(0,0,2*R,2*R,QPen(Qt::black),QBrush(QColor("#81b2d2")));
        QGraphicsEllipseItem *innerCircle = canvas->addEllipse(0,0,2*r,2*r,QPen(Qt::black),QBrush(Qt::white));
        circle->setPos(x,y);
        innerCircle->setPos(x+offset,y+offset);

    }

    QImage image(static_cast<int>(canvas->width()),static_cast<int>(canvas->height()),QImage::Format_RGB888);
    QPainter painter( &image );
    canvas->render( &painter );
    return image;
}

void CalibrationTargets::setTargetTest(){
    canvas->clear();

    qint32 screenw = static_cast<qint32>(canvas->sceneRect().width());
    qreal offset = R - r;

    lastTimeStamp = -1;
    movingAverage.setWindowSize(500);

    for (qint32 i = 0; i < borderTargets.size(); i++){
        QGraphicsEllipseItem *circle = canvas->addEllipse(0,0,2*R,2*R,QPen(Qt::black),QBrush(Qt::darkBlue));
        QGraphicsEllipseItem *innerCircle = canvas->addEllipse(0,0,2*r,2*r,QPen(Qt::black),QBrush(Qt::yellow));
        qreal x = borderTargets.at(i).x();
        qreal y = borderTargets.at(i).y();
        circle->setPos(x,y);
        innerCircle->setPos(x+offset,y+offset);
    }

    for (qint32 i = 0; i < calibrationTargets.size(); i++){
        QGraphicsEllipseItem *circle = canvas->addEllipse(0,0,2*R,2*R,QPen(Qt::black),QBrush(QColor("#81b2d2")));
        QGraphicsEllipseItem *innerCircle = canvas->addEllipse(0,0,2*r,2*r,QPen(Qt::black),QBrush(Qt::white));
        qreal x = calibrationTargets.at(i).x();
        qreal y = calibrationTargets.at(i).y();
        circle->setPos(x,y);
        innerCircle->setPos(x+offset,y+offset);
    }

    // Appending test text. Original sizes are 40 and 23.
    qint32 large_font_scale = static_cast<qint32>(40*vrScale);
    qint32 small_font_scale = static_cast<qint32>(23*vrScale);
    QGraphicsTextItem *testText1 = canvas->addText("This is a very large sentence to test how the eye tracking works",QFont("Mono",large_font_scale,QFont::Bold));
    QGraphicsTextItem *testText2 = canvas->addText("Esta es una oración larga para ver cuan bien fuciona el seguimiento ocular",QFont("Mono",small_font_scale,QFont::Bold));
    freqDisplay = canvas->addText("0",QFont("Mono",large_font_scale,QFont::Bold));
    testText1->setPos((screenw - testText1->boundingRect().width())/2,4*R);
    freqDisplayY = testText1->pos().y() + testText1->boundingRect().height()*2;
    freqDisplay->setPos((screenw - freqDisplay->boundingRect().width())/2,freqDisplayY);
    testText2->setPos((screenw - testText2->boundingRect().width())/2,16*R);

    // Initializing the
    leftEye = canvas->addEllipse(0,0,2*r,2*r,QPen(),QBrush(QColor(0,0,255,100)));
    rightEye = canvas->addEllipse(0,0,2*r,2*r,QPen(),QBrush(QColor(0,255,0,100)));
}

QImage CalibrationTargets::renderCurrentPosition(qint32 rx, qint32 ry, qint32 lx, qint32 ly, qreal timestamp){
    if (!canvas) return QImage();

    leftEye->setPos(lx-r,ly-r);
    rightEye->setPos(rx-r,ry-r);

    qreal f = 0;
    if (lastTimeStamp > 0){
        qreal T = timestamp - lastTimeStamp;
        f = 1000.0/T;
    }
    lastTimeStamp = timestamp;
    movingAverage.add(f);
    f = movingAverage.getAvearage();

    qreal screenw = canvas->sceneRect().width();
    freqDisplay->setPlainText(QString::number(f));
    freqDisplay->setPos((screenw - freqDisplay->boundingRect().width())/2,freqDisplayY);

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

CalibrationTargets::MovingAverage::MovingAverage(){
    windowSize = 10;
    avearage = 0;
}

qreal CalibrationTargets::MovingAverage::getAvearage(){
    return avearage;
}

void CalibrationTargets::MovingAverage::setWindowSize(qint32 n){
    windowSize = n;
}

void CalibrationTargets::MovingAverage::add(qreal v){
    window << v;
    if (window.size() == windowSize){
        qreal acc = 0;
        for (qint32 i = 0; i < windowSize; i++){
            acc = acc + v;
        }
        avearage = acc/windowSize;
        window.clear();
    }
}


