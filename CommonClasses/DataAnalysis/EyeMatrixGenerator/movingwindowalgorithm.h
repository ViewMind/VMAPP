#ifndef MOVINGWINDOWALGORITHM_H
#define MOVINGWINDOWALGORITHM_H

#include <QtMath>
#include <QList>
#include <QDebug>

typedef QList<qreal>   DataRow;
typedef QList<DataRow> DataMatrix;

struct Fixation{
    qreal x;
    qreal y;
    qreal duration;
    qint32 indexFixationStart;
    qint32 indexFixationEnd;

    // Checks if the fixation fall in a given range.
    bool isIn(qreal minX, qreal maxX, qreal minY, qreal maxY) const {
        bool ans = (minX <= x);
        ans = ans && (x <= maxX);
        ans = ans && (minY <= y);
        ans = ans && (y <= maxY);
        return ans;
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
};

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

    // Moving window algorithm. This will use the data contained in the columns xI and yI as the x y coordinates.
    // tI indicates the time column.
    Fixations computeFixations(const DataMatrix &data, qint32 xI, qint32 yI, qint32 tI);

    // Simple access to the parameters of the moving window algorithm
    MovingWindowParameters parameters;

private:

    struct MinMax{
        qreal min;
        qreal max;
        qreal diff() const {return max-min;}
    };

    // Find the maximum and minimum fo the start window.
    MinMax findDispLimits(const DataMatrix &data, qint32 col, qint32 startI, qint32 endI) const;

    // Calculates the fixation point.
    Fixation calculateFixationPoint(const DataMatrix &data, qint32 xI, qint32 yI, qint32 startI, qint32 endI, qint32 tI);

};

#endif // MOVINGWINDOWALGORITHM_H
