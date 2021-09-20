#ifndef MOVINGWINDOWALGORITHM_H
#define MOVINGWINDOWALGORITHM_H

//#define ENABLE_MWA_DEBUG

#ifdef ENABLE_MWA_DEBUG
#include "../../LogInterface/loginterface.h"
#endif

#include <QtMath>
#include <QList>
#include <QDebug>
#include <QJsonDocument>
#include <QTextStream>
#include <QFile>

typedef QList<qreal>   DataRow;
typedef QList<DataRow> DataMatrix;

struct Fixation{
    qreal x;
    qreal y;
    qreal duration;
    qreal fixStart;
    qreal fixEnd;
    qint32 indexFixationStart;
    qint32 indexFixationEnd;
    qreal pupil;
    qreal time;
    qint32 pupilZeroCount;

    // Explicit for reading.
    qreal sentence_char;
    qreal sentence_word;

    bool isValid() const { return ((indexFixationEnd >= 0) && (indexFixationStart >= 0)); }

    // Checks if the fixation falls in a given range.
    bool isIn(qreal minX, qreal maxX, qreal minY, qreal maxY) const {
        bool ans = (minX <= x);
        ans = ans && (x <= maxX);
        ans = ans && (minY <= y);
        ans = ans && (y <= maxY);
        return ans;
    }

    bool isSame(const Fixation &f) const {
        if (x != f.x) return false;
        if (y != f.y) return false;
        if (duration != f.duration) return false;
        if (fixStart != f.fixStart) return false;
        if (fixEnd  != f.fixEnd) return false;
        return true;
    }

    QString toString() const {
        return "(" + QString::number(x) + "," + QString::number(y) + "). "
                + QString::number(fixEnd) + " - " + QString::number(fixStart) + " = " + QString::number(duration);
    }

};

typedef QList<Fixation> Fixations;

struct FixationList{
    QString experimentDescription;
    QList<QStringList> trialID;
    QList<Fixations> left;
    QList<Fixations> right;
    void clear(){
        trialID.clear();
        left.clear();
        right.clear();
    }
    qint32 indexOfTrial(const QStringList &toFind) const;
    void fillFixationsLists();
    void displayFixList() const;
    bool checkCoherenceInListSizes() const;
};

// Structure that contains the data of the Moving window algorithm.
struct MovingWindowParameters{
public:
    qreal sampleFrequency;        // In hertz
    qreal maxDispersion;          // In pixels
    qreal minimumFixationLength;  // In miliseconds
//    qreal resolutionScaling;      // Scale the input x and y values to this value, according to resolution.
//    qreal resolutionWidth;        // Maximum Resolution on X, for resolution scaling
//    qreal resolutionHeight;       // Maximum Resolution on Y, for resolution scaling
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

    // Moving window algorithm. This will use the data contained in the columns xI and yI as the x y coordinates.
    // tI indicates the time column.
    Fixations computeFixations(const DataMatrix &data, qint32 xI, qint32 yI, qint32 tI);

    // If this file is set a complete log of how the fixations were formed is stored and saved at the end of the trial.
    void setOnlineFixationAnalysisFileOutput(const QString &filename);

    // Saves the log file, if this was enabled.
    void finalizeOnlineFixationLog();

    // If set to anything greater than 0 (disabled), any fixation over the length specified will be forcibily formed.
    void setMaximumFixationLength(qint32 length_in_miliseconds);

    // Uses the exact same algorithm to compute the fixation with one data point at a time.
    Fixation calculateFixationsOnline(qreal x, qreal y, qreal timeStamp, qreal pupil, qreal schar = 0, qreal word = 0);
    Fixation finalizeOnlineFixationCalculation(const QString &logfileIdentier = "");

    // Simple access to the parameters of the moving window algorithm
    MovingWindowParameters parameters;

#ifdef ENABLE_MWA_DEBUG
    void setLog(const QString &fileName) { logger.setLogFileLocation(fileName); }
    void logMessage(const QString &s) { logger.appendStandard(s); }
#endif

private:

    struct MinMax{
        qreal min;
        qreal max;
        qreal diff() const {return max-min;}
        QString toString() const { return "[" + QString::number(min) + "," + QString::number(max) + "]"; }
    };

    struct DataPoint{
        qreal x;
        qreal y;
        qreal timestamp;
        qreal pupil;
        // Used only for reading.
        qreal word;
        qreal schar;
        QString toString() const { return "@" + QString::number(timestamp) + "(" + QString::number(x) + "," + QString::number(y) + ")"; }
    };

    // Compute the actual fixation based on matrix data.
    Fixation calculateFixationPoint(const DataMatrix &data, qint32 xI, qint32 yI, qint32 startI, qint32 endI, qint32 tI);

    // Find the maximum and minimum fo the start window.
    MinMax findDispLimits(const DataMatrix &data, qint32 col, qint32 startI, qint32 endI) const;

    // Online versions of the helper functions for cuputinf fixations
    void onlineFindDispLimits();
    Fixation onlineCalcuationOfFixationPoint();

    // Online calculation fixation data structures.
    QList<DataPoint> onlinePointsForFixation;
    MinMax onlineMMMY;
    MinMax onlineMMMX;

    qint32 maxFixationLength;

    QString onlineFixationLogFile;
    /**
     * @brief onlineFixationLog - Log that stores the fixation for file storage and later analysis.
     * @details Each Item in the Log is composed of two fields: 'id' and 'list'. List field is a QVariantMap List with
     * each item in the list having several fields to log in everything that happens during online fixations.
     */
    QList<QVariantMap> onlineFixationLog;

    static const qreal PUPIL_ZERO_TOL;

#ifdef ENABLE_MWA_DEBUG
    LogInterface logger;
#endif

};

#endif // MOVINGWINDOWALGORITHM_H