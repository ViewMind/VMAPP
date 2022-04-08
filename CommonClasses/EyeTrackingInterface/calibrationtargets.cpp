#include "calibrationtargets.h"

CalibrationTargets::CalibrationTargets()
{
    canvas = nullptr;
    rightEyeTracker = nullptr;
    rightEyeTracker = nullptr;
    validationTarget = nullptr;
    enableEyeTrackingInValidation = false;
}

void CalibrationTargets::enableEyeFollowersDuringValidation(bool enable){
    enableEyeTrackingInValidation = enable;
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
    this->setupCalibrationSequence(npoints);
    this->setupValidationTarget();
    isVerification = true;
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
    validationTarget = nullptr;
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

    if (indexInCalibrationSequence <= (calibrationSequenceIndex.size()-1)){
        indexInCalibrationSequence++;

        qreal offset = (R-r);

        qreal x = calibrationTargets.at(calibrationSequenceIndex.at(indexInCalibrationSequence)).x();
        qreal y = calibrationTargets.at(calibrationSequenceIndex.at(indexInCalibrationSequence)).y();

        QGraphicsEllipseItem *circle = canvas->addEllipse(0,0,2*R,2*R,QPen(Qt::black),QBrush(COLOR_OUTSIDE_CIRCLE_CALIBRATION));
        QGraphicsEllipseItem *innerCircle = canvas->addEllipse(0,0,2*r,2*r,QPen(Qt::black),QBrush(Qt::white));
        circle->setPos(x,y);
        innerCircle->setPos(x+offset,y+offset);

    }

    QImage image(static_cast<int>(canvas->width()),static_cast<int>(canvas->height()),QImage::Format_RGB888);
    QPainter painter( &image );
    canvas->render( &painter );
    return image;
}



void CalibrationTargets::setupValidationTarget(){
    canvas->clear();

    // Creating the validation target.
    validationTarget = canvas->addEllipse(0,0,2*R,2*R,QPen(Qt::black),QBrush(COLOR_VALIDATION_TARGET_NOT_HIT));

    // Creating the eye tracking circles.
    leftEyeTracker = canvas->addEllipse(0,0,2*r,2*r,QPen(Qt::black),QBrush(COLOR_LEFT_EYE_TRACKER)); // Left is Blue
    rightEyeTracker = canvas->addEllipse(0,0,2*r,2*r,QPen(Qt::black),QBrush(COLOR_RIGHT_EYE_TRACKER)); // Right is Green.

    // Set it outside of the view
    leftEyeTracker->setPos(-2*R,-2*R);
    rightEyeTracker->setPos(-2*R,-2*R);

    this->moveValidationTarget();

}

void CalibrationTargets::moveValidationTarget(){
    if (indexInCalibrationSequence <= (calibrationSequenceIndex.size()-1)){
        indexInCalibrationSequence++;
        qreal x = calibrationTargets.at(calibrationSequenceIndex.at(indexInCalibrationSequence)).x();
        qreal y = calibrationTargets.at(calibrationSequenceIndex.at(indexInCalibrationSequence)).y();
        validationTarget->setPos(x,y);
    }
}

QImage CalibrationTargets::renderCurrentPosition(qint32 rx, qint32 ry, qint32 lx, qint32 ly, qreal tolerance ,bool *hitL, bool *hitR){
    if (!canvas) return QImage();
    if (leftEyeTracker == nullptr) return QImage();
    if (validationTarget == nullptr) return QImage();
    if (!isVerification) return QImage();

    if (enableEyeTrackingInValidation){
        leftEyeTracker->setPos(lx-r,ly-r);
        rightEyeTracker->setPos(rx-r,ry-r);
    }

    *hitL = this->isPointWithinCurrentTarget(lx,ly,tolerance);
    *hitR = this->isPointWithinCurrentTarget(rx,ry,tolerance);

    QColor target = COLOR_VALIDATION_TARGET_NOT_HIT;
    if (*hitL){
        if (*hitR){
            target = COLOR_VALIDATION_TARGET_HIT_BOTH;
        }
        else {
            target = COLOR_VALIDATION_TARGET_HIT_LEFT;
        }
    }
    else if (*hitR){
        target = COLOR_VALIDATION_TARGET_HIT_RIGHT;
    }

    validationTarget->setBrush(QBrush(target));

    QImage image(static_cast<int>(canvas->width()),static_cast<int>(canvas->height()),QImage::Format_RGB888);
    QPainter painter( &image );
    canvas->render( &painter );
    return image;
}

CalibrationTargets::~CalibrationTargets(){

}

