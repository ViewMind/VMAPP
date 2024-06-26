#include "viewminddatacontainer.h"

ViewMindDataContainer::ViewMindDataContainer()
{
    error = "";
}

bool ViewMindDataContainer::saveJSONFile(const QString &file_name){

    // Compute the hash every time before saving
    setChecksumHash();

    QJsonDocument json = QJsonDocument::fromVariant(data);

    QFile file(file_name);
    if (!file.open(QFile::WriteOnly)){
        error = "Could not open " + file_name + " for writing";
        return false;
    }

    QTextStream writer(&file);

    bool pretty_print = false;
    if (DBUGBOOL(Debug::Options::PRETTY_PRINT_STUDIES)){
        pretty_print = true;
    }

    if (pretty_print) writer << json.toJson(QJsonDocument::Indented);
    else writer << json.toJson(QJsonDocument::Compact);
    file.close();

    return true;
}

void ViewMindDataContainer::clearAllData(){
    this->data.clear();
}

bool ViewMindDataContainer::loadFromJSONFile(const QString &file_name){

    if (!QFile(file_name).exists()){
        error = "Trying to load " + file_name + " that doesn't exist";
        return false;
    }

    QJsonParseError json_error;

    QFile file(file_name);
    if (!file.open(QFile::ReadOnly)){
        error = "Could not open file " + file_name + " for reading";
        return false;
    }
    QString val = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8(),&json_error);
    if (doc.isNull()){
        error = "Error loading raw data file: " + json_error.errorString();
        return false;
    }
    data = doc.object().toVariantMap();

    //Debug::prettpPrintQVariantMap(data);

    return true;
}

QString ViewMindDataContainer::getError() const{
    return error;
}

///////////////////////////////////////////////// CHECKSUM RELATED FUNCTIONS /////////////////////////////////////////////////////////////////////////
void ViewMindDataContainer::setChecksumHash(){
    data[MAIN_FIELD_HASH] = computeCheckSumHash();
}

bool ViewMindDataContainer::verifyChecksumHash(){
    QString previous_hash = data.value(MAIN_FIELD_HASH).toString();
    QString hash = computeCheckSumHash();
    data[MAIN_FIELD_HASH] = previous_hash;
    //qDebug() << "HASH ON DATA"  << previous_hash;
    //qDebug() << "HASH COMPUTED" << hash;
    return (hash == previous_hash);
}

QString ViewMindDataContainer::computeCheckSumHash(){
    data[MAIN_FIELD_HASH] = "";
    QJsonDocument json = QJsonDocument::fromVariant(data);
    QString hash;
    hash = QString(QCryptographicHash::hash(json.toJson(QJsonDocument::Compact),QCryptographicHash::Sha3_512).toHex());
    return hash;
}

///////////////////////////////////////////////// READING DATA FUNCTIONS /////////////////////////////////////////////////////////////////////////

QStringList ViewMindDataContainer::getStudies() {
    QStringList ans;
    QStringList hierarchy; hierarchy << MAIN_FIELD_STUDIES;
    if (!checkHiearchyChain(hierarchy)) return ans;
    ans = data.value(MAIN_FIELD_STUDIES).toMap().keys();
    return ans;
}

QString ViewMindDataContainer::getStudyStatus(const QString &study){
    QStringList hierarchy; hierarchy << MAIN_FIELD_STUDIES << study << VMDC::StudyField::STATUS;
    if (!checkHiearchyChain(hierarchy)) return "";
    return data.value(MAIN_FIELD_STUDIES).toMap().value(study).toMap().value(VMDC::StudyField::STATUS).toString();
}

QString ViewMindDataContainer::getEvaluationID(){
    QStringList hierarchy; hierarchy << MAIN_FIELD_METADATA << VMDC::MetadataField::EVALUATION_ID;
    if (!checkHiearchyChain(hierarchy)) return "";
    return data.value(MAIN_FIELD_METADATA).toMap().value(VMDC::MetadataField::EVALUATION_ID).toString();
}

QVariantMap ViewMindDataContainer::getStudyConfiguration(const QString study){
    QVariantMap sc;
    QString check = VMDC::Study::validate(study);

    if (check != ""){
        error = "Attempting to get study configuration: " + check;
        return sc;
    }
    QStringList hierarchy; hierarchy << MAIN_FIELD_STUDIES << study << VMDC::StudyField::STUDY_CONFIGURATION;
    if (!checkHiearchyChain(hierarchy)) return sc;
    return data.value(MAIN_FIELD_STUDIES).toMap().value(study).toMap().value(VMDC::StudyField::STUDY_CONFIGURATION).toMap();
}

QString ViewMindDataContainer::getStudyCode(const QString &study) {

    QString check = VMDC::Study::validate(study);

    if (check != ""){
        error = "Attempting to get study configuration: " + check;
        return "";
    }

    QStringList hierarchy; hierarchy << MAIN_FIELD_STUDIES << study;
    if (!checkHiearchyChain(hierarchy)) return "";

    QString code = data.value(MAIN_FIELD_STUDIES).toMap().value(study).toMap().value(VMDC::StudyField::ABBREVIATION).toString();
    code = code + data.value(MAIN_FIELD_STUDIES).toMap().value(study).toMap().value(VMDC::StudyField::CONFIG_CODE).toString();
    return code;

}

qreal ViewMindDataContainer::getStudyDuration(const QString &study){

    QString check = VMDC::Study::validate(study);

    if (check != ""){
        error = "Attempting to get study configuration: " + check;
        return -1;
    }

    QStringList hierarchy; hierarchy << MAIN_FIELD_STUDIES << study;
    if (!checkHiearchyChain(hierarchy)) return -1;

    return data.value(MAIN_FIELD_STUDIES).toMap().value(study).toMap().value(VMDC::StudyField::STUDY_DURATION).toReal();

}

qreal ViewMindDataContainer::getStudyPauseDuration(const QString &study){

    QString check = VMDC::Study::validate(study);

    if (check != ""){
        error = "Attempting to get study configuration: " + check;
        return -1;
    }

    QStringList hierarchy; hierarchy << MAIN_FIELD_STUDIES << study;
    if (!checkHiearchyChain(hierarchy)) return -1;

    QVariantList pauseDurations =  data.value(MAIN_FIELD_STUDIES).toMap().value(study).toMap().value(VMDC::StudyField::PAUSE_DURATION).toList();

    qreal total = 0;
    for (qint32 i = 0; i < pauseDurations.size(); i++){
        total = total + pauseDurations.at(i).toReal();
    }

    return total;

}

QStringList ViewMindDataContainer::getMetaDataDateTime(){
    QStringList hierarchy; hierarchy << MAIN_FIELD_METADATA;
    if (!checkHiearchyChain(hierarchy)) return QStringList();

    QString date = data.value(MAIN_FIELD_METADATA).toMap().value(VMDC::MetadataField::DATE).toString();
    QString hour = data.value(MAIN_FIELD_METADATA).toMap().value(VMDC::MetadataField::HOUR).toString();
    QDateTime dt = QDateTime::fromString(date + " " + hour,"yyyy-MM-dd HH:mm:ss");
    QStringList ans; ans << dt.toString("dd/MM/yyyy HH:mm");
    ans << date << hour << dt.toString("yyyyMMddHHmmss");
    return ans;
}

QString ViewMindDataContainer::getMetadataDiscardReason(){
    QStringList hierarchy; hierarchy << MAIN_FIELD_METADATA << VMDC::MetadataField::DISCARD_REASON;
    if (!checkHiearchyChain(hierarchy)) return "";
    return data.value(MAIN_FIELD_METADATA).toMap().value(VMDC::MetadataField::DISCARD_REASON).toString();
}

QVariantMap ViewMindDataContainer::getApplicationUserData(const QString &type) {

    QVariantMap ret;
    QString check = VMDC::AppUserType::validate(type);

    if (check != ""){
        error = "Attempting to get application user data: " + check;
        return ret;
    }

    QStringList hierarchy; hierarchy << MAIN_FIELD_APPLICATION_USER << type;
    if (!checkHiearchyChain(hierarchy)) return ret;
    return data.value(MAIN_FIELD_APPLICATION_USER).toMap().value(type).toMap();

}

QVariantMap ViewMindDataContainer::getSubjectData() {
    QStringList hierarchy; hierarchy << MAIN_FIELD_SUBJECT_DATA;
    if (!checkHiearchyChain(hierarchy)) return QVariantMap();
    return data.value(MAIN_FIELD_SUBJECT_DATA).toMap();
}

QVariantMap ViewMindDataContainer::getProcessingParameters() const {
    return data.value(MAIN_FIELD_PROCESSING_PARAMETERS).toMap();
}

QVariantMap ViewMindDataContainer::getQCParameters() const {
    return data.value(MAIN_FIELD_FREQUENCY_CHECK_PARAMETERS).toMap();
}

QString ViewMindDataContainer::getCurrentStudy() const {
    return currentlySelectedStudy;
}

void ViewMindDataContainer::DebugPrintContentToConsole() const{
    Debug::prettyPrintQVariantMap(data);
}

bool ViewMindDataContainer::isStudy3D(const QString &study) const {
    QStringList hierarchy; hierarchy << MAIN_FIELD_STUDIES << study;
    // We base the search on finding (or not) the trial_list field in the study itself.
    if (data.value(MAIN_FIELD_STUDIES).toMap().value(study).toMap().contains(VMDC::StudyField::TRIAL_LIST)) return false;
    else return true;
}


///////////////////////////////////////////////// SETTING DATA FUNCTIONS /////////////////////////////////////////////////////////////////////////

bool ViewMindDataContainer::setMetadata(const QVariantMap &metadata){
    QStringList mandatory;
    mandatory = VMDC::MetadataField::valid;

    // Version should not be a mandatory field as it is set here.
    mandatory.removeOne(VMDC::MetadataField::VERSION);

    QStringList keys = metadata.keys();
    for (qint32 i = 0; i < mandatory.size(); i++){
        if (!keys.contains(mandatory.at(i))){
            error = "Cannot set metadata withouth field: " + mandatory.at(i);
            return false;
        }
    }

    // In order to make sure no previously set data is lost.
    QVariantMap current_metada = data.value(MAIN_FIELD_METADATA).toMap();
    for (qint32 i = 0; i < mandatory.size(); i++){
        current_metada[mandatory.at(i)] = metadata.value(mandatory.at(i));
    }


    // Ensuring the proper version.
    current_metada[VMDC::MetadataField::VERSION] = CURRENT_JSON_STRUCT_VERSION;

    // Saving as metadata
    data[MAIN_FIELD_METADATA] = current_metada;
    return true;
}

void ViewMindDataContainer::setSystemSpecs(const QVariantMap &system_specs){
    // No checking is done for the system specs as they are varied.
    data[MAIN_FIELD_SYSTEM_SPECS] = system_specs;
}

void ViewMindDataContainer::addCustomMetadataFields(const QString field_name, const QString field_value){
    QVariantMap current_metada = data.value(MAIN_FIELD_METADATA).toMap();
    current_metada.insert(field_name,field_value);
    data[MAIN_FIELD_METADATA] = current_metada;
}

void ViewMindDataContainer::setCalibrationValidationData(const QVariantMap &calibrationValidationData){
    data[MAIN_FIELD_CALIBRATION] = calibrationValidationData;
}

bool ViewMindDataContainer::setProcessingParameters(const QVariantMap &pp){

    // All processing parameters will be considered mandatory.
    QStringList mandatory;
    mandatory << VMDC::ProcessingParameter::MAX_DISPERSION_WINDOW
              << VMDC::ProcessingParameter::SAMPLE_FREQUENCY
              << VMDC::ProcessingParameter::LATENCY_ESCAPE_RADIOUS
              << VMDC::ProcessingParameter::RESOLUTION_HEIGHT
              << VMDC::ProcessingParameter::RESOLUTION_WIDTH;

    QStringList keys = pp.keys();
    for (qint32 i = 0; i < mandatory.size(); i++){
        if (!keys.contains(mandatory.at(i))){
            error = "Cannot set processing parameters withouth field: " + mandatory.at(i);
            return false;
        }
    }

    // There could be keys that are not supposed to be here. We check.
    QString check = VMDC::ProcessingParameter::validateList(keys);
    if (check != "") {
        error = "While validating processing parameter list: " + check;
        return false;
    }

    QVariantMap ppmap;
    for (qint32 i = 0; i < keys.size(); i++){
        ppmap.insert(keys.at(i),pp.value(keys.at(i)));
    }

    data[MAIN_FIELD_PROCESSING_PARAMETERS] = ppmap;
    return true;
}


bool ViewMindDataContainer::setQCVector(const QString &studyName, const QString &qcfield, const QVariantList &vector){
    QString check = VMDC::QCFields::validate(qcfield);
    if (check != ""){
        error = "Setting QC Vector: " + check;
        return false;
    }
    QStringList hieararchy; hieararchy << MAIN_FIELD_STUDIES << studyName;
    if (!checkHiearchyChain(hieararchy)) return false;

    QVariantMap studies = data.value(MAIN_FIELD_STUDIES).toMap();
    QVariantMap study = studies.value(studyName).toMap();
    QVariantMap qc = study.value(VMDC::StudyField::QUALITY_CONTROL).toMap();
    qc[qcfield] = vector;
    study[VMDC::StudyField::QUALITY_CONTROL] = qc;
    studies[studyName] = study;
    data[MAIN_FIELD_STUDIES] = studies;
    return true;
}

bool ViewMindDataContainer::setQCValue(const QString &studyName, const QString &qcfield, const QVariant &value){
    QString check = VMDC::QCFields::validate(qcfield);
    if (check != ""){
        error = "Setting QC Vector: " + check;
        return false;
    }
    QStringList hieararchy; hieararchy << MAIN_FIELD_STUDIES << studyName;
    if (!checkHiearchyChain(hieararchy)) return false;

    QVariantMap studies = data.value(MAIN_FIELD_STUDIES).toMap();
    QVariantMap study = studies.value(studyName).toMap();
    QVariantMap qc = study.value(VMDC::StudyField::QUALITY_CONTROL).toMap();
    qc[qcfield] = value;
    study[VMDC::StudyField::QUALITY_CONTROL] = qc;
    studies[studyName] = study;
    data[MAIN_FIELD_STUDIES] = studies;
    return true;
}

void ViewMindDataContainer::setQCStudyStructClear() {

    QVariantMap studies = data.value(MAIN_FIELD_STUDIES).toMap();
    QStringList keys = studies.keys();
    for (qint32 i = 0; i < keys.size(); i++){
        QVariantMap studydata = studies.value(keys.at(i)).toMap();
        studydata[VMDC::StudyField::QUALITY_CONTROL] = QVariantMap();
        studies[keys.at(i)] = studydata;
    }
    data[MAIN_FIELD_STUDIES] = studies;

}

bool ViewMindDataContainer::setCurrentStudy(const QString &study){

    if (!data.value(MAIN_FIELD_STUDIES).toMap().contains(study)){
        error = study + " has not been configured for this container";
        return false;
    }

    //qDebug() << "Setting current study to " << study;

    currentlySelectedStudy = study;
    currentDataSetMap.clear();
    currentRawDataList.clear();
    currentLFixationVectorL.clear();
    currentLFixationVectorR.clear();
    currentTrial.clear();
    currentTrialList.clear();

    return true;

}

void ViewMindDataContainer::setExplanationPhaseStart(){
    studyDurationMeasure.start();
    explanationPhaseDuration = 0;
    examplePhaseDuration = 0;
}

void ViewMindDataContainer::studyPauseSet(bool start){
    if (start){
        pauseDurationMeasure.start();
    }
    else {
        pauseDuration << pauseDurationMeasure.elapsed();
    }
}

void ViewMindDataContainer::setExamplePhaseStart(){
    // When the example phase starts then the explanation phase has ended.
    explanationPhaseDuration = studyDurationMeasure.elapsed();
    studyDurationMeasure.start();
}

void ViewMindDataContainer::setCurrentStudyStart(){

    // When the study actually starts, then the example phase has ended.
    examplePhaseDuration = studyDurationMeasure.elapsed();
    studyDurationMeasure.start();

    startTimeForCurrentStudy = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    studyDurationMeasure.start();

}

bool ViewMindDataContainer::setExperimentDescriptionMap(const QVariantMap &expDesc){

    if (currentlySelectedStudy == ""){
        error = "Trying to set experiment description withouth setting the study first";
        return false;
    }

    if (!data.value(MAIN_FIELD_STUDIES).toMap().contains(currentlySelectedStudy)){
        error = currentlySelectedStudy + " has not been configured for this container";
        return false;
    }

    QVariantMap studies = data.value(MAIN_FIELD_STUDIES).toMap();
    QVariantMap cstudy  = studies.value(currentlySelectedStudy).toMap();
    cstudy[VMDC::StudyField::EXPERIMENT_DESCRIPTION] = expDesc;
    studies[currentlySelectedStudy] = cstudy;

    data[MAIN_FIELD_STUDIES] = studies;

    return true;

}

bool ViewMindDataContainer::setSubjectData(const QVariantMap &subject_data){

    //RDC::prettpPrintQVariantMap(data);

    QStringList fields = VMDC::SubjectField::valid;

    QVariantMap map;

    // Will treat all fields as mandatory.
    for (qint32 i = 0; i < fields.size(); i++){
        if (!subject_data.contains(fields.at(i))){
            error = QString("Subject field ") + fields.at(i) + QString(" is required for setting the subject data");
            return false;
        }
        map[fields.at(i)] = subject_data.value(fields.at(i));
    }

    data[MAIN_FIELD_SUBJECT_DATA] = map;
    return true;
}


void ViewMindDataContainer::setQCParameters(const QVariantMap &qcparams){
    // This assumes that the parameters have already been checked.
    data[MAIN_FIELD_FREQUENCY_CHECK_PARAMETERS] = qcparams;
}

bool ViewMindDataContainer::setApplicationUserData(const QString &type, const QVariantMap &au_data){

    //RDC::prettpPrintQVariantMap(data);

    QString check = VMDC::AppUserType::validate(type);
    if (check != ""){
        error = "Attempting to set application user data: " + check;
        return false;
    }

    QStringList fields = VMDC::AppUserField::valid;
    QVariantMap map;

    // Will treat all fields as mandatory.
    for (qint32 i = 0; i < fields.size(); i++){
        if (!au_data.contains(fields.at(i))){
            error = QString("Application user field ") + fields.at(i) + QString(" is required for setting the subject data");
            return false;
        }
        map[fields.at(i)] = au_data.value(fields.at(i));
    }

    QVariantMap users = data.value(MAIN_FIELD_APPLICATION_USER).toMap();
    //qDebug() << "Setting application user data for type" << type;
    //RDC::prettpPrintQVariantMap(map);
    users[type] = map;
    data[MAIN_FIELD_APPLICATION_USER] = users;

    return true;
}

///////////////////////////////////////////////// FILLING CONTAINER FUNCTIONS /////////////////////////////////////////////////////////////////////////

bool ViewMindDataContainer::addStudy(const QString &study, const QVariantMap &studyConfiguration, const QString &experimentDescription, const QString &version){

    error = "";
    QString check = VMDC::Study::validate(study);
    if (check != ""){
        error = "While adding study: " + check;
        return false;
    }

    QStringList parameters_to_check;
    QMap<QString,QStringList> valid_parameter_values;
    valid_parameter_values[VMDC::StudyParameter::VALID_EYE] = VMDC::Eye::valid;
    valid_parameter_values[VMDC::StudyParameter::DEFAULT_EYE] = VMDC::Eye::valid;

    // Computing the time stamp fields
    QString date = QDateTime::currentDateTime().toString("dd/MM/yyyy");
    QString hour = QDateTime::currentDateTime().toString("HH:mm");

    // Set the list of the parameters to check depending on study.
    if ((study == VMDC::Study::BINDING_BC_2_SHORT) || (study == VMDC::Study::BINDING_UC_2_SHORT) ||
        (study == VMDC::Study::BINDING_BC_3_SHORT) || (study == VMDC::Study::BINDING_UC_3_SHORT)){
        valid_parameter_values[VMDC::StudyParameter::NUMBER_TARGETS] = VMDC::BindingTargetCount::valid;
        valid_parameter_values[VMDC::StudyParameter::TARGET_SIZE]    = VMDC::BindingTargetSize::valid;
    }
    else if ((study == VMDC::Study::NBACK_3) || (study == VMDC::Study::NBACK_3)){
        valid_parameter_values[VMDC::StudyParameter::NUMBER_TARGETS] = VMDC::NBackVSTargetCount::valid;
    }

    // Now we validate both the that all parameters are there and their values.
    QVariantMap finalStudyConfiguration = studyConfiguration;
    parameters_to_check = valid_parameter_values.keys();
    for (qint32 i = 0; i < parameters_to_check.size(); i++){
        QString parameter = parameters_to_check.at(i);
        if (!finalStudyConfiguration.contains(parameter)){
            error = "Parameter for configuring study " + study + " is missing: " + parameters_to_check.at(i);
            return false;
        }
        QString value = finalStudyConfiguration.value(parameter).toString();
        if (!valid_parameter_values.value(parameter).contains(value)){
            error = "Invalid parameter value " + value  + " for parameter " + parameter + " for study " + study;
            return false;
        }
        //finalStudyConfiguration.insert(parameter,value);
    }

    QVariantMap studies = data.value(MAIN_FIELD_STUDIES,QVariantMap()).toMap();
    QVariantMap studyToConfigure;

//    QString config_code;
//    QString eye = finalStudyConfiguration.value(VMDC::StudyParameter::VALID_EYE).toString();

//    if (eye == VMDC::Eye::LEFT) config_code = "L";
//    else if (eye == VMDC::Eye::RIGHT) config_code = "R";
//    else config_code = "B";

    // Specific study configurations.
//    QString abbreviation;

//    if (study == VMDC::Study::BINDING_BC){
//        config_code = config_code + finalStudyConfiguration.value(VMDC::StudyParameter::NUMBER_TARGETS).toString() +
//                finalStudyConfiguration.value(VMDC::StudyParameter::TARGET_SIZE).toString().mid(0,1).toUpper();
//        abbreviation = "bc";
//    }
//    else if (study == VMDC::Study::BINDING_UC){
//        config_code = config_code + finalStudyConfiguration.value(VMDC::StudyParameter::NUMBER_TARGETS).toString() +
//                finalStudyConfiguration.value(VMDC::StudyParameter::TARGET_SIZE).toString().mid(0,1).toUpper();
//        abbreviation = "uc";
//    }
//    else if (study == VMDC::Study::READING){
//        config_code = config_code + finalStudyConfiguration.value(VMDC::StudyParameter::LANGUAGE).toString().mid(0,2).toUpper();
//        abbreviation = "rd";
//    }
//    else if (study == VMDC::Study::GONOGO){
//        abbreviation = "gn";
//    }
//    else if (study == VMDC::Study::NBACKMS){
//        abbreviation = "ms";
//    }
//    else if (study == VMDC::Study::NBACKRT){
//        abbreviation = "rt";
//    }
//    else if (study == VMDC::Study::NBACKVS){
//        config_code = config_code + finalStudyConfiguration.value(VMDC::StudyParameter::NUMBER_TARGETS).toString();
//        if (finalStudyConfiguration.value(VMDC::StudyParameter::NBACK_LIGHT_ALL).toBool()){
//            config_code = config_code + "A";
//        }
//        else {
//            config_code = config_code + "S";
//        }
//        abbreviation = "vs";
//    }
//    else if (study == VMDC::Study::DOTFOLLOW){
//        abbreviation = "df";
//    }
//    else if (study == VMDC::Study::GONOGO_SPHERE){
//        abbreviation = "gng3D";
//    }

    //qDebug() << "Setting STUDY CONFIG in Final Json VMDC";
    //Debug::prettpPrintQVariantMap(finalStudyConfiguration);

    // Date and Hour are redundant with the study start.
    // However I want to keep them so that it is not necessary to update backend processing.

    studyToConfigure.insert(VMDC::StudyField::DATE,date);
    studyToConfigure.insert(VMDC::StudyField::HOUR,hour);
    studyToConfigure.insert(VMDC::StudyField::EXPERIMENT_DESCRIPTION,experimentDescription);
    studyToConfigure.insert(VMDC::StudyField::VERSION,version);
    //studyToConfigure.insert(VMDC::StudyField::CONFIG_CODE,config_code);
    //studyToConfigure.insert(VMDC::StudyField::ABBREVIATION,abbreviation);
    studyToConfigure.insert(VMDC::StudyField::STUDY_CONFIGURATION,finalStudyConfiguration);
    studyToConfigure.insert(VMDC::StudyField::STATUS,VMDC::StatusType::ONGOING);

    studies.insert(study,studyToConfigure);

    data[MAIN_FIELD_STUDIES] = studies;

    //std::cout << "DATA AFTER ADDING" << study.toStdString() << std::endl;
    //RDC::prettpPrintQVariantMap(data);

    return true;

}


void ViewMindDataContainer::clearFieldsForIndexFileCreation(){
    // This is used to create a "just the header file" for quick loading of information.
    QStringList allstudies = data.value(MAIN_FIELD_STUDIES).toMap().keys();
    QVariantMap allstudymaps = data.value(MAIN_FIELD_STUDIES).toMap();
    for (qint32  i = 0; i < allstudies.size(); i++){
        QVariantMap study = allstudymaps.value(allstudies.at(i)).toMap();
        study[VMDC::StudyField::TRIAL_LIST] = QVariantMap(); // This is for 2D studies.
        study[VMDC::StudyField::STUDY_DATA] = QVariantMap(); // This is for 3D Studies
        study[VMDC::StudyField::EXPERIMENT_DESCRIPTION] = QVariantMap(); // In 3D studies this can contain base64 encoded files which are very large.
        allstudymaps[allstudies.at(i)] = study;
    }    
    data[MAIN_FIELD_STUDIES] = allstudymaps;
}

void ViewMindDataContainer::clearAndStoreSubjectData(){
    // Now we anonymize the study file.
    this->temporaryStorage =  data.value(MAIN_FIELD_SUBJECT_DATA).toMap();
    QVariantMap temp;
    QStringList tocopy; tocopy << VMDC::SubjectField::LOCAL_ID; tocopy << VMDC::SubjectField::YEARS_FORMATION; tocopy << VMDC::SubjectField::GENDER;
    for (qint32 i = 0; i < tocopy.size(); i++){
        temp[tocopy.at(i)] = this->temporaryStorage.value(tocopy.at(i));
    }

    // Computing the current age.
    QDate bdate = temporaryStorage.value(VMDC::SubjectField::BIRTH_DATE).toDate();
    int days = bdate.daysTo(QDate::currentDate());
    int age = qFloor(days/365);
    temp[VMDC::SubjectField::AGE_AT_EVALUATION] = age;

    //qDebug() << "Birth date is" << temporaryStorage.value(VMDC::SubjectField::BIRTH_DATE).toString() << "So age is" << age;

    data[MAIN_FIELD_SUBJECT_DATA] = temp;
}

void ViewMindDataContainer::restoreSubjectData(){
    data[MAIN_FIELD_SUBJECT_DATA] = this->temporaryStorage;
}

bool ViewMindDataContainer::setFullTrialList(const QVariantList &fullTrialList,
                                             qint32 explanationPhaseDuration,
                                             qint32 examplePhaseDuration,
                                             qint32 pauseDuration,
                                             qint32 evaluationDuration,
                                             const QString &startTime){

    // Getting the current study structure from the current studies list.
    QString studyName = currentlySelectedStudy;
    QVariantMap studies = data.value(MAIN_FIELD_STUDIES).toMap();
    QVariantMap study = studies.value(studyName).toMap();

    study.insert(VMDC::StudyField::TRIAL_LIST,fullTrialList);
    study.insert(VMDC::StudyField::EXAMPLE_TIME,examplePhaseDuration);
    study.insert(VMDC::StudyField::EXPLANATION_TIME,explanationPhaseDuration);
    study.insert(VMDC::StudyField::START_TIME,startTime);
    study.insert(VMDC::StudyField::PAUSE_DURATION,pauseDuration);
    study.insert(VMDC::StudyField::END_TIME,QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    study.insert(VMDC::StudyField::STUDY_DURATION,evaluationDuration);
    study.insert(VMDC::StudyField::STATUS,VMDC::StatusType::FINALIZED);

    studies[currentlySelectedStudy] = study;

    data[MAIN_FIELD_STUDIES] = studies;

    // Clearing all data.
    currentDataSetMap.clear();
    currentTrial.clear();
    currentRawDataList.clear();
    currentLFixationVectorL.clear();
    currentLFixationVectorR.clear();
    currentTrialList.clear();

    return true;


}

bool ViewMindDataContainer::setFullElement3D(const QVariantMap &elements,
                                             qint32 explanationPhaseDuration,
                                             qint32 examplePhaseDuration, qint32 pauseDuration,
                                             qint32 evaluationDuration,
                                             const QString &startTime){

    // Getting the current study structure from the current studies list.
    QString studyName = currentlySelectedStudy;
    QVariantMap studies = data.value(MAIN_FIELD_STUDIES).toMap();
    QVariantMap study = studies.value(studyName).toMap();

    study.insert(VMDC::StudyField::STUDY_DATA,elements);
    study.insert(VMDC::StudyField::EXAMPLE_TIME,examplePhaseDuration);
    study.insert(VMDC::StudyField::EXPLANATION_TIME,explanationPhaseDuration);
    study.insert(VMDC::StudyField::START_TIME,startTime);
    study.insert(VMDC::StudyField::PAUSE_DURATION,pauseDuration);
    study.insert(VMDC::StudyField::END_TIME,QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    study.insert(VMDC::StudyField::STUDY_DURATION,evaluationDuration);
    study.insert(VMDC::StudyField::STATUS,VMDC::StatusType::FINALIZED);

    studies[currentlySelectedStudy] = study;

    data[MAIN_FIELD_STUDIES] = studies;

    // Clearing all data.
    currentDataSetMap.clear();
    currentTrial.clear();
    currentRawDataList.clear();
    currentLFixationVectorL.clear();
    currentLFixationVectorR.clear();
    currentTrialList.clear();

    return true;



}

bool ViewMindDataContainer::addNewTrial(const QString &trial_id, const QString &type, const QString &correct_response, const QVariantMap &trialMetadata){
    currentTrial.clear();
    currentRawDataList.clear();
    currentLFixationVectorL.clear();
    currentLFixationVectorR.clear();

    // All trials have a data subfield.
    currentTrial.insert(VMDC::TrialField::ID,trial_id);
    currentTrial.insert(VMDC::TrialField::DATA,QVariantMap());
    currentTrial.insert(VMDC::TrialField::TRIAL_TYPE,type);
    currentTrial.insert(VMDC::TrialField::RESPONSE,"");
    currentTrial.insert(VMDC::TrialField::CORRECT_RESPONSE,correct_response);
    // Adding any extra information that is required on a per trial basis for the study.
    currentTrial.insert(VMDC::TrialField::METADATA,trialMetadata);

    if (type == "") {
        error = "Trial Type cannot be empty";
        return false;
    }

    return true;
}

bool ViewMindDataContainer::setCurrentDataSet(const QString &data_set_type){
    QString check = VMDC::DataSetType::validate(data_set_type);
    if (check != ""){
        error = "While setting the data set type: " + check;
        return false;
    }
    currentDataSetType = data_set_type;
    return true;
}

void ViewMindDataContainer::finalizeDataSet(){

    QVariantMap currentDataSetRawData;

    // Creating the raw data field in the data set where the actual data is stored.
    currentDataSetRawData.insert(VMDC::DataSetField::RAW_DATA,currentRawDataList);
    currentDataSetRawData.insert(VMDC::DataSetField::FIXATION_L,currentLFixationVectorL);
    currentDataSetRawData.insert(VMDC::DataSetField::FIXATION_R,currentLFixationVectorR);

    // Setting the current data set to its defined type.
    currentDataSetMap.insert(currentDataSetType,currentDataSetRawData);

    // Clearing the data that was just added.
    currentRawDataList.clear();
    currentLFixationVectorL.clear();
    currentLFixationVectorR.clear();

}

void ViewMindDataContainer::finalizeTrial(const QString &response){
    // All trials must contain a response. Even if empty.
    currentTrial[VMDC::TrialField::RESPONSE] = response;

    // Saving the data sets to the trial data field.
    currentTrial.insert(VMDC::TrialField::DATA,currentDataSetMap);

    // Adding the trial to the trial list.
    currentTrialList << currentTrial;

    // Clearing the trial that was just added.
    currentTrial.clear();
}

bool ViewMindDataContainer::finalizeStudy(){
    return finalizeStudy(QVariantMap());
}

bool ViewMindDataContainer::finalizeStudy(const QVariantMap &study_data,
                                          qint64 overwrite_study_duration,
                                          bool onlyStoreData){


    // Getting the current study structure from the current studies list.
    QString studyName = currentlySelectedStudy;

    if (studyName == ""){
        error = "Trying to finalize an unitialized study";
        return false;
    }

    //qDebug() << "Finalizing Study" << studyName << "with input variant map";
    //Debug::prettpPrintQVariantMap(study_data);
    //qDebug() << "Is Empty" << study_data.isEmpty() << study_data.empty() << " And count" << study_data.size();

    QVariantMap studies = data.value(MAIN_FIELD_STUDIES).toMap();
    QVariantMap study = studies.value(studyName).toMap();

    // Storing the trial list for the study.
    if (study_data.isEmpty()){
        //qDebug() << "Inserting a list of" << currentTrialList.size();
        study.insert(VMDC::StudyField::TRIAL_LIST,currentTrialList);
    }
    else {
        //qDebug() << "Inserting the field study data in the study " << studyName;
        study.insert(VMDC::StudyField::STUDY_DATA,study_data);
    }

    if (!onlyStoreData){
        study.insert(VMDC::StudyField::STATUS,VMDC::StatusType::FINALIZED);
        study.insert(VMDC::StudyField::EXAMPLE_TIME,examplePhaseDuration);
        study.insert(VMDC::StudyField::EXPLANATION_TIME,explanationPhaseDuration);
        study.insert(VMDC::StudyField::START_TIME,startTimeForCurrentStudy);
        study.insert(VMDC::StudyField::PAUSE_DURATION,pauseDuration);
        study.insert(VMDC::StudyField::END_TIME,QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    }
    if (overwrite_study_duration < 0) study.insert(VMDC::StudyField::STUDY_DURATION,studyDurationMeasure.elapsed());
    else study.insert(VMDC::StudyField::STUDY_DURATION,overwrite_study_duration);

    pauseDuration.clear(); // Once stored we make sure it's clear for the following studies.

    studies[studyName] = study;

    data[MAIN_FIELD_STUDIES] = studies;

    // Clearing all data.
    currentDataSetMap.clear();
    currentTrial.clear();
    currentRawDataList.clear();
    currentLFixationVectorL.clear();
    currentLFixationVectorR.clear();
    currentTrialList.clear();

    return true;

}

void ViewMindDataContainer::addNewRawDataVector(const QVariantMap &raw_data_vector){
    // This should be checked, however in order to optimize peformance (just in case) no check is done. As long as
    // the vector is correctly constructed once all checks will always pass.
    currentRawDataList << raw_data_vector;
}

void ViewMindDataContainer::addFixationVectorL(const QVariantMap &fixation_vector){
    currentLFixationVectorL << fixation_vector;
}

void ViewMindDataContainer::addFixationVectorR(const QVariantMap &fixation_vector){
    currentLFixationVectorR << fixation_vector;
}


///////////////////////////////////////////////// READING DATA FROM CONTAINER FUNCTIONS /////////////////////////////////////////////////////////////////////////
QVariantList ViewMindDataContainer::getStudyTrialList(const QString &study){
    QStringList hierarchy; hierarchy << MAIN_FIELD_STUDIES << study <<  VMDC::StudyField::TRIAL_LIST;
    if (!checkHiearchyChain(hierarchy)) return QVariantList();
    return data.value(MAIN_FIELD_STUDIES).toMap().value(study).toMap().value(VMDC::StudyField::TRIAL_LIST).toList();
}

QVariant ViewMindDataContainer::get3DStudyData(const QString &study, const QString &field){
    QStringList hierarchy; hierarchy << MAIN_FIELD_STUDIES << study << VMDC::StudyField::STUDY_DATA;
    if (!checkHiearchyChain(hierarchy)) return QVariant();

    QVariant sdata = data.value(MAIN_FIELD_STUDIES).toMap().value(study).toMap().value(VMDC::StudyField::STUDY_DATA);

    if (field != ""){
        if (sdata.toMap().contains(field)){
            return sdata.toMap().value(field);
        }
        else return sdata;
    }
    else return sdata;

}

////////////////////////////////////// HIEARCHY CHECK FUNCTION //////////////////////////////////////
bool ViewMindDataContainer::checkHiearchyChain(const QStringList &hieararchy) {
    QVariantMap map = data;
    QString last = "";
    for (qint32 i = 0; i < hieararchy.size(); i++){
        QString current = hieararchy.at(i);
        if (map.contains(current)){
            last = current;
            map = map.value(current).toMap();
        }
        else{
            if (last != "") error = "Field " + current + " doesn't exists as part of " + last;
            else error = "First field " + current + "doesn't exist";
            return false;
        }
    }
    return true;
}

////////////////////////////////////// DEBUG FUNCTIONS //////////////////////////////////////
void ViewMindDataContainer::printRawDataCSV(const QString &filename, const QString &study , const QStringList whichRawDataValues){
    QVariantList trial_list = data.value(MAIN_FIELD_STUDIES).toMap().value(study).toMap().value(VMDC::StudyField::TRIAL_LIST).toList();
    QStringList rows;
    for (qint32 t = 0; t < trial_list.size(); t++){
        //qDebug() << trial_list.size();
        QString trialID = trial_list.at(t).toMap().value(VMDC::TrialField::ID).toString();
        QString trialType = trial_list.at(t).toMap().value(VMDC::TrialField::TRIAL_TYPE).toString();
        QString trialTypeSize = QString::number(trialType.split(" ").size());
        QVariantMap trial_data = trial_list.at(t).toMap().value(VMDC::TrialField::DATA).toMap();
        //qDebug() << trial_data.size();
        QStringList DataSetTypes = trial_data.keys();
        //qDebug() << DataSetTypes.size();
        for (qint32 dst = 0; dst < DataSetTypes.size(); dst++){
            QVariantList raw_data = trial_data.value(DataSetTypes.at(dst)).toMap().value(VMDC::DataSetField::RAW_DATA).toList();
            //qDebug() << raw_data.size();
            for (qint32 i = 0; i < raw_data.size(); i++){
                QStringList row;
                row << trialID << DataSetTypes.at(dst) << trialTypeSize;
                QVariantMap datum = raw_data.at(i).toMap();
                for (qint32 k = 0; k < whichRawDataValues.size(); k++){
                    row << datum.value(whichRawDataValues.at(k)).toString();
                }
                rows << row.join(",");
            }

        }
    }

    QFile towrite(filename);
    if (!towrite.open(QFile::WriteOnly)) return;
    QTextStream writer(&towrite);
    writer << rows.join("\n");
    towrite.close();
}

////////////////////////////////////// STATIC STRING INITIALIZATION //////////////////////////////////////

QString ViewMindDataContainer::MAIN_FIELD_SUBJECT_DATA                = "subject";
QString ViewMindDataContainer::MAIN_FIELD_PROCESSING_PARAMETERS       = "processing_parameters";
QString ViewMindDataContainer::MAIN_FIELD_FREQUENCY_CHECK_PARAMETERS  = "qc_parameters";
QString ViewMindDataContainer::MAIN_FIELD_METADATA                    = "metadata";
QString ViewMindDataContainer::MAIN_FIELD_APPLICATION_USER            = "application_user";
QString ViewMindDataContainer::MAIN_FIELD_STUDIES                     = "studies";
QString ViewMindDataContainer::MAIN_FIELD_HASH                        = "hash";
QString ViewMindDataContainer::MAIN_FIELD_CALIBRATION                 = "calibration";
QString ViewMindDataContainer::MAIN_FIELD_SYSTEM_SPECS                = "system_specs";

QString ViewMindDataContainer::CURRENT_JSON_STRUCT_VERSION            = "1";

