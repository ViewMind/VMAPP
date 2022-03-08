#ifndef GONOGOPARSER_H
#define GONOGOPARSER_H

#include <QString>
#include <QList>
#include <QPoint>
#include <QRectF>
#include <QSet>
#include <QtMath>
#include <QDebug>
#include "../../eyetracker_defines.h"
#include "../drawingconstantscalculator.h"


class GoNoGoParser
{
public:

    struct Trial{
        QString id;
        qint32 type;
    };

    static const qint32 TRIAL_TYPE_RLEFT;
    static const qint32 TRIAL_TYPE_GLEFT;
    static const qint32 TRIAL_TYPE_RRIGHT;
    static const qint32 TRIAL_TYPE_GRIGHT;
    static const QStringList TrialTypeList;

    static const qreal SIDE_MARGIN;
    static const qreal TARGET_SIZE;
    static const qreal TARGET_TOL;
    static const qreal ARROW_LENGTH;
    static const qreal ARROW_INDICATOR_LENGTH;
    static const qreal CROSS_LINE_LENGTH;
    static const qreal TARGET_LINE_WIDTH;
    static const qreal LINE_WIDTH;
    static const qreal INDICATOR_LINE_LENGTH;
    static const qreal ARROW_TARGET_BOX_WMARGIN;
    static const qreal ARROW_TARGET_BOX_HMARGIN;

    GoNoGoParser();

    bool parseGoNoGoExperiment(const QString &contents, const qreal &resolutionWidth, const qreal &resolutionHeight);

    QList<Trial> getTrials() const;
    QString getError() const;
    QString getVersionString() const;
    QList<QRectF> getTargetBoxes() const;
    QRectF getArrowTargetBox() const;
    QList<qint32> getCorrectAnswerArray() const;
    QString getTrialTypeByIndex(qint32 index_of_type);

private:
    QList<Trial> trials;
    QString versionString;
    QString error;
    QList<QRectF> targetBoxes;
    QRectF arrowTargetBox;
    QList<qint32> answerArray;


};

#endif // GONOGOPARSER_H
