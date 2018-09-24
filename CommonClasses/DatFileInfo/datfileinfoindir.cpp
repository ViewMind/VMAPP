#include "datfileinfoindir.h"

DatFileInfoInDir::DatFileInfoInDir(){

}


void DatFileInfoInDir::setDatDirectory(const QString &dir)
{
    filesByDate.clear();

    // STEP 1: Creating the structure with just the .dat information.
    QStringList filters;
    filters << "*.dat";
    QStringList fileList = QDir(dir).entryList(filters,QDir::Files);

    for (qint32 i = 0; i < fileList.size(); i++){

        QString fname = fileList.at(i);
        DatInfo datInfo;
        if (fname.startsWith("binding")){
            datInfo = getBindingFileInformation(fileList.at(i));
        }
        else if (fname.startsWith("reading")){
            datInfo = getReadingInformation(fileList.at(i));
        }
        // Non binding or reading files are ignored.
        else continue;

        DatInfoAndRep infoAndRep;
        DatInfoHash hash;

        if (filesByDate.contains(datInfo.date)) {
            infoAndRep = filesByDate.value(datInfo.date);
            hash = infoAndRep.datInfo;
        }
        QList<DatInfo> infoList;
        if (hash.contains(datInfo.basename)) infoList = hash.value(datInfo.basename);
        insertDatIntoInfoList(&infoList,datInfo);
        hash[datInfo.basename] = infoList;
        infoAndRep.datInfo = hash;
        filesByDate[datInfo.date] = infoAndRep;

    }

    // STEP 2: Adding the existing report information and gathering the set of files that need to be processed.
    filters.clear();
    filters << "*.rep";
    fileList = QDir(dir).entryList(filters,QDir::Files);
    QSet<QString> existing = fileList.toSet();
    QStringList dates = filesByDate.keys();

    for (qint32 i = 0; i < dates.size(); i++){
        //qWarning() << "Doing" << dates.at(i) << "Existing files are" << existing;
        setExpectedReportFileSet(dates.at(i),existing);
    }
    //qWarning() << "DONE WITH SET EXPECTED REPORT FILE";

}

void DatFileInfoInDir::printData(){

    QStringList keys = filesByDate.keys();
    for (qint32 i = 0; i < keys.size(); i++){

        qWarning() << "DATE: " << keys.at(i);
        DatInfoAndRep datInfoAndRep = filesByDate.value(keys.at(i));
        qWarning() << "   REP FILES STRUCT: ";
        QStringList expectedReps = datInfoAndRep.reportFileSet.keys();
        for (qint32 j = 0; j < expectedReps.size(); j++){
            qWarning() << "   " <<expectedReps.at(j) << "-->" <<  datInfoAndRep.reportFileSet.value(expectedReps.at(j));
        }
        qWarning() << "   FILES BY TYPE: ";
        DatInfoHash hash = datInfoAndRep.datInfo;
        QStringList ftypes = hash.keys();

        for (qint32 j = 0; j < ftypes.size(); j++){
            qWarning() << "      " << ftypes.at(j);
            QList<DatInfo> list = hash.value(ftypes.at(j));
            for (qint32 k = 0; k < list.size(); k++){
                qWarning() << "         " << list.at(k).toString();
            }
        }

    }

}

void DatFileInfoInDir::insertDatIntoInfoList(QList<DatInfo> *list, const DatInfo &info){
    bool inserted = false;
    for (qint32 i = 0; i < list->size(); i++){
        if (list->at(i).hour > info.hour){
            inserted = true;
            list->insert(i,info);
            break;
        }
    }
    if (!inserted) list->append(info);
}


bool DatFileInfoInDir::hasPendingReports() const {
    QStringList dates = filesByDate.keys();
    for (qint32 i = 0; i < dates.size(); i++){
        if (!filesByDate.value(dates.at(i)).reportFileSet.isEmpty()) return true;
    }
    return false;
}

void DatFileInfoInDir::prepareToInterateOverPendingReportFileSets(){
    currentFileSet = 0;
    fileSets.clear();
    QStringList dates = filesByDate.keys();
    for (qint32 i = 0; i < dates.size(); i++){
        QHash<QString, QStringList> hash = filesByDate.value(dates.at(i)).reportFileSet;
        QStringList repFiles = hash.keys();
        for (qint32 j = 0; j < repFiles.size(); j++){
            fileSets << hash.value(repFiles.at(j));
        }
    }
}

QStringList DatFileInfoInDir::nextPendingReportFileSet(){
    QStringList ans;
    if (currentFileSet < fileSets.size()){
        ans = fileSets.at(currentFileSet);
        currentFileSet++;
    }
    return ans;
}

DatFileInfoInDir::DatInfo DatFileInfoInDir::getDatFileInformation(const QString &file){
    if (file.startsWith(FILE_OUTPUT_BINDING_BC)) return getBindingFileInformation(file);
    else if (file.startsWith(FILE_OUTPUT_BINDING_UC)) return getBindingFileInformation(file);
    else if (file.startsWith(FILE_OUTPUT_READING)) return getReadingInformation(file);
    else return DatInfo();
}

DatFileInfoInDir::DatInfo DatFileInfoInDir::getBindingFileInformation(const QString &bindingFile){

    QStringList parts = bindingFile.split(".",QString::SkipEmptyParts);
    QString baseName = parts.first();
    parts = baseName.split("_",QString::SkipEmptyParts);
    DatInfo info;
    info.fileName = bindingFile;
    if (parts.size() == 5){
        // This is an old file so it only contains a date as timestamp.
        // Target size was large and number of targets were two.
        info.date = parts.at(2) + "_" + parts.at(3) + "_" + parts.at(4);
        info.extraInfo = "2l";
        info.validEye = QString::number(EYE_BOTH);;
        info.hour = 0;
        info.basename = parts.at(0) + "_" + parts.at(1);
    }
    else if (parts.size() == 7){
        // File name before time stamp included hours and minutes
        info.extraInfo = parts.at(2) + parts.at(3);
        info.date = parts.at(4) + "_" + parts.at(5) + "_" + parts.at(6);
        info.hour = 0;
        info.validEye = QString::number(EYE_BOTH);
        info.basename = parts.at(0) + "_" + parts.at(1);
    }
    else if (parts.size() == 10){
        // Full file name with time stamp including hours and minutes.
        info.extraInfo = parts.at(2) + parts.at(3);
        info.date = parts.at(5) + "_" + parts.at(6) + "_" + parts.at(7);
        info.hour = QString(parts.at(8) + parts.at(9)).toInt();
        info.basename = parts.at(0) + "_" + parts.at(1);
        info.validEye = parts.at(4);
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
        ans.hour = 0;
        ans.validEye = QString::number(EYE_BOTH);
        ans.basename = parts.at(0);
    }
    else if (parts.size() == 7){
        // File name before time stamp included hours and minutes
        ans.date = parts.at(2) + "_" + parts.at(3) + "_" + parts.at(4);
        ans.validEye = parts.at(1);
        ans.hour = QString(parts.at(5) + parts.at(6)).toInt();
        ans.basename = parts.at(0);
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

DatFileInfoInDir::DatInfo DatFileInfoInDir::getRerportInformation(const QString &repfile){

    QStringList parts = repfile.split(".",QString::SkipEmptyParts);
    QString baseName = parts.first();
    parts = baseName.split("_",QString::SkipEmptyParts);
    DatInfo ans;
    ans.extraInfo = "";
    ans.fileName = repfile;
    if (parts.size() >= 4){

        // The last 3 parts are the date. While the first is the base name, which should alwasy be report.
        ans.basename = parts.at(0);
        qint32 last = parts.size()-1;
        ans.date = parts.at(last-2) + "_" + parts.at(last-1) + "_" + parts.at(last);
        ans.hour = 0;

        if (parts.size() > 4) {
            // There is extra information regarding what is in the report.
            QStringList extraInfo;
            for (qint32 i = 1; i < last-2; i++){
                extraInfo << parts.at(i);
            }
            if (extraInfo.size() > 1) ans.extraInfo = extraInfo.join("_");
            else ans.extraInfo = extraInfo.first();
        }

    }
    return ans;
}

void DatFileInfoInDir::setExpectedReportFileSet(const QString &date, const QSet<QString> existingReports){

    QStringList binding;
    DatInfoAndRep infoAndRep = filesByDate.value(date);
    DatInfoHash hash = infoAndRep.datInfo;

    QString newestReadingFile;

    // For each type of binding (as defined by extra info) the pair of uc and bc binding files are saved.
    QHash<QString,QStringList> newestBinding;


    bool hasReading = hash.contains(FILE_OUTPUT_READING);
    if (hasReading){
        newestReadingFile = hash.value(FILE_OUTPUT_READING).last().fileName;
    }

    if (hash.contains(FILE_OUTPUT_BINDING_BC) && hash.contains(FILE_OUTPUT_BINDING_UC)){
        QSet<QString> bcTypes, ucTypes;

        QList<DatInfo> datInfoList = hash.value(FILE_OUTPUT_BINDING_BC);
        for (qint32 i = 0; i < datInfoList.size(); i++) bcTypes << datInfoList.at(i).extraInfo;

        datInfoList = hash.value(FILE_OUTPUT_BINDING_UC);
        for (qint32 i = 0; i < datInfoList.size(); i++) ucTypes << datInfoList.at(i).extraInfo;

        binding = bcTypes.intersect(ucTypes).toList();

        //qWarning() << "COMMON BINDING TYPES" << binding;

        // Getting the newest file for each of the binding files for each fo the binding study types.
        for (qint32 i = 0; i < binding.size(); i++){
            datInfoList = hash.value(FILE_OUTPUT_BINDING_BC);
            QStringList pair;
            for (qint32 j = datInfoList.size()-1; j >= 0; j--){
                if (datInfoList.value(j).extraInfo == binding.at(i)){
                    pair << datInfoList.value(j).fileName;
                    break;
                }
            }

            datInfoList = hash.value(FILE_OUTPUT_BINDING_UC);
            for (qint32 j = datInfoList.size()-1; j >= 0; j--){
                if (datInfoList.value(j).extraInfo == binding.at(i)){
                    pair << datInfoList.value(j).fileName;
                    break;
                }
            }
            //qWarning() << "SAVING BINDING PAIR" << pair;
            newestBinding[binding.at(i)] = pair;
        }

    }

    QString expectedRep;
    if (binding.isEmpty() && hasReading){
        expectedRep = QString(FILE_REPORT_NAME) + "_r_" + date + ".rep";
        QStringList filesForReport;
        filesForReport << newestReadingFile;
        infoAndRep.reportFileSet[expectedRep] = filesForReport;
        filesByDate[date] = infoAndRep;
        return;
    }

    for (qint32 i = 0; i < binding.size(); i++){

        if (hasReading) expectedRep = QString(FILE_REPORT_NAME) + "_r_b" + binding.at(i) + "_" + date + ".rep";
        else expectedRep = QString(FILE_REPORT_NAME) + "_b" + binding.at(i) + "_" + date + ".rep";

        QStringList filesForReport;
        // If the existing reports do not contain the expected repor then the file set is added.
        if (!existingReports.contains(expectedRep)){
            if (hasReading) filesForReport << newestReadingFile;
            filesForReport << newestBinding.value(binding.at(i));
            infoAndRep.reportFileSet[expectedRep] = filesForReport;
            filesByDate[date] = infoAndRep;
        }
    }

}
