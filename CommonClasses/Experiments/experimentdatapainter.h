#ifndef EXPERIMENTDATAPAINTER_H
#define EXPERIMENTDATAPAINTER_H

//#include <QGraphicsScene>
//#include <QGraphicsEllipseItem>
#include <QPainter>
//#include <QApplication>
#include "../RenderServerClient/RenderServerPackets/renderserverpacket.h"
#include "../RenderServerClient/RenderServerGraphics/renderservercircleitem.h"
#include "../RenderServerClient/RenderServerGraphics/renderserverscene.h"
#include "../RenderServerClient/RenderServerGraphics/renderserverarrowitem.h"
#include "../RenderServerClient/RenderServerGraphics/renderserverlineitem.h"
#include "../RenderServerClient/RenderServerGraphics/renderserverrectitem.h"
#include "../RenderServerClient/RenderServerGraphics/renderservertextitem.h"
#include "../RenderServerClient/RenderServerGraphics/renderserverscene.h"
#include "../ConfigurationManager/configurationmanager.h"
#include "../eyetracker_defines.h"
#include "../debug.h"

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

    // Required for properly cycling through the explanation screen in a generic manner.
    qint32 getNumberOfStudyExplanationScreens() const;
    QString getStringKeyForStudyExplanationList() const;

    // Sets the flag so that each study can finalized after a very reduce set of studies.
    void enableShortStudyMode();

    // Renders the study explanation screen according to index.
    virtual void renderStudyExplanationScreen(qint32 screen_index){Q_UNUSED(screen_index)}

    // This will make the next trial after loopValue be 0, effectively putting the study in a cycle.
    void setTrialCountLoopValue(qint32 loopValue);

    // Some studies have the trial counter control outside, so we need to get this value.
    qint32 getLoopValue() const;

    RenderServerScene getImage() const;

    QString getError() const {return error;}

    //QGraphicsScene * getCanvas() {return canvas;}
    RenderServerScene *getCanvas() {return &canvas;}

    QString getVersion() const { return versionString; }

    QList<QStringList> getExpectedIDs() const { return expectedIDs; }

protected:

    void updateGazePosition();
    void clearCanvas();

    qreal ScreenResolutionWidth;
    qreal ScreenResolutionHeight;

    QStringList explanationText;

    // The expected ids for the experiment
    QList<QStringList> expectedIDs;

    RenderServerScene canvas;
    QString error;
    QString versionString;

    qint32 trialCountLoopValue;

    // Check if short mode is enabled.
    bool shortModeEnabled;

    // The number of explanation screens and the text key for the explanation screen text. These will be filled by the constructor or each child class.
    QString explanationListTextKey;
    qint32 numberOfExplanationScreens;

    // Different text explanation keys based on different studies.
    const char * STUDY_TEXT_KEY_BINDING_UC = "explanation_phase_list_binding_uc";
    const char * STUDY_TEXT_KEY_BINDING_BC = "explanation_phase_list_binding_bc";
    const char * STUDY_TEXT_KEY_GONOGO     = "explanation_phase_list_gonogo";
    const char * STUDY_TEXT_KEY_GONOGO_3D  = "explanation_phase_list_gonogo_3D";
    const char * STUDY_TEXT_KEY_NBACKRT    = "explanation_phase_list_nbackrt";
    const char * STUDY_TEXT_KEY_NBACKVS    = "explanation_phase_list_nbackvs";

    const qint32 NUMBER_OF_TRIALS_IN_SHORT_MODE = 5;

};

#endif // EXPERIMENTDATAPAINTER_H

