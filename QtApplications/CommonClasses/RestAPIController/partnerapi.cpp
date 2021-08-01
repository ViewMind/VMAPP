#include "partnerapi.h"

PartnerAPI::PartnerAPI(QObject *parent) : QObject(parent)
{
    connect(&api,&RESTAPIController::gotReplyData,this,&PartnerAPI::receivedReplayData);
}


QString PartnerAPI::getError() const {
    return error;
}

void PartnerAPI::receivedReplayData(){
    onReplyReceived();
}

bool PartnerAPI::requestInformation(const QVariantMap &configuration){
    Q_UNUSED(configuration)
    return false;
}

bool PartnerAPI::addMedicsAsAppUsers() const{
    return false;
}

bool PartnerAPI::addMedicsAsNonLoginUsers() const{
    return false;
}

QVariantList PartnerAPI::getMedicInformation() const{
    return medics;
}

QVariantList PartnerAPI::getRegisteredPatientInformation() const {
    return patients;
}

QString PartnerAPI::getPartnerType() const {
    return "";
}

void PartnerAPI::onReplyReceived(){

}


QVariant PartnerAPI::MapHiearchyCheck(const QVariantMap &map, const QStringList &hiearchy, QString *message){

    QVariantMap temp = map;
    *message = "";

    for (qint32 i = 0; i < hiearchy.size(); i++){
        if (temp.contains(hiearchy.at(i))){
            if (i < hiearchy.size()-1) temp = temp.value(hiearchy.at(i)).toMap();
            else return temp.value(hiearchy.at(i));
        }
        else{
            *message = "Hiearchy check of " + hiearchy.join("->") + " failed because " + hiearchy.at(i) + " is missing";
            return QVariant();
        }
    }

    return temp;

}
