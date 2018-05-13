#ifndef IMAGEEXPERIMENT_H
#define IMAGEEXPERIMENT_H

#include <QTimer>
#include <QGraphicsPixmapItem>

#include "experiment.h"
#include "../../CommonClasses/Experiments/bindingmanager.h"

class ImageExperiment : public Experiment
{
public:
    ImageExperiment(bool bound, QWidget *parent = 0);
    ~ImageExperiment();

    // Reimplementation of virtual functions
    bool startExperiment(ConfigurationManager *c);
    void togglePauseExperiment();

public slots:
    void newEyeDataAvailable(const EyeTrackerData &data);
    void onTimeOut();

protected:
    void keyPressEvent(QKeyEvent *event);

private:

    // The Binding manager handle
    BindingManager *m;

    // Used to define a state machine for the experiment
    TrialStateBinding trialState;

    // Check if the experiment is bound or unbound
    bool isBound;

    // This will indicate the current image.
    qint32 currentTrial;

    // The timer to automatically go from trial to test image.
    QTimer *stateTimer;
    qint32 timerCounter;

    // Flag use to sincronize data writing to the file.
    bool ignoreData;

    // Variable used to define answer to experiment, based on keypress
    QString answer;

    // Function to draw the next image.
    void drawCurrentImage();

    // Advance to the next image
    void advanceTrial();

    // Functions that moves the state machine
    void nextState();

    // Functions to append data to the file.
    void newImage(QString name, qint32 isTrial);
    void addAnswer(QString ans = "");

};

#endif // IMAGEEXPERIMENT_H
