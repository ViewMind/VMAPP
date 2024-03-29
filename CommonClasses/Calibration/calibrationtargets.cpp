#include "calibrationtargets.h"

CalibrationTargets::CalibrationTargets()
{
    connect(&animator,&AnimationManager::animationUpdated,this,&CalibrationTargets::onUpdateAnimation);
    connect(&animator,&AnimationManager::reachedAnimationStop,this,&CalibrationTargets::onReachedAnimationStop);
    expectedAnimationSignalName = "";
}

void CalibrationTargets::onUpdateAnimation(){
    emit CalibrationTargets::newAnimationFrame();
}

void CalibrationTargets::onReachedAnimationStop(const QString &variable, qint32 animationStopIndex, qint32 animationSignalType){
    // So any animation stop index, other than the first one should be notified.
    if (variable == expectedAnimationSignalName){
        if (animationStopIndex > 0){
            if (animationSignalType == RenderServerItem::ANIMATION_BEGIN_DELAY){
                calibrationCounter++;
                emit CalibrationTargets::calibrationPointStatus(calibrationCounter,false);
            }
            else if (animationSignalType == RenderServerItem::ANIMATION_DELAY_END){
                emit CalibrationTargets::calibrationPointStatus(calibrationCounter,true);
            }
        }
    }
}


RenderServerScene CalibrationTargets::getCurrentCalibrationAnimationFrame() const {
//    RenderServerScene s = animator.getCurrentFrameAsScene();
//    qreal hw = canvas.width()/2;
//    qreal hh = canvas.height()/2;
//    s.addLine(hw,0,hw,canvas.height(),QPen());
//    s.addLine(0,hh,canvas.width(),hh,QPen());
//    return s;
    return animator.getCurrentFrameAsScene();
}


void CalibrationTargets::initialize(qint32 screenw,
                                    qint32 screenh,
                                    bool useBorderTargetsAsCalibration){


    //canvas = new QGraphicsScene(0,0,screenw,screenh);
    canvas.setSceneRect(0,0,screenw,screenh);

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


qreal CalibrationTargets::getCalibrationTargetDiameter() const{
    return 2*R;
}

QVariantList CalibrationTargets::getCalibrationTargetCorners() const{

    QVariantList list;
    for (qint32 i = 1; i < calibrationSequenceIndex.size(); i++){
        QVariantMap point;
        QPointF p = calibrationTargets.at(calibrationSequenceIndex.at(i));
        point["x"] = p.x();
        point["y"] = p.y();
        list << point;
    }

    return list;
}

QList<QPointF> CalibrationTargets::setupCalibrationSequence(qint32 npoints, qint32 hold_time_for_targets){

    QList<QPointF> targetCenters;
    calibrationSequenceIndex.clear();

    calibrationSequenceIndex << 4; // Calibration point shall alwasy start in the middle before moving on to it's actual end.

    if (npoints == 9){
//        calibrationSequenceIndex << 0 << 1 << 2
//                                 << 3 << 4 << 5
//                                 << 6 << 7 << 8;
        calibrationSequenceIndex << 0 << 1 << 2 << 5 << 8 << 7 << 6 << 3 << 4;
    }
    else{
//        calibrationSequenceIndex << 0      << 2
//                                 << 4
//                                 << 6      << 8;
        calibrationSequenceIndex << 0 << 2 << 8 << 6 << 4;
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

    canvas.clear();
    canvas.setBackgroundBrush(QBrush(Qt::gray));

    RenderServerCircleItem *circle = canvas.addEllipse(0,0,2*R,2*R,QPen(Qt::black),QBrush(COLOR_OUTSIDE_CIRCLE_CALIBRATION));
    RenderServerCircleItem *innerCircle = canvas.addEllipse(0,0,2*r,2*r,QPen(Qt::black),QBrush(Qt::white));

    qreal alpha = 0.2;
    qreal animationSmallR = R*alpha + (1-alpha)*r;

    qint32 outerCircleID = circle->getItemID();
    qint32 innerCircleID = innerCircle->getItemID();

    // FIRST we add the pulsating animation for the outer circle.
    animator.setScene(canvas);
    animator.addVariableStop(outerCircleID,RenderControlPacketFields::RADIOUS,animationSmallR,0,false,0);
    animator.addVariableStop(outerCircleID,RenderControlPacketFields::RADIOUS,R,ANIMATION_OUTTER_CIRCLE_PULSATING_R,false,0);

    // The X and Y of the circle represent it's center and not it's upper left corner.
    QString xname = RenderControlPacketFields::X + ".0";
    QString yname = RenderControlPacketFields::Y + ".0";

    expectedAnimationSignalName = QString::number(outerCircleID) + "-" + xname;

    // Now we add the animation path for the the actual calibration.
    for (qint32 i = 0; i < targetCenters.size(); i++){
        QPointF p = targetCenters.at(i);

        qint32 holdtime = 0;
        if (i == 1){
            holdtime = ANIMATION_HOLD_TIME_INTIAL_CENTER_TARGET;
        }
        else {
            holdtime = hold_time_for_targets;
        }

        // Outer circle first.
        animator.addVariableStop(outerCircleID,xname,p.x(),ANIMATION_MOVE_TIME,true,holdtime);
        animator.addVariableStop(outerCircleID,yname,p.y(),ANIMATION_MOVE_TIME,false,holdtime);

        // And the inner circle
        animator.addVariableStop(innerCircleID,xname,p.x(),ANIMATION_MOVE_TIME,true,holdtime);
        animator.addVariableStop(innerCircleID,yname,p.y(),ANIMATION_MOVE_TIME,false,holdtime);

    }

    // Before returning the target centers, we need to remove the first "fake" middle target.
    targetCenters.removeFirst();

    return targetCenters;
}

void CalibrationTargets::getWaitScreenInfo(qreal *RR, qreal *rr, QColor *outerCircleColor, QList<QPointF> *outsideCenters){
    outsideCenters->clear();
    outerCircleColor->setNamedColor(COLOR_OUTSIDE_CIRCLE_CALIBRATION.name());
    *RR = R;
    *rr = r;

    QList<qint32> cornerIndexes; cornerIndexes << 0 << 2 << 6 << 8;

    for (qint32 i = 0; i < calibrationTargets.size(); i++){
        if (!cornerIndexes.contains(i)) continue;
        QPointF p = calibrationTargets.at(i);
        // The list hast the upper left corner of the circle. It's center is +R lower and to the right.
        p.setX(p.x()+R);
        p.setY(p.y()+R);
        outsideCenters->append(p);
    }
}

void CalibrationTargets::calibrationAnimationControl(bool start){
    if (start) {
        animator.startAnimation();
        calibrationCounter = -1;
    }
    else {
        expectedAnimationSignalName = "";
        animator.stopAnimation();
    }
}

CalibrationTargets::~CalibrationTargets(){

}

