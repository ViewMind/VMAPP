#include "patientnamemapmanager.h"

PatientNameMapManager::PatientNameMapManager()
{
}

void PatientNameMapManager::loadPatNameDB(){
    QFile file(FILENAME_DBDATA_MAP);
    if (!file.open(QFile::ReadOnly)) return;
    QDataStream reader(&file);
    reader >> dbdata;
    file.close();

    QFile file2(FILENAME_PATUID_MAP);
    if (!file2.open(QFile::ReadOnly)) return;
    QDataStream reader2(&file2);
    reader2 >> dbpuid;
    file2.close();


}

QString PatientNameMapManager::fromSerializedMapData(const QString &data){

    QString error;
    dbdata = VariantMapSerializer::serialTwoLevelStringToVariantMap(data,dbdata,&error);
    if (!error.isEmpty()) return error;

    QFile file(FILENAME_DBDATA_MAP);
    if (!file.open(QFile::WriteOnly)) return "Could not open output file for writing";

    QDataStream reader(&file);
    reader << dbdata;
    file.close();

    return "";
}

QString PatientNameMapManager::addSerializedIDMap(const QString &data){
    QString error;
    dbpuid = VariantMapSerializer::serialOneLevelStringToVariantMap(data,dbpuid,&error,true);
    if (!error.isEmpty()) return error;

    QFile file(FILENAME_PATUID_MAP);
    if (!file.open(QFile::WriteOnly)) return "Could not open output file for writing";

    QDataStream reader(&file);
    reader << dbpuid;
    file.close();

    return "";
}

ConfigurationManager PatientNameMapManager::getPatientNameFromDirname(const QString &dirname) const{
    ConfigurationManager config;

    QString puid;
    QStringList parts = dirname.split("/",QString::SkipEmptyParts);

    if (parts.size() >= 2) puid = parts.at(parts.size()-2);
    else return config;

    // This will get rid of the extra zeros that the dirname might have.
    qint32 puidnum = puid.toInt();
    puid = QString::number(puidnum);
    config.addKeyValuePair(CONFIG_PATIENT_UID,puid);

    qWarning() << "PUID " << puid << " from dir " << dirname;

    if (!dbpuid.contains(puid)) return config;
    QString uid = dbpuid.value(puid).toString(); // Conversion from puid (keyid from patid table) to uid (id generated locally)
    //qWarning() << puid << "<=>" << uid;
    if (!dbdata.contains(uid)) return config;
    QVariantMap patdata = dbdata.value(uid).toMap();

    //qWarning() << "Adding the data" << patdata;
    config.addKeyValuePair("PUID",uid);
    config.addKeyValuePair(CONFIG_PATIENT_NAME,patdata.value(TPATDATA_COL_FIRSTNAME).toString() + " " + patdata.value(TPATDATA_COL_LASTNAME).toString());
    config.addKeyValuePair(CONFIG_PATIENT_DISPLAYID,patdata.value(TPATDATA_NONCOL_DISPLAYID).toString());
    return config;
}

void PatientNameMapManager::printMap() const{
    QStringList keys = dbdata.keys();
    for (qint32 i = 0; i < keys.size(); i++){
        QVariantMap m = dbdata.value(keys.at(i)).toMap();
        qDebug() << keys.at(i);
        QStringList keys2 = m.keys();
        for (qint32 j = 0; j < keys2.size(); j++){
            qDebug() << "   " << keys2.at(j) << ":=" << m.value(keys2.at(j));
        }
    }

    keys = dbpuid.keys();
    for (qint32 i = 0; i < keys.size(); i++){
        qDebug() << keys.at(i) << ":=" << dbpuid.value(keys.at(i));
    }

}
