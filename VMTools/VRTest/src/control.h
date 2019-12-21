#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include "../../../CommonClasses/OpenVRControlObject/openvrcontrolobject.h"
#include "./EyeTrackerInterface/HTCVIVEEyePro/htcviveeyeproeyetrackinginterface.h"
#include "../../../VMSoftwareSuite/EyeExperimenter/src/Experiments/experiment.h"
#include "../../../VMSoftwareSuite/EyeExperimenter/src/Experiments/readingexperiment.h"
#include "../../../VMSoftwareSuite/EyeExperimenter/src/Experiments/imageexperiment.h"
#include "../../../CommonClasses/common.h"

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
    Q_INVOKABLE void initialize();
    Q_INVOKABLE void loadLastCalibration();

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

    typedef enum { RENDERING_NONE, RENDERING_TARGET_TEST, RENDERING_EXPERIMENT } RenderState;

    OpenVRControlObject *openvrco;
    CalibrationTargets tt;
    HTCViveEyeProEyeTrackingInterface *eyetracker;

    RenderState renderState;
    QImage displayImage;
    QImage generateHMDImage();

    // Experiments
    ConfigurationManager configExperiments;
    Experiment *experiment;

};

#endif // CONTROL_H
