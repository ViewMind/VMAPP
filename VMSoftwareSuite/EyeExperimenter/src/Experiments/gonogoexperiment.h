#ifndef GONOGOEXPERIMENT_H
#define GONOGOEXPERIMENT_H

#include <QTimer>
#include <QSet>

#include "experiment.h"
#include "../../../CommonClasses/Experiments/gonogomanager.h"
#include "../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/movingwindowalgorithm.h"

#define  GONOGO_TIME_CROSS      1000
#define  GONOGO_TIME_ESTIMULUS  2000

class GoNoGoExperiment: public Experiment
{
public:
    GoNoGoExperiment(QWidget *parent = nullptr, const QString &studyType = "");

    // Reimplementation of virtual functions
    bool startExperiment(const QString &workingDir, const QString &experimentFile,
                         const QVariantMap &studyConfig,
                         bool useMouse, QVariantMap pp) override;

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

    // For fixation recognition.
    MovingWindowAlgorithm rMWA;
    MovingWindowAlgorithm lMWA;

    QVariantMap setGoNoGoTargetBoxes(QVariantMap pp);
    bool addNewTrial();


};

#endif // GONOGOEXPERIMENT_H
