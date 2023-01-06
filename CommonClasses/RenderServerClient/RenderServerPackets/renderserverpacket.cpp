#include "renderserverpacket.h"

RenderServerPacket::RenderServerPacket() {
    this->error = "";
    this->type = "";
    this->isSkippable = false;
}

RenderServerPacket::RXState RenderServerPacket::rxBytes(const QByteArray &bytes){

    RenderServerPacketRecognizer::RecogState recog = RenderServerPacketRecognizer::WAITING_FOR_PACKET;

    for (qsizetype i = 0; i < bytes.size(); i++){

        recog = recognizer.isNewBytePartOfPacket(bytes.at(i));
        if (recog == RenderServerPacketRecognizer::IN_PACKET){
            this->rxBuffer.append(bytes.at(i));
        }
        else if (recog == RenderServerPacketRecognizer::PACKET_DONE){
            this->rxBuffer.append(bytes.at(i));
            break;
        }

    }


    if (recog == RenderServerPacketRecognizer::PACKET_DONE){

        // We need to interpret the JSON String as two string fields.
        QJsonParseError json_error;
        QJsonDocument doc = QJsonDocument::fromJson(this->rxBuffer,&json_error);
        if (doc.isNull()){
            this->error = "Failed to decode JSON String for packet. Reason: " + json_error.errorString() + ". JSON STRING: " + QString(this->rxBuffer);
            return RX_ERROR;
        }

        // If parsing went well we need the two obligatory fields.
        QVariantMap map = doc.object().toVariantMap();

        if (!RenderServerPacketFields::ArePresent(map)){
            QStringList keys = map.keys();
            this->error = "Base Packet Fields are missing. Present fields are: " + keys.join(",");
            return RX_ERROR;
        }

        this->type = map.value(RenderServerPacketFields::TYPE).toString();
        QString payload_string = map.value(RenderServerPacketFields::PAYLOAD).toString();


        // Now the payload is ALSO a JSON String Map.
        doc = QJsonDocument::fromJson(payload_string.toUtf8(),&json_error);
        if (doc.isNull()){
            this->error = "Failed to decode JSON String for payload. Reason: " + json_error.errorString() + ". JSON STRING: " + payload_string;
            return RX_ERROR;
        }

        // If parsing went well we need the two obligatory fields.
        this->payload = doc.object().toVariantMap();

        return RX_DONE;


    }

    return RXState::RX_IN_PROGRESS;

}

void RenderServerPacket::setPacketType(const QString &type){
    this->type = type;
}

void RenderServerPacket::setPayloadField(const QString &name, const QVariant &value){
    this->payload[name] = value;
}

void RenderServerPacket::setPacketSkippable(bool skippable) {
    this->isSkippable = skippable;
}

bool RenderServerPacket::isPacketOfType(const QString &type) const{
    return (this->type == type);
}

QString RenderServerPacket::getStringSummary() const {
    if (this->type == RenderServerPacketType::TYPE_2D_RENDER){

        QVariantMap counters;
        QVariantList specs = this->payload.value(RenderControlPacketFields::SPEC_LIST).toList();
        for (qint32 i = 0; i < specs.size(); i++){
            QVariantMap item = specs.at(i).toMap();
            QString name = GL2DItemType::Name(item.value(RenderControlPacketFields::TYPE).toInt());
            if (counters.contains(name)){
                qint32 c = counters.value(name).toInt();
                c++;
                counters[name] = c;
            }
            else {
                counters[name] = 1;
            }
        }

        QStringList types = counters.keys();
        QStringList anslist;
        for (qint32 i = 0; i < types.size(); i++){
            anslist << counters.value(types.at(i)).toString() + " " + types.at(i);
        }
        return anslist.join(", ");

    }
    else return "";
}

QVariant RenderServerPacket::getPayloadField(const QString &name) const {
    return this->payload.value(name);
}

bool RenderServerPacket::containsPayloadField(const QString &name) const{
    return this->payload.contains(name);
}

void RenderServerPacket::resetForRX(){
    this->rxBuffer.clear();
    this->type = "";
    this->payload.clear();
}

QByteArray RenderServerPacket::getByteArrayToSend() const{

    // We first need to transformor the payload to a string.
    QVariantMap map;
    map[RenderServerPacketFields::TYPE] = this->type;

    if (this->isSkippable){ // False can be marked by simply not adding the field.
        qDebug() << "SETTING SKIPPPABLE";
        map[RenderServerPacketFields::SKIPPABLE] = true;
    }

    QJsonDocument doc = QJsonDocument::fromVariant(this->payload);
    map[RenderServerPacketFields::PAYLOAD] = QString(doc.toJson(QJsonDocument::Compact));

    // Now we transfrom the map to a byte array, pre appending the preamble.

    QByteArray tosend;
    tosend = RenderServerPacketRecognizer::PACKET_PREAMBLE.toUtf8();

    doc = QJsonDocument::fromVariant(map);
    tosend.append(doc.toJson(QJsonDocument::Compact));

    return tosend;
}

QString RenderServerPacket::getError() const {
    return this->error;
}

QString RenderServerPacket::getType() const {
    return this->type;
}

QVariantMap RenderServerPacket::getPayload() const {
    return this->payload;
}