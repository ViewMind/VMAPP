#ifndef TARGETTEST_H
#define TARGETTEST_H


#include <QPainter>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QImage>
#include <QDebug>


class CalibrationTargets
{
public:

    CalibrationTargets();
    ~CalibrationTargets();

    void initialize(qint32 screenw, qint32 screenh, bool useBorderTargetsAsCalibration = false);

    // WARNING: This function NEEDS to be called AFTER the calibration has been completed. It will set up the verification with a different number of points.
    void verificationInitialization(qint32 npoints);

    // On verification this function is used to verify if a data point falls within the boundaries of the currently shown target.
    quint8 isPointWithinCurrentTarget(qreal x, qreal y, qreal tolerance);

    // Required for storing calibration validation info.
    qreal getCalibrationTargetDiameter() const;

    // Required for storing calibration validation info.
    QVariantList getCalibrationTargetCorners() const;

    // Sets up calibration interation for 5 or 9 points. Any parameter other than 5 will assume 9 points.
    // Returns a list where the center of the points will be located. The list will have 5 or 9 points accordingly.
    QList<QPointF> setupCalibrationSequence(qint32 npoints = 5);

    // Iterates to the next target, after setting up the CalibrationSequence.
    QImage nextSingleTarget();

    // Gets a clear screen with nothing to show.
    QImage getClearScreen();

    // Renders the image of the current eye positions
    QImage renderCurrentPosition(qint32 rx, qint32 ry, qint32 lx, qint32 ly);

    // Enable or disable eye following
    void enableEyeFollowersDuringValidation(bool enable);

    // Returns if Gaze Following was enabled.
    bool isGazeFollowingEnabled() const;


private:

    const qreal K_LARGE_D = 0.1;
    const qreal K_SMALL_D = 0.02;

    const qreal K_CALIBRATION_LT = 0.20;
    const qreal K_CALIBRATION_MC = 0.50;
    const qreal K_CALIBRATION_RB = 0.80;

    QGraphicsScene *canvas;

    qreal R;
    qreal r;

    // These lists contains the upper left of the circles for all possible circles to either be drawn or shown as targets.
    QList<QPointF> borderTargets;
    QList<QPointF> calibrationTargets;

    // Depending on the number of points this will be a 9 or 5 point list.
    QList<qint32>  calibrationSequenceIndex;

    // Used as a bookmark for which calibration point we are drawing.
    qint32 indexInCalibrationSequence;

    // Variable used to separate between calibration and verification
    bool isVerification;

    // Enable Eye Tracking In Validation.
    bool enableEyeTrackingInValidation;

    QGraphicsEllipseItem *leftEyeTracker;
    QGraphicsEllipseItem *rightEyeTracker;


};

#endif // TARGETTEST_H
