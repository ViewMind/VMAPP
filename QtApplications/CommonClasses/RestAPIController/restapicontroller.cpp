#include "restapicontroller.h"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDebug>
#include <QEventLoop>
#include <QObject>
#include <QVariantMap>
#include <QJsonDocument>
#include <QFile>
#include <QHttpMultiPart>

const char * RESTAPIController::SALT_FIELD = "salt";

RESTAPIController::RESTAPIController(QObject *parent):QObject(parent)
{
    reply = nullptr;
    errorInReply = false;
    sendDataAsJSON = false;
}

void RESTAPIController::setBaseAPI(const QString baseAPI){
    baseAPIURL = baseAPI;
    // Remove trailing slashes.
    while (baseAPIURL.endsWith('/')){
        baseAPIURL = baseAPIURL.remove(baseAPIURL.length()-1,1);
    }
}


void RESTAPIController::setAPIEndpoint(const QString &endpoint){
    this->endpoint = endpoint;

    // Remove trailing slashes.
    while (this->endpoint.endsWith('/')){
        this->endpoint = this->endpoint.remove(this->endpoint.length()-1,1);
    }

    // Also removing any beginning slashes.
    while (this->endpoint.startsWith('/')){
        this->endpoint = this->endpoint.remove(0,1);
    }

    makeEndpointAndParameters();
}

void RESTAPIController::setURLParameters(const QVariantMap &map){
    URLparameters = map;
    makeEndpointAndParameters();
}

bool RESTAPIController::setPOSTDataToSend(const QVariantMap &map){
    if (sendDataAsJSON){
        errors << "Sending data as JSON. Cannot send form POST data in the same request";
        return false;
    }
    dataToSend = map;
    stringifyData();
    return true;
}

void RESTAPIController::setJSONData(const QVariantMap &json){
    filesToSend.clear();
    sendDataAsJSON = true;
    dataToSend = json;
    //stringifyData();
}

void RESTAPIController::setRawStringDataToSend(const QString &rawData){
    dataToSend.clear();
    sendDataAsJSON = true;
    dataToSendAsRawData = rawData;
}

QMap<QString,QString> RESTAPIController::getResponseHeaders() const{
    return responseHeaders;
}

void RESTAPIController::stringifyData(){
    // When hashing, the dataToSend is treated as a JSON string.
    // When sending this over POST to the web sever, the server wlll interpret all values as strings
    // However if the values were set as, numbers, for example the JSON string here will not contain quotation marks around the values and hence
    // The JSON string in of these values in the web server will be different. Hashes will then be different as well.
    // When computing the JSON string of the POST received array there WILL be quotation marks, allways.
    // In order to force quotation marks HERE, we force all values to be strings.
    // That's the point of this loop.
    QStringList keys = dataToSend.keys();
    for (qint32 i = 0; i < keys.size(); i++){
        dataToSend[keys.at(i)] = dataToSend.value(keys.at(i)).toString();
    }
}


void RESTAPIController::setBasicAuth(const QString &username, const QString &password){
    basicAuthPassword = password;
    basicAuthUsername = username;
//    QString concatenated = username + ":" + password;
//    QByteArray data = concatenated.toLocal8Bit().toBase64();
//    QString headerData = "Basic " + data;
//    request.setRawHeader("Authorization", headerData.toLocal8Bit());
}

void RESTAPIController::addHeaderToRequest(const QString &headerName, const QString &headerValue){
    headers[headerName] = headerValue;
}

QStringList RESTAPIController::getErrors() const {
    return errors;
}

void RESTAPIController::resetRequest(){
    errors.clear();
    dataToSend.clear();
    filesToSend.clear();
    replyData.clear();
    headers.clear();
    basicAuthPassword = "";
    basicAuthUsername = "";
    sendDataAsJSON = false;
    URLparameters.clear();
    dataToSendAsRawData = "";
    endPointAndParameters.clear();
    endpoint = "";
}

bool RESTAPIController::appendFileForRequest(const QString &file_path, const QString &file_key){

    if (sendDataAsJSON){
        errors << "Sending data as JSON. Cannot send file in the same request";
        return false;
    }

    if (filesToSend.contains(file_key)){
        errors << "Trying to append a file with key already in use: " + file_key;
        return false;
    }

    if (!QFile(file_path).exists()){
        errors << "Trying to append file " + file_path + " that does not exists";
        return false;
    }

    filesToSend.insert(file_key,file_path);
    return true;
}

bool RESTAPIController::isRequestFinished() const{
    if (reply != nullptr) return reply->isFinished();
    else return false;
}

bool RESTAPIController::sendPOSTRequest() {

    errorInReply = false;
    if (reply != nullptr) delete reply;
    replyData.clear();

    QHttpMultiPart *multipart = nullptr;
    QByteArray jsonData;
    bool added_something = false;

    if (!sendDataAsJSON){

        // We create the request part with whatever data or files need to be sent.
        multipart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

        if (reply != nullptr){
            delete reply;
        }

        if (!dataToSend.isEmpty()){
            QStringList data_keys = dataToSend.keys();
            for (qint32 i = 0; i < data_keys.size(); i++){
                added_something = true;
                QString key = data_keys.at(i);
                QVariant value = dataToSend.value(key);
                QHttpPart postpart;
                postpart.setHeader(QNetworkRequest::ContentDispositionHeader, QString("form-data; name=\"" + key + "\""));
                //postpart.setHeader(QNetworkRequest::ContentDispositionHeader, QString("application/x-www-form-urlencoded"));
                postpart.setBody(value.toByteArray());
                multipart->append(postpart);
            }
        }

        if (!filesToSend.isEmpty()){
            // Adding each file as aprt of the HTTP multi part post.
            QStringList file_keys = filesToSend.keys();
            for (qint32 i = 0; i < file_keys.size(); i++){

                QString file_key = file_keys.at(i);
                QString filename = filesToSend.value(file_key);

                QFile *file = new QFile(filename);
                if(!file->open(QIODevice::ReadOnly)){
                    delete  file;
                    delete multipart;
                    errors << "Could not open file " + filename + " for reading";
                    return false;
                }

                added_something = true;

                QHttpPart postpart;
                postpart.setHeader(QNetworkRequest::ContentDispositionHeader,QString("form-data; name=\"" + file_key + "\"; filename=\"" + filename + "\""));
                postpart.setBodyDevice(file);
                multipart->append(postpart);
                file->setParent(multipart);
            }
        }

    }
    else{        
        // Seding data as JSON or string (-d in CURL command)
        if (dataToSendAsRawData == ""){
            added_something = true;
            QJsonDocument json = QJsonDocument::fromVariant(dataToSend);
            jsonData = json.toJson();
        }
        else{
            added_something = true;
            jsonData = dataToSendAsRawData.toUtf8();
        }
    }

    // Building the url and sending the post request.
    QString address = baseAPIURL;
    if (endPointAndParameters != "") address = address + "/" + endPointAndParameters;
    //qDebug() << "Setting the URL to" << address;
    QUrl url(address);
    QNetworkRequest request(url);
    if (sendDataAsJSON){
       request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    }

    // Adding the specifed headers.
    QStringList headerNames = headers.keys();
    for (qint32 i = 0; i < headerNames.size(); i++){
        QString headerName = headerNames.at(i);
        QString headerValue = headers.value(headerName);
        request.setRawHeader(headerName.toUtf8(),headerValue.toUtf8());
    }

    // A separate case is used if basic auth is required. This WILL override the Authorization field if it was set.
    if (basicAuthUsername != ""){
        QString concatenated = basicAuthUsername + ":" + basicAuthPassword;
        QByteArray data = concatenated.toLocal8Bit().toBase64();
        QString headerData = "Basic " + data;
        //qDebug() << "Setting the Authorization header of Basic Auth to " << headerData;
        request.setRawHeader("Authorization", headerData.toUtf8());
    }


    // Sending something only if there is somthing to send.
    if (!added_something){
        delete multipart;
        reply = manager.post(request, QByteArray());
    }
    else{
        if (sendDataAsJSON){
           //qDebug() << "Doing the post request sending data as JSON valued" << jsonData;
           reply = manager.post(request,jsonData);
        }
        else{
           reply = manager.post(request,multipart);
        }
    }

    if (!connect(reply,&QNetworkReply::finished,this,&RESTAPIController::gotReply)){
        errors << "Could not connect finished with slot gotReply";
        return false;
    }

    return true;
}

bool RESTAPIController::sendGETRequest(){

    if (reply != nullptr) delete reply;

    // Building the url and sending the post request.
    QString address = baseAPIURL;
    if (endPointAndParameters != "") address = address + "/" + endPointAndParameters;
    //qDebug() << "Setting the GET URL to" << address;
    QUrl url(address);
    QNetworkRequest request(url);

    // Adding the specifed headers.
    QStringList headerNames = headers.keys();
    for (qint32 i = 0; i < headerNames.size(); i++){
        QString headerName = headerNames.at(i);
        QString headerValue = headers.value(headerName);
        //qDebug() << headerName << headerValue;
        request.setRawHeader(headerName.toUtf8(),headerValue.toUtf8());
    }

    reply = manager.get(request);

    if (!connect(reply,&QNetworkReply::finished,this,&RESTAPIController::gotReply)){
        errors << "Could not connect finished with slot gotReply";
        return false;
    }

    return true;

}

void RESTAPIController::makeEndpointAndParameters(){
    endPointAndParameters = endpoint;
    if (!URLparameters.isEmpty()){
        endPointAndParameters = endPointAndParameters + "?";
        QStringList equals;
        QStringList keys = URLparameters.keys();
        for (qint32 i = 0; i < keys.size(); i++){
            QString value = URLparameters.value(keys.at(i)).toString();
            value = value.replace(" ","+"); // Spaces are replaced with + characters.
            equals << keys.at(i) + "=" + value;
        }
        endPointAndParameters = endPointAndParameters + equals.join("&");
    }
}

QByteArray RESTAPIController::getPayload() const{
    QByteArray payload;
    payload = endPointAndParameters.toUtf8();

    if (sendDataAsJSON){
        // Only the data to send is used.
        QJsonDocument json = QJsonDocument::fromVariant(dataToSend);
        QString json_string = QString(json.toJson());
        payload = json_string.toUtf8() + payload;
    }
    else{
        // In this case data will part of the $_POST structure so it can't be added straight up.
        QJsonDocument json = QJsonDocument::fromVariant(dataToSend);
        QString json_string = QString(json.toJson(QJsonDocument::Compact));
        //std::cout << json_string.toStdString() << std::endl;
        payload = payload + json_string.toUtf8();

        // We also the the files to send as part of the payload.
        if (!filesToSend.isEmpty()){
            // Adding each file as aprt of the HTTP multi part post.
            QStringList file_keys = filesToSend.keys();
            for (qint32 i = 0; i < file_keys.size(); i++){

                QString file_key = file_keys.at(i);
                QString filename = filesToSend.value(file_key);

                QFile file(filename);
                if(!file.open(QIODevice::ReadOnly)){
                    continue;
                }

                payload = payload + file.readAll();
                file.close();
            }
        }
    }

    // std::cout << payload.toStdString() << std::endl;

    // Concatenating the end point and parameters.
    // qDebug() << endPointAndParameters;
    // payload = payload + endPointAndParameters.toUtf8();

    return payload;
}

void RESTAPIController::gotReply(){
    replyData = reply->readAll();

    qDebug() << "GOT A REPLY REST API CONTROLLER";

    // Saving Headers
    QList<QNetworkReply::RawHeaderPair> temp = reply->rawHeaderPairs();
    responseHeaders.clear();
    for (qint32 i = 0; i < temp.size(); i++){
        QNetworkReply::RawHeaderPair pair = temp.at(i);
        QString key = QString::fromUtf8(pair.first);
        QString value = QString::fromUtf8(pair.second);
        responseHeaders[key] = value;
    }

    if (reply->error() != QNetworkReply::NoError){
        errorInReply = true;
        errors << reply->errorString();
    }
    emit(gotReplyData());
}


void RESTAPIController::addSalt(){
    dataToSend[SALT_FIELD] = QDateTime::currentDateTime().toString("dd_MM_yyyy_hh_mm_ss_zzz");
}

bool RESTAPIController::didReplyHaveAnError() const{
    return errorInReply;
}

QByteArray RESTAPIController::getReplyData() const{
    return replyData;
}
