#ifndef EYETRACKERDATA_H
#define EYETRACKERDATA_H

#include <QtCore>

struct EyeTrackerCalibrationParameters{
    QString name;
    bool forceCalibration;
    qint32 number_of_calibration_points;
};

class EyeTrackerData
{
public:


    typedef enum {TU_MS, TU_US} TimeUnit;

    // The Eyetracker data should have the pixel being looked at by the left and right eyes and the pupil diameter.
    // The values dimension depend on the what the ET Provides.

    EyeTrackerData();
    qint32 xRight;
    qint32 yRight;
    qint32 xLeft;
    qint32 yLeft;
    qreal pdLeft;
    qreal pdRight;
    qint64 time;
    TimeUnit timeUnit;

    bool operator==(const EyeTrackerData &d){
        return (d.time == time);
    }

    double avgX() const {return ((qreal)xRight + (qreal)xLeft)/2;}
    double avgY() const {return ((qreal)yRight + (qreal)yLeft)/2;}

    bool isRightZero() const {return ((xRight == 0) && (yRight == 0));}
    bool isLeftZero() const {return ((xLeft == 0) && (yLeft == 0));}

    QString toString() const;

private:


};

#endif // EYETRACKERDATA_H
