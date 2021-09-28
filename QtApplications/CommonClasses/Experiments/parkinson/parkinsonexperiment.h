#ifndef PARKINSONEXPERIMENT_H
#define PARKINSONEXPERIMENT_H

#include <QTimer>
#include "../experiment.h"
#include "parkinsonmanager.h"

#define  PARKINSON_TIMEOUT    40000 //ms

class ParkinsonExperiment: public Experiment
{

public:
    ParkinsonExperiment(QWidget *parent = nullptr, const QString &study_type = "");
    ~ParkinsonExperiment() override;
    bool startExperiment(const QString &workingDir, const QString &experimentFile,
                         const QVariantMap &studyConfig) override;
    void togglePauseExperiment();

public slots:
    void newEyeDataAvailable(const EyeTrackerData &data) override;

private slots:
    void on_timeOut();

protected:
    void keyPressEvent(QKeyEvent *event) override;

    QString getExperimentDescriptionFile(const QVariantMap &studyConfig) override;

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


    void resetStudy() override;
};

#endif // PARKINSONEXPERIMENT_H
