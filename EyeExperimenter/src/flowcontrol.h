#ifndef FLOWCONTROL_H
#define FLOWCONTROL_H

#include <QObject>
#include <QWidget>
#include <QProcess>
#include <QtMath>
#include <QFileDialog>

#include "../../CommonClasses/common.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../CommonClasses/DatFileInfo/datfileinfoindir.h"
#include "../../CommonClasses/PNGWriter/repfileinfo.h"
#include "../../CommonClasses/PNGWriter/imagereportdrawer.h"

#include "Experiments/readingexperiment.h"
#include "Experiments/imageexperiment.h"
#include "Experiments/fieldingexperiment.h"

#include "sslclient/ssldataprocessingclient.h"

#include "EyeTrackerInterface/Mouse/mouseinterface.h"
#include "EyeTrackerInterface/GazePoint/opengazeinterface.h"

#include "monitorscreen.h"
#include "uiconfigmap.h"

#ifdef USE_IVIEW
#include "EyeTrackerInterface/RED/redinterface.h"
#endif

class FlowControl : public QWidget
{
    Q_OBJECT
public:
    explicit FlowControl(QWidget *parent = Q_NULLPTR, ConfigurationManager *c = nullptr, UIConfigMap *ui = nullptr);
    FlowControl::~FlowControl();
    Q_INVOKABLE void connectToEyeTracker();
    Q_INVOKABLE void calibrateEyeTracker();
    Q_INVOKABLE bool startNewExperiment(qint32 experimentID);
    Q_INVOKABLE bool isConnected() const { return connected; }
    Q_INVOKABLE bool isCalibrated() const { return calibrated; }
    Q_INVOKABLE bool isExperimentEndOk() const {return experimentIsOk;}
    Q_INVOKABLE bool areThereFrequencyErrorsPresent() const {return frequencyErrorsPresent;}
    Q_INVOKABLE void setupSecondMonitor();
    Q_INVOKABLE void eyeTrackerChanged();
    Q_INVOKABLE void resolutionCalculations();
    Q_INVOKABLE bool checkSSLAvailability() {return sslDataProcessingClient->sslEnabled();}
    Q_INVOKABLE void requestReportData();
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
    Q_INVOKABLE quint8 getSSLTransactionError() {return sslDataProcessingClient->getProcessingCode();}
    Q_INVOKABLE void moveFileToArchivedFileFolder(const QString &filename);
    Q_INVOKABLE void doFrequencyAnalysis(const QString &filename);

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

public slots:

    // When an experiment finishes.
    void on_experimentFinished(const Experiment::ExperimentResult & er);

    // When an SSL Transaction finishes
    void onDisconnectionFinished();

    // Eye tracker control changes
    void onEyeTrackerControl(quint8 code);

    // For receiving information to send to the server
    void onFileSetEmitted(const QStringList &fileSetAndName);

private slots:
    // The function that actually draws the report
    void drawReport();

private:

    // Delays saving the report until the wait dialog can be shown.
    QTimer delayTimer;

    // The second screen for monitoring the experiments.
    MonitorScreen *monitor;

    // The currently selected experiment
    Experiment *experiment;

    // The currently selected eyetracker
    EyeTrackerInterface *eyeTracker;

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

    // Flags to avoid reconnecting during recalibration
    bool connected;

    // Flag to check if the eyetracker is calibrated or not
    bool calibrated;

    // Binary status for the end of an experiment.
    bool experimentIsOk;

    // Flag to indicate frequency errors and use the appropiate message.
    bool frequencyErrorsPresent;

    // Flag to check on transaction status
    bool sslTransactionAllOk;

    // Flag used to generate demo transaction
    bool demoTransaction;

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
