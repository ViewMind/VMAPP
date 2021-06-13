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
#include "../../../CommonClasses/RawDataContainer/viewminddatacontainer.h"
#include "../../../CommonClasses/LogInterface/loginterface.h"
#include "../eyexperimenter_defines.h"

//#define  MULTI_PART_FILE_STUDY_IDENTIFIER      "===>>"
//#define  EXTRA_EXTENSION_FOR_MULTI_PART_FILES  "mp"

//#define  PERCEPTION_STUDY_LAST_MULTI_PART_IDENTIFIER  "7"   // This means that the study will end upon doing it 8 times (0 through 8).

class Experiment : public QWidget
{
    Q_OBJECT
public:

    // The constructor will set up the basic background view for the widget.
    explicit Experiment(QWidget *parent = nullptr, const QString &studyType = "");

    // Used to determine how the experiment
    typedef enum {ER_NORMAL, ER_ABORTED, ER_FAILURE, ER_WARNING} ExperimentResult;

    // Used to determine the state of the experiment
    typedef enum {STATE_RUNNING, STATE_PAUSED, STATE_STOPPED} ExperimentState;

    // This is starts the experiment in a basically autonomous way.
    virtual bool startExperiment(const QString &workingDir,
                                 const QString &experimentFile,
                                 const QVariantMap &studyConfig,
                                 bool useMouse,
                                 QVariantMap pp);

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
    void addFixations(qint32 rx, qint32 ry, qint32 lx, qint32 ly);
    void addDebugMessage(const QString &message, bool append);

    // Signal that provides VR with the image to show.
    void updateVRDisplay();


public slots:

    // This slot will receive the new eye tracker data. The implementation should decide what to do with it.
    virtual void newEyeDataAvailable(const EyeTrackerData &data);

protected:

    // The manager for the experiment
    ExperimentDataPainter *manager;

    // Where the data will be stored.
    ViewMindDataContainer rawdata;

    // The set of processing parameters is checked.
    // QVariantMap proc_params;

    // Cofiguration for the current Study
    QVariantMap studyConfiguration;

    // This is the study type flag which will be set by eaach individual study.
    QString studyType;

    // The pointer to the GraphicsView where the drawing and showing will take place.
    QGraphicsView *gview;

    // Deterimine if the experiment is running
    ExperimentState state;

    // Error message
    QString error;

    // Where the data file will be stored and any other required image.
    QString workingDirectory;

    // Where the data from the experiment will be stored
    QString dataFile;

    // Flag used to indicate that debug mode is enabled. To be used to print auxiliary data.
    bool debugMode;

    // Online fixation computation.
    Fixation lastFixationL;
    Fixation lastFixationR;
    MovingWindowAlgorithm rMWA;
    MovingWindowAlgorithm lMWA;

    // Sets up the view given the configuration
    void setupView(qint32 monitor_resolution_width, qint32 monitor_resolution_height);

    // Steps for aborted experiment
    void experimenteAborted();

    // Move the garbage data file to the aborted directory, returns the new file name, or empty if there was an error
    QString moveDataFileToAborted();

    // Saving the data to the disk.
    bool saveDataToHardDisk();

    // Variables necessary for raw data insertion control.
    QString currentTrialID;

    // So that the logic of handling keys can be implemente by each experiment.
    virtual void keyPressHandler(int keyPressed);

    void keyPressEvent(QKeyEvent *event) override;

    // Update Image or HMD Logic is the same for all experiments
    void updateSecondMonitorORHMD();

    // Takes the study type and configuration and produces the internal experiment description.
    QString getExperimentDescriptionFile() const;

    // Finalizes and restes the state machine for computing online fixations.
    void finalizeOnlineFixations();

    // Feeds data to the online fixation maching. If a fixation is computed, it is stored in the rawdata structure.
    // The last two parameters are reading ONLY.
    void computeOnlineFixations(const EyeTrackerData &data, qreal l_schar = 0, qreal l_word = 0, qreal r_schar = 0, qreal r_word = 0);

    // Transform the fixation struct into a the valid struct expected by the ViewMind Data Container.
    QVariantMap fixationToVariantMap(const Fixation &f);



};

#endif // EXPERIMENT_H
