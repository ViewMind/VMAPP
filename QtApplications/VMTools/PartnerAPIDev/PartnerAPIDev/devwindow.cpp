#include "devwindow.h"
#include "ui_devwindow.h"

DevWindow::DevWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DevWindow)
{
    ui->setupUi(this);
    connect(&orbit,&PartnerAPI::finished,this,&DevWindow::onFinished);
    connect(&apiclient,&RESTAPIController::gotReplyData,this,&DevWindow::onGotReply);
}

DevWindow::~DevWindow()
{
    delete ui;
}


void DevWindow::on_pbTest_clicked()
{
    QVariantMap conf;
    conf[OrbitPartnerAPI::KEY] = "5teue50i6qf5mnk3iv81aqv1ih";
    conf[OrbitPartnerAPI::SECRET] = "1te3a79cvi0oqc3ar1j1f84ahfp3iodrjnle25nbl3q030mgal5c";
    conf[OrbitPartnerAPI::AUTH_URL] = "https://orbit-lab-dev.auth.ap-southeast-1.amazoncognito.com/oauth2/token";
    conf[OrbitPartnerAPI::API_URL] = "https://lab-api.dev.orbit.health";

    orbit.requestInformation(conf);

}

void DevWindow::onFinished(){
    QString error = orbit.getError();

    if (error != ""){
        std::cout << "ERROR: " << std::endl;
        std::cout << error.toStdString() << std::endl;
        return;
    }

    //qDebug() << "Doctors";
    //qDebug() << orbit.getMedicInformation();
    //qDebug() << "Patients";
    //qDebug() << orbit.getRegisteredPatientInformation();

    // Sending the Doctor data to the backend.

    QVariantList doctors = orbit.getMedicInformation();

    // Setting up the API Client.
    QString key                     = "2a119e8e0f1e95a02b3b3520a9bca8f1e7f1621c070fce57a9b6f3bcd1f13ebdba10fd9720beb7862fd4134b18d97bb0e95d618e98660b01b3d75df264c6e371";
    QString secret                  = "1522352f8e0326d4fb301e5ff9336ec382a064e16694dbdaf1b62fdb14981bd7e6f7b6cbb0ef7ca4e5abde496908d0f1e238a679b68a5ccd82d6046e09c9ae12c6633b055921128cca6c88ec7169cff60916fb4badbd89a32cf85945c2be919ed89f8453eea3e9c82f1cdbefc1e22b35f2d34af0d5ff9ccc63af2b498dc88a43";
    QString APIURL                  = "http://localhost/vmapi";
    QString endpoint                = "/portal_users/addnologpusers/1";

    apiclient.setBaseAPI(APIURL);
    apiclient.setAPIEndpoint(endpoint);

    QVariantMap data;
    data.insert("institution_id","1");
    data.insert("institution_instance","0");
    data.insert("data",doctors);

    apiclient.setJSONData(data);

    // Adding salt to ensure uniqueness of signature.
    apiclient.addSalt();

    //std::cout << "Sending Payload: " << std::endl;
    //std::cout << QString(apiclient.getPayload()).toStdString() << std::endl;

    QString auth_string = QMessageAuthenticationCode::hash(apiclient.getPayload(), secret.toUtf8(), QCryptographicHash::Sha3_512).toHex();

    // Adding headers to the request.
    apiclient.addHeaderToRequest("AuthType","VMClient");
    apiclient.addHeaderToRequest("Authentication",key);
    apiclient.addHeaderToRequest("Signature",auth_string);

    // Generate the request.
    if (!apiclient.sendPOSTRequest()){
        qDebug() << "Error sending POST request: " << apiclient.getErrors();
    }

    std::cout << "Sending Data to the Back End" << std::endl;


}


void DevWindow::onGotReply(){
    if (apiclient.didReplyHaveAnError()){
        qDebug() << "Error in the request";
        qDebug() << apiclient.getErrors();
        QByteArray raw_reply = apiclient.getReplyData();
        QJsonParseError json_error;
        QJsonDocument doc = QJsonDocument::fromJson(raw_reply,&json_error);
        if (json_error.error != QJsonParseError::NoError){
            std::cout << QString(raw_reply).toStdString() << std::endl;
        }
        else{
           std::cout << QString(doc.toJson(QJsonDocument::Indented)).toStdString() << std::endl;
        }
    }
    else{
        QByteArray raw_reply = apiclient.getReplyData();
        qDebug() << "Size of reply: " << raw_reply.size();
        QMap<QString,QString> rheaders = apiclient.getResponseHeaders();
        QString searchFor = "Content-Disposition";

        if (rheaders.contains(searchFor)){
            // Getting the file name.
            QString filename = "";
            QString content_disposition = rheaders.value(searchFor);
            QStringList parts = content_disposition.split(";");
            for (qint32 i = 0; i < parts.size(); i++){
                if (parts.at(i).contains("filename")){
                    QStringList key_value = parts.at(i).split("=");
                    filename = key_value.last();
                    break;
                }
            }

            if (filename.isEmpty()){
                qDebug() << "Could not determine filename";
                return;
            }

            QFile receivedFile(filename);
            if (!receivedFile.open(QFile::WriteOnly)){
                qDebug() << "Could not open " + receivedFile.fileName() + " for writing";
                return;
            }
            QDataStream fileWriter(&receivedFile);
            qDebug() << "Raw Reply Size" << raw_reply.size();
            //fileWriter << raw_reply;
            fileWriter.writeRawData(raw_reply.constData(), raw_reply.size());
            receivedFile.close();
            std::cout << "Created " << receivedFile.fileName().toStdString() << std::endl;
            qDebug() << "Receive file size: " << receivedFile.size();

        }
        else{
            //std::cout << QString(raw_reply).toStdString() << std::endl;
            QJsonParseError json_error;
            QJsonDocument doc = QJsonDocument::fromJson(raw_reply,&json_error);
            if (json_error.error != QJsonParseError::NoError){
                std::cout << "Could not parse JSON Ouput. Reason: " << json_error.errorString().toStdString() << std::endl;
                std::cout << raw_reply.toStdString() << std::endl;
            }
            else{
                std::cout << QString(doc.toJson(QJsonDocument::Indented)).toStdString() << std::endl;
            }
        }
    }
}
