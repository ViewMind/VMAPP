#ifndef EXPERIMENT_H
#define EXPERIMENT_H

#include <QWidget>
#include <QDialog>
#include <QVariantHash>
#include <QDataStream>
#include <QFile>
#include <QFileInfo>
#include <QKeyEvent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QDebug>
#include <QMessageBox>
#include "../monitorscreen.h"
#include "../EyeTrackerInterface/eyetrackerdata.h"
#include "../../../CommonClasses/Experiments/experimentdatapainter.h"
#include "../../../CommonClasses/DataAnalysis/FrequencyAnalsis/freqanalysis.h"
#include "../../../CommonClasses/common.h"

class Experiment : public QWidget
{
    Q_OBJECT
public:

    // The constructor will set up the basic background view for the widget.
    explicit Experiment(QWidget *parent = nullptr);

    // Used to determine how the experiment
    typedef enum {ER_NORMAL, ER_ABORTED, ER_FAILURE, ER_WARNING} ExperimentResult;

    // Used to determine the state of the experiment
    typedef enum {STATE_RUNNING, STATE_PAUSED, STATE_STOPPED} ExperimentState;

    // This is starts the experiment in a basically autonomous way.
    virtual bool startExperiment(ConfigurationManager *c);

    // Used to stop the experiment for any reason, like calibration
    virtual void togglePauseExperiment();

    // To verify that the data was gathered correctly.
    bool doFrequencyCheck();

    // To obtain the experiment state
    ExperimentState getExperimentState() const {return state;}

    // Getting the error
    QString getError() const {return error;}

    // Getting the generated data file
    QString getDataFileLocation() const {return dataFile;}

    // Enable and disable the debug mode
    void setDebugMode(bool enable){debugMode = enable;}

    // Whenever not in mouse mode the cursor should be hidden
    void hideCursor() {this->setCursor(Qt::BlankCursor);}

    // Keyboard press handling function
    void keyboardKeyPressed(int keyboardKey);

    // FOR DEBUGGING ONLY
    void setupFreqCheck(ConfigurationManager *c, QString fileName) { dataFile = fileName; config = c; }

    // Image required for VR Display
    QImage getVRDisplayImage() const;

signals:

    // Should be emitted when the experiment is finalized
    void experimentEndend(const ExperimentResult &result);   

    // This method will be used for calibration request in mid-experiment.
    // When called the receiving slot will call pauseExperiment.
    void calibrationRequest();

    // Used for drawing images and following the eyes in the second monitor.
    void updateBackground(const QPixmap &pixmap);    
    void updateEyePositions(qint32 rx, qint32 ry, qint32 lx, qint32 ly);

    // Signal that provides VR with the image to show.
    void updateVRDisplay();


public slots:

    // This slot will receive the new eye tracker data. The implementation should decide what to do with it.
    virtual void newEyeDataAvailable(const EyeTrackerData &data);

protected:

    // The manager for the experiment
    ExperimentDataPainter *manager;

    // The configuration structure
    ConfigurationManager *config;

    // The pointer to the GraphicsView where the drawing and showing will take place.
    QGraphicsView *gview;

    // Deterimine if the experiment is running
    ExperimentState state;

    // The experiment header to save to the data file
    QString expHeader;

    // The base name for the data output file
    QString outputDataFile;

    // The formatted data received from the ET.
    QVariantList etData;

    // Error message
    QString error;

    // Where the data file will be stored and any other required image.
    QString workingDirectory;

    // Where the data from the experiment will be stored
    QString dataFile;

    // Flag used to indicate that debug mode is enabled. To be used to print auxiliary data.
    bool debugMode;

    // Sets up the view given the configuration
    void setupView();

    // Steps for aborted experiment
    void experimenteAborted();

    // Move the garbage data file to the aborted directory, returns the new file name, or empty if there was an error
    QString moveDataFileToAborted();

    // This will change the extension of a dat file to a datf file. This will mark the file a having had frequency errors.
    QString changeFileExtensionToDatF();

    // Saving the data to the disk.
    bool saveDataToHardDisk();

    // Control flag for VR logic changes
    bool vrEnabled;

    // So that the logic of handling keys can be implemente by each experiment.
    virtual void keyPressHandler(int keyPressed);

    void keyPressEvent(QKeyEvent *event) override;

    // Update Image or HMD Logic is the same for all experiments
    void updateSecondMonitorORHMD();

};

#endif // EXPERIMENT_H
