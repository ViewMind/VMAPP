#ifndef PERCEPTIONEXPERIMENT_H
#define PERCEPTIONEXPERIMENT_H

#include <QTimer>
#include <QSet>

#include "../../EyeSelector/eyeselector.h"
#include "../experiment.h"
#include "perceptionmanager.h"

class PerceptionExperiment: public Experiment
{
public:
    PerceptionExperiment(QWidget *parent = nullptr, const QString &study_type = "");

    // Reimplementation of virtual functions
    bool startExperiment(const QString &workingDir, const QString &experimentFile,
                         const QVariantMap &studyConfig) override;


public slots:
    void newEyeDataAvailable(const EyeTrackerData &data) override;
    void onTimeOut();

protected:
    void keyPressHandler(int keyPressed) override;
    QString getExperimentDescriptionFile(const QVariantMap &studyConfig) override;

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

    // Adding a new trial to the raw data structure
    bool addNewTrial();

    static const qint32 HOLD_TIME_FOR_ANS_SELECTION;
    static const qint32 HOLD_TIME_CROSS_ONLY;
    static const qint32 HOLD_TIME_YES_NO_REHAB;
    static const qint32 HOLD_TIME_YES_NO_TRAIN;
    static const qint32 HOLD_TIME_TRI_REHAB;
    static const qint32 HOLD_TIME_TRI_TRAIN;

    void resetStudy() override;

};

#endif // PERCEPTIONEXPERIMENT_H
