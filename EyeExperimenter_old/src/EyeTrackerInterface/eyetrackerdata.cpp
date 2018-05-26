#include "eyetrackerdata.h"

EyeTrackerData::EyeTrackerData()
{

}


QString EyeTrackerData::toString() const{
    QString ans = "Time: " + QString::number(time);
    ans = ans + ". LEFT (" + QString::number(xLeft) + "," + QString::number(yLeft) + ") || ";
    ans = ans + ". RIGHT (" + QString::number(xRight) + "," + QString::number(xRight) + ") || ";
    ans = ans + ". PD: " + QString::number(pdLeft) + "," + QString::number(pdRight);
    return ans;
}
