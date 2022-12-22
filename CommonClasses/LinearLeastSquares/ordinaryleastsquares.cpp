#include "ordinaryleastsquares.h"

OrdinaryLeastSquares::OrdinaryLeastSquares()
{

}

void OrdinaryLeastSquares::addSample(QVector<qreal> sample, qreal target){
    samples << sample;
    targets << target;
}

void OrdinaryLeastSquares::reset() {
    samples.clear();
    targets.clear();
}

QString OrdinaryLeastSquares::getError() const {
    return error;
}

SimpleMatrix OrdinaryLeastSquares::getCoefficients() const {
    return Beta;
}

qreal OrdinaryLeastSquares::getRSquared() const {
    return Rsquared;
}

bool OrdinaryLeastSquares::computeCoefficients() {

    SimpleMatrix X;
    X.fill(samples);

    // The number of observations.
    qint32 n = static_cast<qint32>(samples.size());

    //qDebug() << "X Created. Size" << X.getDim().str() << X.checkMatrixConsistency();

    if (!X.isValid()){
        error = "Could not gerate the X Matrix. This can only be due to inconsistent sample vector size";
        return false;
    }

    // The y vector of the reference results.
    SimpleMatrix y;
    QVector< QVector<qreal> > row_matrix;
    row_matrix << targets;
    y.fill(row_matrix);
    y = y.transpose();

    //qDebug() << "Created target transpose. Size" << y.getDim().str() << y.checkMatrixConsistency();

    SimpleMatrix Xt = X.transpose();
    if (!Xt.isValid()){
        error = "Unable to compute Xt.";
        return false;
    }

    //qDebug() << "Created Xt. Size" << Xt.getDim().str() << Xt.checkMatrixConsistency();

    SimpleMatrix XtX = Xt.multiplyBy(X);
    if (!XtX.isValid()){
        error = "Unable to compute XtX.";
        return false;
    }

    //qDebug() << "Created XtX. Size" << XtX.getDim().str() << XtX.checkMatrixConsistency();

    SimpleMatrix iXtX = XtX.inverse();

    //qDebug() << "Created iXtX. Size" << iXtX.getDim().str() << iXtX.checkMatrixConsistency();

    if (!iXtX.isValid()){
        error = "Unable to compute the inverse of the Matrix Xt . X";
        return false;
    }

    SimpleMatrix fX = iXtX.multiplyBy(Xt);

    if (!fX.isValid()){
        error = "Unable to compute inv(Xt*X)*Xt.";
        return false;
    }

    //qDebug() << "Final Matrix Created. Size: " << fX.getDim().str();

    Beta = fX.multiplyBy(y);

    //qDebug() << "Resulting Coefficient Size: " << Beta.getDim().str();

    if (!Beta.isValid()){
        error = "Invalid coeffient matrix computed";
        return false;
    }

    /// We now compute the R^2 coefficient.
    SimpleMatrix yhat = X.multiplyBy(Beta);

    // We now just get the predictions as a simple list
    yhat = yhat.transpose();
    QVector<qreal> ypred = yhat.getData().first(); // A list of the predicted vlaues.

    if (ypred.size() != targets.size()){
        error = "Something went wrong. The size fo the the predictor list is " + QString::number(ypred.size()) + " but the size of the target list is " + QString::number(targets.size());
        return false;
    }

    // We now compute the average of the targets.
    qreal nt = static_cast<qreal>(targets.size());
    qreal ydash = 0;
    for (qint32 i = 0; i < targets.size(); i++){
        ydash = ydash + targets.at(i);
    }
    ydash = ydash/nt;

    qreal numerator    = 0;
    qreal denominator = 0;

    for (qint32 i = 0; i < targets.size(); i++){
        qreal a = ypred.at(i) - ydash;
        qreal b = targets.at(i) - ydash;
        numerator = numerator + a*a;
        denominator = denominator + b*b;
    }

    Rsquared = numerator/denominator;

    return true;

}
