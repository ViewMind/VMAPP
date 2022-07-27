#ifndef LINEARCOEFFICIENTS_H
#define LINEARCOEFFICIENTS_H

#include <QVariantMap>
#include <QString>

class LinearCoefficients
{
public:
    LinearCoefficients();
    LinearCoefficients(qreal m, qreal b);

    bool isValid() const;
    qreal getM() const;
    qreal getB() const;

    qreal predict(qreal x) const;

    QVariantMap toMap() const;
    QString toString() const;
    void fromMap(const QVariantMap &map);

private:
    qreal m;
    qreal b;
    bool valid;

};

#endif // LINEARCOEFFICIENTS_H
