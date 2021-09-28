#ifndef IMAGEEXPERIMENT_H
#define IMAGEEXPERIMENT_H

#include <QTimer>
#include <QElapsedTimer>
#include <QGraphicsPixmapItem>

//#include "timer.h"
#include "../experiment.h"
#include "bindingmanager.h"

// Timer constants
#define   TIME_START_CROSS                              2000//250
#define   TIME_IMAGE_1                                  2000
#define   TIME_WHITE_TRANSITION                         900
#define   TIME_IMAGE_2_TIMEOUT                          10000
#define   TIME_FINISH                                   1000

class ImageExperiment : public Experiment
{
public:

    // State machine states for Binding trials
    typedef enum {TSB_CENTER_CROSS,TSB_SHOW,TSB_TRANSITION,TSB_TEST,TSB_FINISH} TrialStateBinding;

    ImageExperiment(QWidget *parent = nullptr, const QString &study_type = "");
    ~ImageExperiment() override;

    // Reimplementation of virtual functions
    bool startExperiment(const QString &workingDir, const QString &experimentFile,
                         const QVariantMap &studyConfig) override;

public slots:
    void newEyeDataAvailable(const EyeTrackerData &data) override;
    //void onTimeOut();
    // Functions that moves the state machine
    void nextState();

protected:
    void keyPressHandler(int keyPressed) override;

    QString getExperimentDescriptionFile(const QVariantMap &studyConfig) override;
private:

    //QElapsedTimer mtime;

    LogInterface *logger;

    // The Binding manager handle
    BindingManager *m;

    // Used to define a state machine for the experiment
    TrialStateBinding trialState;

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

    // RawDataContainer Management for adding a new trial.
    bool addNewTrial();

    void resetStudy() override;

};

#endif // IMAGEEXPERIMENT_H
