#ifndef LOCALINFORMATIONMANAGER_H
#define LOCALINFORMATIONMANAGER_H

#include <QVariantMap>
#include <QDataStream>

#include "../../CommonClasses/common.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../CommonClasses/LogInterface/loginterface.h"
#include "../../CommonClasses/SQLConn/dbdescription.h"
#include "eye_experimenter_defines.h"

class LocalInformationManager
{
public:

    LocalInformationManager(ConfigurationManager *c);
    bool addDoctorData(const QString &dr_uid, const QStringList &cols, const QStringList &values);
    bool addPatientData(const QString &patient_uid, const QStringList &cols, const QStringList &values);
    QStringList getPatientListForDoctor();
    QStringList getDoctorList(bool onlyUIDs);
    void setCurrentDoctor(const QString &uid) {currentDr = uid;}

    // FOR DEBUGGING ONLY
    void printLocalDB();

private:

    static const QString PATIENT_DATA;
    static const qint32 LOCAL_DB_VERSION = 0;

    ConfigurationManager *config;
    LogInterface log;
    QVariantMap localDB;
    QString currentDr;

    bool backupDB();
    void loadDB();

};
#endif // LOCALINFORMATIONMANAGER_H
