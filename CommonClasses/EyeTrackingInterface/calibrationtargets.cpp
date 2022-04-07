#include "calibrationtargets.h"

CalibrationTargets::CalibrationTargets()
{
    canvas = nullptr;
    rightEyeTracker = nullptr;
    rightEyeTracker = nullptr;
    enableEyeTrackingInValidation = false;
}

void CalibrationTargets::enableEyeFollowersDuringValidation(bool enable){
    enableEyeTrackingInValidation = enable;
}

bool CalibrationTargets::isGazeFollowingEnabled() const{
    return enableEyeTrackingInValidation;
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
    isVerification = false;

}

void CalibrationTargets::verificationInitialization(qint32 npoints){
    indexInCalibrationSequence = -1;
    isVerification = true;
    this->setupCalibrationSequence(npoints);
}

qreal CalibrationTargets::getCalibrationTargetDiameter() const{
    return 2*R;
}

QVariantList CalibrationTargets::getCalibrationTargetCorners() const{
    QVariantList list;
    for (qint32 i = 0; i < calibrationTargets.size(); i++){
        QVariantMap point;
        point["x"] =  calibrationTargets.at(i).x();
        point["y"] =  calibrationTargets.at(i).y();
        list << point;
    }
    return list;
}

quint8 CalibrationTargets::isPointWithinCurrentTarget(qreal x, qreal y, qreal tolerance){

    if (indexInCalibrationSequence >= (calibrationSequenceIndex.size())) return 2;
    if (indexInCalibrationSequence < 0) return 2;

    qreal x_target = calibrationTargets.at(calibrationSequenceIndex.at(indexInCalibrationSequence)).x();
    qreal y_target = calibrationTargets.at(calibrationSequenceIndex.at(indexInCalibrationSequence)).y();

    qreal dimension = 2*R;
    qreal tol = tolerance*dimension;
    qreal tol_offset = tol/2;
    qreal xmin = x_target - tol_offset;
    qreal xmax = x_target + dimension + tol_offset;
    qreal ymin = y_target - tol_offset;
    qreal ymax = y_target + dimension + tol_offset;

    if ((x >= xmin) && (x <= xmax)){
        if ((y >= ymin) && (y <= ymax)){
            return 1;
        }
    }
    return 0;

}

QImage CalibrationTargets::getClearScreen(){
    canvas->clear();
    leftEyeTracker = nullptr;
    rightEyeTracker = nullptr;
    isVerification = false;
    canvas->setBackgroundBrush(QBrush(Qt::gray));
    QImage image(static_cast<int>(canvas->width()),static_cast<int>(canvas->height()),QImage::Format_RGB888);
    QPainter painter( &image );
    canvas->render( &painter );
    return image;
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

    //qDebug() << "Returning Target Centers: " << targetCenters;

    return targetCenters;
}

QImage CalibrationTargets::nextSingleTarget(){
    canvas->clear();
    leftEyeTracker = nullptr;
    rightEyeTracker = nullptr;

    if (indexInCalibrationSequence <= (calibrationSequenceIndex.size()-1)){
        indexInCalibrationSequence++;

        qreal offset = (R-r);

        qreal x = calibrationTargets.at(calibrationSequenceIndex.at(indexInCalibrationSequence)).x();
        qreal y = calibrationTargets.at(calibrationSequenceIndex.at(indexInCalibrationSequence)).y();

        QGraphicsEllipseItem *circle = canvas->addEllipse(0,0,2*R,2*R,QPen(Qt::black),QBrush(QColor("#81b2d2")));
        QGraphicsEllipseItem *innerCircle;
        if (isVerification){
            innerCircle = canvas->addEllipse(0,0,2*r,2*r,QPen(Qt::black),QBrush(Qt::blue));
        }
        else {
            innerCircle = canvas->addEllipse(0,0,2*r,2*r,QPen(Qt::black),QBrush(Qt::white));
        }
        circle->setPos(x,y);
        innerCircle->setPos(x+offset,y+offset);

    }
    else {
        isVerification = false;
    }

    if (enableEyeTrackingInValidation && isVerification){
        // Must create the dots again.
        leftEyeTracker = canvas->addEllipse(0,0,2*r,2*r,QPen(),QBrush(QColor(0,0,255,100))); // Left is Blue
        rightEyeTracker = canvas->addEllipse(0,0,2*r,2*r,QPen(),QBrush(QColor(0,255,0,100))); // Right is Green.

        // If EyeTracking In Validation is enabled and we are actually validating. Then this means that the eye tracking values will generate the image and it does not need to be generated here.
        return QImage();
    }
    else {
        QImage image(static_cast<int>(canvas->width()),static_cast<int>(canvas->height()),QImage::Format_RGB888);
        QPainter painter( &image );
        canvas->render( &painter );
        return image;
    }
}

QImage CalibrationTargets::renderCurrentPosition(qint32 rx, qint32 ry, qint32 lx, qint32 ly){
    if (!canvas) return QImage();
    if (leftEyeTracker == nullptr) return QImage();

    leftEyeTracker->setPos(lx-r,ly-r);
    rightEyeTracker->setPos(rx-r,ry-r);

    QImage image(static_cast<int>(canvas->width()),static_cast<int>(canvas->height()),QImage::Format_RGB888);
    QPainter painter( &image );
    canvas->render( &painter );
    return image;
}

CalibrationTargets::~CalibrationTargets(){

}

