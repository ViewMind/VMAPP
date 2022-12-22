#ifndef EYETRACKERDATA_H
#define EYETRACKERDATA_H

#include <QtCore>

#define TOL_ZERO 1e-6

struct EyeTrackerCalibrationParameters{
    QString name;
    bool forceCalibration;
    bool mode3D;
    qint32 number_of_calibration_points;
    qint32 gather_time;
    qint32 wait_time;
};

class EyeTrackerData
{
public:

    EyeTrackerData();

    qreal xr() const;
    qreal yr() const;
    qreal zr() const;

    qreal xl() const;
    qreal yl() const;
    qreal zl() const;

    qint32 ixr() const;
    qint32 iyr() const;

    qint32 ixl() const;
    qint32 iyl() const;

    void setXR(qreal x);
    void setYR(qreal y);
    void setZR(qreal z);

    void setXL(qreal x);
    void setYL(qreal y);
    void setZL(qreal z);

    void setPupilRight(qreal p);
    void setPupilLeft(qreal p);

    qreal pr() const;
    qreal pl() const;

    void setTimeStamp(qint64 t);
    qint64 timestamp() const;

    bool operator==(const EyeTrackerData &d){
        return (d.time == time);
    }

    double avgX2D() const;
    double avgY2D() const;

    bool isRightZero(bool mode3D) const;
    bool isLeftZero(bool mode3D) const;

    QString toString(bool mode3D) const;

private:

    qreal xRight;
    qreal yRight;
    qreal xLeft;
    qreal yLeft;
    qreal zRight;
    qreal zLeft;
    qreal pdLeft;
    qreal pdRight;
    qint64 time;

};

#endif // EYETRACKERDATA_H
