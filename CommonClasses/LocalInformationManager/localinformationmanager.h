#ifndef LOCALINFORMATIONMANAGER_H
#define LOCALINFORMATIONMANAGER_H

#define USESSL

#include <QVariantMap>
#include <QDataStream>
#include <QCryptographicHash>

#include "../../CommonClasses/common.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../CommonClasses/LogInterface/loginterface.h"
#include "../../CommonClasses/SQLConn/dbdescription.h"
#include "../../CommonClasses/DatFileInfo/datfileinfoindir.h"

#ifdef USESSL
#include "sslclient/ssldbclient.h"
#endif

#define   LOCAL_DB                                      "localdb.dat"

class LocalInformationManager
{
public:

    LocalInformationManager(ConfigurationManager *c);
    void addDoctorData(const QString &dr_uid, const QStringList &cols, const QStringList &values, const QString &password, bool hidden);
    void addPatientData(const QString &patient_uid, const QStringList &cols, const QStringList &values);
    bool isDoctorValid(const QString &dr_uid);
    bool doesDoctorExist(const QString &uid) const;
    void validateDoctor(const QString &dr_uid);
    QString getDoctorPassword(QString uid = "");
    QList<QStringList> getPatientListForDoctor();
    QList<QStringList> getDoctorList(bool forceShow = false);
    QString getFieldForCurrentPatient(const QString &field) const;
    QVariantMap getCurrentDoctorInfo() {return localDB.value(config->getString(CONFIG_DOCTOR_UID)).toMap();}
    QVariantMap getCurrentPatientInfo() {
        return localDB.value(config->getString(CONFIG_DOCTOR_UID)).toMap().value(PATIENT_DATA).toMap().value(config->getString(CONFIG_PATIENT_UID)).toMap();
    }    
    void setUpdateFlagTo(bool flag);

    // Synch function. Returns false if the there is nothing to synch. (No changes to Doctor and Patient data).
#ifdef USESSL
    bool setupDBSynch(SSLDBClient *client);
#endif
    // Functions for iterating over file sets to be processed.
    void preparePendingReports();
    QStringList nextPendingReport();

    // Used ONLY in the LocalDBMng program
    void deleteDoctor(const QString &uid);
    void makeVisible(const QString &uid);

    // FOR DEBUGGING ONLY
    void printLocalDB();

private:

    static const QString PATIENT_DATA;
    static const QString DOCTOR_UPDATE;
    static const QString PATIENT_UPDATE;
    static const QString DOCTOR_PASSWORD;
    static const QString DOCTOR_VALID;
    static const QString DOCTOR_HIDDEN;
    static const qint32  LOCAL_DB_VERSION = 2;

    ConfigurationManager *config;
    LogInterface log;
    QVariantMap localDB;

    // Used to iterate over unprocessed information.
    QHash<QString, DatFileInfoInDir> patientReportInformation;
    void fillPatientDatInformation();

    void backupDB();
    void loadDB();
    bool isHidden(const QString &uid);


};
#endif // LOCALINFORMATIONMANAGER_H
