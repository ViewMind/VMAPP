#ifndef DATAEXTRACTOR_H
#define DATAEXTRACTOR_H

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include "../../CommonClasses/common.h"

#define  FILE_BINDING  "binding"

class DataExtractor
{
public:

    typedef QList< QList<qreal> > DataMatrix;

    struct DataPoints{
        QList<DataMatrix> datapoints;
        qreal width;
        qreal height;
        QString error;
    };

    DataExtractor();
    DataPoints analyzeFile(const QString &fname);

private:

    typedef enum {TCB_LINE_SIZE_CHANGE, TCB_LINE_VALUE_CHANGE} TrialChangeBehaviour;

    struct DataPointsExtractParameters {
        TrialChangeBehaviour trialChangeBehaviour;
        qint32 dataLineSize;
        qint32 valueChangeColumn;
        QString header;
        QList<qint32> colsWithData;
        qint32 colWithID;
    };

    QString fileToAnalyze;

    QStringList getFileLines(QString *error);
    QStringList removeHeader(const QStringList &lines, const QString &header);
    qreal calculateFrequency(const QList<qreal> &times);
    DataPoints extractDataPoints(const DataPointsExtractParameters &dpep);
};

#endif // DATAEXTRACTOR_H
