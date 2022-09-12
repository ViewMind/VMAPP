#ifndef GONOGOMANAGER_H
#define GONOGOMANAGER_H

#include <QGraphicsLineItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QTextStream>
#include <QFile>
#include <QSet>
#include <QtMath>

#include "gonogoparser.h"
#include "../experimentdatapainter.h"

class GoNoGoManager: public ExperimentDataPainter
{
public:
    GoNoGoManager();

    bool parseExpConfiguration(const QString &contents) override;
    void init(qreal display_resolution_width, qreal display_resolution_height ) override;
    qint32 size() const override;
    qreal sizeToProcess() const override;
    qint32 numberOfStudyExplanationScreens() const override {return 1;}
    void renderStudyExplanationScreen(qint32 screen_index) override;

    // Will return false if the last trial drawn was the last.
    bool drawCross();
    void drawTrialByID(const QString &id);
    void drawCurrentTrial();

    QString getCurrentTrialHeader() const;

    bool isPointInSideCorrectTargetForCurrentTrial(qreal x, qreal y) const;

    // This will make the next trial after loopValue be 0. Hence this will repeat the first few trials infinitely.
    void loopTrialsAt(qint32 loopValue);

    void enableDemoMode() override;

    void resetStudy();

    QRectF getArrowBox() const;
    QList<QRectF> getLeftAndRightHitBoxes() const;

private:
    QList<GoNoGoParser::Trial> gonogoTrials;
    QList<QRectF> targetBoxes;
    QRectF arrowTargetBox;
    QList<qint32> answerArray;

    void drawAllElements();

    QList<QGraphicsItem*>  gCross;
    QList<QGraphicsItem*>  gGRArrow;
    QList<QGraphicsItem*>  gGLArrow;
    QList<QGraphicsItem*>  gRRArrow;
    QList<QGraphicsItem*>  gRLArrow;
    QList<QGraphicsItem*>  gTargets;

    qint32 trialIndex;

    void setVisibilityToElementList(QList<QGraphicsItem*> list, bool makeVisible);

    static const char*  RED_ARROW_COLOR;
    static const char*  GREEN_ARROW_COLOR;

};

#endif // GONOGOMANAGER_H
