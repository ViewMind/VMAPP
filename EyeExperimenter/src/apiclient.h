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
#include "eyexperimenter_defines.h"

class APIClient : public QObject
{
    Q_OBJECT
public:
    explicit APIClient(QObject *parent = nullptr);

    // Command for compressor.
    static const QString TAR_EXE;

    static const qint32 API_OPERATING_INFO         = 1;
    static const qint32 API_REQUEST_REPORT         = 2;
    static const qint32 API_REQUEST_UPDATE         = 3;
    static const qint32 API_SYNC_PARTNER_MEDIC     = 4;

    // Parameters required for configuration. Numbers must be used and sent as strings anyways. So string parameters are accepted.
    void configure(const QString &institution_id,
                   const QString &instance_number,
                   const QString &version,
                   const QString &region,
                   const QString &key,
                   const QString &hash_code);

    // Request the list of medical professionals associated with this institution.
    bool requestOperatingInfo();

    // Request processing for report.
    bool requestReportProcessing(const QString &jsonFile);

    // Resquest the download of the update file.
    bool requestUpdate(const QString &pathToSaveAFile);

    // Request to add non login portal users sent as JSON data.
    bool requestAdditionOfNonLoginPortalUsers(const QVariantList &pusers);


    QString getError() const;

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

    // The actual endpoints.    
    const QString ENDPOINT_OPERATING_INFO    = "/institution/operating_information";
    const QString ENDPOINT_GET_UPDATE        = "/instances/getupdate";
    const QString ENDPOINT_REPORT_GENERATION = "/reports/generate";
    const QString ENDPOINT_ADDNOLOGIN_MEDIC  = "/portal_users/addnologpusers";

    // URL parameters.
    const QString URLPARAM_PPKEY            = "ppkey";
    const QString URLPARAM_INSTANCE         = "instance";
    const QString URLPARAM_VERSION          = "version";
    const QString URLPARAM_REGION           = "region";

    // Header values.
    const QString HEADER_AUTHTYPE           = "AuthType";
    const QString HEADER_AUTHENTICATION     = "Authentication";
    const QString HEADER_SIGNATURE          = "Signature";
    const QString AUTH_TYPE                 = "VMClient";

    // POST Data.
    const QString POST_FIELD_INSTITUTION_ID       = "institution_id";
    const QString POST_FIELD_INSTITUTION_INSTANCE = "institution_instance";

    // Key for the $_FILES structure.
    const QString FILE_KEY                        = "FileToProcess";

    // Signs the message and sends the request.
    bool sendRequest();

};

#endif // APICLIENT_H
