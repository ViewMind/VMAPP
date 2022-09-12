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
    qint32 size() const override;
    void renderStudyExplanationScreen(qint32 screen_index) override;

    // Draw the cross
    void drawCenter();

    // Clear the screen
    void drawClear() { clearCanvas(); }

    // Draw a particular trial
    void drawTrial(qint32 currentTrial, bool show, bool enableRenderDualMode = false);

    // Get the info on one trial.
    BindingParser::BindingTrial getTrial(qint32 trial) {return parser.getTrials().at(trial); }

    // Configuration required to know if the key for the explanation is for BC or UC
    static const char * CONFIG_IS_BC;
    static const char * CONFIG_N_TARGETS;

private:

    typedef enum {RFT_NORMAL,RFT_ENCODING_SIDE,RFT_SIDE_BY_SIDE} RenderFlagType;
    typedef enum {SRL_NORMAL, SRL_LEFT, SRL_RIGHT} ScreenRenderLocation;

    // Lines used to draw a cross in the center of the screen
    QLineF line0, line1;

    // The class that actually parses the experiment description.
    BindingParser parser;    

    // Obtained from parsing study file and computing the drawing constants.
    QRectF gridBoundingRect;

    // Flags used for proper rendering and messaging during study explantion.
    bool isBC;
    qint32 numberOfTargets;

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
    const qreal DUAL_RENDERING_ARROW_HTOW_RATIO    = 0.9; // The arrow is this many times high as it is wide.

    // Index representing the first few slides fo the study, for reference used during study explanations.
    const qint32 SE_BC_ENCODING_DIFF               = 0;
    const qint32 SE_BC_DECODING_DIFF               = 1;
    const qint32 SE_BC_ENCODING_SAME               = 2;
    const qint32 SE_BC_DECODING_SAME               = 3;

    const qint32 SE_UC_ENCODING_DIFF               = 0;
    const qint32 SE_UC_DECODING_DIFF               = 1;
    const qint32 SE_UC_ENCODING_SAME               = 2;
    const qint32 SE_UC_DECODING_SAME               = 3;

    // Index representing trials for the explanation phase.
    const qint32 SE_BC_2_DIFF_TRIAL                = 7;
    const qint32 SE_BC_2_SAME_TRIAL                = 6;

    const qint32 SE_UC_2_DIFF_TRIAL                = 7;
    const qint32 SE_UC_2_SAME_TRIAL                = 6;

    const qint32 SE_BC_3_DIFF_TRIAL                = 7;
    const qint32 SE_BC_3_SAME_TRIAL                = 6;

    const qint32 SE_UC_3_DIFF_TRIAL                = 6;
    const qint32 SE_UC_3_SAME_TRIAL                = 7;


};

#endif // BINDINGDRAW_H
