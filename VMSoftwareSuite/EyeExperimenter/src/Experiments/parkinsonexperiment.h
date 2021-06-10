#ifndef PARKINSONEXPERIMENT_H
#define PARKINSONEXPERIMENT_H

#include <QTimer>
#include "experiment.h"
#include "../../../CommonClasses/Experiments/parkinsonmanager.h"

#define  PARKINSON_TIMEOUT    40000 //ms

class ParkinsonExperiment: public Experiment
{

public:
    ParkinsonExperiment(QWidget *parent = nullptr, const QString &studyType = "");
    ~ParkinsonExperiment();
    bool startExperiment(const QString &workingDir, const QString &experimentFile,
                         const QVariantMap studyConfig, bool useMouse,
                         QVariantMap pp);
    void togglePauseExperiment();

public slots:
    void newEyeDataAvailable(const EyeTrackerData &data);

private slots:
    void on_timeOut();

protected:
    void keyPressEvent(QKeyEvent *event);

private:

    // Handle to the Reading manager
    ParkinsonManager *m;

    // Time out timer.
    QTimer timer;

    // The pixel values for the end checkout.
    qreal xEnd, yEnd;

    // Flag to avoid saving data
    bool skipEyeData;

    void appendETData(const EyeTrackerData &data);

    // Helper fucntion to do all the stuff that needs to be done when a new maze is drawn.
    void newMaze();

};

#endif // PARKINSONEXPERIMENT_H
