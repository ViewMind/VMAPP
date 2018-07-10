#ifndef EYEMATRIXPROCESSOR_H
#define EYEMATRIXPROCESSOR_H

#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QDate>
#include <QHash>
#include "dataset.h"
#include "csvcheckedreader.h"
#include "../../CommonClasses/SQLConn/dbdescription.h"


// Reading processing parameters
#define   READING_N_TO_FILTER_FROM_START                10
#define   READING_MIN_VALID_FIXATION                    51
#define   READING_MAX_VALID_FIXATION                    1750

class EyeMatrixProcessor
{
public:

    typedef QHash<QString,qreal> DBHash;

    EyeMatrixProcessor(quint8 eye);

    // If thre was an error
    QString getError() const {return error;}

    // Process the CSV files.
    QString processReading(const QString &csvFile, DBHash *dbdata = nullptr);
    QString processBinding(const QString &csvFile, bool bound, DBHash *dbdata = nullptr);
    QString processFielding(const QString &csvFile, qint32 numberOfTrials, DBHash *dbdata = nullptr);

    DataSet::ProcessingResults getResults() const {return results;}


private:

    // The error message
    QString error;

    // The processing results
    DataSet::ProcessingResults results;

    // Data generated from reading CSV, separated by Eye.
    DataSet csvReading;

    // The selected data processing eye. The actual eye selected which changes with each experiment and the string representation
    quint8 eyeForResults;
    quint8 selectedEye;
    QString selectedEyeStr;

    // The binding CSV data.
    DataSet csvBindingBC;
    DataSet csvBindingUC;

    // Requires a two pass for processBiding with both UC and BC to calculate the final report value.
    // The variables serve the purposes of flags AND to save the best eye for each Binding experiment
    quint8 eyeForUC;
    quint8 eyeForBC;

    // Fielding data
    DataSet csvFielding;

    // Loading a CSV and loading to ExperimentData according to Checker description.
    void loadReadingCSV(const QString &filename);
    void loadBindingCSV(const QString &filename, bool boundColors);
    void loadFieldingCSV(const QString &filename);
    DataSet loadCSV(const QString &filename, CSVCheckedReader::ColumnChecks cc, qint32 eyeColumn, bool hasHeader = true);

    // Function that writes the tex file and an auxiliary function.
    QString statIDToString(qint32 stat) const;

    // Function that sets the slected eye variables.
    void setSelectedEyeVariables(const DataSet &set);


};

#endif // EYEMATRIXPROCESSOR_H
