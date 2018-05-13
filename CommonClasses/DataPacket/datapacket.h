#ifndef DATAPACKET_H
#define DATAPACKET_H

#include <QByteArray>
#include <QVariant>
#include "../../CommonClasses/common.h"

class DataPacket
{
public:

    struct Field {
        QVariant data;
        DataPacketFieldType fieldType;
    };

    typedef QList<Field> Fields;

    DataPacket();

    // Interface to add the different type of fields.
    bool addFile (const QString &fileName);
    void addValue(qreal value);
    void addString(const QString &data);

    // Transforms packet into a byte array to send.
    QByteArray toByteArray() const;

    // Load the information from a existing byte array.
    void fromByteArray(const QByteArray &array);

    // Saves a particular field as a file in the specified directory.
    bool saveFile (const QString &directory, qint32 fieldIndex);

    // Transform a size to a numberic representation of 4 bytes.
    static QByteArray sizeToByteArray(quint32 size);
    static quint32 byteArrayToSize(const QByteArray &size);

private:
    Fields fields;

    // Generic interface to add a field as is.
    void addField(DataPacketFieldType dpft, const QVariant & data);

};

#endif // DATAPACKET_H
