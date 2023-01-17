#ifndef EYETRACKERDATA_H
#define EYETRACKERDATA_H

#include <QtCore>

#define TOL_ZERO 1e-6

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

    QVector<qreal> leftVector() const;
    QVector<qreal> rightVector() const;

    qreal pr() const;
    qreal pl() const;

    void setTimeStamp(qint64 t);
    qint64 timestamp() const;

    bool operator==(const EyeTrackerData &d){
        return (d.time == time);
    }

    double avgX2D() const;
    double avgY2D() const;

    bool isRightZero(bool mode3D = false) const;
    bool isLeftZero(bool mode3D = false) const;

    QString toString(bool mode3D = false) const;

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
