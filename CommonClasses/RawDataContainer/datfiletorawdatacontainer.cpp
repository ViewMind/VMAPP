#include "datfiletorawdatacontainer.h"

DatFileToRawDataContainer::DatFileToRawDataContainer()
{

}

QString DatFileToRawDataContainer::getError() const {
    return error;
}

RawDataContainer DatFileToRawDataContainer::fromDatFile(const QString &dat_file){

    error = "";
    RawDataContainer rdc;

    DatFileInfoInDir::DatInfo dat_info = DatFileInfoInDir::getDatFileInformation(file_path);
    if (dat_info.category == ""){
        error = "Could not determine the study type for " + file_path;
        return rdc;
    }

    // Valid Eye Conversion. The code used for the eyes correspond to the string location in this list.
    QList<RawDataContainer::StudyConfigurationValue> eyes; eyes << RawDataContainer::SCV_EYE_LEFT << RawDataContainer::SCV_EYE_RIGHT << RawDataContainer::SCV_EYE_BOTH;

//    // It was a success the basic meta data is filled.
//    data[MAIN_FIELD_METADATA][FIELD_DATE] = dat_info.date;
//    data[MAIN_FIELD_METADATA][FIELD_HOUR] = dat_info.hour;
//    data[MAIN_FIELD_METADATA][FIELD_STUDY_CONFIGURATION] = QVariantMap();
//    data[MAIN_FIELD_METADATA][FIELD_STUDY_CONFIGURATION][FIELD_VALID_EYE] = eyes.at(dat_info.validEye.toInt());

//    // The first two letters of the category tell teh type of study.
//    QString code = dat_info.category.mid(0,2);
//    if (code == "RD"){
//        // Extra parts is the language code. And this is the only configuration, besides they eyes for a reading experiment.
//        data[MAIN_FIELD_METADATA][FIELD_STUDY_CONFIGURATION] = dat_info.extraInfo.toUpper();
//    }
//    else{
//        error = "Unrecognized code " + code + " from " + file_path;
//        return rdc;
//    }
    return RawDataContainer();

}


QStringList DatFileToRawDataContainer::separateStudyDescriptionFromData(const QString &file_path, const QString &header){

    QFile file(file_path);
    if (!file.open(QFile::ReadOnly)){
        error = "Could not open file for reading: " + file_path;
        return QStringList();
    }

    QTextStream reader(&file);
    QString content = reader.readAll();
    file.close();

    QStringList parts = content.split(header,QString::SkipEmptyParts);

    // There should be either two or three parts.
    if (parts.size() == 2) return parts;
    if (parts.size() == 3){
        // Assuming that the first part is trash and ignore it.
        parts.removeFirst();
        return parts;
    }
    else{
        error = "Format error of input file. Split by header parts: " + QString::number(parts.size());
        return QStringList();
    }
}
