#ifndef FIELDINGEXPERIMENT_H
#define FIELDINGEXPERIMENT_H

#include <QTimer>
#include <QSet>

#include "../experiment.h"
#include "fieldingmanager.h"

class FieldingExperiment : public Experiment
{
public:

    // State machine states for Fielding
    typedef enum {TSF_START,
                  TSF_SHOW_DOT_1,
                  TSF_SHOW_DOT_2,
                  TSF_SHOW_DOT_3,
                  TSF_TRANSITION,
                  TSF_SHOW_BLANK_3,
                  TSF_SHOW_BLANK_2,
                  TSF_SHOW_BLANK_1} TrialStateFielding;

    FieldingExperiment(QWidget *parent = nullptr, const QString &study_type = "");

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

    // Handle to the fielding manager.
    FieldingManager *m;

    // The current state of the experiment
    TrialStateFielding tstate;

    // Variables to define which sequence and which image to show
    qint32 currentTrial;
    qint32 currentImage;

    QString currentDataSetType;

    // The timer and the counter.
    QTimer stateTimer;

    // Flag use to sincronize data writing to the file.
    bool ignoreData;

    // Advancing to the next state
    void nextState();

    // Drawing the current image
    void drawCurrentImage();

    // Last steps if experiment has ended. Returns false if the experiment is still going.
    bool finalizeExperiment();

    // Shows in screen the message "Presione una tecla para seguir" for 32 and 64 trials pauses
    void drawPauseImage();

    QVariantMap addHitboxesToProcessingParameters(QVariantMap pp);
    bool addNewTrial();

    // Timer timess
    static const qint32 TIME_TRANSITION;
    static const qint32 TIME_TARGET;
    static const qint32 TIME_CONDITION;
    //static const qint32 TIMER_TIME_INTERVAL                           10

    // Possible pauses for the fielding experiment
    static const qint32 PAUSE_TRIAL_1;
    static const qint32 PAUSE_TRIAL_2;

    void resetStudy() override;

    void updateFrontEndMessages();
    
};

#endif // FIELDINGEXPERIMENT_H
