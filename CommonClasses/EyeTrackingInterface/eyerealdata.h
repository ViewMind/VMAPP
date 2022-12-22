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
    qreal zLeft;
    qreal zRight;

    QVector<qreal> leftVector() const;
    QVector<qreal> rightVector() const;

    QString toString(bool useFor3dData) const;
    bool validRight(bool useFor3dData) const;
    bool validLeft(bool useFor3dData) const;

private:
    bool useFor3dData;

};

#endif // EYEREALDATA_H
