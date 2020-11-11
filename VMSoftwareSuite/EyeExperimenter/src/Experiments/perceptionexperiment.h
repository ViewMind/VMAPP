#ifndef PERCEPTIONEXPERIMENT_H
#define PERCEPTIONEXPERIMENT_H

#include <QTimer>
#include <QSet>

#include "experiment.h"
#include "../../../CommonClasses/Experiments/perceptionmanager.h"
#include "../../../CommonClasses/EyeSelector/eyeselector.h"

#define  HOLD_TIME_FOR_ANS_SELECTION 1     // Seconds
#define  HOLD_TIME_CROSS_ONLY        500   // ms.
#define  HOLD_TIME_YES_NO_REHAB      3000  // ms.
#define  HOLD_TIME_YES_NO_TRAIN      2000  // ms.
#define  HOLD_TIME_TRI_REHAB         700   // ms
#define  HOLD_TIME_TRI_TRAIN         300   // ms

class PerceptionExperiment: public Experiment
{
public:
    PerceptionExperiment(QWidget *parent = nullptr);

    // Reimplementation of virtual functions
    bool startExperiment(ConfigurationManager *c) override;
    void togglePauseExperiment() override;

public slots:
    void newEyeDataAvailable(const EyeTrackerData &data) override;
    void onTimeOut();

protected:
    void keyPressHandler(int keyPressed) override;

private:

    // Handle to the percepetion manager.
    PerceptionManager *m;

    // The timer and how much time to hold for each part of the trial
    QTimer stateTimer;

    qint32 holdTimeYesAndNo;
    qint32 holdTimeTriangles;

    // Variable to check if answer was set.
    bool wasAnswerSet;

    // To select the yes or no
    EyeSelector eyeSelector;

};

#endif // PERCEPTIONEXPERIMENT_H
