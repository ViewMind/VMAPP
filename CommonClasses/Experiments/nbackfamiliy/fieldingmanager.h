#ifndef FIELDINGDRAW_H
#define FIELDINGDRAW_H

#include <QGraphicsLineItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QTextStream>
#include <QFile>
#include <QSet>
#include <QTimer>

#include "fieldingparser.h"
#include "../experimentdatapainter.h"

class FieldingManager: public QObject, public ExperimentDataPainter
{    
    Q_OBJECT
public:

    // Draw states for the Fielding experiments
    typedef enum {DS_CROSS, DS_CROSS_TARGET, DS_1, DS_2, DS_3, DS_TARGET_BOX_ONLY} DrawState;

    // Constructor
    FieldingManager();

    // Basic functions to reimplement.
    bool parseExpConfiguration(const QString &contents) override;
    void init(qreal display_resolution_width, qreal display_resolution_height) override;
    void configure(const QVariantMap &config) override;
    qint32 size() const override {return fieldingTrials.size();}
    qint32 numberOfStudyExplanationScreens() const override {return 1;}
    void renderStudyExplanationScreen(qint32 screen_index) override;

    // DEPRACATED.And wrong when using NBackVS with more than 3 targets.
    qreal sizeToProcess() const override {return fieldingTrials.size()*3;}

    // The actual drawing function for the background.
    void drawBackground();

    // Selects which element to show based on the draw state
    void setDrawState(DrawState ds);

    // Setting the target (Red Circle) position.
    void setTargetPosition(qint32 trial, qint32 image);

    // Setting the target position from the trial name.
    bool setTargetPositionFromTrialName(const QString &trial, qint32 image);

    // The coordinates where the target is drawn.
    QPoint getTargetPoint(qint32 trial, qint32 image) const;

    // Getting an indivual trial.
    FieldingParser::Trial getTrial(qint32 i) const { return fieldingTrials.at(i);}

    // Gets the expected hit sequence for a given trial
    QList<qint32> getExpectedTargetSequenceForTrial(qint32 trial, qint32 targetNum) const;

    // Used mainly for raw data container data
    QString getFullSequenceAsString(qint32 trial);

    // Used only in DBug Mode to show the next box to hit in the sequence.
    void setDebugSequenceValue(qint32 next_target_box);

    // Auxiliary function that check if a point is in a given target box
    bool isPointInTargetBox(qreal x, qreal y, qint32 targetBox) const;

    // Variation of the function above. It verifies if x and y point is in any box. If it is it returns the index. Otherwise it returns -1
    qint32 pointIsInWhichTargetBox(qreal x, qreal y) const;

    QList<QRectF> getHitTargetBoxes() const { return hitTargetBoxes; }

    // Light Boxes ON/OFF.
    bool lightUpBox(qint32 box_index);
    void lightOffCheck();
    void ligthOffAllBoxes();

    // Drawing the pause text
    void drawPauseScreen();

    void enableDemoMode() override;

    static const char * CONFIG_IS_VR_BEING_USED;
    static const char * CONFIG_PAUSE_TEXT_LANG;

    static const char * LANG_ES;
    static const char * LANG_EN;

private slots:
    void onLightOffTimeout();

private:

    // The list of trials
    QList<FieldingParser::Trial> fieldingTrials;

    // Graphical items shown in the screen
    QGraphicsEllipseItem    *gTarget;
    QGraphicsLineItem       *gCrossLine0;
    QGraphicsLineItem       *gCrossLine1;
    QGraphicsSimpleTextItem *gText1;
    QGraphicsSimpleTextItem *gText2;
    QGraphicsSimpleTextItem *gText3;
    QGraphicsSimpleTextItem *gDebugSequenceValue;

    // The actual target boxes. (To recognize a hit AND to be drawn)
    QList<QRectF> hitTargetBoxes;
    QList<QRectF> drawTargetBoxes;

    // Required for the use of the ligthup feature.
    QList<QGraphicsRectItem*> graphicalTargetBoxes;

    // Used for turning on and off the target boxes.
    qint32 currentlyLitUp;
    qint32 lastBoxLitUp;
    QTimer litUpTimer;

    static const qreal K_TARGET_R;
    static const qreal K_TARGET_OFFSET_X;
    static const qreal K_TARGET_OFFSET_Y;
    static const qreal K_CROSS_LINE_LENGTH;

    QString pauseText;
    bool vr_being_used;

    static const char * PAUSE_TEXT_SPANISH;
    static const char * PAUSE_TEXT_ENGLISH;

    const qint32 LIGHT_OFF_TIME = 100; // Milliseconds.

};

#endif // FIELDINGDRAW_H
