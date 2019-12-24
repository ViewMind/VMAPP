#include "datfileinfoindir.h"

DatFileInfoInDir::DatFileInfoInDir(){

}

void DatFileInfoInDir::setDatDirectory(const QString &dir)
{

    filesReading.clear();
    filesBindingBC.clear();
    filesBindingUC.clear();
    filesFielding.clear();

    // STEP 1: Creating the structure with just the .dat information.
    QStringList filters;
    filters << "*.dat" << "*.datf";
    QStringList fileList = QDir(dir).entryList(filters,QDir::Files);

    //qWarning() << "SETTING DAT DIRECTORY: File list is" << fileList;

    // Saving the doctor directory.
    QStringList orderReading;
    QStringList orderBindingBC;
    QStringList orderBindingUC;
    QStringList orderFielding;

    for (qint32 i = 0; i < fileList.size(); i++){

        QString fname = fileList.at(i);
        //qWarning() << "Processing file" << fname;
        //DatInfo datInfo;
        if (fname.startsWith(FILE_OUTPUT_READING)) insertIntoListAccordingToOrder(fname,&filesReading,&orderReading);
        else if (fname.startsWith(FILE_OUTPUT_BINDING_BC)) insertIntoListAccordingToOrder(fname,&filesBindingBC,&orderBindingBC);
        else if (fname.startsWith(FILE_OUTPUT_BINDING_UC)) insertIntoListAccordingToOrder(fname,&filesBindingUC,&orderBindingUC);
        else if (fname.startsWith(FILE_OUTPUT_FIELDING)) insertIntoListAccordingToOrder(fname,&filesFielding,&orderFielding);

    }
}


bool DatFileInfoInDir::hasPendingReports() const {
    //qWarning() << "HAS PENDING REPORTS" << filesReading << filesBindingBC << filesBindingUC;
    if (!filesReading.isEmpty()) return true;
    if (!filesFielding.isEmpty()) return true;
    if (!filesBindingBC.isEmpty() && !filesBindingUC.isEmpty()){
        // This should return true ONLY if for at least one BC file there is a compatible UC file.
        for (qint32 i = 0; i < filesBindingBC.size(); i++){
            QList<qint32> dummy;
            if (!getBindingUCFileListCompatibleWithSelectedBC(i,&dummy).isEmpty()) {
                //qWarning() << "List of Compatible UC Files (indexes)" << dummy;
                return true;
            }
        }
        return false;
    }
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

QStringList DatFileInfoInDir::getFieldingFileList() const{
    return getFileList(filesFielding);
}

QStringList DatFileInfoInDir::getBindingUCFileListCompatibleWithSelectedBC(qint32 selectedBC){
    return getBindingUCFileListCompatibleWithSelectedBC(selectedBC,&filesBindingUCValidIndexes);
}

QStringList DatFileInfoInDir::getBindingUCFileListCompatibleWithSelectedBC(qint32 selectedBC, QList<qint32> *validIndexes) const{
    DatInfo bc = getBindingFileInformation(filesBindingBC.at(selectedBC));
    validIndexes->clear();
    QStringList ans;
    //qWarning() << "Comparing" << bc.toString();
    for (qint32 i = 0; i < filesBindingUC.size(); i++){
        //qWarning() << "   WITH: " << filesBindingUC.at(i).toString();
        DatInfo uc = getBindingFileInformation(filesBindingUC.at(i));
        if (bc.isFileCompatibleWith(uc)){
            validIndexes->append(i);
            ans << uc.code;
        }
    }
    return ans;
}


QStringList DatFileInfoInDir::getFileSetAndReportName(const QStringList &fileList){

    ReportGenerationStruct repgen;
    repgen.clear();

//    QList<QStringList> allLists;
//    allLists << filesReading << filesBindingBC << filesBindingUC << filesFielding;

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
        else if (fileList.at(i).startsWith(FILE_OUTPUT_FIELDING)){
            filesFielding << fileList.at(i);
            repgen.fieldingFileIndex = filesFielding.size() - 1;
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
        expectedReportName = QString(FILE_REPORT_NAME) + "_R" + reading_file.extraInfo + reading_file.validEye;
        date = reading_file.date;
        time = reading_file.hour;
        ans << reading_file.fileName;
    }

    if ((repgen.bindingBCFileIndex != -1) && (repgen.bindingUCFileIndex != -1)){
        DatInfo uc = getBindingFileInformation(filesBindingUC.at(filesBindingUCValidIndexes.at(repgen.bindingUCFileIndex)));
        DatInfo bc = getBindingFileInformation(filesBindingBC.at(repgen.bindingBCFileIndex));
        if (expectedReportName.isEmpty()) expectedReportName = FILE_REPORT_NAME;
        // Extra info and date must have matched for these two files to have been selected.
        expectedReportName = expectedReportName + "_B" + uc.extraInfo + uc.validEye;
        if (date.isEmpty() || (date < uc.date)) {
            date = uc.date;
            time = uc.hour;
        }
        else if ((date == uc.date) && (time < uc.hour)){
            time = uc.hour;
        }
        ans << uc.fileName << bc.fileName;
    }

    if (repgen.fieldingFileIndex != -1){
        DatInfo fielding_file = getFieldingInformation(filesFielding.at(repgen.fieldingFileIndex));
        if (expectedReportName.isEmpty()) expectedReportName = FILE_REPORT_NAME;
        // Extra info and date must have matched for these two files to have been selected.
        expectedReportName = expectedReportName + "_N" + fielding_file.extraInfo + fielding_file.validEye;
        ans << fielding_file.fileName;
        if (date.isEmpty() || (date < fielding_file.date)) {
            date = fielding_file.date;
            time = fielding_file.hour;
        }
        else if ((date == fielding_file.date) && (time < fielding_file.hour)){
            time = fielding_file.hour;
        }
    }

    if (!expectedReportName.isEmpty()){
        expectedReportName = expectedReportName + "_" + date + "_" + time + ".rep";
        ans.prepend(expectedReportName);
    }

    return ans;
}

QString DatFileInfoInDir::getDatFileNameFromSelectionDialogIndex(qint32 index, qint32 whichList) const{
    switch(whichList){
    case LIST_INDEX_READING:
        return filesReading.at(index);
    case LIST_INDEX_BINDING_BC:
        return filesBindingBC.at(index);
    case LIST_INDEX_BINDING_UC:
        return filesBindingUC.at(filesBindingUCValidIndexes.at(index));
    case LIST_INDEX_FIELDING:
        return filesFielding.at(index);
    }
    return "";
}

void DatFileInfoInDir::insertIntoListAccordingToOrder(const QString &fileName, QStringList *list, QStringList *order){
    DatInfo info = getDatFileInformation(fileName);
    bool inserted = false;
    for (qint32 i = 0; i < order->size(); i++){
        if (order->at(i) < info.orderString){
            inserted = true;
            order->insert(i,info.orderString);
            list->insert(i,fileName);
            break;
        }
    }
    if (!inserted){
        order->append(info.orderString);
        list->append(fileName);
    }
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
    else if (file.startsWith(FILE_OUTPUT_FIELDING)) return getFieldingInformation(file);
    else return DatInfo();
}

DatFileInfoInDir::DatInfo DatFileInfoInDir::getBindingFileInformation(const QString &bindingFile){

    QStringList parts = bindingFile.split(".",QString::SkipEmptyParts);

    QString codePrefix;
    if (bindingFile.contains("bc")) codePrefix = "BC";
    else codePrefix = "UC";    

    QString baseName = parts.first();
    QString fmark = "";
    if (parts.last() == "datf") fmark = " (FE)";

    parts = baseName.split("_",QString::SkipEmptyParts);
    DatInfo info;
    info.category = codePrefix;
    info.fileName = bindingFile;
    if (parts.size() == 5){
        // This is an old file so it only contains a date as timestamp.
        // Target size was large and number of targets were two.
        info.date = parts.at(2) + "_" + parts.at(3) + "_" + parts.at(4);
        info.extraInfo = "2l";
        info.validEye = QString::number(EYE_BOTH);
        info.hour = "00_00";
        info.basename = parts.at(0) + "_" + parts.at(1);
        info.code = codePrefix + info.extraInfo + info.validEye + " - " + parts.at(4) + "/" + parts.at(3) + "/" + parts.at(2) + fmark;
        info.orderString =  parts.at(2) + parts.at(3) + parts.at(4) + "0000";
    }
    else if (parts.size() == 7){
        // File name before time stamp included hours and minutes
        info.extraInfo = parts.at(2) + parts.at(3);
        info.date = parts.at(4) + "_" + parts.at(5) + "_" + parts.at(6);
        info.hour = "00_00";
        info.validEye = QString::number(EYE_BOTH);
        info.basename = parts.at(0) + "_" + parts.at(1);
        info.code = codePrefix + info.extraInfo + info.validEye + " - " + parts.at(6) + "/" + parts.at(5) + "/" + parts.at(4) + fmark;
        info.orderString =  parts.at(4) + parts.at(5) + parts.at(6) + "0000";
    }
    else if (parts.size() == 10){
        // Full file name with time stamp including hours and minutes.
        info.extraInfo = parts.at(2) + parts.at(3);
        info.date = parts.at(5) + "_" + parts.at(6) + "_" + parts.at(7);
        info.hour = parts.at(8) + "_" +  parts.at(9);
        info.basename = parts.at(0) + "_" + parts.at(1);
        info.validEye = parts.at(4);
        info.code = codePrefix + info.extraInfo + info.validEye + " - " + parts.at(7) + "/" + parts.at(6) + "/" + parts.at(5) + " " + parts.at(8) + ":" + parts.at(9) + fmark;
        info.orderString = parts.at(5) + parts.at(6) + parts.at(7) + parts.at(8) + parts.at(9);
    }
    info.category = info.category + info.extraInfo;

    return info;
}

DatFileInfoInDir::DatInfo DatFileInfoInDir::getReadingInformation(const QString &readingFile){

    QStringList parts = readingFile.split(".",QString::SkipEmptyParts);
    QString baseName = parts.first();

    QString fmark = "";
    if (parts.last() == "datf") fmark = " (FE)";

    parts = baseName.split("_",QString::SkipEmptyParts);
    DatInfo ans;
    ans.extraInfo = "";
    ans.fileName = readingFile;
    ans.category = "RD";

    if (parts.size() == 4){
        // This is an old file so it only contains a date as timestamp.
        ans.date = parts.at(1) + "_" + parts.at(2) + "_" + parts.at(3);
        ans.hour = "00_00";
        ans.validEye = QString::number(EYE_BOTH);
        ans.basename = parts.at(0);
        ans.code = "R" + ans.validEye + " - " + parts.at(3) + "/" + parts.at(2) + "/" + parts.at(1) + fmark;
        ans.orderString = parts.at(1) +  parts.at(2) + parts.at(3) + "00_00";
    }
    else if (parts.size() == 7){
        // File name includes time stamp included hours and minutes
        ans.date = parts.at(2) + "_" + parts.at(3) + "_" + parts.at(4);
        ans.validEye = parts.at(1);
        ans.hour = parts.at(5) + "_" + parts.at(6);
        ans.basename = parts.at(0);
        ans.code = "R" + ans.validEye + " - " + parts.at(4) + "/" + parts.at(3) + "/" + parts.at(2) + " " + parts.at(5) + ":" + parts.at(6) + fmark;
        ans.orderString = parts.at(2) + parts.at(3) + parts.at(4) + parts.at(5) + parts.at(6);
    }
    else if (parts.size() == 8){
        // File name includes time stamp included hours and minutes and language.
        ans.date = parts.at(3) + "_" + parts.at(4) + "_" + parts.at(5);
        ans.validEye = parts.at(2);
        ans.extraInfo = parts.at(1).toUpper();
        ans.hour = parts.at(6) + "_" + parts.at(7);
        ans.basename = parts.at(0);
        ans.code = "R" + ans.extraInfo +  ans.validEye + " - " + parts.at(5) + "/" + parts.at(4) + "/" + parts.at(3) + " " + parts.at(6) + ":" + parts.at(7) + fmark;
        ans.orderString = parts.at(3) + parts.at(4) + parts.at(5) + parts.at(5) + parts.at(6);
    }
    ans.category = ans.category + ans.extraInfo;

    return ans;
}

DatFileInfoInDir::DatInfo DatFileInfoInDir::getFieldingInformation(const QString &fieldingFile){

    QStringList parts = fieldingFile.split(".",QString::SkipEmptyParts);
    QString baseName = parts.first();

    QString fmark = "";
    if (parts.last() == "datf") fmark = " (FE)";

    parts = baseName.split("_",QString::SkipEmptyParts);
    DatInfo ans;
    ans.extraInfo = "";
    ans.fileName = fieldingFile;
    ans.category = "NB";

    if (parts.size() == 7){
        ans.date = parts.at(2) + "_" + parts.at(3) + "_" + parts.at(4);
        ans.hour = parts.at(5) + "_" + parts.at(6);
        ans.validEye = parts.at(1);
        ans.basename = parts.at(0);
        ans.code = "NB" + ans.validEye + " - " + parts.at(4) + "/" + parts.at(3) + "/" + parts.at(2) + fmark;
        ans.orderString = parts.at(2) + parts.at(3) + parts.at(4) + parts.at(5) + parts.at(6);
    }
    ans.category = ans.category + ans.extraInfo;

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


