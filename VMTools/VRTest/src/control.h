#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include "../../../CommonClasses/OpenVRControlObject/openvrcontrolobject.h"
#include "CalibrationTargets.h"
#include "viveeyepoller.h"
#include "../../../VMSoftwareSuite/EyeExperimenter/src/Experiments/experiment.h"
#include "../../../VMSoftwareSuite/EyeExperimenter/src/Experiments/readingexperiment.h"
#include "../../../CommonClasses/common.h"

class Control : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QImage image READ image NOTIFY newImageAvailable)

public:
    explicit Control(QObject *parent = nullptr);

    Q_INVOKABLE void startCalibration();
    Q_INVOKABLE bool isCalibrated();
    Q_INVOKABLE void testEyeTracking();
    Q_INVOKABLE void keyboardKeyPressed(int key);
    Q_INVOKABLE void startReadingExperiment(QString lang);

    // The image to be shown.
    QImage image() const;

signals:
    void calibrationFinished();
    void newImageAvailable();

public slots:
    void onCalibrationTimerTimeout();
    void onRequestUpdate();
    //void onExperimentImageChanged();
    void onExperimentFinished(const Experiment::ExperimentResult &result);

private:

    typedef enum { RENDERING_NONE, RENDERING_TARGET_TEST, RENDERING_EXPERIMENT } RenderState;

    OpenVRControlObject *openvrco;
    CalibrationTargets tt;

    static const qint32 CALIBRATION_WAIT = 1000; // Waits 1 second for transition
    static const qint32 CALIBRATION_SHOW = 2000; // Gathers data for 3 seconds.

    QTimer calibrationTimer;
    QList<QPoint> calibrationPoints;
    qint32 calibrationPointIndex;
    bool isWaiting;
    RenderState renderState;
    bool calibrationPassed;

    VIVEEyePoller eyetracker;

    QImage displayImage;
    QImage generateHMDImage();

    // Experiments
    ConfigurationManager configExperiments;
    Experiment *experiment;

};

#endif // CONTROL_H
