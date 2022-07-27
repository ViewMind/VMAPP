#include "linearcoefficients.h"

LinearCoefficients::LinearCoefficients(qreal mm, qreal bb)
{
    m = 0;
    b = 0;
    valid = false;

    // When creating the coefficients we check if they are valid or nto.
    if (qIsNaN(mm) || qIsNaN(bb)){
        return;
    }

    m = mm;
    b = bb;
    valid = true;

}

LinearCoefficients::LinearCoefficients()
{
    m = 0;
    b = 0;
    valid = false;
}

qreal LinearCoefficients::predict(qreal x) const{
    // qDebug() << "M" << m << "b" << b << "@" << x << m*x+b;
    return m*x + b;
}

qreal LinearCoefficients::getM() const {
    return m;
}

qreal LinearCoefficients::getB() const {
    return b;
}

bool LinearCoefficients::isValid() const {
    return valid;
}

QVariantMap LinearCoefficients::toMap() const {
    QVariantMap map;
    map.insert("m",m);
    map.insert("b",b);
    map.insert("valid",valid);
    return map;
}

QString LinearCoefficients::toString() const{
    QString ans =  "M: " + QString::number(m) + ". B: " + QString::number(b);
    if (valid) ans = ans + ". VALID";
    else ans = ans + ". INVALID";
    return ans;
}

void LinearCoefficients::fromMap(const QVariantMap &map){
    m = map.value("m").toDouble();
    b = map.value("b").toDouble();
    valid = map.value("valid").toBool();
}
