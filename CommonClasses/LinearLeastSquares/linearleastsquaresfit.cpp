#include "linearleastsquaresfit.h"

LinearLeastSquaresFit::LinearLeastSquaresFit(){
    RSquared = 0;
}

void LinearLeastSquaresFit::reset(){
    input.clear();
    target.clear();
    fittedData.clear();
    RSquared = 0;
}

qreal LinearLeastSquaresFit::R2() const {
    return RSquared;
}

void LinearLeastSquaresFit::addInputAndTarget(qreal input_value, qreal target_value){
    input << input_value;
    target << target_value;
}

QList<qreal> LinearLeastSquaresFit::getPredictedData() const {
    return fittedData;
}

LinearCoefficients LinearLeastSquaresFit::linearFit() {
    LinearCoefficients lc = this->computeLinearCoefficients();
    this->computeFittedDataAndRSquared(lc);
    return lc;
}


LinearCoefficients LinearLeastSquaresFit::computeLinearCoefficients(){

    // Computing the matrix coefficients
    qreal m11 = 0;
    qreal m12 = 0;
    qreal m21 = 0;
    qreal m22 = 0;

    // Target value vector
    qreal t1 =0;
    qreal t2 = 0;
    for (int i = 0; i < input.size(); i++){
        if (qIsNaN(input.at(i))){ // in case I get NaN values.
            continue;
        }
        m12 = m12 + input.at(i);
        m22 = m22 + input.at(i)*input.at(i);
        t1 = t1 + target.at(i);
        t2 = t2 + target.at(i)*input.at(i);
        m11++;
    }
    m21 = m12;

    // Calculating the inverse
    qreal det = m22*m11 - m12*m21;
    if (det == 0.0) {
        return LinearCoefficients();
    }

    qreal im11 = m22/det;
    qreal im12 = -m12/det;
    qreal im21 = -m21/det;
    qreal im22 = m11/det;

    // Target value vector
    qreal b = im11*t1 + im12*t2;
    qreal m = im21*t1 + im22*t2;

    LinearCoefficients lc (m,b);
    return lc;

}

void LinearLeastSquaresFit::computeFittedDataAndRSquared(const LinearCoefficients &lfit){

    fittedData.clear();

    qsizetype N = input.size();
    qreal target_median = 0;
    qreal residual_sum_of_squares = 0;

    for (qint32 i = 0; i < N; i++){
        qreal f = lfit.predict(input.at(i));
        fittedData << f;
        qreal e = target.at(i) - f;
        residual_sum_of_squares = residual_sum_of_squares + e*e;
        target_median = target_median + target.at(i);
    }

    target_median = target_median/static_cast<qreal>(N);

    // Now we need to compute the total sum of the squares.
    qreal total_sum_of_squares = 0;
    for (qint32 i = 0; i < N; i++){
        qreal d = target.at(i) - target_median;
        total_sum_of_squares = total_sum_of_squares + d*d;
    }

    //qDebug() << "Target Median" << target_median << "RSS and TSS" << residual_sum_of_squares << total_sum_of_squares;

    RSquared = 1 - residual_sum_of_squares/total_sum_of_squares;

}
