#ifndef DATAPACKET_H
#define DATAPACKET_H

#include <QByteArray>
#include <QVariant>
#include <QFileInfo>
#include <QDir>
#include <QCryptographicHash>
#include "../../CommonClasses/common.h"

class DataPacket
{
public:

    static const quint8 DPFT_FILE                                     = 0;
    static const quint8 DPFT_REAL_VALUE                               = 1;
    static const quint8 DPFT_STRING                                   = 2;
    static const quint8 DPFT_INVALID                                  = 3;

    static const quint8 DPFI_DOCTOR_ID                                = 0;
    static const quint8 DPFI_PATIENT_ID                               = 1;
    static const quint8 DPFI_BINDING_UC                               = 2;
    static const quint8 DPFI_BINDING_BC                               = 3;
    static const quint8 DPFI_READING                                  = 4;
    static const quint8 DPFI_PATIENT_FILE                             = 5;
    static const quint8 DPFI_SEND_INFORMATION                         = 6;
    static const quint8 DPFI_REPORT                                   = 7;
    static const quint8 DPFI_DEMO_MODE                                = 8;
    static const quint8 DPFI_DB_QUERY_TYPE                            = 9;
    static const quint8 DPFI_DB_TABLE                                 = 10;
    static const quint8 DPFI_DB_COL                                   = 11;
    static const quint8 DPFI_DB_VALUE                                 = 12;
    //static const quint8 DPFI_DB_CONDITIION                            = 13;
    static const quint8 DPFI_DB_ERROR                                 = 14;
    //static const quint8 DPFI_DB_SET_ACK                               = 15;
    static const quint8 DPFI_DB_INST_PASSWORD                         = 16;
    static const quint8 DPFI_DB_INST_UID                              = 17;
    static const quint8 DPFI_DB_ET_SERIAL                             = 18;
    static const quint8 DPFI_PROCESSING_ACK                           = 19;
    static const quint8 DPFI_UPDATE_EYEEXP_ID                         = 20;
    static const quint8 DPFI_UPDATE_EYEEXP                            = 21;
    static const quint8 DPFI_UPDATE_EYELAUNCHER                       = 22;
    static const quint8 DPFI_UPDATE_CONFIG                            = 23;
    static const quint8 DPFI_UPDATE_LOGFILE                           = 24;
    static const quint8 DPFI_UPDATE_FLOGNAMES                         = 25;
    static const quint8 DPFI_UPDATE_FLOGCONTENT                       = 26;
    static const quint8 DPFI_UPDATE_LANG                              = 27;
    static const quint8 DPFI_UPDATE_RESULT                            = 28;
    static const quint8 DPFI_UPDATE_CHANGES                           = 29;
    static const quint8 DPFI_INST_NAMES                               = 30;
    static const quint8 DPFI_DATE_START                               = 31;
    static const quint8 DPFI_DATE_END                                 = 32;
    static const quint8 DPFI_RAW_DATA_NAMES                           = 33;
    static const quint8 DPFI_RAW_DATA_CONTENT                         = 34;
    static const quint8 DPFI_LOCAL_DB_BKP                             = 35;
    static const quint8 DPFI_PUID_LIST                                = 36;
    //static const quint8 DPFI_PATNAME_LIST                             = 37;
    static const quint8 DPFI_SERIALIZED_DB                            = 38;
    static const quint8 DPFI_OLD_REP_FILE                             = 39;
    static const quint8 DPFI_DOCTOR_FILE                              = 40;
    static const quint8 DPFI_CONF_FILE                                = 41;
    static const quint8 DPFI_NUM_EVALS                                = 42;
    static const quint8 DPFI_VMID_TABLE                               = 43;

    static const quint8 DATABUFFER_RESULT_NOT_DONE                    = 0;
    static const quint8 DATABUFFER_RESULT_ERROR                       = 1;
    static const quint8 DATABUFFER_RESULT_DONE                        = 2;

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
    QString saveFile(const QString &directory, quint8 fieldInfo, const QString &fileNameSuffix = "");

    // Saves all files in the packet to a directory
    bool saveFiles(const QString& directory);

    // Checks for a field type
    bool hasInformationField(quint8 field_info) {return fields.contains(field_info);}
    bool isInformationFieldOfType(quint8 field_info, quint8 field_type);

    // Get field for information
    Field getField(quint8 field_info) {return fields.value(field_info,Field());}

    // Clear all data.
    void clearAll() { buffer.clear(); fields.clear(); packetSize = 0; }

    // Get the last file names saved
    QStringList getLastFilesSaved() const { return fileNamesSaved; }

    // Retruns what percet of the calculated packete size has been
    qreal getPercentArrived() { return percentArrived; }

    // For Debuggging
    void print();

    // Transform a size to a numberic representation of 4 bytes.
    static QByteArray sizeToByteArray(quint32 size);
    static quint32 byteArrayToSize(const QByteArray &size);
    static QString getFileHash(const QString &filePath);

private:

    // Constant sizes for information
    static const quint8 BYTES_FOR_SIZE = 4;
    static const quint8 BYTES_FOR_REAL = 8;

    // The buffer array.
    QByteArray buffer;

    // Expected number of bytes to arrive
    quint32 packetSize;
    qreal percentArrived;

    // The data
    Fields fields;

    // The names of the last set of files saved
    QStringList fileNamesSaved;

    // Generic interface to add a field as is.
    void addField(quint8 dpft, const QVariant & data, quint8 field_information);

};

#endif // DATAPACKET_H
