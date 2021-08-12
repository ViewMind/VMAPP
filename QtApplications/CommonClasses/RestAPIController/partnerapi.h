#ifndef PARTNERAPI_H
#define PARTNERAPI_H

#include <QThread>
#include <QObject>
#include <QVariantMap>
#include "restapicontroller.h"

namespace PartnerMedic {
static const QString NAME            = "name";
static const QString LASTNAME        = "lastname";
static const QString EMAIL           = "email";
static const QString PARTNER_ID      = "partner_id";
static const QString PARTNER_UID     = "partner_uid";
}

namespace ParterPatient {
static const QString NAME             = "name";
static const QString LASTNAME         = "last_name";
static const QString PARTNER_MEDIC_ID = "partner_medic_id";
static const QString NATIONALITY      = "birth_country_code";
static const QString AGE              = "age";
static const QString BIRTHDATE        = "birth_date";
static const QString YEARS_FORMATION  = "years_formation";
static const QString PARTNER_UID      = "internal_id";
static const QString GENDER           = "gender";
}

class PartnerAPI : public QObject
{
    Q_OBJECT
public:

    explicit PartnerAPI(QObject *parent = nullptr);

    virtual bool requestInformation(const QVariantMap &configuration);
    virtual QString getPartnerType() const;
    virtual bool addMedicsAsAppUsers() const;
    virtual bool addMedicsAsNonLoginUsers() const;
    virtual bool useEmailAsUID() const;

    QVariantList getMedicInformation() const;
    QVariantList getRegisteredPatientInformation() const;

    QString getError() const;

    static QVariant MapHiearchyCheck(const QVariantMap &map, const QStringList &hiearchy, QString *message);

private slots:
    void receivedReplayData();

signals:
    void finished();

protected:

    RESTAPIController api;
    QString error;

    QVariantList medics;
    QVariantList patients;

    virtual void onReplyReceived();

};

#endif // PARTNERAPI_H
