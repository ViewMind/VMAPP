#ifndef APICLIENT_H
#define APICLIENT_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QProcess>
#include <QFileInfo>
#include <QMessageAuthenticationCode>
#include "../../CommonClasses/RestAPIController/restapicontroller.h"
#include "../../CommonClasses/LogInterface/logprep.h"
#include "../../CommonClasses/json_file_store.h"
#include "eyexperimenter_defines.h"

namespace HTTPCodes {

   // Disabled instance
   const qint32 INSTANCE_DISABLED           = 599;

   // ViewMind Func Verif Failuers
   const qint32 FUNC_VERIF_FAIL_BAD_PASSWD  = 700;
   const qint32 FUNC_VERIF_FAIL_UNAUTHUSER  = 701;
   const qint32 FUNC_VERIF_FAIL_BADSN       = 702;

   // ViewMind Activation Failures
   const qint32 ACT_FAILED_NOT_LIBERATED    = 600;
   const qint32 ACT_FAILED_INVALID_SN       = 601;

}

class APIClient : public QObject
{
    Q_OBJECT
public:
    explicit APIClient(QObject *parent = nullptr);

    static const qint32 API_OPERATING_INFO         = 1;    
    static const qint32 API_REQUEST_REPORT         = 2;
    static const qint32 API_REQUEST_UPDATE         = 3;
    static const qint32 API_ACTIVATE               = 4;
    static const qint32 API_OPERATING_INFO_AND_LOG = 6;
    static const qint32 API_SENT_SUPPORT_EMAIL     = 7;
    static const qint32 API_FUNC_CTL_NEW           = 8;
    static const qint32 API_FUNC_CTL_HMD_CHANGE    = 9;

    // Parameters required for configuration. Numbers must be used and sent as strings anyways. So string parameters are accepted.
    void configure(const QString &institution_id,
                   const QString &instance_number,
                   const QString &version,
                   const QString &region,
                   const QString &key,
                   const QString &hash_code);

    // Request the list of medical professionals associated with this institution, along with QC threshold and processing parameters.
    // If sendLog is true then the current logfile is uploaded.
    // If logOnly is true AND sendLog is true, then the code for the last API request is set to API_OP_INFO_LOG_ONLY
    bool requestOperatingInfo(const QString &hardware_description_string, bool sendLog, QVariantMap updated_subject_records);

    // Once we get the key from the RRS it needs to be set here.
    void setEyeTrackerKey(const QString &key);

    // The key is stored in the studies, so it needs to be retrieved.
    QString getEyeTrackerKey() const;

    // Requensts sending a support email.
    bool requestSupportEmail(const QString &subject, const QString &email_file);

    // Request processing for report.
    bool requestReportProcessing(const QString &tarFile);

    // Resquest the download of the update file.
    bool requestUpdate(const QString &pathToSaveAFile);

    // Send notification that the functional control has been passed. Intent is deduced from whether or not name and last name are provided.
    bool requestFunctionalControl(const QVariantMap &hwInfo,
                                  const QString &email,
                                  const QString &password,
                                  const QString &name,
                                  const QString &lastname,
                                  const QString &instance_identifier);

    // Sends a query to the activation endpoint.
    bool requestActivation(qint32 institution, qint32 instance, const QString &key, const QString &hardware_description_string, const QVariantMap &hwinfo);

    // Call function of the same name on the rest api controller.
    void clearFileToSendHandles();

    // Gets the error.
    QString getError() const;

    // If requesting the operating information generated a new logfile name, that name will be stored and will can be read using this fucntion.
    QString getLastGeneratedLogFileName() const;

    // If requesting the operating information genrated a new subject update json file, then the filename will be stored and it can be read using this function.
    QString getLastGeneratedSubjectJSONFile() const;

    // We need this name to clear the file after it has finished.
    QString getLatestGeneratedSupportEmail() const;

    // Returning the parsed information.
    QVariantMap getMapDataReturned() const;

    // Get the last type of request.
    qint32 getLastRequestType() const;

signals:
    void requestFinish();

private slots:
    void gotReply();

private:

    RESTAPIController rest_controller;

    QString API;
    QString key;
    QString institution_id;
    QString instance_number;
    QString region;
    QString version;
    QString secret;
    QString error;
    QString pathToSaveAFile;
    QVariantMap retdata;
    qint32 lastRequest;
    QString lastGeneratedLogFileName;
    QString lastRequestEmailFile;
    QString lastSubjectUpdateJSONFile;
    QString eyeTrackerKey;

    // The actual endpoints.    
    const QString ENDPOINT_OPERATING_INFO          = "/institution/operating_information";
    const QString ENDPOINT_GET_UPDATE              = "/instances/getupdate";
    const QString ENDPOINT_REPORT_GENERATION       = "/reports/generate";
    const QString ENDPOINT_ACTIVATION_ENDPOINT     = "/instances/activate";
    const QString ENDPOINT_SEND_SUPPORT_EMAIL      = "/tickets/send_support_email";
    const QString ENDPOINT_FUNC_CTL                = "/instances/functional_control";

    // URL parameters.
    const QString URLPARAM_PPKEY                   = "ppkey";
    const QString URLPARAM_INSTANCE                = "instance";
    const QString URLPARAM_VERSION                 = "version";
    const QString URLPARAM_REGION                  = "region";
    const QString URLPARAM_INSTITUTION             = "institution";
    const QString URLPARAM_KEY                     = "key";

    // Header values.
    const QString HEADER_AUTHTYPE                  = "AuthType";
    const QString HEADER_AUTHENTICATION            = "Authentication";
    const QString HEADER_SIGNATURE                 = "Signature";
    const QString AUTH_TYPE                        = "VMClient";

    // POST Data.
    const QString POST_FIELD_INSTITUTION_ID       = "institution_id";
    const QString POST_FIELD_INSTITUTION_INSTANCE = "institution_instance";
    const QString POST_FIELD_HW_STRING            = "hardware_description_string";

    // Key for the $_FILES structure.
    const QString FILE_KEY                        = "FileToProcess";
    const QString FAILED_CALIBRATION_FILE_PREFIX  = "FailedCalibrationFile_";
    const QString SUPPORT_EMAIL_LOG               = "SupportEmailLog";
    const QString SUPPORT_EMAIL_FILE              = "SupportEmailFile";
    const QString SUBJECT_UPDATE_FILE             = "SubjectUpdateFile";

    // JSON Field names
    const QString JFIELD_HWINFO                   = "hwinfo";
    const QString JFIELD_PASSWORD                 = "password";
    const QString JFIELD_EMAIL                    = "email";
    const QString JFIELD_FNAME                    = "firstname";
    const QString JFIELD_LNAME                    = "lastname";
    const QString JFIELD_INTENT                   = "intent";
    const QString JFIELD_HW_DESC_STRING           = "hardware_description_string";

    // Signs the message and sends the request.
    bool sendRequest(bool nosign = false);

};

#endif // APICLIENT_H
