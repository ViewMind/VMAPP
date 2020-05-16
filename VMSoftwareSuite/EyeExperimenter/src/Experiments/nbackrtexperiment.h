#ifndef NBACKRTEXPERIMENT_H
#define NBACKRTEXPERIMENT_H

#include <QTimer>
#include <QSet>

#include "experiment.h"
#include "../../../CommonClasses/Experiments/fieldingmanager.h"
#include "../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/movingwindowalgorithm.h"

//#define MANUAL_TRIAL_PASS

// Timer timess
#define   TIME_TRANSITION                               500
#define   TIME_TARGET                                   250
#define   TIME_OUT_BLANKS                               3000

// Possible pauses for the fielding experiment
#define   PAUSE_TRIAL_1                                 32
#define   PAUSE_TRIAL_2                                 64

class NBackRTExperiment: public Experiment
{
public:
    NBackRTExperiment(QWidget *parent = nullptr);

    // Reimplementation of virtual functions
    bool startExperiment(ConfigurationManager *c) override;
    void togglePauseExperiment() override;

public slots:
    void newEyeDataAvailable(const EyeTrackerData &data) override;
    void onTimeOut();

protected:
    void keyPressHandler(int keyPressed) override;

private:
    // State machine states for Fielding
    typedef enum {TSF_START,
                  TSF_SHOW_DOT_1,
                  TSF_SHOW_DOT_2,
                  TSF_SHOW_DOT_3,
                  TSF_SHOW_BLANKS} TrialStateNBackRT;

    struct TrialRecognitionMachine {
        void configure(MovingWindowParameters mwp);
        void reset(const QList<qint32> &trialRecogSeq);
        bool isSequenceOver(qreal rX, qreal rY, qreal lX, qreal lY, FieldingManager *m, qreal timeStamp);
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

    // Handle to the fielding manager. Can be used for this experiment as well.
    FieldingManager *m;

    // The current state of the experiment
    TrialStateNBackRT tstate;

    // Variables to define which sequence and which image to show
    qint32 currentTrial;
    qint32 currentImage;

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

    // Adding the header for the trial data.
    void addTrialHeader();

    // Shows in screen the message "Presione una tecla para seguir" for 32 and 64 trials pauses
    void drawPauseImage();

};

#endif // NBACKRTEXPERIMENT_H