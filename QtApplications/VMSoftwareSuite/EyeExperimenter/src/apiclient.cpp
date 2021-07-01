#include "apiclient.h"

APIClient::APIClient(QObject *parent) : QObject(parent)
{
    API = Globals::API_URL;
    rest_controller.setBaseAPI(API);
    connect(&rest_controller,&RESTAPIController::gotReplyData,this,&APIClient::gotReply);

}

QVariantMap APIClient::getMapDataReturned() const {
    return retdata;
}

qint32 APIClient::getLastRequestType() const {
    return lastRequest;
}

void APIClient::configure(const QString &institution_id, const QString &instance_number, const QString &key, const QString &hash_code){
    this->institution_id = institution_id;
    this->instance_number = instance_number;
    this->key = key;
    this->secret = hash_code;
}

bool APIClient::requestOperatingInfo(){
    // Clearing everything but the URL.
    rest_controller.resetRequest();

    // Forming the URL
    rest_controller.setAPIEndpoint(ENDPOINT_OPERATING_INFO + "/" + institution_id);
    QVariantMap map;
    map.insert(URLPARAM_PPKEY,Globals::EyeTracker::PROCESSING_PARAMETER_KEY);

    // Setting the required post data.
    QVariantMap postdata;
    postdata.insert(POST_FIELD_INSTITUTION_ID,institution_id);
    postdata.insert(POST_FIELD_INSTITUTION_INSTANCE,instance_number);
    rest_controller.setPOSTDataToSend(postdata);

    rest_controller.setURLParameters(map);

    lastRequest = API_OPERATING_INFO;

    return sendRequest();
}

bool APIClient::requestReportProcessing(const QString &jsonFile){

    // Before sending the file must be compressed. We use tar.exe.   
    // First we need to get the working directory.
    QFileInfo info(jsonFile);

    QString directory = info.absolutePath();
    QString basename = info.baseName();

    QString localJSON = basename + ".json";
    QString zipfile =  basename + ".zip";

    QStringList arguments;
    arguments << "-c";
    arguments << "-z";
    arguments << "-f";
    arguments << zipfile;
    arguments << localJSON;

    //qDebug() << "jsonFile" << jsonFile << "zip" << zipfile;

    QProcess tar;
    tar.setWorkingDirectory(directory);
    tar.start(TAR_EXE,arguments);
    tar.waitForFinished();

    qint32 exit_code = tar.exitCode();

    zipfile = directory + "/" + zipfile;

    if (!QFile(zipfile).exists()){
        error = "Could not zip the input file " + jsonFile + ". Exit code for TAR.exe is: " + QString::number(exit_code);
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

    if (!rest_controller.appendFileForRequest(zipfile,FILE_KEY)){
        error = "Could not append ZIP File " + zipfile + " for request. Reason: " + rest_controller.getErrors().join("\n");
        return false;
    }

    lastRequest = API_REQUEST_REPORT;
    return sendRequest();

}

QString APIClient::getError() const{
    return error;
}


void APIClient::gotReply(){
    retdata.clear();;
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
    emit(requestFinish());
}

bool APIClient::sendRequest(){

    // Adding salt to ensure uniqueness of signature.
    rest_controller.addSalt();
    QString auth_string = QMessageAuthenticationCode::hash(rest_controller.getPayload(), secret.toUtf8(), QCryptographicHash::Sha3_512).toHex();

    // Adding headers to the request.
    // qDebug() << HEADER_AUTHENTICATION << HEADER_AUTHTYPE;
    rest_controller.addHeaderToRequest(HEADER_AUTHTYPE,AUTH_TYPE);
    rest_controller.addHeaderToRequest(HEADER_AUTHENTICATION,key);
    rest_controller.addHeaderToRequest(HEADER_SIGNATURE,auth_string);

    // Generate the request.
    if (!rest_controller.sendPOSTRequest()){
        error =  "Error sending POST request\n   " + rest_controller.getErrors().join("\n   ");
        return false;
    }

    return true;
}

