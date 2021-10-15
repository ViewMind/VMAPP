#ifndef FIXATION_H
#define FIXATION_H

#include <QString>
#include <QList>

class Fixation
{
public:

    Fixation();

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

    // Construct a fixation from a list of DataPoints
    void construct(const QList<DataPoint> &onlinePointsForFixation, bool is_done);

    // Fixation type
    typedef enum {FIX_STARTED, FIX_INVALID, FIX_FINISHED} FixationStatus;

    // Getting the status in different convenient manners.
    bool hasFinished() const;
    bool hasStarted() const;
    bool isInvalid() const;

    // Getting the values.
    qreal getX() const;
    qreal getY() const;
    qreal getDuration() const;
    qreal getStartTime() const;
    qreal getEndTime() const;
    qreal getPupil() const;
    qreal getTime() const;
    qreal getBlinks() const;
    qreal getChar() const;
    qreal getWord() const;

    // Checks if the fixation falls in a given range.
    bool isIn(qreal minX, qreal maxX, qreal minY, qreal maxY) const;

    QString toString() const;

private:
    FixationStatus status;
    qreal x;
    qreal y;
    qreal duration;
    qreal fixStart;
    qreal fixEnd;
    qreal pupil;
    qreal time;
    qint32 pupilZeroCount;

    // Explicit for reading.
    qreal sentence_char;
    qreal sentence_word;

    // For comparison with zero.
    static const qreal PUPIL_ZERO_TOL;

};

#endif // FIXATION_H
