#ifndef BINDINGDRAW_H
#define BINDINGDRAW_H

/***************************************
 * Class that controls the flag drawing
 * for the binding experiment.
 * **************************************/

#include "../experimentdatapainter.h"
#include "bindingparser.h"
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QFile>
#include <QTextStream>
#include <QtMath>

class BindingManager:  public ExperimentDataPainter
{
public:

    BindingManager();

    // Basic functions to reimplement.
    bool parseExpConfiguration(const QString &contents);
    void init(qreal display_resolution_width, qreal display_resolution_height);
    void configure(const QVariantMap &configuration);
    qint32 size() const { return parser.getTrials().size();}
    qreal sizeToProcess() const { return parser.getTrials().size()*2;}

    // Flag drawing function
    void drawFlags(const BindingParser::BindingSlide &slide);

    // Overloaded function to use for image generation.
    bool drawFlags(const QString &trialName, bool show);

    // Draw the cross
    void drawCenter();

    // Clear the screen
    void drawClear() { clearCanvas(); }

    // Draw a particular trial
    void drawTrial(qint32 currentTrial, bool show);

    // Get the info on one trial.
    BindingParser::BindingTrial getTrial(qint32 trial) {return parser.getTrials().at(trial);}

    void enableDemoMode();

    // The number of targets is set simply by selectig which experimet file to parse.
    static const char * CONFIG_USE_SMALL_TARGETS;

private:

    // Lines used to draw a cross in the center of the screen
    QLineF line0, line1;

    // The class that actually parses the experiment description.
    BindingParser parser;

    // Use small targets or not
    bool smallTargets;

};

#endif // BINDINGDRAW_H
