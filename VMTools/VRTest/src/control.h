#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include "../../../CommonClasses/OpenVRControlObject/openvrcontrolobject.h"
#include "../../../VMSoftwareSuite/EyeExperimenter/src/EyeTrackerInterface/HTCVIVEEyePro/htcviveeyeproeyetrackinginterface.h"
#include "../../../VMSoftwareSuite/EyeExperimenter/src/EyeTrackerInterface/Mouse/mouseinterface.h"
#include "../../../VMSoftwareSuite/EyeExperimenter/src/Experiments/experiment.h"
#include "../../../VMSoftwareSuite/EyeExperimenter/src/Experiments/readingexperiment.h"
#include "../../../VMSoftwareSuite/EyeExperimenter/src/Experiments/imageexperiment.h"
#include "../../../VMSoftwareSuite/EyeExperimenter/src/Experiments/fieldingexperiment.h"
#include "../../../VMSoftwareSuite/EyeExperimenter/src/Experiments/gonogoexperiment.h"
#include "../../../VMSoftwareSuite/EyeExperimenter/src/Experiments/nbackrtexperiment.h"
#include "../../../VMSoftwareSuite/EyeExperimenter/src/Experiments/perceptionexperiment.h"
#include "../../../CommonClasses/common.h"

// When enabling design mode. the mouse will be automatically selected as an EyeTracker and full screen will be used for experiment selection.
#define DESIGN_MODE_ENABLED

class Control : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QImage image READ image NOTIFY newImageAvailable)

public:
    explicit Control(QObject *parent = nullptr);

    Q_INVOKABLE void startCalibration();
    Q_INVOKABLE void testEyeTracking();
    Q_INVOKABLE void keyboardKeyPressed(int key);
    Q_INVOKABLE void startReadingExperiment(QString lang);
    Q_INVOKABLE void startBindingExperiment(bool isBound,qint32 targetNum, bool areTargetsSmall);
    Q_INVOKABLE void startFieldingExperiment();
    Q_INVOKABLE void startGoNoGoExperiment();
    Q_INVOKABLE void startNBackRTExperiment(qint32 numTargets, qint32 nbacktype);
    Q_INVOKABLE void startPerceptionExperiment(bool isTraining);
    Q_INVOKABLE void initialize();
    Q_INVOKABLE void loadLastCalibration();
    Q_INVOKABLE void loadViewMindWaitScreen();

    // The image to be shown.
    QImage image() const;

signals:
    void calibrationFinished();
    void newImageAvailable();

public slots:

    void onRequestUpdate();
    //void onExperimentImageChanged();
    void onExperimentFinished(const Experiment::ExperimentResult &result);
    void onEyeTrackerControl(quint8);

private:

    typedef enum { RENDERING_NONE, RENDERING_TARGET_TEST, RENDERING_EXPERIMENT, RENDER_WAIT_SCREEN } RenderState;

    OpenVRControlObject *openvrco;
    CalibrationTargets tt;
#ifndef DESIGN_MODE_ENABLED
    HTCViveEyeProEyeTrackingInterface *eyetracker;
#else
    MouseInterface *eyetracker;
#endif

    RenderState renderState;
    QImage displayImage;
    QImage generateHMDImage();

    // Experiments
    ConfigurationManager configExperiments;
    Experiment *experiment;

};

#endif // CONTROL_H
