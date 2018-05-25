#ifndef MAINGUI_H
#define MAINGUI_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QRect>
#include <QApplication>

#include <iostream>

#include "../../CommonClasses/LogInterface/loginterface.h"

#include "Experiments/readingexperiment.h"
#include "Experiments/imageexperiment.h"
#include "Experiments/fieldingexperiment.h"

#include "EyeTrackerInterface/Mouse/mouseinterface.h"
#include "EyeTrackerInterface/RED/redinterface.h"

#include "experimentchooserdialog.h"
#include "eyetrackerselectiondialog.h"
#include "processorthread.h"
#include "monitorscreen.h"
#include "settingsdialog.h"
#include "instructiondialog.h"

namespace Ui {
class MainGUI;
}

class MainGUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainGUI(QWidget *parent = 0);
    ~MainGUI();

private slots:
    // Created slots
    void on_experimentFinished(const Experiment::ExperimentResult & er);

    // GUI Slots
    void on_actionStart_Experiment_triggered();
    void on_actionCalibrate_Eye_Tracker_triggered();
    void on_actionConnect_to_EyeTracker_triggered();
    void on_actionConfigure_triggered();

    // Processing thread finished.
    void on_pthread_finished();

private:
    Ui::MainGUI *ui;

    // The second screen for monitoring the experiments.
    MonitorScreen *monitor;

    // The currently selected experiment
    Experiment *experiment;

    // The currently selected eyetracker
    EyeTrackerInterface *eyeTracker;

    // The Log interface
    LogInterface logger;

    // The configuration structure
    ConfigurationManager configuration;    

    // The sequence of experiments required.
    QList<qint32> experimentSequence;
    qint32 expInSeq;

    // Convenience function to implement the sequence
    void startExperiment();

    // Calls the eye data processor;
    ProcessorThread thread;

    // Used to check that the app is running in the right screen.
    bool checkResolution();

    // Moves this window to the primary screen and the second window to occupy the second screen (if available).
    void setWidgetPositions();

    // Contains all the logic to interpret the expanded version of the Binding experiment
    QString getBindingExperiment(bool bc);

};

#endif // MAINGUI_H
