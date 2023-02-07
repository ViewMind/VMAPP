#include "simplematrix.h"

SimpleMatrix::SimpleMatrix()
{
    m.clear();
}

bool SimpleMatrix::isValid() const{
    return (m.size() > 0);
}

bool SimpleMatrix::isSquare() const {
    qsizetype nrows = m.size();
    qsizetype ncols = 0;
    if (nrows > 0) ncols = m.first().size();
    else return false;
    return (ncols == nrows);
}

bool SimpleMatrix::isSizeOne() const {
    qsizetype nrows = m.size();
    qsizetype ncols = 0;
    if (nrows > 0) ncols = m.first().size();
    else return false;
    bool ans = (ncols == nrows);
    return ans && (ncols == 1);
}

QVector < QVector<qreal> > SimpleMatrix::getData() const {
    return m;
}

bool SimpleMatrix::isPostionValid(qint32 i, qint32 j) const{

    if (!isValid()) return false;

    if (i < 0) return false;
    if (j < 0) return false;

    if (i < m.size()){
        if (j >= m.at(i).size()) return false;
        else return true;
    }
    else return false;
}

void SimpleMatrix::clear(qint32 N, qint32 M, qreal value){
    m.clear();

    if ((N <= 0) || (M <= 0)){
        return;
    }

    for (qint32 i = 0; i < N; i++){

        QVector<qreal> col;
        for (qint32 j = 0; j < M; j++){
            col << value;
        }

        m << col;
    }

}

void SimpleMatrix::fill(const QVector< QVector<qreal> > &data){

    bool isConsistent = true;
    bool foundNonZeroColumnSize = false;
    m.clear();
    qsizetype n = 0;

    for (qint32 i = 0; i < data.size(); i++){

        if (n == 0) {
            n = data.at(i).size();
            foundNonZeroColumnSize = (n != 0);
        }
        else if (n != data.at(i).size()){
            isConsistent = false;
            break;
        }
    }

    if ((isConsistent) && (foundNonZeroColumnSize)){
        m = data;
    }

}

void SimpleMatrix::setIdentity(qint32 size){
    m.clear();
    if (size <= 0) return;

    for (qint32 i = 0; i < size; i++){
        QVector<qreal> v;
        for (qint32 j = 0; j < size; j++){
            if (i == j) v << 1;
            else v << 0;
        }
        m << v;
    }

}

qreal SimpleMatrix::get(qint32 i, qint32 j) const{
    //qDebug() << "Position" << i << j << "of" << nrows << ncols;

    if (!isPostionValid(i,j)) {
        return qQNaN();
    }

    return m.at(i).at(j);
}

void SimpleMatrix::set(qint32 i, qint32 j, qreal v){
    if (!isPostionValid(i,j)) {
        return;
    }

    QVector<qreal> vec = m.at(i);
    vec[j] = v;
    m[i] = vec;
}

void SimpleMatrix::increseBy1(qint32 i, qint32 j){
    set(i,j,get(i,j)+1);
}

void SimpleMatrix::decreaseBy1(qint32 i, qint32 j){
    set(i,j,get(i,j)-1);
}

SimpleMatrix::MatDim SimpleMatrix::getDim() const {
    MatDim md;
    md.nrows = static_cast<qint32>(m.size());
    md.ncols = 0;
    if (md.nrows > 0){
        md.ncols = static_cast<qint32>(m.first().size());
    }
    return md;
}


QString SimpleMatrix::prettyPrint() const {

    if (!isValid()){
        return "INVALID MATRIX";
    }

    QList<QStringList> values;
    qint32 normalize_to = 0;

    for (qint32 i = 0; i < m.size(); i++){
        QStringList row;
        for (qint32 j = 0; j < m.at(i).size(); j++){
            QString text = QString::number(get(i,j));
            row << text;
            normalize_to = qMax(normalize_to,static_cast<qint32>(text.size()));
        }
        values << row;
    }

    normalize_to = normalize_to + 2;
    QString ans = "";

    for (qint32 i = 0; i < m.size(); i++){
        for (qint32 j = 0; j < m.at(i).size(); j++){
            QString text = values.at(i).at(j);
            while (text.size() < normalize_to) text = text + " ";
            ans = ans + text;
        }
        ans = ans + "\n";
    }

    return ans;
}

SimpleMatrix SimpleMatrix::getCofactor(qint32 i, qint32 j) const{

    if (!isPostionValid(i,j)) return SimpleMatrix();

    QVector< QVector<qreal> > new_m;

    for (qint32 r = 0; r < m.size(); r++){
        QVector <qreal> row;

        if (r == i) continue;
        for (qint32 c = 0; c < m.at(r).size(); c++){

            if (c == j) continue;
            row << get(r,c);
        }
        new_m << row;
    }

    //qDebug() << new_m;

    SimpleMatrix sm;
    sm.fill(new_m);
    return sm;

}

qreal SimpleMatrix::getDeterminant() const{
    if (!isValid()) return qQNaN();
    if (!isSquare()) return qQNaN();
    SimpleMatrix temp; temp.fill(m);
    return ComputeDeterminantRecursively(temp);
}

SimpleMatrix SimpleMatrix::transpose() const {

    QVector< QVector<qreal> > t;

    qint32 ncols = 0;
    if (m.size() > 0){
        ncols = static_cast<qint32>(m.first().size());
    }
    else return SimpleMatrix();

    for (qint32 j = 0; j < ncols; j++){
        QVector<qreal> vec;
        for (qint32 i = 0; i < m.size(); i++){
            //vec << m.at(i).at(j);
            vec << get(i,j);
        }
        t << vec;
    }

    SimpleMatrix sm; sm.fill(t);
    return sm;

}

SimpleMatrix SimpleMatrix::adjoint() const {

    if (!isSquare()) return SimpleMatrix();

    SimpleMatrix sm;
    MatDim md = getDim();
    sm.clear(md.nrows,md.ncols);

    if (isSizeOne()){
        sm.set(0,0,1);
        return sm;
    }


    qreal sign = 1;

    for (qint32 i = 0; i < m.size(); i++){
        for (qint32 j = 0; j < m.at(i).size(); j++){

            // Get the cofactor of i,j
            SimpleMatrix cofactor = getCofactor(i,j);

            // Sign of the adjunct of i,j is positive when the index sum of the row and column is even.
            if (((i+j) % 2) == 0){
                sign = 1;
            }
            else {
                sign = -1;
            }

            // We interchange the rows and columns to get the transpose of the cofactor matrix.
            sm.set(j,i,sign * cofactor.getDeterminant());

        }
    }

    return sm;

}

QString SimpleMatrix::checkMatrixConsistency() const {
    qint32 n = 0;
    for (qint32 i = 0; i < m.size(); i++){
        qint32 temp = static_cast<qint32>(m.at(i).size());
        if (n == 0) n = temp;
        else if (n != temp) return "BAD MATRIX: Row " + QString::number(i) + " has " + QString::number(temp) + " columns, But the matrix should have " + QString::number(n);
    }

    return "OK";
}

SimpleMatrix SimpleMatrix::inverse() const {

    if (!isSquare()) return SimpleMatrix();

    qreal det = getDeterminant();

    // Using a fuzzy comparsion to determine if it's zero.
    if (qAbs(det) < 0.0001){
        // Zero determinant.
        return SimpleMatrix();
    }

    // Getting the adjunct.
    SimpleMatrix adjunct = adjoint();

    // Dividing by the determinat.
    return adjunct.scaleBy(1.0/det);

}

SimpleMatrix SimpleMatrix::multiplyBy(const SimpleMatrix &b) const{

    SimpleMatrix result;
    // Size compatibility comparison.
    MatDim size_a = getDim();
    MatDim size_b = b.getDim();

    if (size_a.ncols != size_b.nrows) return result;
    qint32 M = size_a.ncols;

    result.clear(size_a.nrows,size_b.ncols);

    MatDim r_size = result.getDim();

    //qDebug() << "Mult Value M is" << M << "Result size is " << r_size.str() << "Left side is" << size_a.str() << "Right side is" << size_b.str();
    //qDebug() << "i va entre 0 y " << r_size.nrows << "j va entre 0 y" << r_size.ncols << "k va a ir entre 0 y" << M;
    //qDebug().noquote() << "A\n" << this->prettyPrint() << "\nB\n" << b.prettyPrint();

    //QElapsedTimer mtimer;
    //mtimer.start();

    for (qint32 i = 0; i < r_size.nrows; i++){

        for (qint32 j = 0; j < r_size.ncols; j++){

            qreal v = 0;
            for (qint32 k = 0; k < M; k++){

                //v = v + get(i,k)*b.get(k,j);
                v = v + m.at(i).at(k)*b.getData().at(k).at(j);
            }

            result.set(i,j,v);

        }
    }

    //qDebug() << "Matrix multiplication of " << r_size.nrows << r_size.ncols << " and internal loop of " << M << "took" << mtimer.elapsed() << "ms";

    //qDebug() << "Returning result";

    return result;


}

SimpleMatrix SimpleMatrix::add(const SimpleMatrix &b) const{
    return addOrSub(b,1);
}

SimpleMatrix SimpleMatrix::substract(const SimpleMatrix &b) const{
    return addOrSub(b,-1);
}

SimpleMatrix SimpleMatrix::scaleBy(qreal value) const {

    SimpleMatrix sm;
    MatDim md = getDim();
    sm.clear(md.nrows,md.ncols);

    for (qint32 i = 0; i < m.size(); i++){
        for (qint32 j = 0; j < m.at(i).size(); j++){
            sm.set(i,j,get(i,j)*value);
        }
    }

    return sm;

}

SimpleMatrix SimpleMatrix::addOrSub(const SimpleMatrix &b, qreal mod) const{

    if (getDim().isEqual(b.getDim())) return SimpleMatrix();

    SimpleMatrix sm;
    MatDim md = getDim();
    sm.clear(md.nrows,md.ncols);

    for (qint32 i = 0; i < m.size(); i++){
        for (qint32 j = 0; j < m.at(i).size(); j++){
            sm.set(i,j,get(i,j) + mod*b.get(i,j));
        }
    }

    return sm;

}

qreal SimpleMatrix::ComputeDeterminantRecursively(const SimpleMatrix &matrix){

    if (!matrix.isValid()) return qQNaN();
    if (!matrix.isSquare()) return qQNaN();
    if (matrix.isSizeOne()){
        return matrix.get(0,0);
    }

    qint32 ncols = matrix.getDim().ncols;

    // If we got here we need to do computations.
    qreal D = 0;

    // To keep track of the sign.
    qreal sign = 1;

    // To compute the determinant we always use the first row.
    for (qint32 j = 0; j < ncols; j++){
        SimpleMatrix sm = matrix.getCofactor(0,j);
        D = D + sign*matrix.get(0,j)*ComputeDeterminantRecursively(sm);
        sign = -sign; // Sign needs to be alternating
    }

    return D;

}

SimpleMatrix SimpleMatrix::FromString(const QString &matrix) {

    SimpleMatrix m;

    QVector < QVector<qreal> > values;

    QStringList lines = matrix.split("\n",Qt::SkipEmptyParts);
    //qDebug() << "Splitting lines" << lines;
    qint32 ncols = 0;

    for (qint32 i = 0; i < lines.size(); i++){
        QStringList tokens = lines.at(i).split(" ",Qt::SkipEmptyParts);
        //qDebug() << "Analizing tokens" << tokens;
        QList<qreal> row;
        for (qint32 j = 0; j < tokens.size(); j++){
            QString token = tokens.at(j);
            token = token.trimmed();
            if (token == "") continue;
            bool ok = false;
            qreal v = token.toDouble(&ok);
            if (!ok) continue;
            row << v;
        }

        //qDebug() << "Resulting row" << row << "ncols" << ncols;

        if (ncols == 0) ncols = static_cast<qint32>(row.size());
        // Error.
        else if (ncols != row.size()) return m;

        values << row;
    }


    m.fill(values);

    return m;

}

qreal SimpleMatrix::sum() const{
    qreal s = 0;
    for (qint32 i = 0; i < m.size(); i++){
        for (qint32 j = 0; j < m.at(i).size(); j++){
            s = s + m.at(i).at(j);
        }
    }
    return s;
}

qint32 SimpleMatrix::sumForceInt() const{
    qint32 s = 0;
    for (qint32 i = 0; i < m.size(); i++){
        for (qint32 j = 0; j < m.at(i).size(); j++){
            s = s + static_cast<qint32>(m.at(i).at(j));
        }
    }
    return s;
}

SimpleMatrix SimpleMatrix::flatten() const{
    SimpleMatrix ans;
    QVector< QVector<qreal> > temp_data;
    QVector<qreal> long_row;
    for (qint32 i = 0; i < m.size(); i++){
        long_row.append(m.at(i));
    }
    temp_data << long_row;
    ans.fill(temp_data);
    return ans;
}

SimpleMatrix SimpleMatrix::normalizeEachElement() const {
    SimpleMatrix ans;
    ans.clear(this->getDim().nrows,this->getDim().ncols,0);

    qreal min = m.first().first();
    qreal max = m.first().first();

    for (qint32 i = 0; i < m.size(); i++){
        for (qint32 j = 0; j < m.at(i).size(); j++){
            max = qMax(m.at(i).at(j),max);
            min = qMin(m.at(i).at(j),min);
        }
    }

    if (qAbs(max-min) < 0.00001){
        ans.fill(m);
        return ans;
    }

    for (qint32 i = 0; i < m.size(); i++){
        for (qint32 j = 0; j < m.at(i).size(); j++){
            ans.set(i,j,(get(i,j)-min)/(max-min));
        }
    }

    return ans;
}

qreal SimpleMatrix::mean() const {
    qreal n = static_cast<qreal>(getDim().nrows);
    n = n * static_cast<qreal>(getDim().ncols);
    if (n < 1) return 0; // empty matrix.
    return sum()/n;
}
