#include "viewminddatacontainer.h"

ViewMindDataContainer::ViewMindDataContainer()
{
    error = "";
}

bool ViewMindDataContainer::saveJSONFile(const QString &file_name, bool pretty_print){
    QJsonDocument json = QJsonDocument::fromVariant(data);

    QFile file(file_name);
    if (!file.open(QFile::WriteOnly)){
        error = "Could not open " + file_name + " for writing";
        return false;
    }

    QTextStream writer(&file);
    writer.setCodec("UTF-8");
    if (pretty_print) writer << json.toJson(QJsonDocument::Indented);
    else writer << json.toJson(QJsonDocument::Compact);
    file.close();

    return true;
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
    return true;
}

QString ViewMindDataContainer::getError() const{
    return error;
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

QString ViewMindDataContainer::getMetadaStatus(){
    QStringList hierarchy; hierarchy << MAIN_FIELD_METADATA << VMDC::MetadataField::STATUS;
    if (!checkHiearchyChain(hierarchy)) return "";
    return data.value(MAIN_FIELD_METADATA).toMap().value(VMDC::MetadataField::STATUS).toString();
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
    return ret;
}

QVariantMap ViewMindDataContainer::getSubjectData() {
    QStringList hierarchy; hierarchy << MAIN_FIELD_SUBJECT_DATA;
    if (!checkHiearchyChain(hierarchy)) return QVariantMap();
    return data.value(MAIN_FIELD_SUBJECT_DATA).toMap();
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

    //Debug::prettpPrintQVariantMap(current_metada);

    // Checking for valid status.
    QString check = VMDC::StatusType::validate(current_metada.value(VMDC::MetadataField::STATUS).toString());
    if (!check.isEmpty()){
        error = "Error setting metadata. Reason: " + check;
        return false;
    }

    // Ensuring the proper version.
    current_metada[VMDC::MetadataField::VERSION] = CURRENT_JSON_STRUCT_VERSION;

    // Saving as metadata
    data[MAIN_FIELD_METADATA] = current_metada;
    return true;
}

void ViewMindDataContainer::addCustomMetadataFields(const QString field_name, const QString field_value){
    QVariantMap current_metada = data.value(MAIN_FIELD_METADATA).toMap();
    current_metada.insert(field_name,field_value);
    data[MAIN_FIELD_METADATA] = current_metada;
}

bool ViewMindDataContainer::setProcessingParameters(const QVariantMap &pp){

    // All processing parameters will be considered mandatory.
    QStringList mandatory;
    mandatory << VMDC::ProcessingParameter::MAX_DISPERSION_WINDOW
              << VMDC::ProcessingParameter::MIN_FIXATION_DURATION
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


bool ViewMindDataContainer::setCurrentStudy(const QString &study){

    if (!data.value(MAIN_FIELD_STUDIES).toMap().contains(study)){
        error = study + " has not been configured for this container";
        return false;
    }

    currentlySelectedStudy = study;
    currentDataSetMap.clear();
    currentRawDataList.clear();
    currentLFixationVectorL.clear();
    currentLFixationVectorR.clear();
    currentTrial.clear();
    currentTrialList.clear();

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

    // Computing the time stamp fields
    QString date = QDateTime::currentDateTime().toString("dd/MM/yyyy");
    QString hour = QDateTime::currentDateTime().toString("HH:mm");

    // Set the list of the parameters to check depending on study.
    if ((study == VMDC::Study::BINDING_BC) || (study == VMDC::Study::BINDING_UC)){
        valid_parameter_values[VMDC::StudyParameter::NUMBER_TARGETS] = VMDC::BindingTargetCount::valid;
        valid_parameter_values[VMDC::StudyParameter::TARGET_SIZE] = VMDC::BindingTargetSize::valid;
    }
    else if (study == VMDC::Study::READING){
        valid_parameter_values[ VMDC::StudyParameter::LANGUAGE] = VMDC::ReadingLanguage::valid;
    }
    else if (study == VMDC::Study::GONOGO){

    }
    else if (study == VMDC::Study::NBACKMS){

    }
    else if (study == VMDC::Study::NBACKRT){

    }
    else if (study == VMDC::Study::NBACKVS){
        valid_parameter_values[VMDC::StudyParameter::NUMBER_TARGETS] = VMDC::NBackVSTargetCount::valid;
    }
    else if ( (study == VMDC::Study::PERCEPTION_1) || (study == VMDC::Study::PERCEPTION_2) || (study == VMDC::Study::PERCEPTION_3) || (study == VMDC::Study::PERCEPTION_4) ||
              (study == VMDC::Study::PERCEPTION_5) || (study == VMDC::Study::PERCEPTION_6) || (study == VMDC::Study::PERCEPTION_7) || (study == VMDC::Study::PERCEPTION_8) ){
        valid_parameter_values[VMDC::StudyParameter::PERCEPTION_TYPE] = VMDC::PerceptionType::valid;
    }

    // Now we validate both the that all parameters are there and their values.
    QVariantMap finalStudyConfiguration;
    parameters_to_check = valid_parameter_values.keys();
    for (qint32 i = 0; i < parameters_to_check.size(); i++){
        QString parameter = parameters_to_check.at(i);
        if (!studyConfiguration.contains(parameter)){
            error = "Parameter for configuring study " + study + " is missing: " + parameters_to_check.at(i);
            return false;
        }
        QString value = studyConfiguration.value(parameter).toString();
        if (!valid_parameter_values.value(parameter).contains(value)){
            error = "Invalid parameter value " + value  + " for parameter " + parameter + " for study " + study;
            return false;
        }
        finalStudyConfiguration.insert(parameter,value);
    }

    QVariantMap studies = data.value(MAIN_FIELD_STUDIES,QVariantMap()).toMap();
    QVariantMap studyToConfigure;

    QString config_code;
    QString eye = finalStudyConfiguration.value(VMDC::StudyParameter::VALID_EYE).toString();
    if (eye == VMDC::Eye::LEFT) config_code = "L";
    else if (eye == VMDC::Eye::RIGHT) config_code = "R";
    else config_code = "B";

    //studyToConfigure[RDC::StudyParameter::VALID_EYE] = mapStudyValues.value(studyConfiguration[SCP_EYES]);

    // Specific study configurations.
    QString abbreviation;

    if (study == VMDC::Study::BINDING_BC){
        config_code = config_code + finalStudyConfiguration.value(VMDC::StudyParameter::NUMBER_TARGETS).toString() +
                finalStudyConfiguration.value(VMDC::StudyParameter::TARGET_SIZE).toString().mid(0,1).toUpper();
        abbreviation = "bc";
    }
    else if (study == VMDC::Study::BINDING_UC){
        config_code = config_code + finalStudyConfiguration.value(VMDC::StudyParameter::NUMBER_TARGETS).toString() +
                finalStudyConfiguration.value(VMDC::StudyParameter::TARGET_SIZE).toString().mid(0,1).toUpper();
        abbreviation = "uc";
    }
    else if (study == VMDC::Study::READING){
        config_code = config_code + finalStudyConfiguration.value(VMDC::StudyParameter::LANGUAGE).toString().mid(0,2).toUpper();
        abbreviation = "rd";
    }
    else if (study == VMDC::Study::GONOGO){
        abbreviation = "gn";
    }
    else if (study == VMDC::Study::NBACKMS){
        abbreviation = "ms";
    }
    else if (study == VMDC::Study::NBACKRT){
        abbreviation = "rt";
    }
    else if (study == VMDC::Study::NBACKVS){
        config_code = config_code + finalStudyConfiguration.value(VMDC::StudyParameter::NUMBER_TARGETS).toString();
        abbreviation = "vs";
    }
    else if (study.contains(VMDC::MultiPartStudyBaseName::PERCEPTION)){
        valid_parameter_values[VMDC::StudyParameter::PERCEPTION_TYPE] = VMDC::PerceptionType::valid;
        config_code = config_code + finalStudyConfiguration.value(VMDC::StudyParameter::PERCEPTION_TYPE).toString().mid(0,1).toUpper();
    }

    studyToConfigure.insert(VMDC::StudyField::DATE,date);
    studyToConfigure.insert(VMDC::StudyField::HOUR,hour);
    studyToConfigure.insert(VMDC::StudyField::EXPERIMENT_DESCRIPTION,experimentDescription);
    studyToConfigure.insert(VMDC::StudyField::VERSION,version);
    studyToConfigure.insert(VMDC::StudyField::CONFIG_CODE,config_code);
    studyToConfigure.insert(VMDC::StudyField::ABBREVIATION,abbreviation);
    studyToConfigure.insert(VMDC::StudyField::STUDY_CONFIGURATION,finalStudyConfiguration);
    studyToConfigure.insert(VMDC::StudyField::STATUS,VMDC::StatusType::ONGOING);

    studies.insert(study,studyToConfigure);

    data[MAIN_FIELD_STUDIES] = studies;

    //std::cout << "DATA AFTER ADDING" << study.toStdString() << std::endl;
    //RDC::prettpPrintQVariantMap(data);

    return true;

}


void ViewMindDataContainer::markFileAsFinalized(){
    QVariantMap metadata = data.value(MAIN_FIELD_METADATA).toMap();
    metadata[VMDC::MetadataField::STATUS] = VMDC::StatusType::FINALIZED;
    data[MAIN_FIELD_METADATA] = metadata;
}

////////////////////////////////////// CREATING HIEARARCHY //////////////////////////////////////

bool ViewMindDataContainer::addNewTrial(const QString &trial_id, const QString &type){
    currentTrial.clear();
    currentRawDataList.clear();
    currentLFixationVectorL.clear();
    currentLFixationVectorR.clear();

    // All trials have a data subfield.
    currentTrial.insert(VMDC::TrialField::ID,trial_id);
    currentTrial.insert(VMDC::TrialField::DATA,QVariantMap());
    currentTrial.insert(VMDC::TrialField::TRIAL_TYPE,type);
    currentTrial.insert(VMDC::TrialField::RESPONSE,"");

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

    // Getting the current study structure from the current studies list.
    QString studyName =currentlySelectedStudy;

    if (studyName == ""){
        error = "Trying to finalize an unitialized study";
        return false;
    }

    QVariantMap studies = data.value(MAIN_FIELD_STUDIES).toMap();
    QVariantMap study = studies.value(studyName).toMap();

    // Storing the trial list for the study.
    study.insert(VMDC::StudyField::TRIAL_LIST,currentTrialList);
    study.insert(VMDC::StudyField::STATUS,VMDC::StatusType::FINALIZED);

    //qDebug() << "Finalizing Study" << studyName << " of value " << currentlySelectedStudy;

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

////////////////////////////////////// Generate Vector Functions //////////////////////////////////////

QVariantMap ViewMindDataContainer::GenerateReadingRawDataVector(float timestamp, float xr, float yr, float xl, float yl, float pr, float pl, float char_r, float char_l, float word_r, float word_l){
    QVariantMap vector = ViewMindDataContainer::GenerateStdRawDataVector(timestamp,xr,yr,xl,yl,pr,pl);
    vector.insert(VMDC::DataVectorField::CHAR_L,char_l);
    vector.insert(VMDC::DataVectorField::CHAR_R,char_r);
    vector.insert(VMDC::DataVectorField::WORD_L,word_l);
    vector.insert(VMDC::DataVectorField::WORD_R,word_r);
    return vector;
}

QVariantMap ViewMindDataContainer::GenerateStdRawDataVector(float timestamp, float xr, float yr, float xl, float yl, float pr, float pl){
    QVariantMap vector;
    vector.insert(VMDC::DataVectorField::TIMESTAMP,timestamp);
    vector.insert(VMDC::DataVectorField::X_L,xl);
    vector.insert(VMDC::DataVectorField::X_R,xr);
    vector.insert(VMDC::DataVectorField::Y_L,yl);
    vector.insert(VMDC::DataVectorField::Y_R,yr);
    vector.insert(VMDC::DataVectorField::PUPIL_L,pl);
    vector.insert(VMDC::DataVectorField::PUPIL_R,pr);
    return vector;

}

////////////////////////////////////// STATIC STRING INITIALIZATION //////////////////////////////////////

QString ViewMindDataContainer::MAIN_FIELD_SUBJECT_DATA                = "subject";
QString ViewMindDataContainer::MAIN_FIELD_PROCESSING_PARAMETERS       = "processing_parameters";
QString ViewMindDataContainer::MAIN_FIELD_FREQUENCY_CHECK_PARAMETERS  = "frequency_check_parameters";
QString ViewMindDataContainer::MAIN_FIELD_METADATA                    = "metadata";
QString ViewMindDataContainer::MAIN_FIELD_APPLICATION_USER            = "application_user";
QString ViewMindDataContainer::MAIN_FIELD_STUDIES                     = "studies";

QString ViewMindDataContainer::CURRENT_JSON_STRUCT_VERSION            = "1";

