#ifndef LOCALINFORMATIONMANAGER_H
#define LOCALINFORMATIONMANAGER_H

#include <QVariantMap>
#include <QDataStream>

#include "../../CommonClasses/common.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../CommonClasses/LogInterface/loginterface.h"
#include "../../CommonClasses/SQLConn/dbdescription.h"
#include "sslclient/ssldbclient.h"
#include "eye_experimenter_defines.h"

class LocalInformationManager
{
public:

    LocalInformationManager(ConfigurationManager *c);
    bool addDoctorData(const QString &dr_uid, const QStringList &cols, const QStringList &values);
    bool addPatientData(const QString &patient_uid, const QStringList &cols, const QStringList &values);
    QList<QStringList> getPatientListForDoctor();
    QList<QStringList> getDoctorList();
    QVariantMap getCurrentDoctorInfo() {return localDB.value(config->getString(CONFIG_DOCTOR_UID)).toMap();}
    QVariantMap getCurrentPatientInfo() {
        return localDB.value(config->getString(CONFIG_DOCTOR_UID)).toMap().value(PATIENT_DATA).toMap().value(config->getString(CONFIG_PATIENT_UID)).toMap();
    }

    // Synch functions.
    void setupDBSynch(SSLDBClient *client);


    // FOR DEBUGGING ONLY
    void printLocalDB();
    void printReportRequests();

private:

    static const QString PATIENT_DATA;
    static const QString DOCTOR_UPDATE;
    static const QString PATIENT_UPDATE;
    static const qint32 LOCAL_DB_VERSION = 1;

    ConfigurationManager *config;
    LogInterface log;
    QVariantMap localDB;

    // Used to iterate over unprocessed information.
    QHash<QString, QHash<QString,QStringList> > patientReportRequest;
    void findRemainingReports();

    bool backupDB();
    void loadDB();


};
#endif // LOCALINFORMATIONMANAGER_H
