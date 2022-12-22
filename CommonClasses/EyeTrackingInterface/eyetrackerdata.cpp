#include "eyetrackerdata.h"

EyeTrackerData::EyeTrackerData()
{
    xLeft = 0;
    yLeft = 0;
    xRight = 0;
    yLeft = 0;
    zLeft = 0;
    zRight = 0;
    pdLeft = 0;
    pdRight = 0;    
}


qreal EyeTrackerData::xr() const{
    return xRight;
}
qreal EyeTrackerData::yr() const{
    return yRight;
}
qreal EyeTrackerData::zr() const{
    return zRight;
}

qreal EyeTrackerData::xl() const{
    return xLeft;
}
qreal EyeTrackerData::yl() const{
    return yLeft;
}
qreal EyeTrackerData::zl() const{
    return zLeft;
}

qint32 EyeTrackerData::ixr() const{
    return static_cast<qint32>(xRight);
}
qint32 EyeTrackerData::iyr() const{
    return static_cast<qint32>(yRight);
}

qint32 EyeTrackerData::ixl() const{
    return static_cast<qint32>(xLeft);
}
qint32 EyeTrackerData::iyl() const{
    return static_cast<qint32>(yLeft);
}

void EyeTrackerData::setXR(qreal x){
    xRight = x;
}
void EyeTrackerData::setYR(qreal y){
    yRight = y;
}
void EyeTrackerData::setZR(qreal z){
    zRight = z;
}

void EyeTrackerData::setXL(qreal x){
    xLeft = x;
}
void EyeTrackerData::setYL(qreal y){
    yLeft = y;
}
void EyeTrackerData::setZL(qreal z){
    zLeft = z;
}

void EyeTrackerData::setPupilRight(qreal p){
    pdRight = p;
}
void EyeTrackerData::setPupilLeft(qreal p){
    pdLeft = p;
}

qreal EyeTrackerData::pr() const{
    return pdRight;
}
qreal EyeTrackerData::pl() const{
    return pdLeft;
}

void EyeTrackerData::setTimeStamp(qint64 t){
    time = t;
}
qint64 EyeTrackerData::timestamp() const{
    return time;
}

double EyeTrackerData::avgX2D() const{
    return (xRight + xLeft)/2.0;

}
double EyeTrackerData::avgY2D() const{
    return (yRight + yLeft)/2.0;
}

bool EyeTrackerData::isRightZero(bool mode3D) const{
    if (mode3D) return ((qAbs(xRight) < TOL_ZERO) && (qAbs(yRight) < TOL_ZERO) && (qAbs(zRight) < TOL_ZERO));
    else return ((qAbs(xRight) < TOL_ZERO) && (qAbs(yRight) < TOL_ZERO));
}
bool EyeTrackerData::isLeftZero(bool mode3D) const{
    if (mode3D) return ((qAbs(xLeft) < TOL_ZERO) && (qAbs(yLeft) < TOL_ZERO) && (qAbs(zLeft) < TOL_ZERO) );
    else return ((qAbs(xLeft) < TOL_ZERO) && (qAbs(yLeft) < TOL_ZERO));
}

QString EyeTrackerData::toString(bool mode3D) const{
    QString ans = "Time: " + QString::number(time);
    if (mode3D){
        ans = ans + ". LEFT (" + QString::number(xLeft) + "," + QString::number(yLeft) + "," + QString::number(zLeft) +  ") || ";
        ans = ans + ". RIGHT (" + QString::number(xRight) + "," + QString::number(yRight) + "," + QString::number(zRight) + ") || ";
        ans = ans + ". PD: " + QString::number(pdLeft) + " - " + QString::number(pdRight);
    }
    else {
        ans = ans + ". LEFT (" + QString::number(xLeft) + "," + QString::number(yLeft) + ") || ";
        ans = ans + ". RIGHT (" + QString::number(xRight) + "," + QString::number(yRight) + ") || ";
        ans = ans + ". PD: " + QString::number(pdLeft) + " - " + QString::number(pdRight);
    }

    return ans;
}
