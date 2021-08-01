#ifndef ORBITPARTNERAPI_H
#define ORBITPARTNERAPI_H

#include <QTest>
#include <QJsonObject>
#include "partnerapi.h"
#include <iostream>

namespace OrbitReturn {

   namespace PhysicianList {

       static const QString PAYLOAD = "payload";

       namespace Payload {

          static const QString ROWS = "rows";

          namespace Rows {

             namespace Physician {
                static const QString ID       = "id";
                static const QString NAME     = "firstName";
                static const QString LASTNAME = "lastName";
                static const QString EMAIL    = "email";
             }

             namespace Patient {
                static const QString ID       = "id";
                static const QString LASTNAME = "lastName";
                static const QString NAME     = "firstName";
             }

          }


       }

   }

}


class OrbitPartnerAPI : public PartnerAPI
{
    Q_OBJECT
public:

    static const QString KEY;
    static const QString SECRET;
    static const QString AUTH_URL;
    static const QString API_URL;

    OrbitPartnerAPI();
    bool requestInformation(const QVariantMap &conf) override;
    QString getPartnerType() const override;

protected:
    void onReplyReceived() override;

private:

    typedef enum {GS_TOKEN, GS_PHYSICIAN, GS_PATIENT, GS_FAILED, GS_DONE} GetState;

    GetState getState;

    QString key;
    QString secret;
    QString api_url;
    QString auth_url;
    QString token;

    qint32 physicianIDIterationValue;

    bool tokenReplyReceived;

    qint32 physician_index;

    static const QString ENDPOINT_PHYSICIAN;
    static const QString ENDPOINT_PATIENT;

    static const QString AUTH_DATA_STRING;
    static const QString ACCESS_TOKEN;

};

#endif // ORBITPARTNERAPI_H
