#ifndef EXPERIMENTDATAPAINTER_H
#define EXPERIMENTDATAPAINTER_H

#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QPainter>
#include <QApplication>
#include "../ConfigurationManager/configurationmanager.h"
#include "../eyetracker_defines.h"
#include "../debug.h"

// Number of actual trials to have in demo mode
#define   NUMBER_OF_TRIALS_IN_DEMO_MODE                 5

class ExperimentDataPainter
{
public:
    ExperimentDataPainter();
    virtual ~ExperimentDataPainter();

    // Basic functions to reimplement.
    virtual bool parseExpConfiguration(const QString &contents){ Q_UNUSED(contents) return false;}
    virtual void init(qreal display_resolution_width, qreal display_resolution_height);
    virtual void configure(const QVariantMap &configuration);
    virtual qint32 size() const {return 0;}
    virtual qreal sizeToProcess() const {return 0;}

    // Required for properly cycling through the explanation screen in a generic manner.
    qint32 getNumberOfStudyExplanationScreens() const;

    // For debugging
    void setShortStudy();

    // For on-screen gaze tracking.
    void updateGazePoints(qreal xr, qreal xl, qreal yr, qreal yl);
    void redrawGazePoints();

    // Trims the number of trials to number in demo mode.
    virtual void enableDemoMode(){}

    // Renders the study explanation screen according to index.
    virtual void renderStudyExplanationScreen(qint32 screen_index){Q_UNUSED(screen_index)}

    // This will make the next trial after loopValue be 0, effectively putting the study in a cycle.
    void setTrialCountLoopValue(qint32 loopValue);

    // Some studies have the trial counter control outside, so we need to get this value.
    qint32 getLoopValue() const;

    QPixmap getImage() const;
    QImage getQImage() const;
    QString getError() const {return error;}

    QGraphicsScene * getCanvas() {return canvas;}

    QString getVersion() const { return versionString; }

    QList<QStringList> getExpectedIDs() const { return expectedIDs; }

protected:

    void updateGazePosition();
    void clearCanvas();

    qreal ScreenResolutionWidth;
    qreal ScreenResolutionHeight;

    QStringList explanationText;

    // Enable on-screen gaze tracking
    bool gazeUpdateEnabled; // This flag needs to be used to ensure that NO update is done when the ellipse items don´t exist;
    qreal gazeXr,gazeXl,gazeYr,gazeYl;
    QGraphicsEllipseItem *leftEyeTracker;
    QGraphicsEllipseItem *rightEyeTracker;
    qreal R;

    // The expected ids for the experiment
    QList<QStringList> expectedIDs;

    //ConfigurationManager *config;
    QGraphicsScene *canvas;
    QString error;
    QString versionString;

    qint32 trialCountLoopValue;

    // The number of explanation screens and the text key for the explanation screen text. These will be filled by the constructor or each child class.
    QString explanationListTextKey;
    qint32 numberOfExplanationScreens;

//    const char * STUDY_TEXT_KEY_BINDING_UC = "explanation_binding_uc";
//    const char * STUDY_TEXT_KEY_BINDING_BC = "explanation_binding_bc";
//    const char * STUDY_TEXT_KEY_GONOGO     = "explanation_gonogo";
//    const char * STUDY_TEXT_KEY_NBACKRT    = "explanation_nbackrt";
//    const char * STUDY_TEXT_KEY_NBACKVS    = "explanation_nbackvs";

};

#endif // EXPERIMENTDATAPAINTER_H

