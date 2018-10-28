#include "InstDBComm.h"

InstDBComm::InstDBComm()
{
    // Seeding the random number generator
    qsrand(QTime::currentTime().msec());

    QStringList letters;
    letters << "a" << "b" << "c" << "e" << "e" << "f" << "g" << "h" << "i" << "j" << "k" << "l" << "m"
            << "n" << "o" << "p" << "q" << "r" << "s" << "t" << "u" << "v" << "w" << "x" << "y" << "z";
    QStringList LETTERS;
    for (qint32 i = 0; i < letters.size(); i++){
        LETTERS << letters.at(i).toUpper();
    }
    QStringList numbers;
    numbers << "0" << "1" << "2" << "3" << "4"
            << "5" << "6" << "7" << "8" << "9";

    characterPool << letters << LETTERS << numbers;

}


bool InstDBComm::initConnection(){
    // Loading the database configuration
    ConfigurationManager::CommandVerifications cv;
    ConfigurationManager::Command cmd;

    cmd.clear();
    cv[CONFIG_DBHOST] = cmd;
    cv[CONFIG_DBNAME] = cmd;
    cv[CONFIG_DBPASSWORD] = cmd;
    cv[CONFIG_DBUSER] = cmd;

    cmd.type = ConfigurationManager::VT_INT;
    cmd.optional = true;
    cv[CONFIG_DBPORT] = cmd;

    ConfigurationManager dbconf;
    dbconf.setupVerification(cv);
    if (!dbconf.loadConfiguration(CONFIG_FILE,COMMON_TEXT_CODEC)){
        error = "Loading DB configuration file: " + dbconf.getError();
        return false;
    }

    QString portmsg = "";
    if (dbconf.containsKeyword(CONFIG_DBPORT)){
        portmsg = ": " + dbconf.getString(CONFIG_DBPORT);
    }

    if (!db.initDB(&dbconf)){
        error = db.getError();
        return false;
    }

    connectionMsg = "Established connection to DB: " + dbconf.getString(CONFIG_DBNAME) + " for " + dbconf.getString(CONFIG_DBUSER) + "@" + dbconf.getString(CONFIG_DBHOST) + portmsg;

    return true;
}

bool InstDBComm::addNewInstitution(const Institution &inst){

    // First the existing UIDs need to be gathered.
    QStringList columns;

    columns << TINST_COL_UID;
    if (!db.readFromDB(TABLE_INSTITUTION,columns,"")){
        error = db.getError();
        return false;
    }

    DBData result = db.getLastResult();

    QSet<qint32> existingUIDs;
    if (!result.rows.isEmpty()){
        for (qint32 i = 0; i < result.rows.size(); i++){
            // Added this message just in case I screwe up. Should never show.
            if (result.rows.at(i).isEmpty()) qWarning() << "Attempting to read row with no column!!!";
            existingUIDs << result.rows.at(i).first().toInt();
        }
    }

    // Generating the unique UID and the password.
    generatedUID = generateUID(existingUIDs);
    generatedPassword = generatePassword();

    // Generating the Hash of the password
    QString hashpass(QCryptographicHash::hash(generatedPassword.toUtf8(),QCryptographicHash::Sha256).toHex());

    columns.clear();
    columns << TINST_COL_ETSERIAL << TINST_COL_EVALS << TINST_COL_HASHPASS << TINST_COL_NAME << TINST_COL_UID << TINST_COL_ETBRAND << TINST_COL_ETMODEL;
    QStringList values;
    values << inst.etserial << inst.numEvals << hashpass << inst.name << QString::number(generatedUID) << inst.etbrand << inst.etmodel;

    if (!db.insertDB(TABLE_INSTITUTION,columns,values)){
        error = db.getError();
        return false;
    }

    return true;
}

bool InstDBComm::updateNewInstitution(const Institution &inst){

    QStringList columns;
    QStringList values;

    if (!inst.name.isEmpty()){
        columns << TINST_COL_NAME;
        values << inst.name;
    }

    if (!inst.numEvals.isEmpty()){
        columns << TINST_COL_EVALS;
        values << inst.numEvals;
    }

    if (!inst.etserial.isEmpty()){
        columns << TINST_COL_ETSERIAL;
        values << inst.etserial;
    }

    if (!inst.etmodel.isEmpty()){
        columns << TINST_COL_ETMODEL;
        values << inst.etmodel;
    }

    if (!inst.etbrand.isEmpty()){
        columns << TINST_COL_ETBRAND;
        values << inst.etbrand;
    }

    if (!db.updateDB(TABLE_INSTITUTION,columns,values,QString(TINST_COL_KEYID) + " = '" + inst.keyid + "'")){
        error = db.getError();
        return false;
    }

    return true;

}

bool InstDBComm::resetPassword(const QString &keyidInst){

    // Generating a new password
    generatedPassword = generatePassword();

    // Generating the Hash of the password
    QString hashpass(QCryptographicHash::hash(generatedPassword.toUtf8(),QCryptographicHash::Sha256).toHex());

    QStringList columns;
    QStringList values;

    columns << TINST_COL_HASHPASS;
    values << hashpass;

    if (!db.updateDB(TABLE_INSTITUTION,columns,values,QString(TINST_COL_KEYID) + " = '" + keyidInst + "'")){
        error = db.getError();
        return false;
    }

    return true;

}

bool InstDBComm::deleteUserInfo(const QString &uid){

    QStringList columns;
    columns << TPATREQ_COL_UID;
    QString condition = TPATREQ_COL_DOCTORID;
    condition = condition + " = '" + uid + "'";
    if (!db.readFromDB(TABLE_PATIENTS_REQ_DATA,columns,condition)){
        error = db.getError();
        return false;
    }

    // Getting the UIDs

    DBData data = db.getLastResult();

    if (data.rows.size() > 0){

        QSet<QString> patuids;
        for (qint32 i = 0; i < data.rows.size(); i++){
            if (data.rows.at(i).isEmpty()) continue;
            patuids << data.rows.at(i).first();
        }

        // Deleteing data form REQ Table
        QStringList conds;
        QSetIterator<QString> iter(patuids);
        while (iter.hasNext()){
            conds << QString(TPATREQ_COL_UID) + " = '" + iter.next() + "'";
        }
        condition = conds.join(" OR ");

        if (!db.deleteRowFromDB(TABLE_PATIENTS_REQ_DATA,condition)){
            error = db.getError();
            return false;
        }

        // Deleteing data from OPT Table.
        conds.clear();
        iter.toFront();
        while (iter.hasNext()){
            conds << QString(TPATOPT_COL_PATIENTID) + " = '" + iter.next() + "'";
        }
        condition = conds.join(" OR ");

        if (!db.deleteRowFromDB(TABLE_PATIENTS_OPT_DATA,condition)){
            error = db.getError();
            return false;
        }
    }

    // Finally deleting the doctor
    condition = QString(TDOCTOR_COL_UID) + " = '" + uid + "'";
    if (!db.deleteRowFromDB(TABLE_DOCTORS,condition)){
        error = db.getError();
        return false;
    }

    return true;

}

QList<InstDBComm::Institution> InstDBComm::getAllInstitutions(bool *isOk){
    *isOk = true;
    QList<Institution> ans;

    QStringList columns;
    columns << TINST_COL_NAME << TINST_COL_KEYID;
    if (!db.readFromDB(TABLE_INSTITUTION,columns,"")){
        error = db.getError();
        *isOk = false;
        return ans;
    }

    DBData result = db.getLastResult();

    for (qint32 i = 0; i <  result.rows.size(); i++){
        Institution inst;
        inst.name = result.rows.at(i).first();
        inst.keyid = result.rows.at(i).last();
        ans << inst;
    }

    return ans;
}

QStringList InstDBComm::getPossibleTestUsers(bool *isOk){

    *isOk = true;
    QStringList ans;
    QString condition = TDOCTOR_COL_UID;
    condition = condition + " LIKE '%" + QString(TEST_UID) + "%'";

    QStringList columns;
    columns << TDOCTOR_COL_FIRSTNAME << TDOCTOR_COL_LASTNAME << TDOCTOR_COL_UID;

    if (!db.readFromDB(TABLE_DOCTORS,columns,condition)){
        error = db.getError();
        *isOk = false;
        return ans;
    }

    DBData rows = db.getLastResult();

    for (qint32 i = 0; i < rows.rows.size(); i++){
        if (rows.rows.at(i).size() != columns.size()){
            error = "Geting possible users: Expecting 3 coluns for row " + QString(i) + " but found: " + QString(rows.rows.at(i).size());
            ans.clear();
            return ans;
        }
        ans << rows.rows.at(i).at(0) + " " +  rows.rows.at(i).at(1) + " (" + rows.rows.at(i).at(2) + ")";
        ans << rows.rows.at(i).at(2); // This ias added so the UID does not need to be obtained again.
    }

    return ans;
}

InstDBComm::Institution InstDBComm::getInstitutionInfo(const QString &keyidInst){
    QStringList columns;
    Institution info;

    columns << TINST_COL_ETSERIAL << TINST_COL_EVALS << TINST_COL_NAME << TINST_COL_UID << TINST_COL_KEYID << TINST_COL_ETBRAND << TINST_COL_ETMODEL;
    if (!db.readFromDB(TABLE_INSTITUTION,columns,QString(TINST_COL_KEYID) + " = '" + keyidInst + "'")){
        error = db.getError();
        info.ok = false;
        return info;
    }

    info.ok = true;

    if (db.getLastResult().rows.size() != 1){
        info.ok = false;
        error = "When getting info on Institution, obtained a number of rows different thatn 1: " + QString::number(db.getLastResult().rows.size());
        return info;
    }

    QStringList row = db.getLastResult().rows.first();

    for (qint32 i = 0; i < columns.size(); i++){
        switch (i) {
        case 0:
            info.etserial = row.at(i);
            break;
        case 1:
            info.numEvals = row.at(i);
            break;
        case 2:
            info.name = row.at(i);
            break;
        case 3:
            info.uid = row.at(i);
            break;
        case 4:
            info.keyid = row.at(i);
            break;
        case 5:
            info.etbrand= row.at(i);
            break;
        case 6:
            info.etmodel = row.at(i);
            break;
        default:
            break;
        }
    }

    return info;
}

//===================================================================================================================================

qint32 InstDBComm::generateUID(const QSet<qint32> &existing){
    qint32 number = 0;
    while (true){
        number = 0;
        for (qint32 i = 0; i < MAX_UID_SIZE_IN_BITS; i++){
            number = number + (qrand() % 2);
            number = number << 1;
        }
        if (!existing.contains(number)) return number;
    }

}

QString InstDBComm::generatePassword(){
    QString pass = "";
    for (qint32 i = 0; i < PASSWORD_SIZE; i++){
        qint32 type = (qrand() % characterPool.size());
        qint32 charindex = (qrand() % characterPool.at(type).size());
        pass = pass + characterPool.at(type).at(charindex);
    }
    return pass;
}
