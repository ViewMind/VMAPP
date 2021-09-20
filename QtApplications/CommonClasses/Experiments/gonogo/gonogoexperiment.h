#ifndef GONOGOEXPERIMENT_H
#define GONOGOEXPERIMENT_H

#include <QTimer>
#include <QSet>

#include "../experiment.h"
#include "gonogomanager.h"

//#define DBUG_FIX_TO_MOVE

class GoNoGoExperiment: public Experiment
{
public:
    GoNoGoExperiment(QWidget *parent = nullptr, const QString &study_type = "");

    // Reimplementation of virtual functions
    bool startExperiment(const QString &workingDir, const QString &experimentFile,
                         const QVariantMap &studyConfig,
                         bool useMouse) override;

public slots:
    void newEyeDataAvailable(const EyeTrackerData &data) override;
    void onTimeOut();

protected:

    void keyPressHandler(int keyPressed) override;

    QString getExperimentDescriptionFile(const QVariantMap &studyConfig) override;

    typedef enum {GNGS_CROSS, GNGS_ESTIMULUS} GoNoGoState;

    // Handle to the GoNoGo manager.
    GoNoGoManager *m;
    QTimer stateTimer;
    GoNoGoState gngState;

    // When to ignore data.
    bool ignoreData;

    // Constants
    static const qint32 GONOGO_TIME_CROSS     = 1000;
    static const qint32 GONOGO_TIME_ESTIMULUS = 4000;

    QVariantMap setGoNoGoTargetBoxes(QVariantMap pp);
    bool addNewTrial();

#ifdef DBUG_FIX_TO_MOVE
    QElapsedTimer mtimer;
    QElapsedTimer totalTimer;
#endif


};

#endif // GONOGOEXPERIMENT_H