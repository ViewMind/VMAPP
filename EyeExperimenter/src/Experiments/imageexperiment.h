#ifndef IMAGEEXPERIMENT_H
#define IMAGEEXPERIMENT_H

#include <QTimer>
#include <QElapsedTimer>
#include <QGraphicsPixmapItem>

//#include "timer.h"
#include "experiment.h"
#include "../../CommonClasses/Experiments/bindingmanager.h"

// Timer constants
#define   TIME_START_CROSS                              250
#define   TIME_IMAGE_1                                  2000
#define   TIME_WHITE_TRANSITION                         900
#define   TIME_IMAGE_2_TIMEOUT                          10000
#define   TIME_FINISH                                   1000

class ImageExperiment : public Experiment
{
public:

    // State machine states for Binding trials
    typedef enum {TSB_CENTER_CROSS,TSB_SHOW,TSB_TRANSITION,TSB_TEST,TSB_FINISH} TrialStateBinding;

    ImageExperiment(bool bound, bool use3Targets , QWidget *parent = 0);
    ~ImageExperiment();

    // Reimplementation of virtual functions
    bool startExperiment(ConfigurationManager *c);
    void togglePauseExperiment();

public slots:
    void newEyeDataAvailable(const EyeTrackerData &data);
    //void onTimeOut();
    // Functions that moves the state machine
    void nextState();

protected:
    void keyPressEvent(QKeyEvent *event);

private:

    //QElapsedTimer mtime;

    LogInterface *logger;

    // The Binding manager handle
    BindingManager *m;

    // Used to define a state machine for the experiment
    TrialStateBinding trialState;

    // Check if the experiment is bound or unbound
    bool isBound;

    // This will indicate the current image.
    qint32 currentTrial;
    bool atLast;

    // The timer to automatically go from trial to test image.
    QTimer stateTimer;
    //qint32 timerCounter;

    // Flag use to sincronize data writing to the file.
    bool ignoreData;

    // Variable used to define answer to experiment, based on keypress
    QString answer;

    // Function to draw the next image.
    void drawCurrentImage();

    // Advance to the next image
    bool advanceTrial();

    // Functions to append data to the file.
    void newImage(QString name, qint32 isTrial);
    void addAnswer(QString ans = "");

};

#endif // IMAGEEXPERIMENT_H
