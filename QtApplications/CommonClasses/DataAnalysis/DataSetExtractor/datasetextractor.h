#ifndef DATASETEXTRACTOR_H
#define DATASETEXTRACTOR_H

#include "../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpbase.h"
#include "../../../CommonClasses/common.h"

#define READING_TOKEN_LIST_EXPECTED_SIZE 13

class DataSetExtractor
{
public:

    struct DataSetList {
        QList<QString>    dataSetIDs;
        QList<QString>    extraSetInfo;
        QList<DataMatrix> dataSets;
        QSize resolution;
        QString version;
        QString originalFileName;
        QString originalFilePath;
        QString experimentDescription;
        qint32  dataSetType;
    };

    struct RawDataInfo {
        QString expDescription;
        QString expVersion;
        QString expData;
        QSize   resolution;
        QString error;
    };

    DataSetExtractor();
    bool extractDataSetFromFile(const QString &fileName);

    // Getting the results;
    DataSetList getDataSetList() const {return data;}
    QString getError() const {return error;}

    // Static fucntion for general Data Set processing.
    static RawDataInfo separateRawDataFile(const QString &fileName);

private:
    QString error;
    DataSetList data;
    QStringList content;

    // Actual extraction functions;
    bool dataSetFromReadingFile();

};

#endif // DATASETEXTRACTOR_H
