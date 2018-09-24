#ifndef LOCALINFORMATIONMANAGER_H
#define LOCALINFORMATIONMANAGER_H

#include <QVariantMap>
#include <QDataStream>

#include "../../CommonClasses/common.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../CommonClasses/LogInterface/loginterface.h"
#include "../../CommonClasses/SQLConn/dbdescription.h"
#include "../../CommonClasses/DatFileInfo/datfileinfoindir.h"
#include "sslclient/ssldbclient.h"
#include "eye_experimenter_defines.h"

class LocalInformationManager
{
public:

    LocalInformationManager(ConfigurationManager *c);
    void addDoctorData(const QString &dr_uid, const QStringList &cols, const QStringList &values);
    void addPatientData(const QString &patient_uid, const QStringList &cols, const QStringList &values);
    QList<QStringList> getPatientListForDoctor();
    QList<QStringList> getDoctorList();
    QString getFieldForCurrentPatient(const QString &field) const;
    QVariantMap getCurrentDoctorInfo() {return localDB.value(config->getString(CONFIG_DOCTOR_UID)).toMap();}
    QVariantMap getCurrentPatientInfo() {
        return localDB.value(config->getString(CONFIG_DOCTOR_UID)).toMap().value(PATIENT_DATA).toMap().value(config->getString(CONFIG_PATIENT_UID)).toMap();
    }    
    void setUpdateFlagTo(bool flag);

    // Synch function. Returns false if the there is nothing to synch. (No changes to Doctor and Patient data).
    bool setupDBSynch(SSLDBClient *client);

    // Functions for iterating over file sets to be processed.
    void preparePendingReports();
    QStringList nextPendingReport();

    // FOR DEBUGGING ONLY
    void printLocalDB();

private:

    static const QString PATIENT_DATA;
    static const QString DOCTOR_UPDATE;
    static const QString PATIENT_UPDATE;
    static const qint32 LOCAL_DB_VERSION = 1;

    ConfigurationManager *config;
    LogInterface log;
    QVariantMap localDB;

    // Used to iterate over unprocessed information.
    QHash<QString, DatFileInfoInDir> patientReportInformation;
    void fillPatientDatInformation();

    void backupDB();
    void loadDB();


};
#endif // LOCALINFORMATIONMANAGER_H
