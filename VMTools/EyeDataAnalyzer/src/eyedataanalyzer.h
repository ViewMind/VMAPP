#ifndef EYEDATAANALYZER_H
#define EYEDATAANALYZER_H

#include <QMainWindow>
#include <QFileDialog>
#include <QLineEdit>
#include <QListWidgetItem>
#include <QGroupBox>
#include <QSslSocket>
#include <QMessageBox>
#include <QProgressDialog>
#include <QTimer>
#include <QCryptographicHash>
#include <QMetaEnum>


#define COMPILE_FOR_PRODUCTION

#ifdef COMPILE_FOR_PRODUCTION
#define  SERVER_IP                     "18.220.30.34"  // Production server
#else
#define  SERVER_IP                     "192.168.1.10"  // Local server
#endif

#define   PROGRAM_NAME                 "EyeDataAnalyzer"
#define   PROGRAM_VERSION              "8.6.0" //8.5.2
#define   WORK_DIR                     "work"
#define   FILE_DEFAULT_VALUES          "default_values"

#define   ROLE_DATA                     1500

#define   VIEW_0_DATABASE_VIEW          0
#define   VIEW_1_PROCESSING_VIEW        1

#include "../../../CommonClasses/HTMLWriter/htmlwriter.h"
#include "../../../CommonClasses/DataAnalysis/rawdataprocessor.h"
#include "../../../CommonClasses/GraphicalReport/imagereportdrawer.h"
#include "../../../CommonClasses/RepFileInfo/repfileinfo.h"
#include "../../../../CommonClasses/DataAnalysis/FrequencyAnalsis/freqanalysis.h"
#include "../../../CommonClasses/DataPacket/datapacket.h"
#include "fixationdrawer.h"
#include "patientnamemapmanager.h"
#include "waitdialog.h"
#include "batchcsvprocessing.h"
#include "selectunifiedcsvfolderdiag.h"
#include "idtablediag.h"
#include "languageselectiondialog.h"

namespace Ui {
class EyeDataAnalyzer;
}

class EyeDataAnalyzer : public QMainWindow
{
    Q_OBJECT

public:
    explicit EyeDataAnalyzer(QWidget *parent = nullptr);
    ~EyeDataAnalyzer();

public slots:
    void onProcessorMessage(const QString &msg, qint32 type);

    void on_newUIMessage(const QString &html);

    void on_newUIMessage_for_DB(const QString &html);

    void on_actionDataBase_Connection_triggered();

    void on_actionProcessing_View_triggered();

    void on_lwDirs_itemDoubleClicked(QListWidgetItem *item);

    void on_pbUp_clicked();

    void on_pbAnalyzeData_clicked();

    void on_pbDrawFixations_clicked();

    void on_pgFrequencyAnalsis_clicked();

    void on_pteDefConfig_clicked();

    void on_pbSaveDefConf_clicked();

    void on_pbDATFiles_clicked();

    void on_pbGenerateReport_clicked();

    void on_pbGetData_clicked();

    void on_pbClearLog_clicked();

    void on_pbLocalDB_clicked();

    void on_pbUnifiedCSV_clicked();

    void on_batchProcessing_Done();

    void on_pbIDTable_clicked();

    void on_pbVMIDTableInfo_clicked();

    void on_pbGetMedicalRecords_clicked();

public slots:

    // SSL and TCP Related slots
    void on_encryptedSuccess();
    void on_socketError(QAbstractSocket::SocketError error);
    void on_readyRead();
    void on_sslErrors(const QList<QSslError> &errors);
    void on_socketStateChanged(QAbstractSocket::SocketState state);

    // Timer
    void on_timeOut();


private:

    typedef enum {CS_CONNECTING_FOR_NAME_LIST,
                  CS_GETTING_NAMELIST,
                  CS_GETTING_DATA,
                  CS_GETTING_DB_BKP,
                  CS_GETTING_VMID_TABLE,
                  CS_GETTING_MEDRECS,
                  CS_CONNECTING_FOR_DATA,
                  CS_CONNECTING_VMID_TABLE,
                  CS_CONNECTING_FOR_MEDRECS,
                  CS_CONNECTING_FOR_DB_BKP} ConnectionState;

    QString connectionStateToString() const {
        switch(connectionState){
        case CS_CONNECTING_FOR_DATA: return "Connecting for data";
        case CS_CONNECTING_FOR_DB_BKP: return "Connecting db bkp";
        case CS_CONNECTING_FOR_NAME_LIST: return "Connecting for name list";
        case CS_GETTING_DATA: return "Getting data";
        case CS_GETTING_DB_BKP: return "Getting db bkp";
        case CS_GETTING_NAMELIST: return "Getting uid inst name list";
        case CS_CONNECTING_VMID_TABLE: return "Connecting for VMID table";
        case CS_GETTING_VMID_TABLE: return "Getting for VMID table";
        case CS_CONNECTING_FOR_MEDRECS: return "Connecting Medical Records";
        case CS_GETTING_MEDRECS: return "Getting medical records";
        }
        return "";
    }

    Ui::EyeDataAnalyzer *ui;
    LogInterface logForProcessing;
    LogInterface logForDB;
    QString currentDirectory;
    ConfigurationManager currentEyeRepGen;
    HTMLWriter htmlWriter;

    ConfigurationManager defaultReportCompletionParameters;
    ConfigurationManager defaultValues;

    QList<QGroupBox*> appViews;

    // Load patient name map to its puid
    PatientNameMapManager patNameMng;

    // Processing data.
    WaitDialog *waitDiag;
    QSslSocket *serverConn;
    DataPacket rx;
    ConnectionState connectionState;
    QString institution;
    QTimer timer;
    BatchCSVProcessing batchProcessor;


    void processDirectory();
    void switchViews(qint32 view);
    ConfigurationManager createProcessingConfiguration(bool *ok);
    void overWriteCurrentConfigurationWith(const ConfigurationManager &mng, bool addOnlyNonExistant);

};

#endif // EYEDATAANALYZER_H
