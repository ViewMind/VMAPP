#ifndef APICLIENT_H
#define APICLIENT_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QMessageAuthenticationCode>
#include "../../../CommonClasses/RestAPIController/restapicontroller.h"
#include "eyexperimenter_defines.h"

class APIClient : public QObject
{
    Q_OBJECT
public:
    explicit APIClient(QObject *parent = nullptr);

    // Parameters required for configuration. Numbers must be used and sent as strings anyways. So string parameters are accepted.
    void configure(const QString &institution_id,
                   const QString &instance_number,
                   const QString &key,
                   const QString &hash_code);

    // Request the list of medical professionals associated with this institution.
    bool requestOperatingInfo();

    QString getError() const;

    // Returning the parsed information.
    QVariantMap getMapDataReturned() const;

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
    QString secret;
    QString error;
    QVariantMap retdata;

    const QString API_DBUG = "http://192.168.1.12/vmapi";
    const QString API_PRODCUTION = "https:://eu.viewmind.ai/vmapi"; // This is a fantasy. Don't know the actuall finished address, right now.

    // The actual endpoints.
    const QString ENDPOINT_MEDIC_LIST       = "/portal_users/getallmedical";
    const QString ENDPOINT_OPERATING_INFO   = "/institution/operating_information";

    // URL parameters.
    const QString URLPARAM_PPKEY            = "ppkey";

    // Header values.
    const QString HEADER_AUTHTYPE           = "AuthType";
    const QString HEADER_AUTHENTICATION     = "Authentication";
    const QString HEADER_SIGNATURE          = "Signature";
    const QString AUTH_TYPE                 = "VMClient";

    // POST Data.
    const QString POST_FIELD_INSTITUTION_ID       = "institution_id";
    const QString POST_FIELD_INSTITUTION_INSTANCE = "institution_instance";



    // Signs the message and sends the request.
    bool sendRequest();

};

#endif // APICLIENT_H
