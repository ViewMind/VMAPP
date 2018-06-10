#include "eyetrackerdata.h"

EyeTrackerData::EyeTrackerData()
{
    xLeft = 0;
    yLeft = 0;
    xRight = 0;
    yLeft = 0;
    pdLeft = 0;
    pdRight = 0;
    timeUnit = TU_US; // By default time unit is a microsecond.
}


QString EyeTrackerData::toString() const{
    QString ans = "Time: " + QString::number(time);
    ans = ans + ". LEFT (" + QString::number(xLeft) + "," + QString::number(yLeft) + ") || ";
    ans = ans + ". RIGHT (" + QString::number(xRight) + "," + QString::number(xRight) + ") || ";
    ans = ans + ". PD: " + QString::number(pdLeft) + "," + QString::number(pdRight);
    return ans;
}
