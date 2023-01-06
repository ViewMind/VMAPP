#ifndef MOVINGWINDOWALGORITHM_H
#define MOVINGWINDOWALGORITHM_H

#include <QtMath>
#include <QList>
#include <QDebug>
#include <QJsonDocument>
#include <QTextStream>
#include <QFile>
#include "fixation.h"

typedef QList<Fixation> Fixations;

// Structure that contains the data of the Moving window algorithm.
struct MovingWindowParameters{
public:
    qreal sampleFrequency;        // In hertz
    qreal maxDispersion;          // In pixels
    qreal minimumFixationLength;  // In miliseconds
    qint32 getStartWindowSize() const {return minimumWindowSize;}
    void calculateWindowSize(){
        qreal sampleRateInMs = 1000.0/sampleFrequency;
        minimumWindowSize = qCeil(minimumFixationLength/sampleRateInMs);
    }
private:
    qint32 minimumWindowSize;
};


class MovingWindowAlgorithm
{
public:

    MovingWindowAlgorithm();


    // If this file is set a complete log of how the fixations were formed is stored and saved at the end of the trial.
    void setOnlineFixationAnalysisFileOutput(const QString &filename);

    // Saves the log file, if this was enabled.
    void finalizeOnlineFixationLog();

    // Uses the exact same algorithm to compute the fixation with one data point at a time.
    Fixation calculateFixationsOnline(qreal x, qreal y, qreal timeStamp, qreal pupil);
    Fixation finalizeOnlineFixationCalculation(const QString &logfileIdentier = "");

    // Simple access to the parameters of the moving window algorithm
    MovingWindowParameters parameters;

private:

    struct MinMax{
        qreal min;
        qreal max;
        qreal diff() const {return max-min;}
        QString toString() const { return "[" + QString::number(min) + "," + QString::number(max) + "]"; }
    };

    // Online versions of the helper functions for cuputinf fixations
    void onlineFindDispLimits();

    // Online calculation fixation data structures.
    QList<Fixation::DataPoint> onlinePointsForFixation;
    MinMax onlineMMMY;
    MinMax onlineMMMX;

    QString onlineFixationLogFile;
    /**
     * @brief onlineFixationLog - Log that stores the fixation for file storage and later analysis.
     * @details Each Item in the Log is composed of two fields: 'id' and 'list'. List field is a QVariantMap List with
     * each item in the list having several fields to log in everything that happens during online fixations.
     */
    QList<QVariantMap> onlineFixationLog;

};

#endif // MOVINGWINDOWALGORITHM_H
