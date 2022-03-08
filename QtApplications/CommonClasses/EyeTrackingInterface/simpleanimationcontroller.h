#ifndef SIMPLEANIMATIONCONTROLLER_H
#define SIMPLEANIMATIONCONTROLLER_H

#include <QTimer>
#include <QMap>
#include <QDebug>
#include <QElapsedTimer>

class SimpleAnimationController: public QObject
{
    Q_OBJECT
public:

    SimpleAnimationController();

    // Cleans all variables and configurations.
    void clear();

    // Adds an animation variable. This class will compute all the intermediate values for the variable name starting in startValue. When the number of animation steps
    // reaches the end the variable value will be stopValue.
    void addAnimationVariable(const QString &name, qreal startValue, qreal stopValue);

    // The duration of the animation will be determined by the target framerate and the number of animation steps.
    // There will be an error due to the error of the QTimer. The higher the framerate the worse the error.
    void setAnimatioDuration(qreal targetFramerate, qint32 animationSteps);

    // Starts the animation. Every time a new value for each variable should be used nextStep is emmited.
    void animationStart();

    // Should be called when next step is emitted.
    qreal getCurrentValueForVariable(const QString &name);

signals:
    void nextStep(bool finished);

private slots:
    void timerUp();

private:

    // A simple class to represet the start and stop value for the variable that is moving.
    typedef QPair<qreal,qreal> StartStopPoint;

    // Timer and variable values, deltas and references.
    QTimer timer;
    QMap<QString,StartStopPoint> animationVariables;
    QMap<QString,qreal> currentValues;
    QMap<QString,qreal> variableDeltas;

    // Animation variables.
    qreal stepCounter;
    qreal numberOfAnimationSteps;
    qreal animationDuration;
    qint32 timerTimeOut;

    // For measuring transition durations.
    QElapsedTimer measureTimer;

};

#endif // SIMPLEANIMATIONCONTROLLER_H
