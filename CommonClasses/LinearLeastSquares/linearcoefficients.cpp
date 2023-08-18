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

    mx = 0;
    my = 0;
    mz = 0;
    c  = 0;

}

void LinearCoefficients::clear(){
    m = 0;
    b = 0;
    mx = 0;
    my = 0;
    mz = 0;
    c  = 0;
    valid = false;
}

LinearCoefficients::LinearCoefficients()
{
    m = 0;
    b = 0;
    mx = 0;
    my = 0;
    mz = 0;
    c = 0;
    valid = false;
}

qreal LinearCoefficients::predict(qreal x) const{
    // qDebug() << "M" << m << "b" << b << "@" << x << m*x+b;
    return m*x + b;
}

qreal LinearCoefficients::predict(qreal x, qreal y, qreal z) const {
    return mx*x + my*y + mz*z + c;
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

bool LinearCoefficients::loadFromString(const QString &comma_separted_values){
    QStringList parts = comma_separted_values.split(",",Qt::SkipEmptyParts);
    if (parts.size() == 2){
        valid = true;
        m = parts.at(0).toDouble();
        b = parts.at(1).toDouble();
    }
    else if (parts.size() == 4){
        mx = parts.at(0).toDouble();
        my = parts.at(1).toDouble();
        mz = parts.at(2).toDouble();
        c  = parts.at(3).toDouble();
    }
    else return false;

    return true;
}

QVariantMap LinearCoefficients::toMap() const {
    QVariantMap map;
    map.insert("m",m);
    map.insert("b",b);
    map.insert("mx",mx);
    map.insert("my",my);
    map.insert("mz",mz);
    map.insert("c",c);
    map.insert("valid",valid);
    return map;
}

QString LinearCoefficients::toString(bool twoDString) const{
    if (twoDString){
        QString ans =  "M: " + QString::number(m) + ". B: " + QString::number(b);
        if (valid) ans = ans + ". VALID";
        else ans = ans + ". INVALID";
        return ans;
    }

    QString ans =  "MX: " + QString::number(mx) + ". MY: " + QString::number(my) + ". MZ: " + QString::number(mz) + ". C: " + QString::number(c);
    if (valid) ans = ans + ". VALID";
    else ans = ans + ". INVALID";
    return ans;


}

void LinearCoefficients::fromMap(const QVariantMap &map){
    m  = map.value("m").toDouble();
    b  = map.value("b").toDouble();
    mx = map.value("mx").toDouble();
    my = map.value("my").toDouble();
    mz = map.value("mz").toDouble();
    c  = map.value("c").toDouble();
    valid = map.value("valid").toBool();
}

void LinearCoefficients::fromSimpleMatrix(const SimpleMatrix &beta) {
    SimpleMatrix::MatDim md = beta.getDim();
    if ((md.nrows != 4) || (md.ncols != 1)) return;
    mx = beta.get(0,0);
    my = beta.get(1,0);
    mz = beta.get(2,0);
    c  = beta.get(3,0);
    valid = true;
}
