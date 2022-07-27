#ifndef EYEREALDATA_H
#define EYEREALDATA_H

#include <QString>
#include <QDebug>

class EyeRealData
{
public:
    EyeRealData();

    qreal xLeft;
    qreal xRight;
    qreal yLeft;
    qreal yRight;

    QString toString() const;
    bool validRight() const;
    bool validLeft() const;

};

#endif // EYEREALDATA_H
