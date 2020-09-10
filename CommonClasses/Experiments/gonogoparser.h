#ifndef GONOGOPARSER_H
#define GONOGOPARSER_H

#include <QString>
#include <QList>
#include <QPoint>
#include <QRectF>

#define  GONOGO_TRIAL_TYPE_RLEFT   0
#define  GONOGO_TRIAL_TYPE_GLEFT   1
#define  GONOGO_TRIAL_TYPE_RRIGHT  2
#define  GONOGO_TRIAL_TYPE_GRIGHT  3

// Draw constants need to be here for computations when manager is not available.
#define  GONOGO_SIDE_MARGIN               0.05   // Percent of resolution width
#define  GONOGO_TARGET_SIZE               0.03   // Percent of resolution width
#define  GONOGO_TARGET_TOL                0.04   // Percent of resolution width
#define  GONOGO_ARROW_LENGTH              0.1    // Percent of resolution width
#define  GONOGO_ARROW_INDICATOR_LENGTH    0.4    // Percent of arrow length
#define  GONOGO_CROSS_LINE_LENGTH         0.1    // Percent of resolution width/height.
#define  GONOGO_TARGET_LINE_WIDTH         0.1    // Percent of the diameter.
#define  GONOGO_LINE_WIDTH                0.1    // Percent of the line length.
#define  GONOGO_INDICATOR_LINE_LENGTH     0.3    // Percent of the line length
#define  GONOGO_RED_ARROW_COLOR           "#cc1f1f"
#define  GONOGO_GREEN_ARROW_COLOR         "#28ab14"

class GoNoGoParser
{
public:

    struct Trial{
        QString id;
        qint32 type;
    };

    GoNoGoParser();

    bool parseGoNoGoExperiment(const QString &contents, const qreal &resolutionWidth, const qreal &resolutionHeight);

    QList<Trial> getTrials() const;
    QString getError() const;
    QString getVersionString() const;
    QList<QRectF> getTargetBoxes() const;
    QList<qint32> getCorrectAnswerArray() const;

private:
    QList<Trial> trials;
    QString versionString;
    QString error;
    QList<QRectF> targetBoxes;
    QList<qint32> answerArray;

};

#endif // GONOGOPARSER_H
