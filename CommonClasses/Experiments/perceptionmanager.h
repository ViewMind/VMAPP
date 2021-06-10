#ifndef PERCEPTIONMANAGER_H
#define PERCEPTIONMANAGER_H

#include <QGraphicsRectItem>
#include <QPolygonF>

#include "perceptionparser.h"
#include "experimentdatapainter.h"

class PerceptionManager:  public ExperimentDataPainter
{
public:

    static const char * CONFIGURE_YES_WORD;

    PerceptionManager();

    typedef enum {PTS_CROSS_ONLY, PTS_TRIANGLES, PTS_YES_OR_NO} PerceptionTrialState;

    bool parseExpConfiguration(const QString &contents) override;
    void init(qreal display_resolution_width, qreal display_resolution_height) override;
    void configure(const QVariantMap &configuration);
    qint32 size() const override;
    qreal sizeToProcess() const override;

    void prepareForTrialSet();
    bool drawNextTrialState();

    PerceptionTrialState getPerceptionTrialState() const;

    QList<QRectF> getYesAndNoTargetBoxes() const;

    QString getCurrentTrialHeader() const;

    void highlightSelection(qreal x, qreal y);

    void enableDemoMode() override;

private:

    QList<PerceptionParser::PerceptionTrial> perceptionTrials;
    void drawTrial(qint32 trial_index);

    PerceptionTrialState currentTrialState;
    qint32 currentTrial;

    QString yesText;
    QString noText;

    QList<QRectF> yesAndNoTargetBoxes;
    QList<QGraphicsRectItem*> yesAndNoBoxes;

    QPolygonF getTrianglePoligonAt(qreal x, qreal y, qreal a, PerceptionParser::TriangleType triangle_type);

    static const qreal  K_WIDTH_MARGIN;
    static const qint32 NUMBER_OF_ELEMENTS_PER_ROW;
    static const qint32 NUMBER_OF_ELEMENTS_PER_COL;
    static const qreal  YES_NO_FONT_SIZE;
    static const qreal  AIR_YES_NO_TEXT_W;
    static const qreal  AIR_YES_NO_TEXT_H;
    static const qreal  MARGIN_YES_NO_TEXT;

};

#endif // PERCEPTIONMANAGER_H
