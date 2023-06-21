#ifndef RENDERSERVERPACKET_H
#define RENDERSERVERPACKET_H

#include <QVariantMap>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>

#include "RenderServerStrings.h"
#include "renderserverpacketrecognizer.h"

class RenderServerPacket
{
public:

    typedef enum {RX_DONE, RX_ERROR, RX_IN_PROGRESS} RXState;

    RenderServerPacket();

    // Receive functions.
    RXState rxBytes(const QByteArray &bytes);
    void resetForRX();

    // Transmit functions.
    QByteArray getByteArrayToSend() const;

    // General functions
    QString getError() const;
    QString getType() const;
    QVariantMap getPayload() const;

    // Creation and data extraction function.
    void setPacketType(const QString &type);
    void setPayloadField(const QString &name, const QVariant &vallue);
    void setFullPayload(const QVariantMap &map);
    QVariant getPayloadField(const QString &name) const;
    bool containsPayloadField(const QString &name) const;
    bool isPacketOfType (const QString &type) const;

private:

    QString type;
    QVariantMap payload;
    QByteArray rxBuffer;
    QString error;

    RenderServerPacketRecognizer recognizer;

};

#endif // RENDERSERVERPACKET_H
