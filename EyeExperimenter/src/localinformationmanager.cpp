#include "localinformationmanager.h"

const QString LocalInformationManager::PATIENT_DATA   = "PATIENT_DATA";
const QString LocalInformationManager::PATIENT_UPDATE = "PATIENT_UPDATE";
const QString LocalInformationManager::DOCTOR_UPDATE  = "DOCTOR_UPDATE";

LocalInformationManager::LocalInformationManager(ConfigurationManager *c)
{
    config = c;
    loadDB();
}

void LocalInformationManager::setupDBSynch(SSLDBClient *client){

    client->setDBTransactionType(SQL_QUERY_TYPE_SET);

    QStringList allDrUIDs = localDB.keys();
    for (qint32 i = 0; i < allDrUIDs.at(i); i++){

        // Checking if the doctor needs to be synched up.
        QString druid = allDrUIDs.at(i);
        QVariantMap drmap = localDB.value(druid).toMap();

        bool addDoctor = false;
        if (!drmap.contains(DOCTOR_UPDATE)) addDoctor = true;
        else if (drmap.value(DOCTOR_UPDATE).toBool()) addDoctor = true;

        if (addDoctor){
            QStringList columns;
            QStringList values;
            QSet<QString> avoid; avoid << DOCTOR_UPDATE << PATIENT_DATA;
            QStringList keys = drmap.keys();
            for (qint32 i = 0; i < keys.size(); i++){
                if (avoid.contains(keys.at(i))) continue;
                columns << keys.at(i);
                values << drmap.value(keys.at(i)).toString();
            }
            client->appendSET(TABLE_DOCTORS,columns,values);
        }

        // Checking if any of the doctor's patients need to be synched up.
        QVariantMap patientData = drmap.value(PATIENT_DATA).toMap();
        QStringList allPatUIDs = patientData.keys();

        for (qint32 j = 0; j < allPatUIDs.size(); j++){

            QString patID = allPatUIDs.at(i);
            QVariantMap patientMap = patientData.value(patID).toMap();

            bool addPatient = false;
            if (!patientMap.contains(PATIENT_UPDATE)) addPatient = true;
            else if (patientMap.value(PATIENT_UPDATE).toBool()) addPatient = true;

            if (addPatient){
                QStringList columns;
                QStringList values;
                QStringList columnsOpt;
                QStringList valuesOpt;
                QSet<QString> avoid; avoid << PATIENT_UPDATE;
                QSet<QString> required; required << TPATREQ_COL_BIRTHCOUNTRY << TPATREQ_COL_BIRTHDATE << TPATREQ_COL_FIRSTNAME << TPATREQ_COL_IDTYPE
                                                 << TPATREQ_COL_LASTNAME << TPATREQ_COL_SEX << TPATREQ_COL_UID;
                QStringList keys = patientMap.keys();
                for (qint32 i = 0; i < keys.size(); i++){
                    if (avoid.contains(keys.at(i))) continue;
                    if (required.contains(keys.at(i))){
                        columns << keys.at(i);
                        values << patientMap.value(keys.at(i)).toString();
                    }
                    else{
                        columnsOpt << keys.at(i);
                        valuesOpt << patientMap.value(keys.at(i)).toString();
                    }
                }

                // Adding the DR ID.
                columns << TPATREQ_COL_DOCTORID; values << druid;
                client->appendSET(TABLE_PATIENTS_REQ_DATA,columns,values);
                client->appendSET(TABLE_PATIENTS_OPT_DATA,columnsOpt,valuesOpt);
            }
        }
    }

}

void LocalInformationManager::findRemainingReports(){

    patientReportRequest.clear();

    // Creating the source directory path.
    QString baseDir = config->getString(CONFIG_OUTPUT_DIR) + "/" + QString(DIRNAME_RAWDATA)
            + "/" + config->getString(CONFIG_DOCTOR_UID);

    QDir doctorDir(baseDir);
    if (!doctorDir.exists()){
        log.appendError("Base directory for the current Doctor: " + baseDir + " was not found");
        return;
    }

    // The Directory exists, so a list of all directories inside it, is generated.
    QStringList directories = doctorDir.entryList(QStringList(),QDir::Dirs | QDir::NoDotAndDotDot);
    QVariantMap patientData = localDB.value(config->getString(CONFIG_DOCTOR_UID)).toMap().value(PATIENT_DATA).toMap();
    for (qint32 i = 0; i < directories.size(); i++){
        if (!patientData.contains(directories.at(i))) continue;

        // Separating all files by extension and prefix if necessary.
        QStringList filters;
        QDir patientDir(baseDir + "/" + directories.at(i));

        filters << "*.rep";
        QStringList reports = patientDir.entryList(filters,QDir::Files);

        filters.clear();
        filters << QString(FILE_OUTPUT_BINDING_BC) +  "*.dat";
        QStringList bcFiles = patientDir.entryList(filters,QDir::Files);

        filters.clear();
        filters << QString(FILE_OUTPUT_BINDING_UC) +  "*.dat";
        QStringList ucFiles = patientDir.entryList(filters,QDir::Files);

        filters.clear();
        filters << QString(FILE_OUTPUT_READING) +  "*.dat";
        QStringList readFiles = patientDir.entryList(filters,QDir::Files);

        QHash<QString, QStringList> fileSetsByTimeStamp;

        // Joining BC and UC Files as set of files.
        QSet<QString> bcSuffixes;
        for (qint32 j = 0; j < bcFiles.size(); j++){
            QString suffix = bcFiles.at(j);
            suffix = suffix.mid(QString(FILE_OUTPUT_BINDING_BC).size(),-1);
            bcSuffixes << suffix;
        }
        for (qint32 j = 0; j < ucFiles.size(); j++){
            QString suffix = ucFiles.at(j);
            suffix = suffix.mid(QString(FILE_OUTPUT_BINDING_UC).size(),-1);
            if (bcSuffixes.contains(suffix)){
                // This is a pair of binding files that need to be processed.
                QStringList list;
                list << patientDir.path() + "/" + ucFiles.at(j)
                     << patientDir.path() + "/" + QString(FILE_OUTPUT_BINDING_UC) + suffix;
                // Getting the timestamp for the file
                QStringList info = getBindingFileInformation(ucFiles.at(j));
                if (!info.isEmpty()){
                    fileSetsByTimeStamp[info.last()] = list;
                }
            }
        }

        // Checking the reading files.
        for (qint32 j = 0; j < readFiles.size(); j++){
            // Getting the timestamp.
            QStringList info = getReadingInformation(readFiles.at(j));
            if (!info.isEmpty()){
                if (fileSetsByTimeStamp.contains(info.first())){
                    fileSetsByTimeStamp[info.first()].append(patientDir.path() + "/" + readFiles.at(j));
                }
                else{
                    QStringList list;
                    list << patientDir.path() + "/" + readFiles.at(j);
                    fileSetsByTimeStamp[info.first()] = list;
                }
            }
        }


        // Checking the generated files against the report time stamp.
        QSet<QString> existingTimeStamps;
        for (qint32 j = 0; j < reports.size(); j++){
            QString repfile = reports.at(i);
            QStringList parts = repfile.split(".");
            repfile = parts.first();
            QString prefix = QString(FILE_REPORT_NAME) + "_";
            QString timestamp = repfile.mid(prefix.size());
            existingTimeStamps << timestamp;
        }

        QStringList keys = fileSetsByTimeStamp.keys();
        QHash<QString, QStringList> fileSetsWithoutReport;
        for (qint32 j = 0; j < keys.size(); j++){
            if (!existingTimeStamps.contains(keys.at(j))){
                // This report must be generated
                fileSetsWithoutReport[keys.at(j)] = fileSetsByTimeStamp.value(keys.at(j));
            }
        }

        if (fileSetsWithoutReport.size() > 0){
            patientReportRequest[directories.at(i)] = fileSetsWithoutReport;
        }
    }

    printReportRequests();

}

bool LocalInformationManager::addDoctorData(const QString &dr_uid,const QStringList &cols, const QStringList &values){

    // No need to check for UID unicity. If the UID is the same this is an update.

    QVariantMap drinfo;
    // Adding the dr information.
    for (qint32 i = 0; i < cols.size(); i++){
        drinfo[cols.at(i)] = values.at(i);
    }
    drinfo[DOCTOR_UPDATE] = true;

    // Empty map for the patient.
    drinfo[PATIENT_DATA] = QVariantMap();
    localDB[dr_uid] = drinfo;

    return backupDB();
}

bool LocalInformationManager::addPatientData(const QString &patient_uid, const QStringList &cols, const QStringList &values){

    QVariantMap drinfo = localDB.value(config->getString(CONFIG_DOCTOR_UID)).toMap();
    QVariantMap patients = drinfo.value(PATIENT_DATA).toMap();

    // Patient information to variant map.
    QVariantMap thispatient;
    for (qint32 i = 0; i < cols.size(); i++){
        thispatient[cols.at(i)] = values.at(i);
    }
    thispatient[PATIENT_UPDATE] = true;

    // Storing the patient information in the patient map for the doctor
    patients[patient_uid] = thispatient;

    // Storing the new patient map in the doctor information
    drinfo[PATIENT_DATA] = patients;

    // Storing the new dr map in the local db.
    localDB[config->getString(CONFIG_DOCTOR_UID)] = drinfo;

    // Finally saving the DB.
    return backupDB();

}

QList<QStringList> LocalInformationManager::getDoctorList(){
    QList<QStringList> ans;
    QStringList names;
    QStringList uids = localDB.keys();
    for (qint32 i = 0; i < uids.size(); i++){
        QVariantMap drinfo = localDB.value(uids.at(i)).toMap();
        names << drinfo.value(TDOCTOR_COL_FIRSTNAME).toString() + " " + drinfo.value(TDOCTOR_COL_LASTNAME).toString();
    }
    if (!names.isEmpty()) ans << names << uids;
    return ans;
}

QList<QStringList> LocalInformationManager::getPatientListForDoctor(){
    QList<QStringList> ans;
    QStringList names;
    QVariantMap patients = localDB.value(config->getString(CONFIG_DOCTOR_UID)).toMap().value(PATIENT_DATA).toMap();
    QStringList uids = patients.keys();
    for (qint32 i = 0; i < uids.size(); i++){
        QVariantMap patinfo = patients.value(uids.at(i)).toMap();
        names << patinfo.value(TPATREQ_COL_FIRSTNAME).toString() + " " + patinfo.value(TPATREQ_COL_LASTNAME).toString();
    }
    if (!names.isEmpty()) {
        ans << names << uids;
        findRemainingReports();
        QStringList isoklist;
        for (qint32 i = 0; i < uids.size(); i++){
            if (patientReportRequest.contains(uids.at(i))) isoklist << "false";
            else isoklist << "true";
        }
        ans << isoklist;
    }
    return ans;
}

void LocalInformationManager::loadDB(){

    // Checking to see if the directory structure exists.
    QDir basedir(config->getString(CONFIG_OUTPUT_DIR) + "/" + DIRNAME_RAWDATA);
    if (!basedir.exists()){
        log.appendError("LOCALDB LOADING: Base Data directory: " + basedir.path() + " does not exist and it should, at this point");
        return;
    }

    QString dbfile = basedir.path() + "/" + LOCAL_DB;

    QFile file(dbfile);
    if (!file.open(QFile::ReadOnly)){
        log.appendError("LOCALDB: Cannot open DB file " + dbfile  + " for reading");
        return;
    }

    QDataStream reader(&file);
    qint32 dummy;
    localDB.clear();
    reader >> dummy;
    reader >> localDB;
    file.close();

}

bool LocalInformationManager::backupDB(){

    // Checking to see if the directory structure exists.
    QDir basedir(config->getString(CONFIG_OUTPUT_DIR) + "/" + DIRNAME_RAWDATA);
    if (!basedir.exists()){
        log.appendError("LOCALDB: Base Data directory: " + basedir.path() + " does not exist and it should, at this point");
        return false;
    }

    QString dbfile = basedir.path() + "/" + LOCAL_DB;

    QFile file(dbfile);
    if (!file.open(QFile::WriteOnly)){
        log.appendError("LOCALDB: Cannot open DB file " + dbfile  + " for writing");
        return false;
    }

    QDataStream writer(&file);
    writer << LOCAL_DB_VERSION;
    writer << localDB;
    file.close();

    // Backing up the DB.
    QFile(LOCAL_DB).remove();
    QFile::copy(dbfile,LOCAL_DB);

    return true;

}

void LocalInformationManager::printLocalDB(){

    QStringList druids = localDB.keys();
    QString tab1 = "   ";
    QString tab2 = tab1 + tab1;
    QString tab3 = tab2 + tab1;

    for (qint32 i = 0; i < druids.size(); i++){
        qWarning() << "DR UID: " << druids.at(i);
        QVariantMap drinfo = localDB.value(druids.at(i)).toMap();
        QStringList drcols = drinfo.keys();
        for (qint32 j = 0; j < drcols.size(); j++){
            if (drcols.at(j) != PATIENT_DATA)
               qWarning() << tab1 << drcols.at(j) << " = " << drinfo.value(drcols.at(j)).toString();
        }
        QVariantMap patients = drinfo.value(PATIENT_DATA).toMap();
        qWarning() << tab1 << "PATIENT DATA:";
        QStringList patientUIDs = patients.keys();
        for (qint32 j = 0; j < patientUIDs.size(); j++){
            qWarning() << tab2 << patientUIDs.at(j);
            QVariantMap patientInfo = patients.value(patientUIDs.at(j)).toMap();
            QStringList patientCols = patientInfo.keys();
            for (qint32 k = 0; k < patientCols.size(); k++){
                qWarning() << tab3 << patientCols.at(k) << " = " << patientInfo.value(patientCols.at(k)).toString();
            }
        }
    }

}

void LocalInformationManager::printReportRequests(){

    QStringList timestamps = patientReportRequest.keys();
    for (qint32 i = 0; i < timestamps.size(); i++){
        qWarning() << "TIMESTAMP: " << timestamps.at(i) << ". FILES: " << patientReportRequest.value(timestamps.at(i));
    }

}
