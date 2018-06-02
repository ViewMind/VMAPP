#ifndef EYETRACKERDATA_H
#define EYETRACKERDATA_H

#include <QtCore>

struct EyeTrackerCalibrationParameters{
    QString name;
    bool forceCalibration;
};

class EyeTrackerData
{
public:

    // The Eyetracker data should have the pixel being looked at by the left and right eyes and the pupil diameter in mm.

    EyeTrackerData();
    double xRight;
    double yRight;
    double xLeft;
    double yLeft;
    double pdLeft;
    double pdRight;
    qint64 time;

    bool operator==(const EyeTrackerData &d){
        return (d.time == time);
    }

    double avgX() const {return (xRight + xLeft)/2;}
    double avgY() const {return (yRight + yLeft)/2;}

    bool isRightZero() const {return ((xRight == 0) && (yRight == 0));}
    bool isLeftZero() const {return ((xLeft == 0) && (yLeft == 0));}

    QString toString() const;

};

#endif // EYETRACKERDATA_H
