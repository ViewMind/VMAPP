#include "datapacket.h"

DataPacket::DataPacket()
{
}

bool DataPacket::addFile(const QString &fileName, quint8 field_information){

    // Checking the file exists and getting its complete name (filename + extension).
    QFileInfo info(fileName);
    if (!info.exists())  return false;
    QString fname = info.completeBaseName() + "." + info.completeSuffix();

    // Getting the files as raw information.
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) return false;
    QByteArray filedata = file.readAll();
    file.close();

    QList<QVariant> data;
    data << fname;
    data << filedata;

    addField(DPFT_FILE,data,field_information);
    return true;

}

void DataPacket::addString(const QString &data, quint8 field_information){
    addField(DPFT_STRING,data,field_information);
}

void DataPacket::addValue(qreal value, quint8 field_information){
    addField(DPFT_REAL_VALUE,value,field_information);
}

void DataPacket::addField(quint8 dpft, const QVariant &data, quint8 field_information){
    Field f;
    f.fieldType = dpft;
    f.data = data;
    fields[field_information] = f;
}

bool DataPacket::isInformationFieldOfType(quint8 field_info, quint8 field_type){
    if (!fields.contains(field_info)) return false;
    return (fields.value(field_info).fieldType == field_type);
}

QByteArray DataPacket::toByteArray() const{

    QByteArray ans;    
    QList<quint8> fieldInfo = fields.keys();

    for (qint32 i = 0; i < fieldInfo.size(); i++){

        // Adding the type.
        Field f = fields.value(fieldInfo.at(i));
        ans.append(f.fieldType);
        ans.append(fieldInfo.at(i));
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

    // Adding the total byte size at the beginning
    QByteArray size = sizeToByteArray(ans.size());
    size.append(ans);

    return size;
}

quint8 DataPacket::bufferByteArray(const QByteArray &array){

    QByteArray temp;

    if (buffer.size() < BYTES_FOR_SIZE){
        buffer.append(array);
        if (buffer.size() >= BYTES_FOR_SIZE){
            for (qint32 j = 0; j < BYTES_FOR_SIZE; j++){
                temp.append(buffer.at(j));
            }
            packetSize = byteArrayToSize(temp);
            // Check to see if all the data arrived.
            qWarning() << "Buffer size" << buffer.size() << "packet size" << packetSize;
            if ((quint32)buffer.size() < packetSize + BYTES_FOR_SIZE) return DATABUFFER_RESULT_NOT_DONE;
        }
        else return DATABUFFER_RESULT_NOT_DONE;
    }
    else{
        buffer.append(array);
        if ((quint32)buffer.size() < packetSize + BYTES_FOR_SIZE) return DATABUFFER_RESULT_NOT_DONE;
    }

    qint32 i = BYTES_FOR_SIZE;
    quint32 nameSize = 0;
    quint32 strSize = 0;
    quint8 fieldInformation;
    QString str;
    qreal value = 0;
    QList<QVariant> list;

    while (i < buffer.size()){

        Field f;
        f.fieldType = (quint8) buffer.at(i); i++;;
        fieldInformation = (quint8) buffer.at(i); i++;

        switch (f.fieldType){
        case DPFT_FILE:

            // Getting the size of the file name.
            temp.clear();
            for (qint32 j = 0; j < BYTES_FOR_SIZE; j++){
                temp.append(buffer.at(i)); i++;
            }
            nameSize = byteArrayToSize(temp);

            // Getting the file name.
            temp.clear();
            for (quint32 j = 0; j < nameSize; j++){
                temp.append(buffer.at(i)); i++;
            }
            str = QString(temp);

            // Getting the file size.
            temp.clear();
            for (qint32 j = 0; j < BYTES_FOR_SIZE; j++){
                temp.append(buffer.at(i)); i++;
            }
            strSize = byteArrayToSize(temp);

            // Getting the file data.
            temp.clear();
            for (quint32 j = 0; j < strSize; j++){
                temp.append(buffer.at(i)); i++;
            }

            list << str << temp;
            f.data = list;

            break;
        case DPFT_REAL_VALUE:

            // This is always 8 bytes.
            temp.clear();
            for (qint32 j = 0; j < BYTES_FOR_REAL; j++){
                temp.append(buffer.at(i)); i++;
            }
            value = *reinterpret_cast<const qreal*>(temp.data());
            f.data = value;

            break;
        case DPFT_STRING:

            // Getting the size of the string
            temp.clear();
            for (qint32 j = 0; j < BYTES_FOR_SIZE; j++){
                temp.append(buffer.at(i)); i++;
            }
            strSize = byteArrayToSize(temp);

            // Getting the string
            temp.clear();
            for (quint32 j = 0; j < strSize; j++){
                temp.append(buffer.at(i)); i++;
            }
            str = QString(temp);

            f.data = str;
            break;
        default:
            fields.clear();
            clearAll();
            return DATABUFFER_RESULT_ERROR;
        }

        fields[fieldInformation] = f;
    }

    return DATABUFFER_RESULT_DONE;

}

bool DataPacket::saveFiles(const QString &directory){

    QList<quint8> fieldlist = fields.keys();

    for (qint32 i = 0; i < fieldlist.size(); i++){
        if (fields.value(fieldlist.at(i)).fieldType == DPFT_FILE){
            QString ans = saveFile(directory,fieldlist.at(i));
            if (ans.isEmpty()) return false;
        }
    }

    return true;

}


QString DataPacket::saveFile(const QString &directory, quint8 fieldInfo){

    // Checking that everything is kosher.
    if (!fields.contains(fieldInfo)) return "";
    if (fields.value(fieldInfo).fieldType != DPFT_FILE) return "";
    if (!QDir(directory).exists()) return "";

    // Opening a file and saving the data as is.
    Field f =  fields.value(fieldInfo);
    QString fileName = directory + "/" + f.data.toList().first().toString();
    QFile file (fileName);
    if (!file.open(QFile::WriteOnly)) return "";
    qint64 written = file.write(f.data.toList().last().toByteArray());
    file.close();

    // If the numbers of bytes written differs from the number of bytes on the array, there was a problem.
    if (written != f.data.toList().last().toByteArray().size()) return false;
    return fileName;
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
