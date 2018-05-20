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
        quint8 fieldType = DPFT_FILE;
    };

    typedef QHash<quint8,Field> Fields;

    DataPacket();

    // Interface to add the different type of fields. This DOES NOT CHECK for UNIQNESS of the KEY
    bool addFile (const QString &fileName, quint8 field_information);
    void addValue(qreal value, quint8 field_information);
    void addString(const QString &data, quint8 field_information);

    // Transforms packet into a byte array to send.
    QByteArray toByteArray() const;

    // Clear buffered data
    void clearBufferedData() { buffer.clear(); packetSize = 0;}

    // Load the information from a existing byte array.
    quint8 bufferByteArray(const QByteArray &array);

    // Saves a particular field as a file in the specified directory. Returns the full file path if successful.
    // Otherwise returns an empty string.
    QString saveFile(const QString &directory, quint8 fieldInfo);

    // Checks for a field type
    bool hasInformationField(quint8 field_info) {return fields.contains(field_info);}
    bool isInformationFieldOfType(quint8 field_info, quint8 field_type);

    // Get field for information
    Field getField(quint8 field_info) {return fields.value(field_info,Field());}

    // Clear all data.
    void clearAll() { buffer.clear(); fields.clear(); packetSize = 0; }

    // Transform a size to a numberic representation of 4 bytes.
    static QByteArray sizeToByteArray(quint32 size);
    static quint32 byteArrayToSize(const QByteArray &size);

private:

    // The buffer array.
    QByteArray buffer;

    // Expected number of bytes to arrive
    quint32 packetSize;

    // The data
    Fields fields;

    // Generic interface to add a field as is.
    void addField(quint8 dpft, const QVariant & data, quint8 field_information);

};

#endif // DATAPACKET_H
