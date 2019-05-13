#include "patientnamemapmanager.h"

PatientNameMapManager::PatientNameMapManager()
{
}

void PatientNameMapManager::loadPatNameDB(){
    QFile file(FILENAME_PATUID_MAP);
    if (!file.open(QFile::ReadOnly)) return;
    QDataStream reader(&file);
    reader >> patNames;
    file.close();
}

void PatientNameMapManager::addToMap(const QStringList &puids, const QStringList &patnames){
    if (puids.size() != patnames.size()) return;
    for (qint32 i = 0; i < puids.size(); i++){
        patNames[puids.at(i)] = patnames.at(i);
    }

    QFile file(FILENAME_PATUID_MAP);
    if (!file.open(QFile::WriteOnly)) return;

    QDataStream reader(&file);
    reader << patNames;
    file.close();
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

    if (!patNames.contains(puid)) return config;
    config.addKeyValuePair(CONFIG_PATIENT_NAME,patNames.value(puid));
    return config;
}

void PatientNameMapManager::printMap() const{
    QStringList keys = patNames.keys();
    for (qint32 i = 0; i < keys.size(); i++){
        qDebug() << keys.at(i) << ":" << patNames.value(keys.at(i));
    }
}
