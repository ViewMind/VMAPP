#include "simpleanimationcontroller.h"

SimpleAnimationController::SimpleAnimationController()
{
    //connect(&calibrationTimer,&QTimer::timeout,this,&CalibrationLeastSquares::calibrationTimeInTargetUp);
    connect(&timer,&QTimer::timeout,this,&SimpleAnimationController::timerUp);
}

void SimpleAnimationController::clear(){
    stepCounter = 0;
    numberOfAnimationSteps = 0;
    variableDeltas.clear();
    animationVariables.clear();
    currentValues.clear();
    timer.stop();
}

void SimpleAnimationController::addAnimationVariable(const QString &name, qreal startValue, qreal stopValue){
    StartStopPoint ssp;
    ssp.first = startValue;
    ssp.second = stopValue;
    animationVariables.insert(name,ssp);
    currentValues.insert(name,startValue);
}

void SimpleAnimationController::setAnimatioDuration(qreal targetFramerate, qint32 animationSteps){
    //numberOfAnimationSteps = static_cast<qint32>(numberOfMilliseconds*30.0/1000.0);

    timerTimeOut = qRound(1000/targetFramerate);
    animationDuration = timerTimeOut*animationSteps;
    numberOfAnimationSteps = animationSteps;

    //qDebug() << "Set the animation duration to" << animationDuration << "ms. Target Frame Rate" << targetFramerate << "fps. Number of animation steps"
    //<< numberOfAnimationSteps << ". Timeout set to " << timerTimeOut;
}

qreal SimpleAnimationController::getCurrentValueForVariable(const QString &name){
    return currentValues.value(name);
}

void SimpleAnimationController::animationStart(){

    // Computing all deltas.
    variableDeltas.clear();

    QStringList variableNames = animationVariables.keys();

    for (qint32 i = 0; i < variableNames.size(); i++){
        QString vname = variableNames.at(i);
        qreal a = animationVariables.value(vname).first;
        qreal b = animationVariables.value(vname).second;
        qreal delta = (b-a)/static_cast<qreal>(numberOfAnimationSteps);
        variableDeltas.insert(vname,delta);
    }

    // And starting the timer.
    stepCounter = 0;
    timer.start(timerTimeOut);
    measureTimer.start();

}

void SimpleAnimationController::timerUp(){
    stepCounter++;

    //qDebug() << "Step Counter is " << stepCounter << " elapsed time is" << measureTimer.elapsed() << "ms";

    QStringList variableNames = animationVariables.keys();

    for (qint32 i = 0; i < variableNames.size(); i++){
        QString vname = variableNames.at(i);
        qreal v = currentValues.value(vname);
        v = v + variableDeltas.value(vname);
        currentValues.insert(vname,v);
    }

    bool finished = (static_cast<qint32>(stepCounter) == static_cast<qint32>(numberOfAnimationSteps));
    if (finished){
        //qDebug() << "Transition stopped after" << stepCounter << "steps. It lasted" << measureTimer.elapsed() << "ms. It was supposed to last" << animationDuration;
        timer.stop();
    }

    //qDebug() << "Emitting next step with finished as" << finished;

    emit SimpleAnimationController::nextStep(finished);
}
