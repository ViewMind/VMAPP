#ifndef ORDINARYLEASTSQUARES_H
#define ORDINARYLEASTSQUARES_H

/**
 * @brief The OrdinaryLeastSquares class
 * This is the complete form of oridiary least squares that computes the result using matrix operations.
 */

#include "simplematrix.h"

class OrdinaryLeastSquares
{
public:

    OrdinaryLeastSquares();

    void reset();

    // This is the append function. Basically for a given sample of the variables the target is the expected resutl.
    void addSample(QVector<qreal> sample, qreal target);

    QString getError() const;

    bool computeCoefficients();

    SimpleMatrix getCoefficients() const;

    qreal getRSquared() const;

private:

    QString error;
    QVector< QVector<qreal> > samples;
    QVector<qreal> targets;
    SimpleMatrix Beta;
    qreal Rsquared;

};



#endif // ORDINARYLEASTSQUARES_H
