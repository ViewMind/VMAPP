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

#include "nbackparser.h"
#include "../experimentdatapainter.h"
#include "../qgraphicsarrow.h"

class NBackManager: public QObject, public ExperimentDataPainter
{    
    Q_OBJECT
public:

    // Draw states for the Fielding experiments
    typedef enum {DS_CROSS, DS_CROSS_TARGET, DS_1, DS_2, DS_3, DS_TARGET_BOX_ONLY} DrawState;

    // Constructor
    NBackManager();

    // Basic functions to reimplement.
    bool parseExpConfiguration(const QString &contents) override;
    void init(qreal display_resolution_width, qreal display_resolution_height) override;
    void configure(const QVariantMap &config) override;
    qint32 size() const override;
    void renderStudyExplanationScreen(qint32 screen_index) override;

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
    NBackParser::Trial getTrial(qint32 i) const { return nbackTrials.at(i);}

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

    static const char * CONFIG_IS_VR_BEING_USED;
    static const char * CONFIG_PAUSE_TEXT_LANG;
    static const char * CONFIG_IS_VS;
    static const char * LANG_ES;
    static const char * LANG_EN;

private slots:
    void onLightOffTimeout();

private:

    // The list of trials
    QList<NBackParser::Trial> nbackTrials;

    // The target Radious Size is used in several instances of the code, so it computed once and stored.
    qreal TARGET_R;

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

    // Renders arrows fromt the center of one target to the other.
    // If source target is 0, then the center is the source.
    void renderStudyArrows(qint32 source_target, qint32 dest_target, const QColor &color, bool drawStartMark = false);

    // Renders barely visible targets to indicate where they used to be.
    void renderPhantomTargets(QMap<qint32,qint32> rectangle_indexes);

    void setTargetPositionByRectangleIndex(qint32 rectangle_index);

    // Common function that renders a target circle. To have the code all in the same place. It does not position the circle
    // Returuns a pointer to the created circle
    QGraphicsEllipseItem * renderTargetCircle();

    // Referencing the slides for the explanations.
    const qint32 STUDY_EXPLANTION_CROSS_1  = 0;
    const qint32 STUDY_EXPLANTION_TARGET_1 = 1;
    const qint32 STUDY_EXPLANTION_TARGET_2 = 2;
    const qint32 STUDY_EXPLANTION_TARGET_3 = 3;
    const qint32 STUDY_EXPLANTION_LOOK_3   = 4;
    const qint32 STUDY_EXPLANTION_LOOK_2   = 5;
    const qint32 STUDY_EXPLANTION_LOOK_1   = 6;
    const qint32 STUDY_EXPLANTION_CROSS_2  = 7;
    const qint32 STUDY_EXPLANTION_TARGET_4 = 8;
    const qint32 STUDY_EXPLANTION_TARGET_5 = 9;
    const qint32 STUDY_EXPLANTION_TARGET_6 = 10;
    const qint32 STUDY_EXPLANTION_LOOK_6   = 11;
    const qint32 STUDY_EXPLANTION_LOOK_5   = 12;
    const qint32 STUDY_EXPLANTION_LOOK_4   = 13;

    const qint32 NUMBER_OF_EXPLANATION_SLIDES = 14;

};

#endif // FIELDINGDRAW_H

