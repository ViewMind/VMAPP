#include "subjectdirscanner.h"

const char * SubjectDirScanner::STATUS = "status";
const char * SubjectDirScanner::CODE = "code";
const char * SubjectDirScanner::DATE = "date";
const char * SubjectDirScanner::TYPE = "type";
const char * SubjectDirScanner::DISPLAY_ID = "display_id";
const char * SubjectDirScanner::SUBJECT_INSTITUTION_ID = "subject_insitution_id";
const char * SubjectDirScanner::SUBJECT_NAME = "subject_name";
const char * SubjectDirScanner::EVALUATOR_NAME = "evaluator_name";
const char * SubjectDirScanner::EVALUATOR_ID = "evaluator_id";
const char * SubjectDirScanner::MEDIC_NAME = "medic_name";
const char * SubjectDirScanner::MEDIC_ID = "medic_id";
const char * SubjectDirScanner::ORDER_CODE = "medic_id";


SubjectDirScanner::SubjectDirScanner()
{

}


QList<QVariantMap> SubjectDirScanner::scanSubjectDirectoryForEvalutionsFrom(const QString &directory, const QString &evalutor_username, QStringList *errorLst){

    QList<QVariantMap> ans;

    if (!QDir(directory).exists()){
        return ans;
    }

    // Searching for all JSON files.
    QStringList nameFilters; nameFilters << "*.json";
    QStringList json_files = QDir(directory).entryList(nameFilters,QDir::Files);
    if (json_files.empty()) return ans;

    for (qint32 i = 0; i < json_files.size(); i++){

        QString filename = directory + "/" + json_files.at(i);
        RawDataContainer rdc;
        QVariantMap map;

        // We try to laod the JSON File and if it fails, the error field is all that is added to the map.
        if (!rdc.loadFromJSONFile(filename)){
            *errorLst << "Error looding file " + filename + ": " + rdc.getError();
            continue;
        }

        // Getting the evaluator.
        QVariantMap evaluator = rdc.getApplicationUserData(RDC::AppUserType::EVALUATOR);
        if (evaluator.isEmpty()){
            *errorLst << "Error on " + filename  + " getting evaluator data " + rdc.getError();
            continue;
        }

        // We only add those that are from a given evaluator. (or if they have errors so that we can see them).
        if (evaluator.value(RDC::AppUserField::LOCAL_ID).toString() != evalutor_username) continue;

        // Getting the studies.
        QStringList list = rdc.getStudies();
        // Each file should only contain 1 Study.
        if (list.size() != 1){
             *errorLst << "File does not contain 1 study but: " + QString::number(list.size());
             continue;
        }
        QString study = list.first();


        QString status = rdc.getStudyStatus(study);
        if (status == ""){
            *errorLst << "Error on " + filename  + " getting study status " + rdc.getError();
            continue;
        }

        QString study_code = rdc.getStudyCode(study);
        if (study_code == ""){
            *errorLst << "Error on " + filename  + " getting study code " + rdc.getError();
            continue;
        }

        list = rdc.getMetaDataDateTime();
        if (list.size() != 3){
            *errorLst << "Error on " + filename  + " getting study date time " + rdc.getError();
            continue;
        }

        QVariantMap subdata = rdc.getSubjectData();
        if (subdata.isEmpty()){
            *errorLst << "Error on " + filename  + " getting subject data " + rdc.getError();
            continue;
        }

        QVariantMap medic = rdc.getApplicationUserData(RDC::AppUserType::MEDIC);
        if (medic.isEmpty()){
            *errorLst << "Error on " + filename  + " getting medic data " + rdc.getError();
            continue;
        }

        // Required for computing the order code: A string that allows chronological ordering.
        QDateTime dt = QDateTime::fromString(list.first(),"dd/MM/yyyy HH:mm");


        // We've loaded the JSON File, now we try to get the raw data.
        map[STATUS] = status;
        map[CODE]  = study_code;
        map[DATE] = list.first();
        map[TYPE] = study;
        map[DISPLAY_ID] = subdata.value(RDC::SubjectField::LOCAL_ID).toString();
        map[SUBJECT_INSTITUTION_ID] = subdata.value(RDC::SubjectField::INSTITUTION_PROVIDED_ID).toString();
        map[SUBJECT_NAME] = subdata.value(RDC::SubjectField::NAME).toString() + " " + subdata.value(RDC::SubjectField::LASTNAME).toString();
        map[EVALUATOR_NAME] = evaluator.value(RDC::SubjectField::NAME).toString() + " " + evaluator.value(RDC::AppUserField::LASTNAME).toString();
        map[EVALUATOR_ID] = evaluator.value(RDC::AppUserField::LOCAL_ID).toString();
        map[MEDIC_NAME] = medic.value(RDC::AppUserField::NAME).toString() + " " + medic.value(RDC::AppUserField::LASTNAME).toString();
        map[MEDIC_ID] = medic.value(RDC::AppUserField::VIEWMIND_ID).toString();
        map[ORDER_CODE] = dt.toString("yyyyMMddHHmm");
        ans << map;
    }

    return ans;

}



void SubjectDirScanner::sortSubjectDataListByOrder(QList<QVariantMap> *list){
    bool swapDone = true;
    while (swapDone){
        swapDone = false;
        for (qint32 i = 0; i < list->size()-1; i++){
            if (list->at(i).value(ORDER_CODE).toString() > list->at(i+1).value(ORDER_CODE).toString()){
                QVariantMap map = list->at(i);
                list->replace(i,list->at(i+1));
                list->replace(i+1,map);
                swapDone = true;
            }
        }
    }
}

//void SubjectDirScanner::insertByOrderCode(QList<QVariantMap> *list, const QVariatMap &map){

//    // If no order code was generated, it's just appended at the end.
//    // Same if the list is just empty.
//    QString orderCode = map.value(ORDER_CODE,"").toString();
//    if ((orderCode == "") || (list->empty())){
//        list->append(map);
//        return;
//    }

//    bool wasInserted = false;

//    for (qint32 i = 0; i < list.size(); i++){
//        QString ref = list->at(i).value(ORDER_CODE,"").toString();
//        bool insert = false;
//        if (ref == "") || ()
//        if ( > )
//    }

//    if (!wasInserted){
//        list->append(map);
//    }

//}
