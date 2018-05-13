#ifndef EYEMATRIXPROCESSOR_H
#define EYEMATRIXPROCESSOR_H

#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QDate>
#include "dataset.h"
#include "csvcheckedreader.h"
#include "imagereportdrawer.h"
#include "../../CommonClasses/Experiments/common.h"

class EyeMatrixProcessor
{
public:
    EyeMatrixProcessor();

    // If thre was an error
    QString getError() const {return error;}

    // Process the CSV files.
    QString processReading(const QString &csvFile);
    QString processBinding(const QString &csvFile, bool bound);
    QString processFielding(const QString &csvFile);

    ProcessingResults getResults() const {return results;}

private:

    // The error message
    QString error;

    // The processing results
    ProcessingResults results;

    // Data generated from reading CSV, separated by Eye.
    DataSet csvReading;

    // The binding CSV data.
    DataSet csvBindingBC;
    DataSet csvBindingUC;

    // In order to make sure that the binding coefficient is calculated with the same eye.
    qint32 bindingEye;

    // Fielding data
    DataSet csvFielding;

    // Loading a CSV and loading to ExperimentData according to Checker description.
    void loadReadingCSV(const QString &filename);
    void loadBindingCSV(const QString &filename, bool boundColors);
    void loadFieldingCSV(const QString &filename);
    DataSet loadCSV(const QString &filename, CSVCheckedReader::ColumnChecks cc, qint32 eyeColumn, bool hasHeader = true);

    // Function that writes the tex file and an auxiliary function.
    QString statIDToString(qint32 stat) const;


};

#endif // EYEMATRIXPROCESSOR_H
