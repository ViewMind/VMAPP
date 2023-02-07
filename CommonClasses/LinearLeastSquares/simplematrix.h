#ifndef SIMPLEMATRIX_H
#define SIMPLEMATRIX_H

#include <QString>
#include <QVector>
#include <QSize>
#include <QDebug>
#include <QElapsedTimer>

class SimpleMatrix
{
public:
    SimpleMatrix();

    typedef struct MatDim{
        qint32 nrows;
        qint32 ncols;
        QString str() const {
            return QString::number(nrows) + "x" + QString::number(ncols);
        }
        bool isEqual (const MatDim &dm){
            return ((dm.ncols == ncols) && (dm.nrows == nrows));
        }
    } MatDim;


    // Fills a matrix of 0s (or the value passeds as a parameter) with N rows of M columns.
    void clear(qint32 N, qint32 M, qreal value = 0);

    // Creates a matrix assuming the vector sizes are consistent.
    void fill(const QVector< QVector<qreal> > &data);

    // Creates a square identity matrix.
    void setIdentity(qint32 size);

    // Sets value in row i, column j to v
    void set(qint32 i, qint32 j, qreal v);

    // Increase by 1.
    void increseBy1(qint32 i , qint32 j);

    // Decrease by one.
    void decreaseBy1(qint32 i, qint32 j);

    // Gets value in row i, column j.
    qreal get(qint32 i, qint32 j) const;

    // Returns a pretty print representation of the matrix.
    QString prettyPrint() const;

    // Returns the matrix dimensions.
    MatDim getDim() const;

    // Basically returns true if clear was called and false otherwise.
    bool isValid() const;

    // Returns tru if the matrxi is a square matrix.
    bool isSquare() const;

    // Returns true if this a 1x1 matrix.
    bool isSizeOne() const;

    // Computes the determinant if the matrix is square. Convenient call to the recursive function.
    qreal getDeterminant() const;

    // Returs the matrix resultin in ommittin row i and column j.
    SimpleMatrix getCofactor(qint32 i, qint32 j) const;

    // Computes the transpose of the matrix in place.
    SimpleMatrix transpose() const;

    // Computes the adjoint of the matrix.
    SimpleMatrix adjoint() const;

    // Computes the inverse of the matrix.
    SimpleMatrix inverse() const;

    // Returns the raw data.
    QVector < QVector<qreal> > getData() const;

    // Basically creates a simple matrix form string. Simple way to load.
    static SimpleMatrix FromString(const QString &matrix);

    // Recursive computation of the determinant of a givne matrix.
    static qreal ComputeDeterminantRecursively(const SimpleMatrix &matrix);

    // Used for debugging. Checkes that the m data is coheren (all vectors are the same size) and they coincide with nrows and ncols.
    QString checkMatrixConsistency() const;

    // Basic operations.
    SimpleMatrix multiplyBy(const SimpleMatrix &b) const;
    SimpleMatrix add(const SimpleMatrix &b) const;
    SimpleMatrix substract(const SimpleMatrix &b) const;
    SimpleMatrix scaleBy(qreal value) const;

    // Creates a single vector by concatenating all rows in the matrix.
    SimpleMatrix flatten() const;

    // Sums all values of the matrix forcing each value as an int.
    qint32 sumForceInt() const;

    // Sums all values of the matrix.
    qreal sum() const;

    // Scales each element in the matrix in such a way that it's biggest value becomes 1 and it's smallest zero.
    // If the minimum and maximum value are the same, it does nothing.
    SimpleMatrix normalizeEachElement() const;

    // Sum of all elements divided by the number of the elements.
    qreal mean() const;


private:
    QVector< QVector<qreal> >m;

    bool isPostionValid(qint32 i, qint32 j) const;

    SimpleMatrix addOrSub(const SimpleMatrix &b, qreal mod) const;

};

#endif // SIMPLEMATRIX_H
