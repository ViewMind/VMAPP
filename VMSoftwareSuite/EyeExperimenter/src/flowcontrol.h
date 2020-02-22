#ifndef FLOWCONTROL_H
#define FLOWCONTROL_H

#include <QObject>
#include <QWidget>
#include <QProcess>
#include <QtMath>
#include <QFileDialog>
#include <QScreen>

#include <iostream>

#include "../../../CommonClasses/common.h"
#include "../../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../../CommonClasses/DatFileInfo/datfileinfoindir.h"
#include "../../../CommonClasses/PNGWriter/repfileinfo.h"
#include "../../../CommonClasses/PNGWriter/imagereportdrawer.h"
#include "../../../CommonClasses/OpenVRControlObject/openvrcontrolobject.h"

#include "Experiments/readingexperiment.h"
#include "Experiments/imageexperiment.h"
#include "Experiments/fieldingexperiment.h"
#include "Experiments/nbackrtexperiment.h"

#include "sslclient/ssldataprocessingclient.h"

#include "EyeTrackerInterface/Mouse/mouseinterface.h"
#include "EyeTrackerInterface/GazePoint/opengazeinterface.h"
#include "EyeTrackerInterface/HTCVIVEEyePro/htcviveeyeproeyetrackinginterface.h"

#include "monitorscreen.h"
#include "uiconfigmap.h"

#ifdef USE_IVIEW
#include "EyeTrackerInterface/RED/redinterface.h"
#endif

class FlowControl : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QImage image READ image NOTIFY newImageAvailable)

public:
    explicit FlowControl(QWidget *parent = Q_NULLPTR, ConfigurationManager *c = nullptr, UIConfigMap *ui = nullptr);
    ~FlowControl();
    Q_INVOKABLE void connectToEyeTracker();
    Q_INVOKABLE void calibrateEyeTracker();
    Q_INVOKABLE bool startNewExperiment(qint32 experimentID);
    Q_INVOKABLE bool isConnected() const { return connected; }
    Q_INVOKABLE bool isCalibrated() const;
    Q_INVOKABLE bool isRightEyeCalibrated() const;
    Q_INVOKABLE bool isLeftEyeCalibrated() const;
    Q_INVOKABLE bool isExperimentEndOk() const {return experimentIsOk;}
    Q_INVOKABLE bool areThereFrequencyErrorsPresent() const {return frequencyErrorsPresent;}
    Q_INVOKABLE void setupSecondMonitor();
    Q_INVOKABLE void eyeTrackerChanged();
    Q_INVOKABLE void resolutionCalculations();
    Q_INVOKABLE bool checkSSLAvailability() {return sslDataProcessingClient->sslEnabled();}
    Q_INVOKABLE void requestReportData();
    Q_INVOKABLE void sendMedicalRecordsToServer(const QString &patid);
    Q_INVOKABLE bool isSSLTransactionOK() const {return sslTransactionAllOk;}
    Q_INVOKABLE void saveReport();
    Q_INVOKABLE void saveReportAs(const QString &title);
    Q_INVOKABLE void startDemoTransaction();
    Q_INVOKABLE void prepareForReportListIteration(const QString &patientDirectory);
    Q_INVOKABLE QVariantMap nextReportInList();
    Q_INVOKABLE void setReportIndex(qint32 id) { selectedReport = id; }
    Q_INVOKABLE void prepareSelectedReportIteration();
    Q_INVOKABLE QVariantMap nextSelectedReportItem();
    Q_INVOKABLE QStringList getSelectedReportInfo();
    Q_INVOKABLE qint32 getSSLTransactionError() {return sslDataProcessingClient->getProcessingCode();}
    Q_INVOKABLE void moveFileToArchivedFileFolder(const QString &filename);
    Q_INVOKABLE void doFrequencyAnalysis(const QString &filename);
    Q_INVOKABLE void requestDataReprocessing(const QString &reportName, const QString &fileList, const QString &evaluationID);
    Q_INVOKABLE qint32 numberOfEvaluationsReceived() { return sslDataProcessingClient->getNumberOfEvaluations(); }
    Q_INVOKABLE QStringList getDiagnosticClass();
    Q_INVOKABLE void keyboardKeyPressed(int key);
    Q_INVOKABLE void stopRenderingVR();
    Q_INVOKABLE void generateWaitScreen(const QString &message);

    // The image to be shown.
    QImage image() const;

signals:

    // This tells QML that the experiment has finished.
    void experimentHasFinished();

    // Transaction finished.
    void sslTransactionFinished();

    // Signals to indicate all is good.
    void connectedToEyeTracker(bool ok);
    void calibrationDone(bool ok);

    // Requesting file set to process.
    void requestFileSet();

    // Used to signal the UI to open the dialog to wait for the report generation.
    void reportGenerationRequested();
    void reportGenerationDone();

    // Signal to update QML Image on screen.
    void newImageAvailable();

public slots:

    // When an experiment finishes.
    void on_experimentFinished(const Experiment::ExperimentResult & er);

    // When an SSL Transaction finishes
    void onDisconnectionFinished();

    // Eye tracker control changes
    void onEyeTrackerControl(quint8 code);

    // For receiving information to send to the server
    void onFileSetEmitted(const QStringList &fileSetAndName, const QString &evaluationID);

private slots:

    // The function that actually draws the report
    void drawReport();

    // Slot that requests new image to draw from the OpenVR Control Object
    void onRequestUpdate();

private:

    // Render state allows to define what to send to the HMD when using the VR Solution.
    typedef enum { RENDERING_NONE, RENDERING_EXPERIMENT, RENDER_WAIT_SCREEN} RenderState;

    // Delays saving the report until the wait dialog can be shown.
    QTimer delayTimer;

    // The second screen for monitoring the experiments.
    MonitorScreen *monitor;

    // The currently selected experiment
    Experiment *experiment;

    // The currently selected eyetracker
    EyeTrackerInterface *eyeTracker;

    // Open VR Control object if VR is ENABLED.
    OpenVRControlObject *openvrco;

    // Definining the render state for VR
    RenderState renderState;

    // Display image used when using VR Solution. And the Wait Screen.
    QImage displayImage;
    QImage waitScreen;
    QImage logo;  // the viewmind logo.
    QFont waitFont;
    QColor waitScreenBaseColor;

    // The Log interface
    LogInterface logger;

    // The sslclient to send the information process request.
    SSLDataProcessingClient *sslDataProcessingClient;

    // The configuration structure
    ConfigurationManager *configuration;

    // The UI configuration map
    UIConfigMap *uimap;

    // The list of files generated in the current experiment run.
    QStringList currentRunFiles;

    // The report list for a given directory and required selection index and iteration values.
    qint32 selectedReport;
    qint32 selectedReportItemIterator;
    RepFileInfo reportsForPatient;
    QList<QVariantMap> reportItems;
    QStringList reportTextDataIndexes;
    QStringList diagnosisClassText;
    ResultBar::ResultBarCodes resultBarSummary;

    // Flags to avoid reconnecting during recalibration
    bool connected;

    // Flag to check if the eyetracker is calibrated or not. And if not which eye failed.
    bool calibrated;
    bool rightEyeCalibrated;
    bool leftEyeCalibrated;

    // Binary status for the end of an experiment.
    bool experimentIsOk;

    // Flag to indicate frequency errors and use the appropiate message.
    bool frequencyErrorsPresent;

    // Flag to check on transaction status
    bool sslTransactionAllOk;

    // Flag used to generate demo transaction
    bool demoTransaction;

    // Flag to indicate transaction is reprocessing previous data.
    bool reprocessRequest;

    // The patient id whose medical record got synched up.
    QString patientIDMedicalRecordUpdate;

    // Helper function to selecte expanded binding files.
    QString getBindingExperiment(bool bc);

    // Add data to report items
    void addToReportItems(const QStringList &items, const QVariantMap &report, const QStringList &titles, const QStringList &explanations, const QStringList &references);

    // The country codes and the function to load them
    QStringList countryList;
    QStringList countryCodes;
    void fillCountryList();

    // Moves successfully processed files to the corresponding processed folder.
    QStringList fileSetSentToProcess;
    void moveProcessedFilesToProcessedFolder(const QStringList &fileSet);

};

#endif // FLOWCONTROL_H
