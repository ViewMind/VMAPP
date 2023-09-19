#include "supportcontact.h"

SupportContact::SupportContact() {
    connect(&comm,&RESTAPIController::gotReplyData,this,&SupportContact::gotReply);


    this->mailTemplate = "\
            <!DOCTYPE html> \
            <html> \
            <head> \
               <style> \
               table, th, td { \
                 border: 1px solid black; \
                 border-collapse: collapse;\
               }\
               th, td {\
                 padding: 5px;\
               }\
               th {\
                 text-align: left;\
               }\
               </style>\
            </head>\
            <body>\
               <h2>VM Maintenance Tool Support Request </h2>\
               <h3>Comments</h3>\
               <p>||ISSUE||</p>\
            </body>\
            </html>";

}

QString SupportContact::getError() const {
    return this->error;
}

void SupportContact::configure(const QString instID, const QString &number, const QString &key, const QString &hash_code){

    QString api = DebugOptions::DebugString(Globals::DebugOptions::OVERIDE_API);
    qDebug() << "API is" << api;
    if (api == ""){
        api = "https://api.viewmind.ai";
    }
    comm.setBaseAPI(api);

    this->institution = instID;
    this->instance = number;
    this->key = key;
    this->hash = hash_code;
}

void SupportContact::setMessageAndLogFileName(const QString &message, const QString &filename){
    comments = message;
    logFileName = filename;
}


void SupportContact::send(){

    this->error = "";

    // Clearing everything but the URL.
    comm.resetRequest();

    // Forming the URL - The endpoint and subject is fixed.
    comm.setAPIEndpoint("/tickets/send_support_email/VMMaintenanceTool");

    // Setting the required post data.
    QVariantMap postdata;
    postdata.insert("institution_id",institution);
    postdata.insert("institution_instance",instance);
    comm.setPOSTDataToSend(postdata);
    comm.setURLParameters(QVariantMap());

    if (!comm.appendFileForRequest(logFileName,"SupportEmailLog")){
        error = "Could not append log File " + logFileName + " for request. Reason: " + comm.getErrors().join("\n");
        return;
    }

    // We now create the email file.
    QString content = this->mailTemplate;
    content = content.replace("||ISSUE||",comments);

    QString mailFileName = "mail.html";

    QFile mailfile(mailFileName);
    if (!mailfile.open(QFile::WriteOnly)){
        error = "Unable to open the mail.html file for writing";
        return;
    }
    QTextStream writer(&mailfile);
    writer << content;
    mailfile.close();

    if (!comm.appendFileForRequest(mailFileName,"SupportEmailFile")){
        error = "Could not append email File " + mailFileName + " for request. Reason: " + comm.getErrors().join("\n");
        return;
    }

    if (!sendRequest()){
        emit SupportContact::finished();
    }

}

void SupportContact::gotReply(){

    QByteArray raw_reply = comm.getReplyData();
    if (comm.didReplyHaveAnError()){
        error = "Request reply had errors:\n   " + comm.getErrors().join("\n   ");
        error = error + "\nRaw Reply Data: " +  QString(raw_reply);
    }
    else {

        QJsonParseError json_error;
        QJsonDocument doc = QJsonDocument::fromJson(QString(raw_reply).toUtf8(),&json_error);
        if (doc.isNull()){
            error = "Error decoding JSON Data: " + json_error.errorString();
            error = error + "\nRaw Reply Data: " +  QString(raw_reply);
        }
    }

    // Clearing the handles.
    comm.clearFileToSendHandles();
    emit SupportContact::finished();
}


bool SupportContact::sendRequest(){

    // Adding salt to ensure uniqueness of signature.
    QString auth_string = "";

    comm.addSalt();
    auth_string = QMessageAuthenticationCode::hash(comm.getPayload(), hash.toUtf8(), QCryptographicHash::Sha3_512).toHex();


    // Adding headers to the request.
    // qDebug() << HEADER_AUTHENTICATION << HEADER_AUTHTYPE;
    comm.addHeaderToRequest("AuthType","VMClient");

    comm.addHeaderToRequest("Authentication",key);
    comm.addHeaderToRequest("Signature",auth_string);


    // Generate the request.
    if (!comm.sendPOSTRequest()){
        error =  "Error sending POST request\n   " + comm.getErrors().join("\n   ");
        return false;
    }

    return true;

}
