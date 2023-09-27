#include "apiclient.h"

APIClient::APIClient(QObject *parent) : QObject(parent)
{
    API = Globals::API_URL;
    rest_controller.setBaseAPI(API);
    lastGeneratedLogFileName = "";
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

bool APIClient::requestOperatingInfo(const QString &hardware_description_string, bool sendLog, bool logOnly){

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

    // Setting the required post data.
    QVariantMap postdata;
    postdata.insert(POST_FIELD_INSTITUTION_ID,institution_id);
    postdata.insert(POST_FIELD_INSTITUTION_INSTANCE,instance_number);
    postdata.insert(POST_FIELD_HW_STRING, hardware_description_string);
    rest_controller.setPOSTDataToSend(postdata);

    rest_controller.setURLParameters(map);

    // In order to append the log file we need to make a copy of it and change it's name.
    // Othewise when storing in the server the files would be overwritten.
    if (sendLog){
        lastGeneratedLogFileName = institution_id + "_" + instance_number + "_" + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss") + ".log";
        if (!QFile::rename(Globals::Paths::LOGFILE,lastGeneratedLogFileName)){
            error = "Unable to remane the logfile to a new temporary name of " + lastGeneratedLogFileName;
            return false;
        }

        if (!rest_controller.appendFileForRequest(lastGeneratedLogFileName,FILE_KEY)){
            error = "Could not append log File " + lastGeneratedLogFileName + " for request. Reason: " + rest_controller.getErrors().join("\n");
            return false;
        }
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
    if (sendLog && logOnly){
        lastRequest = API_OP_INFO_LOG_ONLY;
    }
    else if (sendLog){
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
    lastGeneratedLogFileName = institution_id + "_" + instance_number + "_" + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss") + ".log";
    if (!QFile::rename(Globals::Paths::LOGFILE,lastGeneratedLogFileName)){
        error = "Unable to remane the logfile to a new temporary name of " + lastGeneratedLogFileName;
        return false;
    }

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

bool APIClient::requestActivation(qint32 institution, qint32 instance, const QString &key, const QString &hardware_description_string){

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
    QVariantMap postdata;
    postdata.insert(POST_FIELD_HW_STRING, hardware_description_string);
    rest_controller.setPOSTDataToSend(postdata);

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
    if (rest_controller.didReplyHaveAnError()){
        error = "Request reply had errors:\n   " + rest_controller.getErrors().join("\n   ");
        error = error + "\nRaw Reply Data: " +  QString(raw_reply);
    }
    else {

        QJsonParseError json_error;
        QJsonDocument doc = QJsonDocument::fromJson(QString(raw_reply).toUtf8(),&json_error);
        if (doc.isNull()){
            error = "Error decoding JSON Data: " + json_error.errorString();
            error = error + "\nRaw Reply Data: " +  QString(raw_reply);
        }
        else{
            retdata = doc.object().toVariantMap();
        }


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

