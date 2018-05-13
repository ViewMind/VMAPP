#ifndef DATASET_H
#define DATASET_H

/*********************************************
 * All experiments are represented in one way
 * or another as trials. The data set is the
 * generic representation of that trials info
 * ********************************************/

#include "../../CommonClasses/Experiments/common.h"
#include "csvcheckedreader.h"
#include <QtMath>


class DataSet
{
public:
    DataSet();    
    void setLeftAndRightData(const CSVData &all, qint32 eyeCol);
    bool isEmpty() const {return (eyes.first().isEmpty() || eyes.last().isEmpty());}

    // Filtering functions functions
    void removeFirstNDifferent(qint32 N, qint32 idCol);
    void removeAllWithValueZero(qint32 idCol);
    void roundColumnValues(qint32 idCol);
    void filterRowsWithColOutsideRange(qreal min, qreal max, qint32 idCol);

    // Processing functions.
    ProcessingResults getProcessingResults(qint32 experiment, int *eyeUsed, int eyeToUse = -1);


private:

    // First is Left Eye and Second is Right eye.
    QList<CSVData> eyes;

    ProcessingResults getReadingResultsFor(qint32 eyeID) const;
    ProcessingResults getBindingResultsFor(qint32 eyeID, bool bound) const;
    ProcessingResults getFieldingResultsFor(qint32 eyeID) const;

};

#endif // DATASET_H
