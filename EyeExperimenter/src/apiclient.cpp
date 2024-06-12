#include "apiclient.h"

APIClient::APIClient(QObject *parent) : QObject(parent)
{
    API = Globals::API_URL;
    rest_controller.setBaseAPI(API);
    lastGeneratedLogFileName = "";
    lastSubjectUpdateJSONFile = "";
    this->eyeTrackerKey = "";
    connect(&rest_controller,&RESTAPIController::gotReplyData,this,&APIClient::gotReply);

}

QVariantMap APIClient::getMapDataReturned() const {
    return retdata;
}

qint32 APIClient::getLastRequestType() const {
    return lastRequest;
}

void APIClient::configure(const QString &institution_id, const QString &instance_number, const QString &version, const QString &region, const QString &key, const QString &hash_code){
    this->institution_id = institution_id;
    this->instance_number = instance_number;
    this->key = key;
    this->version = version;
    this->secret = hash_code;
    this->region = region;
}

void APIClient::setEyeTrackerKey(const QString &key){
    this->eyeTrackerKey = key;
}

QString APIClient::getEyeTrackerKey() const {
    return this->eyeTrackerKey;
}

bool APIClient::requestOperatingInfo(const QString &hardware_description_string, bool sendLog, QVariantMap updated_subject_records){

    error = "";
    lastGeneratedLogFileName = "";

    // Clearing everything but the URL.
    rest_controller.resetRequest();

    // Forming the URL
    // qDebug() << "URL for OI" << ENDPOINT_OPERATING_INFO + "/" + institution_id;
    rest_controller.setAPIEndpoint(ENDPOINT_OPERATING_INFO + "/" + institution_id);
    QVariantMap map;
    map.insert(URLPARAM_PPKEY,this->eyeTrackerKey);
    map.insert(URLPARAM_VERSION,this->version);
    map.insert(URLPARAM_INSTANCE,instance_number);

    // Slashes are problematic. We replace them with dashes.
    QString hwstring = hardware_description_string;
    hwstring = hwstring.replace("/","-");
    hwstring = hwstring.replace("\\","-");

    // Setting the required post data.
    QVariantMap postdata;
    postdata.insert(POST_FIELD_INSTITUTION_ID,institution_id);
    postdata.insert(POST_FIELD_INSTITUTION_INSTANCE,instance_number);
    postdata.insert(POST_FIELD_HW_STRING, hwstring);
    rest_controller.setPOSTDataToSend(postdata);

    rest_controller.setURLParameters(map);

    // In order to append the log file we need to make a copy of it and change it's name.
    // Othewise when storing in the server the files would be overwritten.
    if (sendLog){

        LogPrep logPrep(Globals::Paths::LOGFILE,institution_id,instance_number);
        QString tempName = logPrep.createFullLogBackup();
        if (tempName == ""){
            error = "Unable to create Log Backup. Reason: " + logPrep.getError();
            return false;
        }

        lastGeneratedLogFileName = tempName;

        if (!rest_controller.appendFileForRequest(lastGeneratedLogFileName,FILE_KEY)){
            error = "Could not append log File " + lastGeneratedLogFileName + " for request. Reason: " + rest_controller.getErrors().join("\n");
            return false;
        }
    }

    if (!updated_subject_records.isEmpty()){

        // We need to create the Subject Update File.
        QString filename = institution_id + "_" + instance_number + "_" + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss") + ".json";
        lastSubjectUpdateJSONFile = Globals::Paths::WORK_DIRECTORY + "/" + filename;

        if (!Globals::SaveVariantMapToJSONFile(lastSubjectUpdateJSONFile,updated_subject_records,false)){
            error = "Failed in creating the subject file for updating at: " + lastSubjectUpdateJSONFile;
            return false;
        }

        if (!rest_controller.appendFileForRequest(lastSubjectUpdateJSONFile,SUBJECT_UPDATE_FILE)){
            error = "Could not append log File " + lastSubjectUpdateJSONFile + " for request. Reason: " + rest_controller.getErrors().join("\n");
            return false;
        }

    }
    else {
        lastSubjectUpdateJSONFile = "";
    }

    // Whether we are sending the log or not. If there are failed calibration files we send them.
    QDir calib_dir(Globals::Paths::FAILED_CALIBRATION_DIR);
    if (calib_dir.exists()){

        QStringList filters; filters  << "*.tar.gz";
        QStringList tar_files = calib_dir.entryList(filters,QDir::Files|QDir::NoDotAndDotDot);

        //qDebug() << "TAR FILES for failed calibrations" << tar_files;

        QString prefix = FAILED_CALIBRATION_FILE_PREFIX + "_" + institution_id + "_" + instance_number + "_";

        for (qint32 i = 0; i < tar_files.size(); i++){
            //qDebug() << "Appending file" << tar_files.at(i);
            rest_controller.appendFileForRequest(Globals::Paths::FAILED_CALIBRATION_DIR + "/" + tar_files.at(i),prefix + QString::number(i));
        }

    }


    // If we are using this endpoint ONLY to send the log then we need to check that.
    if (sendLog){
        lastRequest = API_OPERATING_INFO_AND_LOG;
    }
    else {
        lastRequest = API_OPERATING_INFO;
    }

    return sendRequest();
}

bool APIClient::requestSupportEmail(const QString &subject, const QString &email_file){


    error = "";
    lastGeneratedLogFileName = "";

    // Clearing everything but the URL.
    rest_controller.resetRequest();

    // Forming the URL
    rest_controller.setAPIEndpoint(ENDPOINT_SEND_SUPPORT_EMAIL + "/" + subject);

    // Setting the required post data.
    QVariantMap postdata;
    postdata.insert(POST_FIELD_INSTITUTION_ID,institution_id);
    postdata.insert(POST_FIELD_INSTITUTION_INSTANCE,instance_number);
    rest_controller.setPOSTDataToSend(postdata);
    rest_controller.setURLParameters(QVariantMap());

    // Appending the log file for the support request.    
    LogPrep logPrep(Globals::Paths::LOGFILE,institution_id,instance_number);
    QString tempName = logPrep.createFullLogBackup();
    if (tempName == ""){
        error = "Unable to create Log Backup. Reason: " + logPrep.getError();
        return false;
    }

    lastGeneratedLogFileName = tempName;

    if (!rest_controller.appendFileForRequest(lastGeneratedLogFileName,SUPPORT_EMAIL_LOG)){
        error = "Could not append log File " + lastGeneratedLogFileName + " for request. Reason: " + rest_controller.getErrors().join("\n");
        return false;
    }

    lastRequestEmailFile = email_file;
    if (!rest_controller.appendFileForRequest(lastRequestEmailFile,SUPPORT_EMAIL_FILE)){
        error = "Could not append email File " + lastRequestEmailFile + " for request. Reason: " + rest_controller.getErrors().join("\n");
        return false;
    }

    lastRequest = API_SENT_SUPPORT_EMAIL;

    return sendRequest();
}

bool APIClient::requestFunctionalControl(const QVariantMap &hwInfo,
                                         const QString &email,
                                         const QString &password,
                                         const QString &name,
                                         const QString &lastname,
                                         const QString &instance_identifier){

    error = "";

    // Clearing everything as per usal.
    rest_controller.resetRequest();

    // We now make the URL
    rest_controller.setAPIEndpoint(ENDPOINT_FUNC_CTL + "/" + instance_identifier);

    QString intent = "new";
    lastRequest = API_FUNC_CTL_NEW;
    if ((name != "") && (lastname != "")){
        lastRequest = API_FUNC_CTL_HMD_CHANGE;
        intent = "hmd_change";
    }

    QVariantMap json;
    json[JFIELD_HWINFO]   = hwInfo;
    json[JFIELD_EMAIL]    = email;
    json[JFIELD_FNAME]    = name;
    json[JFIELD_LNAME]    = lastname;
    json[JFIELD_PASSWORD] = password;
    json[JFIELD_INTENT]   = intent;

    Debug::prettyPrintQVariantMap(json);

    rest_controller.setJSONData(json);

    return sendRequest(true);

}

bool APIClient::requestActivation(qint32 institution, qint32 instance, const QString &key, const QString &hardware_description_string, const QVariantMap &hwinfo){

    error = "";

    // Clearing everything but the URL.
    rest_controller.resetRequest();

    // Forming the URL
    // qDebug() << "URL for OI" << ENDPOINT_OPERATING_INFO + "/" + institution_id;
    rest_controller.setAPIEndpoint(ENDPOINT_ACTIVATION_ENDPOINT + "/" + QString::number(instance));
    QVariantMap map;
    map.insert(URLPARAM_INSTITUTION,institution);
    map.insert(URLPARAM_KEY,key);

    rest_controller.setURLParameters(map);

    // The actual hardware string is sent via POST data as the json fields are not obtained when doing activation.
    QVariantMap json;
    json[JFIELD_HWINFO] = hwinfo;
    json[JFIELD_HW_DESC_STRING] = hardware_description_string;
    rest_controller.setJSONData(json);

    lastRequest = API_ACTIVATE;

    return sendRequest(true);

}

bool APIClient::requestReportProcessing(const QString &tarFile){

    error = "";

    if (!QFile(tarFile).exists()){
        error = "Could not fint the input file " + tarFile;
        return false;
    }

    // Doing the actual request.
    // Clearing everything but the URL.
    rest_controller.resetRequest();

    // Forming the URL
    rest_controller.setAPIEndpoint(ENDPOINT_REPORT_GENERATION + "/" + institution_id);
    QVariantMap map;
    map.insert(URLPARAM_INSTANCE,instance_number);

    // Setting the required post data.
    QVariantMap postdata;
    postdata.insert(POST_FIELD_INSTITUTION_ID,institution_id);
    postdata.insert(POST_FIELD_INSTITUTION_INSTANCE,instance_number);
    rest_controller.setPOSTDataToSend(postdata);

    rest_controller.setURLParameters(map);

    if (!rest_controller.appendFileForRequest(tarFile,FILE_KEY)){
        error = "Could not append ZIP File " + tarFile + " for request. Reason: " + rest_controller.getErrors().join("\n");
        return false;
    }

    lastRequest = API_REQUEST_REPORT;
    return sendRequest();

}

bool APIClient::requestUpdate(const QString &pathToSaveAFile){
    error = "";

    // Clearing everything but the URL.
    rest_controller.resetRequest();

    // Forming the URL
    rest_controller.setAPIEndpoint(ENDPOINT_GET_UPDATE + "/" + institution_id);
    QVariantMap map;
    map.insert(URLPARAM_INSTANCE,instance_number);

    if ((region == Globals::LOCAL::REGION) || (region == Globals::DEV_SERVER::REGION)){
        map.insert(URLPARAM_REGION,Globals::GLOBAL::REGION);
    }
    else {
        map.insert(URLPARAM_REGION,region);
    }

    // Setting the required post data.
    QVariantMap postdata;
    postdata.insert(POST_FIELD_INSTITUTION_ID,institution_id);
    postdata.insert(POST_FIELD_INSTITUTION_INSTANCE,instance_number);
    rest_controller.setPOSTDataToSend(postdata);

    rest_controller.setURLParameters(map);

    lastRequest = API_REQUEST_UPDATE;

    // Where the file will be saved.
    this->pathToSaveAFile = pathToSaveAFile;

    return sendRequest();
}

QString APIClient::getLastGeneratedLogFileName() const {
    return lastGeneratedLogFileName;
}

QString APIClient::getLastGeneratedSubjectJSONFile() const {
    return lastSubjectUpdateJSONFile;
}

QString APIClient::getLatestGeneratedSupportEmail() const {
    return lastRequestEmailFile;
}

QString APIClient::getError() const{
    return error;
}

void APIClient::clearFileToSendHandles() {
    rest_controller.clearFileToSendHandles();
}

void APIClient::gotReply(){

    //    QByteArray raw_reply = rest_controller.getReplyData();
    //    QMap<QString,QString> rheaders = rest_controller.getResponseHeaders();
    //    QString searchFor = "Content-Disposition";

    retdata.clear();
    QByteArray raw_reply = rest_controller.getReplyData();

    QJsonParseError json_error;
    QJsonDocument doc = QJsonDocument::fromJson(QString(raw_reply).toUtf8(),&json_error);

    if (doc.isNull()){
        error = "Error decoding JSON Data: " + json_error.errorString();
        error = error + "\nRaw Reply Data: " +  QString(raw_reply) + "\n";
    }
    else {
        error = "";
    }
    retdata = doc.object().toVariantMap();

    if (rest_controller.didReplyHaveAnError()){
        error = error + "Request reply had errors:\n   " + rest_controller.getErrors().join("\n   ");
        error = error + "\nRaw Reply Data: " +  QString(raw_reply);
    }
    emit APIClient::requestFinish();

}

bool APIClient::sendRequest(bool nosign){

    // Adding salt to ensure uniqueness of signature.
    QString auth_string = "";
    if (!nosign){
        rest_controller.addSalt();
        auth_string = QMessageAuthenticationCode::hash(rest_controller.getPayload(), secret.toUtf8(), QCryptographicHash::Sha3_512).toHex();
    }

    // Adding headers to the request.
    // qDebug() << HEADER_AUTHENTICATION << HEADER_AUTHTYPE;
    rest_controller.addHeaderToRequest(HEADER_AUTHTYPE,AUTH_TYPE);
    if (!nosign){
        rest_controller.addHeaderToRequest(HEADER_AUTHENTICATION,key);
        rest_controller.addHeaderToRequest(HEADER_SIGNATURE,auth_string);
    }

    // Generate the request.
    if (!rest_controller.sendPOSTRequest()){
        error =  "Error sending POST request\n   " + rest_controller.getErrors().join("\n   ");
        return false;
    }

    return true;
}

