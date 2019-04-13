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
#include <QMetaEnum>

#define COMPILE_FOR_PRODUCTION

#ifdef COMPILE_FOR_PRODUCTION
#define  SERVER_IP                     "18.220.30.34"  // Production server
#else
#define  SERVER_IP                     "192.168.1.10"  // Local server
#endif

#define   PROGRAM_NAME                 "EyeDataAnalyzer"
#define   PROGRAM_VERSION              "4.0.0"
#define   WORK_DIR                     "work"
#define   FILE_DEFAULT_VALUES          "default_values"

#define   ROLE_DATA                     1500

#define   VIEW_0_DATABASE_VIEW          0
#define   VIEW_1_PROCESSING_VIEW        1

#include "../../CommonClasses/HTMLWriter/htmlwriter.h"
#include "../../CommonClasses/DataAnalysis/rawdataprocessor.h"
#include "../../CommonClasses/PNGWriter/imagereportdrawer.h"
#include "../../CommonClasses/PNGWriter/repfileinfo.h"
#include "../../CommonClasses/DataAnalysis/FrequencyAnalsis/freqanalysis.h"
#include "../../CommonClasses/DataPacket/datapacket.h"
#include "fixationdrawer.h"
#include "waitdialog.h"

namespace Ui {
class EyeDataAnalyzer;
}

class EyeDataAnalyzer : public QMainWindow
{
    Q_OBJECT

public:
    explicit EyeDataAnalyzer(QWidget *parent = 0);
    ~EyeDataAnalyzer();

public slots:
    void onProcessorMessage(const QString &msg, qint32 type);

    void on_newUIMessage(const QString &html);

    void on_newUIMessage_for_DB(const QString &html);

    void on_actionDataBase_Connection_triggered();

    void on_actionProcessing_View_triggered();

    void on_lwDirs_itemDoubleClicked(QListWidgetItem *item);

    void on_pushButton_clicked();

    void on_pbGenerateReport_clicked();

    void on_pbDrawFixations_clicked();

    void on_pgFrequencyAnalsis_clicked();

    void on_pteDefConfig_clicked();

    void on_pbSaveDefConf_clicked();

    void on_pbDATFiles_clicked();

    void on_pbGenerateReport_2_clicked();

public slots:

    // SSL and TCP Related slots
    void on_encryptedSuccess();
    void on_socketError(QAbstractSocket::SocketError error);
    void on_readyRead();
    void on_sslErrors(const QList<QSslError> &errors);
    void on_socketStateChanged(QAbstractSocket::SocketState state);

    // Timer
    void on_timeOut();


private slots:
    void on_pbGetData_clicked();

    void on_pbClearLog_clicked();

private:

    typedef enum {CS_CONNECTING_FOR_NAME_LIST,CS_GETTING_NAMELIST,CS_GETTING_DATA,CS_CONNECTING_FOR_DATA} ConnectionState;

    Ui::EyeDataAnalyzer *ui;
    LogInterface logForProcessing;
    LogInterface logForDB;
    QString currentDirectory;
    ConfigurationManager currentEyeRepGen;
    HTMLWriter htmlWriter;    

    ConfigurationManager defaultReportCompletionParameters;
    ConfigurationManager defaultValues;

    QList<QGroupBox*> appViews;

    // Processing data.
    WaitDialog *waitDiag;
    QSslSocket *serverConn;
    DataPacket rx;
    ConnectionState connectionState;
    QString institution;
    QTimer timer;

    void processDirectory();
    void switchViews(qint32 view);
    ConfigurationManager createProcessingConfiguration(bool *ok);
    void overWriteCurrentConfigurationWith(const ConfigurationManager &mng, bool addOnlyNonExistant);

};

#endif // EYEDATAANALYZER_H
