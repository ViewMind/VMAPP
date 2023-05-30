#ifndef TARGETTEST_H
#define TARGETTEST_H

#include <QColor>
#include <QDebug>
#include "../RenderServerClient/RenderServerGraphics/animationmanager.h"
#include "../RenderServerClient/RenderServerGraphics/renderservercircleitem.h"
#include "../RenderServerClient/RenderServerGraphics/renderserverscene.h"

class CalibrationTargets: public QObject
{
    Q_OBJECT

public:

    CalibrationTargets();
    ~CalibrationTargets() override;

    void initialize(qint32 screenw, qint32 screenh, bool useBorderTargetsAsCalibration = false);

        // Required for storing calibration validation info.
    qreal getCalibrationTargetDiameter() const;

    // Required for storing calibration validation info.
    QVariantList getCalibrationTargetCorners() const;

    // Sets up calibration interation for 5 or 9 points. Any parameter other than 5 will assume 9 points.
    // Returns a list where the center of the points will be located. The list will have 5 or 9 points accordingly.
    QList<QPointF> setupCalibrationSequence(qint32 npoints, qint32 hold_time_for_targets);

    // Starts iterating over all targets.
    void calibrationAnimationControl(bool start);

    // The current image.
    RenderServerScene  getCurrentCalibrationAnimationFrame() const;

    qreal getScreenRatioForLeftTop() const { return K_CALIBRATION_LT; }

    // Will return the information necessary for rendering the wait screen targets.
    // This includes the 4 corner target centers , the circle radious, and it's color.
    // Inout pointers are used due to the varied nature of the information.
    // The function requires calling initialize first.
    void getWaitScreenInfo(qreal *RR, qreal *rr, QColor *outerCircleColor, QList<QPointF> *outsideCenters);


signals:
    void newAnimationFrame();
    void calibrationPointStatus(qint32 whichCalibrationPoint, bool isMoving);

private slots:
    void onUpdateAnimation();
    void onReachedAnimationStop(const QString &variable, qint32 animationStopIndex, qint32 animationSignalType);

private:

    const qreal K_LARGE_D = 0.1;
    const qreal K_SMALL_D = 0.02;

    const qreal K_CALIBRATION_LT = 0.30;
    const qreal K_CALIBRATION_MC = 0.50;
    const qreal K_CALIBRATION_RB = 0.70;

//    const qreal K_CALIBRATION_LT = 0.20;
//    const qreal K_CALIBRATION_MC = 0.50;
//    const qreal K_CALIBRATION_RB = 0.80;

    // Animation constants. (times are in milliseconds).
    const qint32 ANIMATION_OUTTER_CIRCLE_PULSATING_R      = 800;
    const qint32 ANIMATION_MOVE_TIME                      = 600;
    const qint32 ANIMATION_HOLD_TIME_INTIAL_CENTER_TARGET = 500;

    //const QColor COLOR_OUTSIDE_CIRCLE_CALIBRATION  = QColor(129,178,210);
    const QColor COLOR_OUTSIDE_CIRCLE_CALIBRATION  = QColor(138,22,25);
    const QColor COLOR_VALIDATION_TARGET_NOT_HIT   = QColor(255,255,255);
    const QColor COLOR_VALIDATION_TARGET_HIT_LEFT  = QColor(0,0,255);
    const QColor COLOR_VALIDATION_TARGET_HIT_RIGHT = QColor(0,255,0);
    const QColor COLOR_VALIDATION_TARGET_HIT_BOTH  = QColor(0,255,255);
    const QColor COLOR_LEFT_EYE_TRACKER            = QColor(0,0,255,100);
    const QColor COLOR_RIGHT_EYE_TRACKER           = QColor(0,255,0,100);

    //QGraphicsScene *canvas;
    RenderServerScene canvas;
    AnimationManager animator;
    QString expectedAnimationSignalName;
    qint32 calibrationCounter;

    qreal R;
    qreal r;

    // These lists contains the upper left of the circles for all possible circles to either be drawn or shown as targets.
    QList<QPointF> borderTargets;
    QList<QPointF> calibrationTargets;

    // Depending on the number of points this will be a 9 or 5 point list.
    QList<qint32>  calibrationSequenceIndex;

    // Used as a bookmark for which calibration point we are drawing.
    qint32 indexInCalibrationSequence;

};

#endif // TARGETTEST_H
