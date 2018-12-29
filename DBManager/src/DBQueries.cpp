#include "DBQueries.h"

DBQueries::DBQueries()
{

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

bool DBQueries::initConnection(){
    // Loading the database configuration
    ConfigurationManager::CommandVerifications cv;
    ConfigurationManager::Command cmd;

    cmd.clear();
    cv[CONFIG_DBHOST]     = cmd;
    cv[CONFIG_DBNAME]     = cmd;
    cv[CONFIG_DBPASSWORD] = cmd;
    cv[CONFIG_DBUSER]     = cmd;

    cv[CONFIG_ID_DBHOST]     = cmd;
    cv[CONFIG_ID_DBNAME]     = cmd;
    cv[CONFIG_ID_DBPASSWORD] = cmd;
    cv[CONFIG_ID_DBUSER]     = cmd;

    cv[CONFIG_PATDATA_DBHOST]     = cmd;
    cv[CONFIG_PATDATA_DBNAME]     = cmd;
    cv[CONFIG_PATDATA_DBPASSWORD] = cmd;
    cv[CONFIG_PATDATA_DBUSER]     = cmd;

    // Not used but part of the configuration file
    cv[CONFIG_S3_ADDRESS]     = cmd;

    cmd.type = ConfigurationManager::VT_INT;
    cv[CONFIG_DBPORT]         = cmd;
    cv[CONFIG_ID_DBPORT]      = cmd;
    cv[CONFIG_PATDATA_DBPORT] = cmd;

    ConfigurationManager dbconf;
    dbconf.setupVerification(cv);
    QString configurationFile = COMMON_PATH_FOR_DB_CONFIGURATIONS;
    qint32 definitions = 0;

#ifdef SERVER_LOCALHOST
    configurationFile = configurationFile + "db_localhost";
    definitions++;
#endif
#ifdef SERVER_DEVELOPMENT
    configurationFile = configurationFile + "db_development";
    definitions++;
#endif
#ifdef SERVER_PRODUCTION
    configurationFile = configurationFile + "db_production";
    definitions++;
#endif

    if (definitions != 1){
        error = "The number of DB Configuration files is " + QString::number(definitions) + " instead of 1" ;
        return false;
    }

    if (!dbconf.loadConfiguration(configurationFile,COMMON_TEXT_CODEC)){
        error = "Loading DB configuration file: " + dbconf.getError();
        return false;
    }

    QString portmsg = "";
    QString portmsgID = "";
    QString portmsgPatdata = "";
    portmsg = ": " + dbconf.getString(CONFIG_DBPORT);
    portmsgID = ": " + dbconf.getString(CONFIG_ID_DBPORT);
    portmsgPatdata = ": " + dbconf.getString(CONFIG_PATDATA_DBPORT);

    dbBase.setupDB(DB_NAME_BASE,dbconf.getString(CONFIG_DBHOST),dbconf.getString(CONFIG_DBNAME),dbconf.getString(CONFIG_DBUSER),dbconf.getString(CONFIG_DBPASSWORD),dbconf.getInt(CONFIG_DBPORT));
    dbPatID.setupDB(DB_NAME_ID,dbconf.getString(CONFIG_ID_DBHOST),dbconf.getString(CONFIG_ID_DBNAME),dbconf.getString(CONFIG_ID_DBUSER),dbconf.getString(CONFIG_ID_DBPASSWORD),dbconf.getInt(CONFIG_ID_DBPORT));
    dbPatData.setupDB(DB_NAME_PATDATA,dbconf.getString(CONFIG_PATDATA_DBHOST),dbconf.getString(CONFIG_PATDATA_DBNAME),dbconf.getString(CONFIG_PATDATA_DBUSER),dbconf.getString(CONFIG_PATDATA_DBPASSWORD),dbconf.getInt(CONFIG_PATDATA_DBPORT));

    if (!dbBase.open()){
        error = "Connecting to Base DB: " + dbBase.getError();
        return false;
    }

    if (!dbPatID.open()){
        error = "Connecting to Pat ID DB: " + dbPatID.getError();
        return false;
    }

    if (!dbPatData.open()){
        error = "Connecting to PatData DB: " + dbPatID.getError();
        return false;
    }

    connectionMsg = "Established connection to DBs:\n" + dbconf.getString(CONFIG_DBNAME) + " for " + dbconf.getString(CONFIG_DBUSER) + "@" + dbconf.getString(CONFIG_DBHOST) + portmsg + "\n";
    connectionMsg = connectionMsg + dbconf.getString(CONFIG_ID_DBNAME) + " for " + dbconf.getString(CONFIG_ID_DBUSER) + "@" + dbconf.getString(CONFIG_ID_DBHOST) + portmsgID + "\n";
    connectionMsg = connectionMsg + dbconf.getString(CONFIG_PATDATA_DBNAME) + " for " + dbconf.getString(CONFIG_PATDATA_DBUSER) + "@" + dbconf.getString(CONFIG_PATDATA_DBHOST) + portmsgID + "\n";

    // Seeding the random number generator (this ensures that the seed is done in the proper thread).
    int seed = QTime::currentTime().msec();
    qsrand(seed);

    return true;
}

qint32 DBQueries::addNewProduct(StringMap product){

    // Inserting the produc info
    product.remove(TPLACED_PROD_COL_KEYID);
    QStringList columns, values;
    stringMapToColumnValuePair(product,&columns,&values);
    if (!dbBase.insertDB(TABLE_PLACEDPRODUCTS,columns,values)){
        error = dbBase.getError();
        return -1;
    }

    // All good, getting the keyid to generate the product serial number
    qint32 keyid = dbBase.getNewestKeyid(TPLACED_PROD_COL_KEYID,TABLE_PLACEDPRODUCTS);
    if (keyid == -1){
        error = dbBase.getError();
        return -1;
    }

    QString valnum = QString::number(keyid);
    while (valnum.size() < 10) valnum = "0" + valnum;

    columns.clear(); values.clear();
    columns << TPLACED_PROD_COL_PRODSN;
    values << "VM" + valnum;
    QString condition = QString(TPLACED_PROD_COL_KEYID) + " = '" + QString::number(keyid) + "'";
    if (!dbBase.updateDB(TABLE_PLACEDPRODUCTS,columns,values,condition)){
        error = dbBase.getError();
        return -1;
    }

    return keyid;

}

qint32 DBQueries::addNewInstitution(StringMap inst){

    // First the existing UIDs need to be gathered, in order to ensure that the new one is unique
    QStringList columns;

    columns << TINST_COL_UID;
    if (!dbBase.readFromDB(TABLE_INSTITUTION,columns,"")){
        error = dbBase.getError();
        return -1;
    }

    DBData result = dbBase.getLastResult();

    QSet<qint32> existingUIDs;
    if (!result.rows.isEmpty()){
        for (qint32 i = 0; i < result.rows.size(); i++){
            // Added this message just in case I screwed up. Should never show.
            if (result.rows.at(i).isEmpty()) qWarning() << "Attempting to read row with no column!!!";
            existingUIDs << result.rows.at(i).first().toInt();
        }
    }

    // Generating the unique UID and the password.
    generatedUID = generateUID(existingUIDs);
    generatedPassword = generatePassword();

    // Generating the Hash of the password
    QString hashpass(QCryptographicHash::hash(generatedPassword.toUtf8(),QCryptographicHash::Sha256).toHex());
    inst[TINST_COL_HASHPASS] = hashpass;
    inst[TINST_COL_UID] = QString::number(generatedUID);

    columns.clear();
    QStringList values;

    // Removing the keyid column
    inst.remove(TINST_COL_KEYID);
    // And transforming the map into columns.
    stringMapToColumnValuePair(inst,&columns,&values);

    if (!dbBase.insertDB(TABLE_INSTITUTION,columns,values)){
        error = dbBase.getError();
        return -1;
    }

    // All good getting the keyid
    qint32 keyid = dbBase.getNewestKeyid(TINST_COL_KEYID,TABLE_INSTITUTION);
    if (keyid == -1){
        error = dbBase.getError();
        return -1;
    }

    return keyid;
}

bool DBQueries::updateNewInstitution(StringMap inst){

    QStringList columns;
    QStringList values;

    QString keyid = inst.value(TINST_COL_KEYID);
    inst.remove(TINST_COL_KEYID);

    stringMapToColumnValuePair(inst,&columns,&values);

    if (!dbBase.updateDB(TABLE_INSTITUTION,columns,values,QString(TINST_COL_KEYID) + " = '" + keyid + "'")){
        error = dbBase.getError();
        return false;
    }

    return true;

}

bool DBQueries::updateProduct(StringMap product){

    QStringList columns;
    QStringList values;

    QString keyid = product.value(TPLACED_PROD_COL_KEYID);
    product.remove(TPLACED_PROD_COL_KEYID);
    product.remove(TINST_COL_HASHPASS);
    product.remove(TINST_COL_NAME);

    stringMapToColumnValuePair(product,&columns,&values);

    if (!dbBase.updateDB(TABLE_PLACEDPRODUCTS,columns,values,QString(TINST_COL_KEYID) + " = '" + keyid + "'")){
        error = dbBase.getError();
        return false;
    }

    return true;

}


bool DBQueries::resetPassword(const QString &keyidInst){

    // Generating a new password
    generatedPassword = generatePassword();

    // Generating the Hash of the password
    QString hashpass(QCryptographicHash::hash(generatedPassword.toUtf8(),QCryptographicHash::Sha256).toHex());

    QStringList columns;
    QStringList values;

    columns << TINST_COL_HASHPASS;
    values << hashpass;

    if (!dbBase.updateDB(TABLE_INSTITUTION,columns,values,QString(TINST_COL_KEYID) + " = '" + keyidInst + "'")){
        error = dbBase.getError();
        return false;
    }

    return true;

}

bool DBQueries::deleteTestUsers(){

    QString patuid = QCryptographicHash::hash(QString(TEST_PAT_UID).toLatin1(),QCryptographicHash::Sha3_512).toHex();

    // Removing from db patid
    QString condition = QString(TPATID_COL_UID) + " ='" + patuid + "'";
    if (!dbPatID.deleteRowFromDB(TABLE_PATIENTD_IDS,condition)){
        error = dbPatID.getError();
        return false;
    }

    qWarning() << "removed from id";

    // Removing all doctor information
    condition = QString(TDOCTOR_COL_UID) + " ='" + QString(TEST_UID) + "'";
    if (!dbBase.deleteRowFromDB(TABLE_DOCTORS,condition)){
        error = dbBase.getError();
        return false;
    }

    qWarning() << "removed doctors";

    // Removing result information
    condition = QString(TEYERES_COL_DOCTORID) + " ='" + QString(TEST_UID) + "'";
    if (!dbBase.deleteRowFromDB(TABLE_EYE_RESULTS,condition)){
        error = dbBase.getError();
        return false;
    }

    qWarning() << "removed results";

    // Removing all patient information for the test doctor
    condition = QString(TPATDATA_COL_DOCTORID) + " ='" + QString(TEST_UID) + "'";
    if (!dbPatData.deleteRowFromDB(TABLE_PATDATA,condition)){
        error = dbPatData.getError();
        return false;
    }

    qWarning() << "removed patient data";

    return true;

}

QList<DBQueries::StringMap> DBQueries::getAllInstitutions(bool *isOk){
    *isOk = true;
    QList<StringMap> ans;

    QStringList columns;
    columns << TINST_COL_NAME << TINST_COL_KEYID;
    if (!dbBase.readFromDB(TABLE_INSTITUTION,columns,"")){
        error = dbBase.getError();
        *isOk = false;
        return ans;
    }

    DBData result = dbBase.getLastResult();

    for (qint32 i = 0; i <  result.rows.size(); i++){
        StringMap map = getEmptyStringMap(BSMT_INSTITUTION);
        map[TINST_COL_NAME]  = result.rows.at(i).first();
        map[TINST_COL_KEYID] = result.rows.at(i).last();
        ans << map;
    }

    return ans;
}

QStringList DBQueries::searchForPlacedProducts(const QString &search, bool *isOk){

    *isOk = true;
    QStringList ans;


    QStringList columns = getEmptyStringMap(BSMT_PLACED_PRODUCT).keys();
    QStringList conditions;
    for (qint32 i = 0; i < columns.size(); i++){
        conditions << "(" + columns.at(i) + " LIKE '%" + search + "%')";
    }
    QString condition = conditions.join(" OR ");

    columns.clear();
    columns << TPLACED_PROD_COL_KEYID;
    if (!dbBase.readFromDB(TABLE_PLACEDPRODUCTS,columns,condition)){
        error = dbBase.getError();
        *isOk = false;
        return ans;
    }

    DBData result = dbBase.getLastResult();

    for (qint32 i = 0; i <  result.rows.size(); i++){
        ans << result.rows.at(i).first();
    }

    return ans;

}

QList<DBQueries::StringMap> DBQueries::getAllProductsForInstitutions(const QString &instUID, bool *isOk){
    *isOk = true;
    QList<StringMap> ans;

    QStringList columns;
    columns << TPLACED_PROD_COL_PRODUCT << TPLACED_PROD_COL_PRODSN << TPLACED_PROD_COL_KEYID;
    if (!dbBase.readFromDB(TABLE_PLACEDPRODUCTS,columns,QString(TPLACED_PROD_COL_INSTITUTION) + " = '" + instUID + "'")){
        error = dbBase.getError();
        *isOk = false;
        return ans;
    }

    DBData result = dbBase.getLastResult();

    for (qint32 i = 0; i <  result.rows.size(); i++){
        StringMap map = getEmptyStringMap(BSMT_PLACED_PRODUCT);
        map[TPLACED_PROD_COL_PRODUCT]  = result.rows.at(i).first();
        map[TPLACED_PROD_COL_PRODSN] = result.rows.at(i).at(1);
        map[TPLACED_PROD_COL_KEYID] = result.rows.at(i).last();
        ans << map;
    }

    return ans;
}


DBQueries::StringMap DBQueries::getInstitutionInfo(const QString &keyidInst, bool *isOk){
    QStringList columns;
    StringMap info;
    *isOk = true;

    info = getEmptyStringMap(BSMT_INSTITUTION);

    columns = info.keys();
    if (!dbBase.readFromDB(TABLE_INSTITUTION,columns,QString(TINST_COL_KEYID) + " = '" + keyidInst + "'")){
        error = dbBase.getError();
        *isOk = false;
        return info;
    }

    if (dbBase.getLastResult().rows.size() != 1){
        *isOk = false;
        error = "When getting info on Institution, obtained a number of rows different thatn 1: " + QString::number(dbBase.getLastResult().rows.size());
        return info;
    }

    QStringList row = dbBase.getLastResult().rows.first();
    for (qint32 i = 0; i < columns.size(); i++){
        info[columns.at(i)] = row.at(i);
    }

    return info;
}

DBQueries::StringMap DBQueries::getProductInformation(const QString &keyidPP, bool *isOk){

    QStringList columns;
    StringMap info;
    *isOk = true;

    info = getEmptyStringMap(BSMT_PLACED_PRODUCT);

    columns = info.keys();
    if (!dbBase.readFromDB(TABLE_PLACEDPRODUCTS,columns,QString(TPLACED_PROD_COL_KEYID) + " = '" + keyidPP + "'")){
        error = dbBase.getError();
        *isOk = false;
        return info;
    }

    if (dbBase.getLastResult().rows.size() != 1){
        *isOk = false;
        error = "When getting info on a placed product, obtained a number of rows different thatn 1: " + QString::number(dbBase.getLastResult().rows.size());
        return info;
    }

    QStringList row = dbBase.getLastResult().rows.first();
    for (qint32 i = 0; i < columns.size(); i++){
        info[columns.at(i)] = row.at(i);
    }

    // Getting the information for the institution for the hashed password.
    columns.clear();
    columns << TINST_COL_HASHPASS << TINST_COL_NAME;
    if (!dbBase.readFromDB(TABLE_INSTITUTION,columns,QString(TINST_COL_UID)+ " = '" + info.value(TPLACED_PROD_COL_INSTITUTION) + "'")){
        *isOk = false;
        error = dbBase.getError();
        return info;
    }

    if (dbBase.getLastResult().rows.size() != 1){
        *isOk = false;
        error = "When getting info on institution for a placed a placed product, obtained a number of rows different thatn 1: " + QString::number(dbBase.getLastResult().rows.size());
        return info;
    }

    row = dbBase.getLastResult().rows.first();
    if (row.size() != 2){
        *isOk = false;
        error = "When getting info on institution for a placed a placed product, obtained a number of columns different than 2: " + QString::number(row.size());
        return info;
    }

    info[TINST_COL_HASHPASS] = row.first();
    info[TINST_COL_NAME] = row.last();

    return info;
}

//===================================================================================================================================

qint32 DBQueries::generateUID(const QSet<qint32> &existing){
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

QString DBQueries::generatePassword(){
    QString pass = "";
    for (qint32 i = 0; i < PASSWORD_SIZE; i++){
        qint32 type = (qrand() % characterPool.size());
        qint32 charindex = (qrand() % characterPool.at(type).size());
        pass = pass + characterPool.at(type).at(charindex);
    }
    return pass;
}

void DBQueries::stringMapToColumnValuePair(const StringMap &map, QStringList *cols, QStringList *vals){
    cols->clear();
    vals->clear();
    QStringList cls = map.keys();
    for (qint32 i = 0; i < cls.size(); i++){
        cols->append(cls.at(i));
        vals->append(map.value(cls.at(i)));
    }
}

DBQueries::StringMap DBQueries::getEmptyStringMap(BaseStringMapType bsmt){

    QStringList cols;
    switch (bsmt){
       case BSMT_INSTITUTION:
          cols << TINST_COL_ADDRESS << TINST_COL_EMAIL << TINST_COL_EVALS << TINST_COL_FNAME << TINST_COL_HASHPASS
               << TINST_COL_LNAME << TINST_COL_NAME << TINST_COL_PHONE << TINST_COL_KEYID << TINST_COL_UID;
       break;
       case BSMT_PLACED_PRODUCT:
          cols << TPLACED_PROD_COL_CHINRESTMODEL << TPLACED_PROD_COL_CHINRESTSN << TPLACED_PROD_COL_ETBRAND << TPLACED_PROD_COL_ETMODEL << TPLACED_PROD_COL_ETSERIAL
               << TPLACED_PROD_COL_INSTITUTION << TPLACED_PROD_COL_PCMODEL << TPLACED_PROD_COL_PRODSN << TPLACED_PROD_COL_PRODUCT << TPLACED_PROD_COL_SOFTVER
               << TPLACED_PROD_COL_KEYID;
       break;
    }
    StringMap map;
    for (qint32 i = 0; i < cols.size(); i++){
        map[cols.at(i)] = "";
    }

    return map;
}
