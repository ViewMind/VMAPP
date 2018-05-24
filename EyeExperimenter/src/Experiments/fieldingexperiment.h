#ifndef FIELDINGEXPERIMENT_H
#define FIELDINGEXPERIMENT_H

#include <QTimer>
#include <QSet>
#include <QGraphicsPixmapItem>

#include "experiment.h"
#include "../../CommonClasses/Experiments/fieldingmanager.h"

// Timer timess
#define   TIME_TRANSITION                               50
//#define   TIME_TARGET                                   25
#define   TIME_TARGET                                   20000000000
#define   TIME_CONDITION                                50
#define   TIMER_TIME_INTERVAL                           10

// Possible pauses for the fielding experiment
#define   PAUSE_TRIAL_1                                 32
#define   PAUSE_TRIAL_2                                 64

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

    FieldingExperiment(QWidget *parent = 0);

    // Reimplementation of virtual functions
    bool startExperiment(ConfigurationManager *c);
    void togglePauseExperiment();

public slots:
    void newEyeDataAvailable(const EyeTrackerData &data);
    void onTimeOut();

protected:
    void keyPressEvent(QKeyEvent *event);

private:

    // Handle to the fielding manager.
    FieldingManager *m;

    // Pause for the middle of the experiments.
    bool Paused;

    // The current state of the experiment
    TrialStateFielding tstate;

    // Variables to define which sequence and which image to show
    qint32 currentTrial;
    qint32 currentImage;

    // The timer and the counter.
    QTimer *stateTimer;
    qint32 timerCounter;

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

    // Convenience function to transform the currentImage index to a number between 1 and 6 representing the sequence
    qint32 currentImageToImageIndex();

    // Shows in screen the message "Presione una tecla para seguir" for 32 and 64 trials pauses
    void drawPauseImage();
    
};

#endif // FIELDINGEXPERIMENT_H
