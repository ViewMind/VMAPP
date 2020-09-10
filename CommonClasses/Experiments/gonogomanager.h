#ifndef GONOGOMANAGER_H
#define GONOGOMANAGER_H

#include <QGraphicsLineItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QTextStream>
#include <QFile>
#include <QSet>

#include "gonogoparser.h"
#include "experimentdatapainter.h"

class GoNoGoManager: public ExperimentDataPainter
{
public:
    GoNoGoManager();

    bool parseExpConfiguration(const QString &contents) override;
    void init(ConfigurationManager *c) override;
    qint32 size() const override;
    qreal sizeToProcess() const override;

    // Will return false if the last trial drawn was the last.
    bool drawCross();
    void drawCurrentTrial();

    QString getCurrentTrialHeader() const;

    bool isPointInSideCorrectTargetForCurrentTrial(qreal x, qreal y) const;

private:
    QList<GoNoGoParser::Trial> gonogoTrials;
    QList<QRectF> targetBoxes;
    QList<qint32> answerArray;

    void enableDemoMode() override;
    void drawAllElements();

    QList<QGraphicsItem*>  gCross;
    QList<QGraphicsItem*>  gGRArrow;
    QList<QGraphicsItem*>  gGLArrow;
    QList<QGraphicsItem*>  gRRArrow;
    QList<QGraphicsItem*>  gRLArrow;
    QList<QGraphicsItem*>  gTargets;

    qint32 trialIndex;

    void setVisibilityToElementList(QList<QGraphicsItem*> list, bool makeVisible);

};

#endif // GONOGOMANAGER_H
