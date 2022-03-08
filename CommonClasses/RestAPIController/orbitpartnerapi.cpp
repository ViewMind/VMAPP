#include "orbitpartnerapi.h"

const QString OrbitPartnerAPI::KEY = "key";
const QString OrbitPartnerAPI::SECRET = "secret";
const QString OrbitPartnerAPI::AUTH_URL = "auth_url";
const QString OrbitPartnerAPI::API_URL = "api_url";

const QString OrbitPartnerAPI::ENDPOINT_PHYSICIAN = "/api/v1/physician";
const QString OrbitPartnerAPI::ENDPOINT_PATIENT   = "/api/v1/physician/{id}/patient";
const QString OrbitPartnerAPI::AUTH_DATA_STRING   = "grant_type=client_credentials";
const QString OrbitPartnerAPI::ACCESS_TOKEN       = "access_token";

OrbitPartnerAPI::OrbitPartnerAPI():PartnerAPI(nullptr)
{

}

QString OrbitPartnerAPI::getPartnerType() const{
    return "Orbit";
}

bool OrbitPartnerAPI::addMedicsAsAppUsers() const{
    return true;
}

bool OrbitPartnerAPI::addMedicsAsNonLoginUsers() const{
    return true;
}

bool OrbitPartnerAPI::requestInformation(const QVariantMap &conf){

    QStringList requirements;
    requirements << OrbitPartnerAPI::KEY << OrbitPartnerAPI::SECRET << OrbitPartnerAPI::API_URL << OrbitPartnerAPI::AUTH_URL;
    for (qint32 i = 0; i < requirements.size(); i++){
        if (!conf.contains(requirements.at(i))){
            error = requirements.at(i) + " field is required for the Orbit API configuration";
            return false;
        }
    }

    secret = conf.value(OrbitPartnerAPI::SECRET).toString();
    key    = conf.value(OrbitPartnerAPI::KEY).toString();
    api_url = conf.value(OrbitPartnerAPI::API_URL).toString();
    auth_url = conf.value(OrbitPartnerAPI::AUTH_URL).toString();
    institution_prefix = conf.value(PartnerAPIConf::INSTITUTION_ID).toString() + "_";

    api.setBaseAPI(auth_url);
    api.resetRequest();

    getState = GS_TOKEN;
    api.setRawStringDataToSend(AUTH_DATA_STRING);
    api.setBasicAuth(key,secret);
    if (!api.sendPOSTRequest()){
        error = "Could not send POST request: \n-" + api.getErrors().join("\n-");
        return false;
    }

    return true;
}

void OrbitPartnerAPI::onReplyReceived(){    
    QJsonParseError json_error;
    QByteArray raw_reply = api.getReplyData();
    QJsonDocument doc = QJsonDocument::fromJson(raw_reply,&json_error);
    if (json_error.error != QJsonParseError::NoError){
        error = "Could not parse JSON Ouput. Reason: " + json_error.errorString();
        error = error + "Reply Size: " + QString::number(raw_reply.size()) + "\n";
        error = error + QString(raw_reply);
        error = error + "REST API Errors:\n-" + api.getErrors().join("\n-");
        getState = GS_FAILED;
        emit(finished());
    }
    else{

        QVariantMap map = doc.object().toVariantMap();

        if (getState == GS_TOKEN){
            //std::cout << QString(doc.toJson(QJsonDocument::Indented)).toStdString() << std::endl;
            if (map.contains(ACCESS_TOKEN)){
                token = map.value(ACCESS_TOKEN).toString();
                getState = GS_PHYSICIAN;
                api.setBaseAPI(api_url);
                api.resetRequest();
                api.addHeaderToRequest("Authorization", "Bearer " + token);
                api.setAPIEndpoint(ENDPOINT_PHYSICIAN);
                api.sendGETRequest();
            }
            else{
                error = "JSON object does not contain expected field of " + ACCESS_TOKEN;
                error = error + "\n" + QString(doc.toJson(QJsonDocument::Indented));
                getState = GS_FAILED;
                emit(finished());
                return;
            }
        }
        else if (getState == GS_PHYSICIAN){
            //getState = GS_PHYSICIAN;
            //std::cout << "All physicans" << std::endl;
            //std::cout << QString(doc.toJson(QJsonDocument::Indented)).toStdString() << std::endl;

            // Doctor list is inside the payload and rows

            QStringList hiearchy; hiearchy << OrbitReturn::PhysicianList::PAYLOAD << OrbitReturn::PhysicianList::Payload::ROWS;
            QString missing;
            //qDebug() << map;
            QVariant ret = MapHiearchyCheck(map,hiearchy,&missing);

            if (missing != ""){
                error = "JSON object: " + missing;
                error = error + "\n" + QString(doc.toJson(QJsonDocument::Indented));
                getState = GS_FAILED;
                emit(finished());
                return;
            }

            //qDebug() << ret;
            //qDebug() << ret.toList();

            QVariantList physician_list = ret.toList();
            medics.clear();
            patients.clear();
            QSet<QString> emails;
            //QSet<qint32> skip; skip << 26 << 24 << 23;
            for (qint32 i = 0; i < physician_list.size(); i++){

                QVariantMap physician = physician_list.at(i).toMap();
                QVariantMap medic;

                // We must make sure that the emails are unique. For orbit the uniquenes does not come from the email but the UID itself.
                // So the UID is used as an "email" of sorts by preappending the institution number to ensure uniqueness in our DBs.
                QString email = physician.value(OrbitReturn::PhysicianList::Payload::Rows::Physician::UID).toString();
                if (emails.contains(email)){
                    error = "Physician list contains repeated unique identifier: " + email;
                    getState = GS_FAILED;
                    emit(finished());
                    return ;
                }
                //emails << email;
                //qint32 id = physician.value(OrbitReturn::PhysicianList::Payload::Rows::Physician::ID).toInt();
                //if (skip.contains(id)) continue;

                medic[PartnerMedic::EMAIL]       = institution_prefix + physician.value(OrbitReturn::PhysicianList::Payload::Rows::Physician::UID).toString();
                medic[PartnerMedic::NAME]        = physician.value(OrbitReturn::PhysicianList::Payload::Rows::Physician::NAME);
                medic[PartnerMedic::LASTNAME]    = physician.value(OrbitReturn::PhysicianList::Payload::Rows::Physician::LASTNAME);
                medic[PartnerMedic::PARTNER_ID]  = physician.value(OrbitReturn::PhysicianList::Payload::Rows::Physician::ID);
                medics << medic;
            }

            // We now start iterating over all medics to get the patients.
            if (medics.size() > 0){
                physician_index = 1;
                api.resetRequest();
                QString endpoint = ENDPOINT_PATIENT;
                endpoint.replace("{id}",medics.first().toMap().value(PartnerMedic::PARTNER_ID).toString());
                api.addHeaderToRequest("Authorization", "Bearer " + token);
                //std::cout << "Setting endpoint: " << endpoint.toStdString() << std::endl;
                api.setAPIEndpoint(endpoint);
                api.sendGETRequest();
                getState = GS_PATIENT;
            }
            else{
                getState = GS_DONE;
                emit(finished());
                return;
            }


            //emit(finished());
        }
        else if (getState == GS_PATIENT){
            qint32 current_physican =  physician_index-1;
            QString email = medics.at(current_physican).toMap().value(PartnerMedic::EMAIL).toString();
            //std::cout << "Patient for physician" << current_physican << std::endl;
            //std::cout << QString(doc.toJson(QJsonDocument::Indented)).toStdString() << std::endl;

            QStringList hiearchy; hiearchy << OrbitReturn::PhysicianList::PAYLOAD << OrbitReturn::PhysicianList::Payload::ROWS;
            QString missing;
            //qDebug() << map;
            QVariant ret = MapHiearchyCheck(map,hiearchy,&missing);

            if (missing != ""){
                error = "JSON object: " + missing;
                error = error + "\n" + QString(doc.toJson(QJsonDocument::Indented));
                getState = GS_FAILED;
                emit(finished());
                return;
            }

            QVariantList patient_list = ret.toList();

            //qDebug() << "Adding " << patient_list.size();

            for (qint32 i = 0; i < patient_list.size(); i++){
                QVariantMap patient = patient_list.at(i).toMap();
                QVariantMap subject;
                subject[ParterPatient::NAME]             = patient.value(OrbitReturn::PhysicianList::Payload::Rows::Patient::NAME);
                subject[ParterPatient::LASTNAME]         = patient.value(OrbitReturn::PhysicianList::Payload::Rows::Patient::LASTNAME);
                subject[ParterPatient::PARTNER_MEDIC_ID] = email;
                subject[ParterPatient::PARTNER_UID]      = patient.value(OrbitReturn::PhysicianList::Payload::Rows::Patient::ID);
                patients << subject;
            }

            if (physician_index < medics.size()){
                api.resetRequest();
                QString endpoint = ENDPOINT_PATIENT;
                endpoint.replace("{id}",medics.at(physician_index).toMap().value(PartnerMedic::PARTNER_ID).toString());
                physician_index++;
                api.addHeaderToRequest("Authorization", "Bearer " + token);
                //std::cout << "Setting endpoint: " << endpoint.toStdString() << std::endl;
                api.setAPIEndpoint(endpoint);
                api.sendGETRequest();
            }
            else{
                getState = GS_DONE;
                emit(finished());
                return;
            }

        }

    }

}

