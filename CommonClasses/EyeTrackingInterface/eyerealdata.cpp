#include "eyerealdata.h"

EyeRealData::EyeRealData()
{
    xLeft = 0;
    yLeft = 0;
    xRight = 0;
    yRight = 0;
}


QString EyeRealData::toString() const {
    return "R: (" + QString::number(xRight) + ", " + QString::number(yRight) + ") L: ("  + QString::number(xLeft) + ", " + QString::number(yLeft) + ")";
}


bool EyeRealData::validLeft() const {
    if (qIsNaN(xLeft)) return false;
    if (qIsNaN(yLeft)) return false;
    return true;
}


bool EyeRealData::validRight() const {
    if (qIsNaN(xRight)) return false;
    if (qIsNaN(yRight)) return false;
    return true;
}

