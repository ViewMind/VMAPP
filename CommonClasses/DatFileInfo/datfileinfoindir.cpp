#include "datfileinfoindir.h"

DatFileInfoInDir::DatFileInfoInDir(){

}

void DatFileInfoInDir::setDatDirectory(const QString &dir, bool listRepEvenIfTheyExist)
{
    Q_UNUSED(listRepEvenIfTheyExist)

    filesReading.clear();
    filesBindingBC.clear();
    filesBindingUC.clear();

    // STEP 1: Creating the structure with just the .dat information.
    QStringList filters;
    filters << "*.dat";
    QStringList fileList = QDir(dir).entryList(filters,QDir::Files);

    for (qint32 i = 0; i < fileList.size(); i++){

        QString fname = fileList.at(i);
        //qWarning() << "Processing file" << fname;
        //DatInfo datInfo;
        if (fname.startsWith(FILE_OUTPUT_READING)) filesReading << fname;
        else if (fname.startsWith(FILE_OUTPUT_BINDING_BC)) filesBindingBC << fname;
        else if (fname.startsWith(FILE_OUTPUT_BINDING_UC)) filesBindingUC << fname;

    }
}


bool DatFileInfoInDir::hasPendingReports() const {
    if (!filesReading.isEmpty()) return true;
    if (!filesBindingBC.isEmpty() && !filesBindingUC.isEmpty()) return true;
    else return false;
}

QStringList DatFileInfoInDir::getReadingFileList() const{
    return getFileList(filesReading);
}

QStringList DatFileInfoInDir::getBindingBCFileList() const{
    return getFileList(filesBindingBC);
}

QStringList DatFileInfoInDir::getBindingUCFileList() const {
    return getFileList(filesBindingUC);
}

QStringList DatFileInfoInDir::getBindingUCFileListCompatibleWithSelectedBC(qint32 selectedBC){
    DatInfo bc = getBindingFileInformation(filesBindingBC.at(selectedBC));
    filesBindingUCValidIndexes.clear();
    QStringList ans;
    //qWarning() << "Comparing" << bc.toString();
    for (qint32 i = 0; i < filesBindingUC.size(); i++){
        //qWarning() << "   WITH: " << filesBindingUC.at(i).toString();
        DatInfo uc = getBindingFileInformation(filesBindingUC.at(i));
        if (bc.isFileCompatibleWith(uc)){
            filesBindingUCValidIndexes << i;
            ans << uc.code;
        }
    }
    return ans;
}

QStringList DatFileInfoInDir::getFileSetAndReportName(const QStringList &fileList){

    ReportGenerationStruct repgen;
    repgen.clear();

    QList<QStringList> allLists;
    allLists << filesReading << filesBindingBC << filesBindingUC;

    for (qint32 i = 0; i < fileList.size(); i++){
        if (fileList.at(i).startsWith(FILE_OUTPUT_READING)){
            filesReading << fileList.at(i);
            repgen.readingFileIndex = filesReading.size()-1;
        }
        else if (fileList.at(i).startsWith(FILE_OUTPUT_BINDING_BC)){
            filesBindingBC << fileList.at(i);
            repgen.bindingBCFileIndex = filesBindingBC.size()-1;
        }
        else if (fileList.at(i).startsWith(FILE_OUTPUT_BINDING_UC)){
            filesBindingUC << fileList.at(i);
            repgen.bindingUCFileIndex = filesBindingUC.size() -1;
        }
    }

    return getFileSetAndReportName(repgen);
}

QStringList DatFileInfoInDir::getFileSetAndReportName(const ReportGenerationStruct &repgen) const{

    QStringList ans;
    QString expectedReportName = "";
    QString date;
    QString time;

    if (repgen.readingFileIndex != -1){
        DatInfo reading_file = getReadingInformation(filesReading.at(repgen.readingFileIndex));
        expectedReportName = QString(FILE_REPORT_NAME) + "_r";
        date = reading_file.date;
        time = reading_file.hour;
        ans << reading_file.fileName;
    }

    if ((repgen.bindingBCFileIndex != -1) && (repgen.bindingUCFileIndex != -1)){
        DatInfo uc = getBindingFileInformation(filesBindingUC.at(filesBindingUCValidIndexes.at(repgen.bindingUCFileIndex)));
        DatInfo bc = getBindingFileInformation(filesBindingBC.at(repgen.bindingBCFileIndex));
        if (expectedReportName.isEmpty()) expectedReportName = FILE_REPORT_NAME;
        // Extra info and date must have matched for these two files to have been selected.
        expectedReportName = expectedReportName + "_" + uc.extraInfo;
        if (date.isEmpty() || (date < uc.date)) {
            date = uc.date;
            time = uc.hour;
        }
        else if ((date == uc.date) && (time < uc.hour)){
            time = uc.hour;
        }
        ans << uc.fileName << bc.fileName;
    }

    if (!expectedReportName.isEmpty()){
       time = time.replace(":","_");
       expectedReportName = expectedReportName + "_" + date + "_" +  ".rep";
       ans.prepend(expectedReportName);
    }

    return ans;
}

QStringList DatFileInfoInDir::getFileList(const QStringList &infoList) const{
    QStringList list;
    for (qint32 i = 0; i < infoList.size(); i++){
        DatInfo file = getDatFileInformation(infoList.at(i));
        //qWarning() << "DATINFO: " << file.toString() << "FROM" << infoList.at(i);
        list << file.code;
    }
    return list;
}


DatFileInfoInDir::DatInfo DatFileInfoInDir::getDatFileInformation(const QString &file){
    if (file.startsWith(FILE_OUTPUT_BINDING_BC)) return getBindingFileInformation(file);
    else if (file.startsWith(FILE_OUTPUT_BINDING_UC)) return getBindingFileInformation(file);
    else if (file.startsWith(FILE_OUTPUT_READING)) return getReadingInformation(file);
    else return DatInfo();
}

DatFileInfoInDir::DatInfo DatFileInfoInDir::getBindingFileInformation(const QString &bindingFile){

    QStringList parts = bindingFile.split(".",QString::SkipEmptyParts);
    QString codePrefix;
    if (bindingFile.contains("bc")) codePrefix = "BC";
    else codePrefix = "UC";
    QString baseName = parts.first();
    parts = baseName.split("_",QString::SkipEmptyParts);
    DatInfo info;
    info.fileName = bindingFile;
    if (parts.size() == 5){
        // This is an old file so it only contains a date as timestamp.
        // Target size was large and number of targets were two.
        info.date = parts.at(2) + "_" + parts.at(3) + "_" + parts.at(4);
        info.extraInfo = "2l";
        info.validEye = QString::number(EYE_BOTH);
        info.hour = "00_00";
        info.basename = parts.at(0) + "_" + parts.at(1);
        info.code = codePrefix + info.extraInfo + info.validEye + " - " + parts.at(4) + "/" + parts.at(3) + "/" + parts.at(2);
        info.orderString =  parts.at(2) + parts.at(3) + parts.at(4) + "0000";
    }
    else if (parts.size() == 7){
        // File name before time stamp included hours and minutes
        info.extraInfo = parts.at(2) + parts.at(3);
        info.date = parts.at(4) + "_" + parts.at(5) + "_" + parts.at(6);
        info.hour = "00_00";
        info.validEye = QString::number(EYE_BOTH);
        info.basename = parts.at(0) + "_" + parts.at(1);
        info.code = codePrefix + info.extraInfo + info.validEye + " - " + parts.at(6) + "/" + parts.at(5) + "/" + parts.at(4);
        info.orderString =  parts.at(4) + parts.at(5) + parts.at(6) + "0000";
    }
    else if (parts.size() == 10){
        // Full file name with time stamp including hours and minutes.
        info.extraInfo = parts.at(2) + parts.at(3);
        info.date = parts.at(5) + "_" + parts.at(6) + "_" + parts.at(7);
        info.hour = parts.at(8) + "_" +  parts.at(9);
        info.basename = parts.at(0) + "_" + parts.at(1);
        info.validEye = parts.at(4);
        info.code = codePrefix + info.extraInfo + info.validEye + " - " + parts.at(7) + "/" + parts.at(6) + "/" + parts.at(5) + " " + parts.at(8) + ":" + parts.at(9);
        info.orderString = parts.at(5) + parts.at(6) + parts.at(7) + parts.at(8) + parts.at(9);
    }

    return info;
}

DatFileInfoInDir::DatInfo DatFileInfoInDir::getReadingInformation(const QString &readingFile){

    QStringList parts = readingFile.split(".",QString::SkipEmptyParts);
    QString baseName = parts.first();
    parts = baseName.split("_",QString::SkipEmptyParts);
    DatInfo ans;
    ans.extraInfo = "";
    ans.fileName = readingFile;

    if (parts.size() == 4){
        // This is an old file so it only contains a date as timestamp.
        ans.date = parts.at(1) + "_" + parts.at(2) + "_" + parts.at(3);
        ans.hour = "00_00";
        ans.validEye = QString::number(EYE_BOTH);
        ans.basename = parts.at(0);
        ans.code = "RD" + ans.validEye + " - " + parts.at(3) + "/" + parts.at(2) + "/" + parts.at(1);
        ans.orderString = parts.at(1) +  parts.at(2) + parts.at(3) + "00_00";
    }
    else if (parts.size() == 7){
        // File name before time stamp included hours and minutes
        ans.date = parts.at(2) + "_" + parts.at(3) + "_" + parts.at(4);
        ans.validEye = parts.at(1);
        ans.hour = parts.at(5) + "_" + parts.at(6);
        ans.basename = parts.at(0);
        ans.code = "RD" + ans.validEye + " - " + parts.at(4) + "/" + parts.at(3) + "/" + parts.at(2) + " " + parts.at(5) + ":" + parts.at(6);
        ans.orderString = parts.at(2) + parts.at(3) + parts.at(4) + parts.at(5) + parts.at(6);
    }

    return ans;
}

qint32 DatFileInfoInDir::getValidEyeForDatList(const QStringList &list){

    qint32 ans = EYE_BOTH;

    for (qint32 i = 0; i < list.size(); i++){
        DatInfo datInfo = getDatFileInformation(list.at(i));
        int val = datInfo.validEye.toInt();
        // Both eyes are used unless ALL files in the list have the same valid eye.
        if (ans == EYE_BOTH) ans = val;
        else if (ans != val) return EYE_BOTH;
    }

    return ans;

}

//DatFileInfoInDir::DatInfo DatFileInfoInDir::getRerportInformation(const QString &repfile){

//    QStringList parts = repfile.split(".",QString::SkipEmptyParts);
//    QString baseName = parts.first();
//    parts = baseName.split("_",QString::SkipEmptyParts);
//    DatInfo ans;
//    ans.extraInfo = "";
//    ans.fileName = repfile;
//    if (parts.size() >= 7){

//        // The last 3 parts are the date. While the first is the base name, which should alwasy be report.
//        ans.basename = parts.at(0);
//        qint32 last = parts.size()-1;
//        ans.date = parts.at(last-5) + "_" + parts.at(last-4) + "_" + parts.at(last-3);
//        ans.hour = parts.at(last-1) + ":" + parts.at(last-2);

//        if (parts.size() > 7) {
//            // There is extra information regarding what is in the report.
//            QStringList extraInfo;
//            for (qint32 i = 1; i < last-2; i++){
//                extraInfo << parts.at(i);
//            }
//            if (extraInfo.size() > 1) ans.extraInfo = extraInfo.join("_");
//            else ans.extraInfo = extraInfo.first();
//        }

//    }
//    return ans;
//}
