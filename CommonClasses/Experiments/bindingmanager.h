#ifndef BINDINGDRAW_H
#define BINDINGDRAW_H

/***************************************
 * Class that controls the flag drawing
 * for the binding experiment.
 * **************************************/

#include "../../CommonClasses/common.h"
#include "experimentdatapainter.h"
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QFile>
#include <QTextStream>
#include <QtMath>

// Binding target dimensions (in mm)
#define   BINDING_LARGE_TARGET_MARK                     "LARGE_TARGETS"
#define   BINDING_TARGET_SIDE                           10
#define   BINDING_TARGET_HS                             0.25
#define   BINDING_TARGET_HL                             2.6
#define   BINDING_TARGET_VS                             1.33
#define   BINDING_TARGET_VL                             3.54
#define   BINDING_TARGET_GRID                           105

// Strings for same and different in the CSV file.
#define   STR_DIFFERENT                                 "different"
#define   STR_SAME                                      "same"

class BindingManager:  public ExperimentDataPainter
{
public:

    struct BindingFlag{
        qint32 x;
        qint32 y;
        QColor back;
        QColor cross;
    };

    typedef QList<BindingFlag> BindingSlide;

    struct BindingTrial {
        QString name;
        qint32 number;
        BindingSlide show;
        BindingSlide test;
        bool isSame;
    };

    struct FlagDrawStructure {
        qint32 FlagSideH;
        qint32 FlagSideV;
        qint32 HSBorder;
        qint32 HLBorder;
        qint32 VSBorder;
        qint32 VLBorder;
        QList<qint32> xpos;
        QList<qint32> ypos;
    };

    BindingManager();

    // Basic functions to reimplement.
    bool parseExpConfiguration(const QString &contents);
    void init(ConfigurationManager *c);
    qint32 size() const { return trials.size();}
    qreal sizeToProcess() const { return trials.size()*2;}

    // Flag drawing function
    void drawFlags(const BindingSlide &slide);

    // Overloaded function to use for image generation.
    bool drawFlags(const QString &trialName, bool show);

    // Draw the cross
    void drawCenter(qint32 currentTrial);

    // Draw a particular trial
    void drawTrial(qint32 currentTrial, bool show);

    // Get the number of trials
    qint32 getNumberOfTrials() const {return trials.size();}

    // Get the info on one trial.
    BindingTrial getTrial(qint32 trial) {return trials.at(trial);}

private:

    // Lines used to draw a cross in the center of the screen
    QLineF line0, line1;

    // The trial structure
    QList<BindingTrial> trials;

    // The number of targets in each slide of the trial
    qint32 numberOfTargets;

    // Tells the system how to draw the flags.
    FlagDrawStructure drawStructure;

    // Aux functions for parsing.
    bool parseFlagPositions(const QString &line, BindingTrial *trial, bool show);
    bool parseColors(const QString &line, BindingTrial *trial, bool background, bool show);
    bool legacyParser(const QString &contents);



};

#endif // BINDINGDRAW_H
