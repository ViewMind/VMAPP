#ifndef PERCEPTIONMANAGER_H
#define PERCEPTIONMANAGER_H

#include <QGraphicsRectItem>
#include <QPolygonF>

#include "perceptionparser.h"
#include "experimentdatapainter.h"

class PerceptionManager:  public ExperimentDataPainter
{
public:
    PerceptionManager();

    typedef enum {PTS_CROSS_ONLY, PTS_TRIANGLES, PTS_YES_OR_NO} PerceptionTrialState;

    bool parseExpConfiguration(const QString &contents) override;
    void init(ConfigurationManager *c) override;
    qint32 size() const override;
    qreal sizeToProcess() const override;

    void prepareForTrialSet();
    bool drawNextTrialState();

    PerceptionTrialState getPerceptionTrialState() const;

    QList<QRectF> getYesAndNoTargetBoxes() const;

    QString getCurrentTrialHeader() const;

    void highlightSelection(qreal x, qreal y);

private:

    QList<PerceptionParser::PerceptionTrial> perceptionTrials;
    void enableDemoMode() override;

    void drawTrial(qint32 trial_index);

    PerceptionTrialState currentTrialState;
    qint32 currentTrial;

    QString yesText;
    QString noText;

    QList<QRectF> yesAndNoTargetBoxes;
    QList<QGraphicsRectItem*> yesAndNoBoxes;

    QPolygonF getTrianglePoligonAt(qreal x, qreal y, qreal a, PerceptionParser::TriangleType triangle_type);

};

#endif // PERCEPTIONMANAGER_H
