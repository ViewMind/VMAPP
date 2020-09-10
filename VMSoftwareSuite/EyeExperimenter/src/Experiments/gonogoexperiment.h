#ifndef GONOGOEXPERIMENT_H
#define GONOGOEXPERIMENT_H

#include <QTimer>
#include <QSet>

#include "experiment.h"
#include "../../../CommonClasses/Experiments/gonogomanager.h"
#include "../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/movingwindowalgorithm.h"

#define  GONOGO_TIME_CROSS      200
#define  GONOGO_TIME_ESTIMULUS  1000

class GoNoGoExperiment: public Experiment
{
public:
    GoNoGoExperiment(QWidget *parent = nullptr);

    // Reimplementation of virtual functions
    bool startExperiment(ConfigurationManager *c) override;
    void togglePauseExperiment() override;

public slots:
    void newEyeDataAvailable(const EyeTrackerData &data) override;
    void onTimeOut();

protected:
    void keyPressHandler(int keyPressed) override;

    typedef enum {GNGS_CROSS, GNGS_ESTIMULUS} GoNoGoState;

    // Handle to the GoNoGo manager.
    GoNoGoManager *m;
    QTimer stateTimer;
    GoNoGoState gngState;

    // When to ignore data.
    bool ignoreData;

    // Separator for each trial.
    void addTrialHeader();

    // For fixation recognition.
    MovingWindowAlgorithm rMWA;
    MovingWindowAlgorithm lMWA;


};

#endif // GONOGOEXPERIMENT_H
