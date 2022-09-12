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
#include <QGraphicsItemGroup>
#include <QFile>
#include <QTextStream>
#include <QtMath>

class BindingManager:  public ExperimentDataPainter
{
public:

    BindingManager();

    // Basic functions to reimplement.
    bool parseExpConfiguration(const QString &contents) override;
    void init(qreal display_resolution_width, qreal display_resolution_height) override;
    void configure(const QVariantMap &configuration) override;
    qint32 size() const override { return parser.getTrials().size();}
    qreal sizeToProcess() const override { return parser.getTrials().size()*2;}
    void renderStudyExplanationScreen(qint32 screen_index) override;

    // Draw the cross
    void drawCenter();

    // Clear the screen
    void drawClear() { clearCanvas(); }

    // Draw a particular trial
    void drawTrial(qint32 currentTrial, bool show, bool enableRenderDualMode = false);

    // Get the info on one trial.
    BindingParser::BindingTrial getTrial(qint32 trial) {return parser.getTrials().at(trial);}

    void enableDemoMode();

    // Configuration required to know if the key for the explanation is for BC or UC
    static const char * IS_BC

private:

    typedef enum {RFT_NORMAL,RFT_ENCODING_SIDE,RFT_SIDE_BY_SIDE} RenderFlagType;
    typedef enum {SRL_NORMAL, SRL_LEFT, SRL_RIGHT} ScreenRenderLocation;

    // Lines used to draw a cross in the center of the screen
    QLineF line0, line1;

    // The class that actually parses the experiment description.
    BindingParser parser;

    // Use small targets or not
    bool smallTargets;

    // Obtained from parsing study file and computing the drawing constants.
    QRectF gridBoundingRect;

    // Flag drawing function
    void drawFlags(const BindingParser::BindingSlide &primary, const BindingParser::BindingSlide &secondary, const RenderFlagType &rtf);

    // Overloaded function to use for image generation.
    bool drawFlags(const QString &trialName, bool show);

    void renderSingleFlag(const BindingParser::BindingSlide &slide, ScreenRenderLocation srl, bool renderArrow = false);

    // Constants for dual rendering
    const qreal DUAL_RENDERING_SCALE_FACTOR        = 0.8;
    const qreal DUAL_RENDERING_MARGIN_PERCENT_OF_W = 0.05;
    const qreal DUAL_RENDERING_ARROW_HEIGHT        = 0.10; // Percent of height.
    const qreal DUAL_RENDERING_ARROW_BODY_HEIGHT   = 0.05; // Percent of height.
    const qreal DUAL_RENDERING_ARROW_WIDTH         = 0.10; // Percent of width
    const qreal DUAL_RENDERING_ARROW_HEAD          = 0.2;  // Percent of arrow width width

    const qreal DUAL_RENDERING_AIR_ARROW           = 0.15; // Of the space between the slides.
    const qreal DUAL_RENDERING_ARROW_HTOW_RATIO    = 1.5; // The arrow is this many times high as it is wide.

};

#endif // BINDINGDRAW_H
