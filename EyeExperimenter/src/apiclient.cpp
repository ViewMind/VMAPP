#include "apiclient.h"

const QString APIClient::TAR_EXE = "tar.exe";

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

void APIClient::configure(const QString &institution_id, const QString &instance_number, const QString &version, const QString &region, const QString &key, const QString &hash_code){
    this->institution_id = institution_id;
    this->instance_number = instance_number;
    this->key = key;
    this->version = version;
    this->secret = hash_code;
    this->region = region;
}

bool APIClient::requestOperatingInfo(const QString &hardware_description_string){

    error = "";

    // Clearing everything but the URL.
    rest_controller.resetRequest();

    // Forming the URL
    // qDebug() << "URL for OI" << ENDPOINT_OPERATING_INFO + "/" + institution_id;
    rest_controller.setAPIEndpoint(ENDPOINT_OPERATING_INFO + "/" + institution_id);
    QVariantMap map;
    map.insert(URLPARAM_PPKEY,Globals::EyeTracker::PROCESSING_PARAMETER_KEY);
    map.insert(URLPARAM_VERSION,this->version);
    map.insert(URLPARAM_INSTANCE,instance_number);    

    // Setting the required post data.
    QVariantMap postdata;
    postdata.insert(POST_FIELD_INSTITUTION_ID,institution_id);
    postdata.insert(POST_FIELD_INSTITUTION_INSTANCE,instance_number);
    postdata.insert(POST_FIELD_HW_STRING, hardware_description_string);
    rest_controller.setPOSTDataToSend(postdata);

    rest_controller.setURLParameters(map);

    lastRequest = API_OPERATING_INFO;

    return sendRequest();
}

bool APIClient::requestReportProcessing(const QString &jsonFile){

    error = "";

    // Before sending the file must be compressed. We use tar.exe.
    // First we need to get the working directory.
    QFileInfo info(jsonFile);

    QString directory = info.absolutePath();
    QString basename = info.baseName();

    QString localJSON = basename + ".json";
    QString localIDX  = basename + ".idx";
    QString zipfile   = basename + ".zip";

    QStringList arguments;
    arguments << "-c";
    arguments << "-z";
    arguments << "-f";
    arguments << zipfile;
    arguments << localJSON;
    arguments << localIDX;

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

bool APIClient::requestUpdate(const QString &pathToSaveAFile){
    error = "";

    // Clearing everything but the URL.
    rest_controller.resetRequest();

    // Forming the URL
    rest_controller.setAPIEndpoint(ENDPOINT_GET_UPDATE + "/" + institution_id);
    QVariantMap map;
    map.insert(URLPARAM_INSTANCE,instance_number);

    if ((region == Globals::LOCAL::REGION) || (region == Globals::DEV_SERVER::REGION)){
        map.insert(URLPARAM_REGION,Globals::EU_REGION::REGION);
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

bool APIClient::requestAdditionOfNonLoginPortalUsers(const QVariantList &pusers){
    error = "";
    rest_controller.resetRequest();
    rest_controller.setAPIEndpoint(ENDPOINT_ADDNOLOGIN_MEDIC + "/" + institution_id);

    QVariantMap data;
    data.insert("institution_id",institution_id);
    data.insert("institution_instance",instance_number);
    data.insert("data",pusers);

    rest_controller.setJSONData(data);

    lastRequest = API_SYNC_PARTNER_MEDIC;

    return sendRequest();
}

QString APIClient::getError() const{
    return error;
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

////// DEPRACATED as the Update Endpoint Now returns a download URL.
//        if (lastRequest != API_REQUEST_UPDATE){
//            QJsonParseError json_error;
//            QJsonDocument doc = QJsonDocument::fromJson(QString(raw_reply).toUtf8(),&json_error);
//            if (doc.isNull()){
//                error = "Error decoding JSON Data: " + json_error.errorString();
//                error = error + "\nRaw Reply Data: " +  QString(raw_reply);
//            }
//            else{
//                retdata = doc.object().toVariantMap();
//            }
//        }
//        else{
//            // The raw reply is a file to be saved.
//            QMap<QString,QString> rheaders = rest_controller.getResponseHeaders();
//            QString searchFor = "Content-Disposition";
//            searchFor = searchFor.toLower();
//            if (rheaders.contains(searchFor)){
//                // Getting the file name.
//                QString filename = "";
//                QString content_disposition = rheaders.value(searchFor);
//                QStringList parts = content_disposition.split(";");
//                for (qint32 i = 0; i < parts.size(); i++){
//                    if (parts.at(i).contains("filename")){
//                        QStringList key_value = parts.at(i).split("=");
//                        filename = key_value.last();
//                        break;
//                    }
//                }

//                if (filename.isEmpty()){
//                    error = searchFor + " header did not contain a filename as expected";
//                    return;
//                }

//                filename = this->pathToSaveAFile + "/" + filename;

//                QFile receivedFile(filename);
//                if (!receivedFile.open(QFile::WriteOnly)){
//                    error = "Could not open " + receivedFile.fileName() + " for writing";
//                    return;
//                }

//                QDataStream fileWriter(&receivedFile);
//                //qDebug() << "Raw Reply Size" << raw_reply.size();
//                fileWriter.writeRawData(raw_reply.constData(), static_cast<qint32>(raw_reply.size()));
//                receivedFile.close();
//                //qDebug() << "Receive file size: " << receivedFile.size();
//            }
//            else{
//                QStringList headerNames = rheaders.keys();
//                error = "Expected header " + searchFor + ", but such header was not found. Response headers are: " + headerNames.join(",");
//            }

//        }
    }
    emit APIClient::requestFinish();
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

