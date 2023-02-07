#ifndef BALLPATHGENERATOR_H
#define BALLPATHGENERATOR_H

#include <QList>
#include <QDebug>
#include <QString>
#include <QPair>
#include <QRandomGenerator>
#include "../LinearLeastSquares/simplematrix.h"

class BallPathGenerator
{
public:
    BallPathGenerator();

    static QList< QList<qint32> > GenerateBallPathSequence(qint32 rows, qint32 cols, qint32 times_per_hole, qint32 n_of_passes, qreal min_distance, qreal mean_distance);

private:

    typedef struct RowColIndex {
        qint32 r = -1;
        qint32 c = -1;
        qint32 i = -1;
        qint32 getIndex(const qint32 &rowCount) const{
            return r*rowCount + c;
        }
        QString toString() const {
            return "(" + QString::number(r) + ", " + QString::number(c) + ") - [" + QString::number(i) + "]";
        }
    } RowColIndex;

    static QList<RowColIndex> GenerateSequenceConditioned(const SimpleMatrix &input_counts,
                                                          const QList<RowColIndex> &sequence,
                                                          qreal min_distance,
                                                          qreal target_mean,
                                                          qreal eps = 0.1);

    /**
     * @brief GenerateSequence1D - Generates a sequence of randomly selected (r,c) pairs so that each cell in the matrix is selected the number of time in inputs counts.
     * @param input_counts - A NxM input matrix of counts.
     * @return - Returns an array of (r,c) indexes pairs. It's length equal to the sum of the input counts
     */

    static QList<RowColIndex> GenerateSequence( const SimpleMatrix &input_counts );

    /**
     * @brief choice - Selects an row and column where probability sum is lower than random probability.
     * @param input - array of probabilities between 0 and 1 of size NxM
     * @details Flattens the input into an array of NxM. The calls choice for a 1D array.
     * Then transforms the index back into a row and column index. And returns that.
     * @return Row and Column of the selected probability.
     */

    static RowColIndex Choice(const SimpleMatrix &probs);

    /**
     * @brief choice - Selects an index where probability sum is lower than random probability.
     * @param input - matrix of probabilities between 0 and 1 of size 1xL
     * @details The input is an array of probabilities. The computes an array of L, where position i is the sum of proabilities from 0 to i.
     * it then select an index from the arry where the probability is less than anohter randomly selected probability between 0 and 1.
     * The index is reuturned in the row index
     * the col index will be -1.
     * @return Row index with the slected index and col index is -1.
     */
    static RowColIndex Choice1D(const SimpleMatrix &probs);


    /**
     * @brief meanOnList - Computes the mean on the input list of numbers.
     * @param list - List of real numbers
     * @return The mean.
     */
    static qreal MeanOnList(const QList<qreal> &list);



};

#endif // BALLPATHGENERATOR_H
