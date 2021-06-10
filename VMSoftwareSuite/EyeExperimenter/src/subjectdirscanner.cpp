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
    workDirectory = "";
    loggedInUser = "";
}

void SubjectDirScanner::setup(const QString &workdir, const QString &loggedUser){
    workDirectory = workdir;
    loggedInUser = loggedUser;
}

QString SubjectDirScanner::getError() const {
    return error;
}

///////////////////////////////////// SCANNING FOR EVALUATIONS //////////////////////////////////////////////
QList<QVariantMap> SubjectDirScanner::scanSubjectDirectoryForEvalutionsFrom() {
    QList<QVariantMap> ans;

    if (!QDir(workDirectory).exists()){
        error = "Setup directory: " + workDirectory + "doesn't exist";
        return ans;
    }

    // Searching for all JSON files.
    QStringList nameFilters; nameFilters << "*.json";
    QStringList json_files = QDir(workDirectory).entryList(nameFilters,QDir::Files);
    if (json_files.empty()) return ans;

    for (qint32 i = 0; i < json_files.size(); i++){

        QString filename = workDirectory + "/" + json_files.at(i);
        RawDataContainer rdc;
        QVariantMap map;

        // We try to laod the JSON File and if it fails, the error field is all that is added to the map.
        if (!rdc.loadFromJSONFile(filename)){
            error =  "Error looding file " + filename + ": " + rdc.getError();
            return ans;
        }

        // Getting the evaluator.
        QVariantMap evaluator = rdc.getApplicationUserData(RDC::AppUserType::EVALUATOR);
        if (evaluator.isEmpty()){
            error = "Error on " + filename  + " getting evaluator data " + rdc.getError();
            return ans;
        }

        // We only add those that are from a given evaluator. (or if they have errors so that we can see them).
        if (evaluator.value(RDC::AppUserField::LOCAL_ID).toString() != loggedInUser) continue;

        // Getting the studies.
        QStringList list = rdc.getStudies();
        // Each file should only contain 1 Study.
        if (list.size() != 1){
            error =  "File does not contain 1 study but: " + QString::number(list.size());
            return ans;
        }
        QString study = list.first();


        QString status = rdc.getMetadaStatus();
        if (status == ""){
            error = "Error on " + filename  + " getting study status " + rdc.getError();
            return ans;
        }

        QString study_code = rdc.getStudyCode(study);
        if (study_code == ""){
            error =  "Error on " + filename  + " getting study code " + rdc.getError();
            return ans;
        }

        list = rdc.getMetaDataDateTime();
        if (list.size() != 3){
            error =  "Error on " + filename  + " getting study date time " + rdc.getError();
            return ans;
        }

        QVariantMap subdata = rdc.getSubjectData();
        if (subdata.isEmpty()){
            error =  "Error on " + filename  + " getting subject data " + rdc.getError();
            return ans;
        }

        QVariantMap medic = rdc.getApplicationUserData(RDC::AppUserType::MEDIC);
        if (medic.isEmpty()){
            error =  "Error on " + filename  + " getting medic data " + rdc.getError();
            return ans;
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


///////////////////////////////////// FINDING NECESSARY EXISTING FILES //////////////////////////////////////////////
QString SubjectDirScanner::findIncompleteBindingStudies(const QString &missing_study, const QVariantMap study_configuration){

    QStringList filters; filters << "*.json";
    QStringList filelist = QDir(workDirectory).entryList(filters,QDir::Files,QDir::Name);  // Since the name contains the timestamp the newer files will be listed first.


    QString study_present;
    if (missing_study == RDC::Study::BINDING_BC){
        study_present = RDC::Study::BINDING_UC;
    }
    else if (missing_study == RDC::Study::BINDING_UC){
        study_present = RDC::Study::BINDING_BC;
    }
    else{
        error = "Finding incomplet Binding files: Invalid study to be missing " + missing_study;
        return "";
    }

    for (qint32 i = 0; i < filelist.size(); i++){

        // We only need to check binding files.
        if (filelist.at(i).contains(Globals::BaseFileNames::BINDING)){

            RawDataContainer rdc;
            if (!rdc.loadFromJSONFile(workDirectory + "/" + filelist.at(i))){
                error = "Error loading JSON File for ongoing analysis: " + rdc.getError();
                return "";
            }

            QStringList study_list = rdc.getStudies();

            // We can assume that, given the file name, the only study is a binding study.
            if (!study_list.contains(study_present)){
                continue;
            }

            if (study_list.contains(missing_study)){
                // This file contains both studies so we can move on.
                continue;
            }

            // The study has what we need. Next we check that the evaluator is the same. A study cannot be continued by a different evaluator as it is signed.
            if (rdc.getApplicationUserData(RDC::AppUserType::EVALUATOR).value(RDC::AppUserField::EMAIL).toString() != loggedInUser){
                //qDebug() << "Moving on due to being different evaluators";
                continue;
            }

            // Same evasluator. Next we check that less than 24 hours have passed in order to make it viable.
            QStringList study_datetime = rdc.getMetaDataDateTime();
            QDateTime dt = QDateTime::fromString(study_datetime.first() + " " + study_datetime.last(), "yyyy-MM-dd HH:mm");

            // And now we check that the study configuration is the same.
            QVariantMap sc = rdc.getStudyConfiguration(study_present);
            if (sc.isEmpty()){
                error = "While getting study configuration for binding matchin in file " + workDirectory + "/" + filelist.at(i) + ". Got: "  + rdc.getError();
                return "";
            }
            bool aresame = true;
//            qDebug() << "Comparing loaded value with passed parameter";
//            Debug::prettpPrintQVariantMap(sc);
//            Debug::prettpPrintQVariantMap(study_configuration);
            aresame = aresame && (sc.value(RDC::StudyParameter::VALID_EYE).toString() == study_configuration.value(RDC::StudyParameter::VALID_EYE).toString());
            aresame = aresame && (sc.value(RDC::StudyParameter::NUMBER_TARGETS).toString() == study_configuration.value(RDC::StudyParameter::NUMBER_TARGETS).toString());
            aresame = aresame && (sc.value(RDC::StudyParameter::TARGET_SIZE).toString() == study_configuration.value(RDC::StudyParameter::TARGET_SIZE).toString());

//            qDebug() << "Are the same" << aresame;

            // They were configured differently.
            if (!aresame) {
                //qDebug() << "Moving on due to study configurations not being the same";
                continue;
            }

            if (dt.secsTo(QDateTime::currentDateTime()) < Globals::NUMBER_SECONDS_IN_A_DAY){
                // So this is a binding study file of the right type, with the same configuration, by the same evaluator withouth the missing study and less than a day old from present time.
                // This is the file we need.
                return filelist.at(i);
            }
        }
    }
    // If we got here, no file was found.
    return "";
}


QString SubjectDirScanner::findIncompletedPerceptionStudy(const qint32 missing_part, const QVariantMap &study_configuration){

    QStringList filters; filters << "*.json";
    QStringList filelist = QDir(workDirectory).entryList(filters,QDir::Files,QDir::Name);

    QStringList studies_that_must_be_contained;
    for (qint32 i = 1; i < missing_part; i++){
        studies_that_must_be_contained << RDC::MultiPartStudyBaseName::PERCEPTION + " " + QString::number(i);
    }

    QString next_part =  RDC::MultiPartStudyBaseName::PERCEPTION + " " + QString::number(missing_part);

    for (qint32 i = 0; i < filelist.size(); i++){

        if (filelist.at(i).contains(Globals::BaseFileNames::PERCEPTION)){

            RawDataContainer rdc;
            if (!rdc.loadFromJSONFile(workDirectory + "/" + filelist.at(i))){
                error = "Error loading JSON File for ongoing analysis: " + rdc.getError();
                return "";
            }

            // File is old and we can ignore.
            QStringList study_datetime = rdc.getMetaDataDateTime();
            QDateTime dt = QDateTime::fromString(study_datetime.first() + " " + study_datetime.last(), "yyyy-MM-dd HH:mm");
            if (dt.secsTo(QDateTime::currentDateTime()) > Globals::NUMBER_SECONDS_IN_A_DAY) continue;

            // The study has what we need. Next we check that the evaluator is the same. A study cannot be continued by a different evaluator as it is signed.
            if (rdc.getApplicationUserData(RDC::AppUserType::EVALUATOR).value(RDC::AppUserField::EMAIL).toString() != loggedInUser){
                //qDebug() << "Moving on due to being different evaluators";
                continue;
            }

            QStringList studies = rdc.getStudies();

            if (studies.contains(next_part)){
                // Completed study.
                continue;
            }

            // We check that these are all perception studies and that they are all of the same type.
            bool aresame = true;
            for (qint32 j = 0; j < studies.size(); j++){
                if (!studies.contains(RDC::MultiPartStudyBaseName::PERCEPTION)){
                    error = "Found perceptio file with a wrong study name: " + studies.at(j);
                    return "";
                }
                QVariantMap sc = rdc.getStudyConfiguration(studies_that_must_be_contained.at(j));
                aresame = aresame && (sc.value(RDC::StudyParameter::VALID_EYE) == study_configuration.value(RDC::StudyParameter::VALID_EYE));
                aresame = aresame && (sc.value(RDC::StudyParameter::PERCEPTION_TYPE) == study_configuration.value(RDC::StudyParameter::PERCEPTION_TYPE));
                if (!aresame) break;
            }

            // Finally we check for any missing parts:
            for (qint32 j = 0; j < studies_that_must_be_contained.size(); j++){
                // Verify all previous studies are there.
                if (!studies.contains(studies_that_must_be_contained.at(j))){
                    error = "Attempting to start perception study part " + QString::number(missing_part) + " but found that file "
                            + workDirectory + "/" + filelist.at(i) + " are missing: " + studies_that_must_be_contained.at(j);
                    return "";
                }
            }

            // So if we got here, we have a perception study that does not missing_part, but has all previous ones and it has the same configuration and is from the same evalutor
            // It is also less than a day old.
            return filelist.at(i);

        }
    }
    return "";
}

///////////////////////////////////// STATIC SORTING FUNCTION //////////////////////////////////////////////

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
