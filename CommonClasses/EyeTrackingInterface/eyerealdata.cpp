#include "eyerealdata.h"

EyeRealData::EyeRealData()
{

    xLeft = 0;
    yLeft = 0;
    xRight = 0;
    yRight = 0;
    zLeft = 0;
    zRight = 0;

}

QVector<qreal> EyeRealData::leftVector() const {
    QVector<qreal> v;
    v << xLeft;
    v << yLeft;
    v << zLeft;
    return v;
}

QVector<qreal> EyeRealData::rightVector() const {
    QVector<qreal> v;
    v << xRight;
    v << yRight;
    v << zRight;
    return v;
}


QString EyeRealData::toString(bool useFor3dData) const {
    if (useFor3dData)
       return "R: (" + QString::number(xRight) + ", " + QString::number(yRight) + ", " + QString::number(zRight) + ") L: ("  + QString::number(xLeft) + ", " + QString::number(yLeft) + ", " + QString::number(zLeft) +  ")";
    else
       return "R: (" + QString::number(xRight) + ", " + QString::number(yRight) + ") L: ("  + QString::number(xLeft) + ", " + QString::number(yLeft) + ")";
}


bool EyeRealData::validLeft(bool useFor3dData) const {
    if (qIsNaN(xLeft)) return false;
    if (qIsNaN(yLeft)) return false;
    if (useFor3dData) if (qIsNaN(zLeft)) return false;
    return true;
}


bool EyeRealData::validRight(bool useFor3dData) const {
    if (qIsNaN(xRight)) return false;
    if (qIsNaN(yRight)) return false;
    if (useFor3dData) if (qIsNaN(zRight)) return false;
    return true;
}

