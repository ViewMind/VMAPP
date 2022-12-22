#ifndef LINEARCOEFFICIENTS_H
#define LINEARCOEFFICIENTS_H

#include <QVariantMap>
#include <QString>

#include "simplematrix.h"

class LinearCoefficients
{
public:
    LinearCoefficients();
    LinearCoefficients(qreal m, qreal b);

    bool isValid() const;
    qreal getM() const;
    qreal getB() const;

    qreal predict(qreal x) const;
    qreal predict(qreal x, qreal y, qreal z) const;

    QVariantMap toMap() const;
    QString toString(bool twoDString = true) const;
    void fromMap(const QVariantMap &map);

    // Assumes a 4x1 Column Matrix.
    void fromSimpleMatrix(const SimpleMatrix &beta);

private:
    qreal m;
    qreal b;

    qreal mx;
    qreal my;
    qreal mz;
    qreal c;

    bool valid;

};

#endif // LINEARCOEFFICIENTS_H
