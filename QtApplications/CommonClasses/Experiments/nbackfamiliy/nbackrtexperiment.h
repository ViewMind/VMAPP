#ifndef NBACKRTEXPERIMENT_H
#define NBACKRTEXPERIMENT_H

#include <QTimer>
#include <QSet>

#include "../experiment.h"
#include "fieldingmanager.h"

//#define MANUAL_TRIAL_PASS

class NBackRTExperiment: public Experiment
{
public:

    NBackRTExperiment(QWidget *parent = nullptr, const QString &study_type = "");

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
    // State machine states for Fielding
    typedef enum {TSF_START,
                  TSF_SHOW_TARGET,
                  TSF_SHOW_BLANKS} TrialStateNBackRT;

    struct TrialRecognitionMachine {
        void reset(const QList<qint32> &trialRecogSeq);
        bool isSequenceOver(const Fixation &r, const Fixation &l, FieldingManager *m);
        Fixation rightLastFixation;
        Fixation leftLastFixation;
        QString getMessages() const { return messages.join("\n"); }
    private:
        MovingWindowAlgorithm rMWA;
        MovingWindowAlgorithm lMWA;
        QList<qint32> rTrialRecognitionSequence;
        QList<qint32> lTrialRecognitionSequence;
        QStringList messages;
    };

    struct VariableSpeedAndTargetNumberConfig {
        // Configuration Parameters.
        qint32 minHoldTime;
        qint32 maxHoldTime;
        qint32 stepHoldTime;
        qint32 startHoldTime;
        qint32 numberOfTrialsForChange;

        qint32 numberOfTargets;

        bool wasSequenceCompleted;

        void resetVSStateMachine();
        void adjustSpeed();
        qint32 getCurrentHoldTime() const;

    private:
        qint32 successfulConsecutiveSequences;
        qint32 failedConsecutiveSequences;
        qint32 currentHoldTime;

    };

    // Structure for variable speed version.
    VariableSpeedAndTargetNumberConfig nbackConfig;

    // Handle to the fielding manager. Can be used for this experiment as well.
    FieldingManager *m;

    // The current state of the experiment
    TrialStateNBackRT tstate;

    // Variables to define which sequence and which image to show
    qint32 currentTrial;
    qint32 currentImage;
    QString currentDataSetType;

    // Used as a state machine to count down correct hits, for the follow through logic.
    TrialRecognitionMachine trialRecognitionMachine;

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

    // Timer timess
    static const qint32 TIME_TRANSITION;
    static const qint32 TIME_TARGET;
    static const qint32 TIME_OUT_BLANKS;
    static const qint32 DEFAULT_NUMBER_OF_TARGETS;

    // Possible pauses for the fielding experiment
    static const qint32 PAUSE_TRIAL_1;
    static const qint32 PAUSE_TRIAL_2;

    static const qint32 NBACKVS_MIN_HOLD_TIME;
    static const qint32 NBACKVS_MAX_HOLD_TIME;
    static const qint32 NBACKVS_STEP_HOLD_TIME;
    static const qint32 NBACKVS_START_HOLD_TIME;
    static const qint32 NBACKVS_NTRIAL_FOR_STEP_CHANGE;

    QVariantMap addHitboxesToProcessingParameters(QVariantMap pp);
    void nextEncodingDataSetType();
    bool addNewTrial();

    void resetStudy() override;

};

#endif // NBACKRTEXPERIMENT_H
