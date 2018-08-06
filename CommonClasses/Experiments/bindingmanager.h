#ifndef BINDINGDRAW_H
#define BINDINGDRAW_H

/***************************************
 * Class that controls the flag drawing
 * for the binding experiment.
 * **************************************/

#include "experimentdatapainter.h"
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QFile>
#include <QTextStream>
#include <QtMath>

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

        // For debugging.
        QString toString() const{
            QString ans = "";
            ans = "(" + QString::number(x) + "," + QString::number(y) + ") ";
            ans = ans + "BACK: " + back.name() + " CROSS: " + cross.name();
            return ans;
        }
    };

    typedef QList<BindingFlag> BindingSlide;

    struct BindingTrial {
        QString name;
        BindingSlide show;
        BindingSlide test;
        bool isSame;

        // For debugging.
        QString toString() const {
            QString ans = name + ". ";
            ans = ans  + "SHOW -> ";
            for (qint32 i = 0; i < show.size(); i++){
                ans = ans + "TARGET " + QString::number(i) + ": " + show.at(i).toString() + "| ";
            }
            ans = ans  + "TEST -> ";
            for (qint32 i = 0; i < test.size(); i++){
                ans = ans + "TARGET " + QString::number(i) + ": " + test.at(i).toString() + "| ";
            }
            return ans;
        }

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
    void drawCenter();

    // Draw a particular trial
    void drawTrial(qint32 currentTrial, bool show);

    // Get the info on one trial.
    BindingTrial getTrial(qint32 trial) {return trials.at(trial);}

private:

    // For debugging
    LogInterface logger;

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

    void enableDemoMode();

};

#endif // BINDINGDRAW_H
