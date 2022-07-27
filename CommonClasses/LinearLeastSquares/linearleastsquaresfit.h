#ifndef LINEARLEASTSQUARESFIT_H
#define LINEARLEASTSQUARESFIT_H

#include "linearcoefficients.h"

class LinearLeastSquaresFit
{
public:

    LinearLeastSquaresFit();

    void reset();

    void addInputAndTarget(qreal input_value, qreal target_value);

    LinearCoefficients linearFit();

    qreal R2() const;

    QList<qreal> getPredictedData() const;

private:
    QList<qreal> input;
    QList<qreal> target;
    QList<qreal> fittedData;
    qreal RSquared;

    LinearCoefficients computeLinearCoefficients();
    void computeFittedDataAndRSquared(const LinearCoefficients &lfit);


};

#endif // LINEARLEASTSQUARESFIT_H
