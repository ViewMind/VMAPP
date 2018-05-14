#include "datapacket.h"

DataPacket::DataPacket()
{
}

bool DataPacket::addFile(const QString &fileName){

    // Checking the file exists and getting its complete name (filename + extension).
    QFileInfo info(fileName);
    if (!info.exists())  return false;
    QString fname = info.completeBaseName();

    // Getting the files as raw information.
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) return false;
    QByteArray filedata = file.readAll();
    file.close();

    QList<QVariant> data;
    data << fname;
    data << filedata;

    addField(DPFT_FILE,data);
    return true;

}

void DataPacket::addString(const QString &data){
    addField(DPFT_STRING,data);
}

void DataPacket::addValue(qreal value){
    addField(DPFT_REAL_VALUE,value);
}

void DataPacket::addField(DataPacketFieldType dpft, const QVariant &data){
    Field f;
    f.fieldType = dpft;
    f.data = data;
    fields << f;
}

QByteArray DataPacket::toByteArray() const{

    QByteArray ans;

    for (qint32 i = 0; i < fields.size(); i++){

        // Adding the type.
        Field f = fields.at(i);
        ans.append((quint8)f.fieldType);
        qreal v;

        switch (f.fieldType){
        case DPFT_FILE:
            // The size of file name
            ans.append(sizeToByteArray(f.data.toList().first().toByteArray().size()));
            // The file name
            ans.append(f.data.toList().first().toByteArray());
            // The size of the file
            ans.append(sizeToByteArray(f.data.toList().last().toByteArray().size()));
            // The file.
            ans.append(f.data.toList().last().toByteArray());
            break;
        case DPFT_REAL_VALUE:
            // This will allways be 8 bytes.
            v = f.data.toDouble();
            ans.append(reinterpret_cast<const char*>(&v),sizeof(v));
            break;
        case DPFT_STRING:
            // The size of the string.
            ans.append(sizeToByteArray(f.data.toByteArray().size()));
            // The string itself
            ans.append(f.data.toByteArray());
            break;
        }

    }

    return ans;
}

void DataPacket::fromByteArray(const QByteArray &array){

    qint32 i = 0;
    QByteArray temp;
    quint32 nameSize = 0;
    quint32 strSize = 0;
    QString str;
    qreal value = 0;
    QList<QVariant> list;

    while (i < array.size()){

        quint8 identifier = (quint8) array.at(i);
        DataPacketFieldType dpft = (DataPacketFieldType) identifier;
        i++;
        Field f;
        f.fieldType = dpft;

        switch (dpft){
        case DPFT_FILE:

            // Getting the size of the file name.
            temp.clear();
            for (qint32 j = 0; j < BYTES_FOR_SIZE; j++){
                temp.append(array.at(i)); i++;
            }
            nameSize = byteArrayToSize(temp);

            // Getting the file name.
            temp.clear();
            for (quint32 j = 0; j < nameSize; j++){
                temp.append(array.at(i)); i++;
            }
            str = QString(temp);

            // Getting the file size.
            temp.clear();
            for (qint32 j = 0; j < BYTES_FOR_SIZE; j++){
                temp.append(array.at(i)); i++;
            }
            strSize = byteArrayToSize(temp);

            // Getting the file data.
            temp.clear();
            for (quint32 j = 0; j < strSize; j++){
                temp.append(array.at(i)); i++;
            }

            list << str << temp;
            f.data = list;

            break;
        case DPFT_REAL_VALUE:

            // This is always 8 bytes.
            temp.clear();
            for (qint32 j = 0; j < BYTES_FOR_REAL; j++){
                temp.append(array.at(i)); i++;
            }
            value = *reinterpret_cast<const qreal*>(temp.data());
            f.data = value;

            break;
        case DPFT_STRING:

            // Getting the size of the string
            temp.clear();
            for (qint32 j = 0; j < BYTES_FOR_SIZE; j++){
                temp.append(array.at(i)); i++;
            }
            strSize = byteArrayToSize(temp);

            // Getting the string
            temp.clear();
            for (quint32 j = 0; j < nameSize; j++){
                temp.append(array.at(i)); i++;
            }
            str = QString(temp);

            f.data = str;
            break;
        }

        fields << f;

    }

}

bool DataPacket::saveFile(const QString &directory, qint32 fieldIndex){

    // Checking that everything is kosher.
    if ((fieldIndex < 0) || (fieldIndex >= fields.size())) return false;
    if (fields.at(fieldIndex).fieldType != DPFT_FILE) return false;
    if (!QDir(directory).exists()) return false;

    // Opening a file and saving the data as is.
    Field f =  fields.at(fieldIndex);
    QString fileName = directory + "/" + f.data.toList().first().toString();
    QFile file (fileName);
    if (!file.open(QFile::WriteOnly)) return false;
    qint64 written = file.write(f.data.toList().last().toByteArray());
    file.close();

    // If the numbers of bytes written differs from the number of bytes on the array, there was a problem.
    if (written != f.data.toList().last().toByteArray().size()) return false;
    return true;
}

QByteArray DataPacket::sizeToByteArray(quint32 size){

    quint32 q1 = size/256;
    quint8  r1 = size - q1*256; // Byte 0
    quint32 q2 = q1/256;
    quint8  r2 = q1 - q2*256;   // Byte 1
    quint32 q3 = q2/256;        // Byte 3
    quint32 r3 = q2 - q3*256;   // Byte 2

    QByteArray ans;
    ans.append((quint8) q3);
    ans.append(r3);
    ans.append(r2);
    ans.append(r1);

    //qWarning() << (quint8) ans.at(0) << (quint8) ans.at(1) <<(quint8)  ans.at(2) <<(quint8)  ans.at(3);

    return ans;

}

quint32 DataPacket::byteArrayToSize(const QByteArray &size){
    if (size.size() != BYTES_FOR_SIZE) return 0;
    QList<quint32> coeffs; coeffs << 16777216 << 65536 << 256 << 1;
    quint32 ans = 0;
    for (qint32 i = 0; i < BYTES_FOR_SIZE; i++){
        ans = ans + coeffs.at(i)*(quint8)size.at(i);
        //qWarning() << "ANS + " << coeffs.at(i) << "*" << (quint8) size.at(i) << "(" << coeffs.at(i)*(quint8)size.at(i) << ") = " << ans;
    }
    return ans;
}
