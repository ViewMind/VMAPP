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
    reader2 >> dbiddata;
    file2.close();

    QFile file3(FILENAME_VMID_MAP);
    if (!file3.open(QFile::ReadOnly)) return;
    QDataStream reader3(&file3);
    reader3 >> dbvmid;
    file3.close();

}

void PatientNameMapManager::setInstitutions(const QStringList &uids, const QStringList &names){
    for (int i = 0; i < uids.size(); i++){
        instituions[uids.at(i)] = names.at(i);
    }
}

QString PatientNameMapManager::fromSerializedMapData(const QString &data){

    QString error;
    dbdata = VariantMapSerializer::serialTwoLevelStringToVariantMap(data,dbdata,&error);
    if (!error.isEmpty()) return error;

    QFile file(FILENAME_DBDATA_MAP);
    if (!file.open(QFile::WriteOnly)) return "Could not open output file for writing" + QString(FILENAME_DBDATA_MAP);

    QDataStream reader(&file);
    reader << dbdata;
    file.close();

    return "";
}

QString PatientNameMapManager::addSerializedIDMap(const QString &data){
    QString error;
    dbiddata = VariantMapSerializer::serialOneLevelStringToVariantMap(data,dbiddata,&error,true);
    if (!error.isEmpty()) return error;

    QFile file(FILENAME_PATUID_MAP);
    if (!file.open(QFile::WriteOnly)) return "Could not open output file for writing" + QString(FILENAME_PATUID_MAP);

    QDataStream reader(&file);
    reader << dbiddata;
    file.close();

    return "";
}

QString PatientNameMapManager::addVMIDTableData(const QString &table, const QString &instuid){

    // Creatign the dual lists.
    QStringList dualTables = table.split("-");
    if (dualTables.size() != 2){
        return "Expecting two lists but got: " + QString::number(dualTables.size());
    }

    QStringList puid = dualTables.first().split(",");
    QStringList hpuid = dualTables.last().split(",");

    if (puid.size() != hpuid.size()) return "Received two lists of different sizes";

    for (qint32 i = 0; i < puid.size(); i++){
        QStringList list; list << hpuid.at(i) << instuid;
        dbvmid[puid.at(i)] = list;
    }
    QFile file(FILENAME_VMID_MAP);
    if (!file.open(QFile::WriteOnly)) return "Could not open output file for writing " + QString(FILENAME_VMID_MAP);

    QDataStream reader(&file);
    reader << dbvmid;
    file.close();

    return "";
}

QStringList PatientNameMapManager::getDBPUIDForInst(const QString &instuid){
    QStringList allpuids = dbvmid.keys();
    QStringList ans;
    for (qint32 i = 0; i < allpuids.size(); i++){
        QStringList list = dbvmid.value(allpuids.at(i)).toStringList();
        if (list.last() == instuid){
            ans << allpuids.at(i);
        }
    }
    return ans;
}

ConfigurationManager PatientNameMapManager::getPatientNameFromDirname(const QString &dirname) const{
    ConfigurationManager config;

    QString dbpuid;
    QString longdbpuid;
    QStringList parts = dirname.split("/",QString::SkipEmptyParts);

    if (parts.size() >= 2) longdbpuid = parts.at(parts.size()-2);
    else return config;

    // This will get rid of the extra zeros that the dirname might have.
    qint32 puidnum = longdbpuid.toInt();
    dbpuid = QString::number(puidnum);
    return getPatientIDInfoFromDBPUID(dbpuid);
}


ConfigurationManager PatientNameMapManager::getPatientIDInfoFromDBPUID(const QString &dbpuid) const{
    ConfigurationManager config;
    config.addKeyValuePair(ID_DBUID,dbpuid);

    if (dbvmid.contains(dbpuid)){
        QStringList list = dbvmid.value(dbpuid).toStringList();
        config.addKeyValuePair(ID_HPUID,list.first());
    }

    if (!dbiddata.contains(dbpuid)) return config;
    QString puid = dbiddata.value(dbpuid).toString(); // Conversion from dbpuid (keyid from patid table) to puid (id generated locally)
    config.addKeyValuePair(ID_PUID,puid);
    if (!dbdata.contains(puid)) return config;
    QVariantMap patdata = dbdata.value(puid).toMap();

    //qWarning() << "Adding the data" << patdata;
    config.addKeyValuePair(ID_NAME,patdata.value(TPATDATA_COL_FIRSTNAME).toString() + " " + patdata.value(TPATDATA_COL_LASTNAME).toString());
    config.addKeyValuePair(ID_DID,patdata.value(TPATDATA_NONCOL_DISPLAYID).toString());
    return config;
}

void PatientNameMapManager::printMap() const{
    qDebug() << "DBDATA MAP";
    QStringList keys = dbdata.keys();
    for (qint32 i = 0; i < keys.size(); i++){
        QVariantMap m = dbdata.value(keys.at(i)).toMap();
        qDebug() << keys.at(i);
        QStringList keys2 = m.keys();
        for (qint32 j = 0; j < keys2.size(); j++){
            qDebug() << "   " << keys2.at(j) << ":=" << m.value(keys2.at(j));
        }
    }

    qDebug() << "DBPUID MAP";
    keys = dbiddata.keys();
    for (qint32 i = 0; i < keys.size(); i++){
        qDebug() << keys.at(i) << ":=" << dbiddata.value(keys.at(i));
    }

    qDebug() << "VMID MAP";
    keys = dbvmid.keys();
    for (qint32 i = 0; i < keys.size(); i++){
        qDebug() << keys.at(i) << ":=" << dbvmid.value(keys.at(i));
    }
}
