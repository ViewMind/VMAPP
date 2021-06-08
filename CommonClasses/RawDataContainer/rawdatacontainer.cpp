#include "rawdatacontainer.h"

RawDataContainer::RawDataContainer()
{
    error = "";
}

bool RawDataContainer::saveJSONFile(const QString &file_name, bool pretty_print){
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

bool RawDataContainer::loadFromJSONFile(const QString &file_name){

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

QString RawDataContainer::getError() const{
    return error;
}

///////////////////////////////////////////////// READING DATA FUNCTIONS /////////////////////////////////////////////////////////////////////////

QStringList RawDataContainer::getStudies() {
    QStringList ans;
    QStringList hierarchy; hierarchy << MAIN_FIELD_STUDIES;
    if (!checkHiearchyChain(hierarchy)) return ans;
    ans = data.value(MAIN_FIELD_STUDIES).toMap().keys();
    return ans;
}

QString RawDataContainer::getStudyStatus(const QString &study){

    QString check = RDC::Study::validate(study);

    if (check != ""){
        error = "Attempting to get study status: " + check;
        return "";
    }

    QStringList hierarchy; hierarchy << MAIN_FIELD_STUDIES << study << RDC::StudyField::STATUS;
    if (!checkHiearchyChain(hierarchy)) return "";
    return data.value(MAIN_FIELD_STUDIES).toMap().value(study).toMap().value(RDC::StudyField::STATUS).toString();
}

QVariantMap RawDataContainer::getStudyConfiguration(const QString study){
    QVariantMap sc;
    QString check = RDC::Study::validate(study);

    if (check != ""){
        error = "Attempting to get study configuration: " + check;
        return sc;
    }
    QStringList hierarchy; hierarchy << MAIN_FIELD_STUDIES << study << RDC::StudyField::STUDY_CONFIGURATION;
    if (!checkHiearchyChain(hierarchy)) return sc;
    return data.value(MAIN_FIELD_STUDIES).toMap().value(study).toMap().value(RDC::StudyField::STUDY_CONFIGURATION).toMap();
}

QString RawDataContainer::getStudyCode(const QString &study) {

    QString check = RDC::Study::validate(study);

    if (check != ""){
        error = "Attempting to get study configuration: " + check;
        return "";
    }

    QStringList hierarchy; hierarchy << MAIN_FIELD_STUDIES << study;
    if (!checkHiearchyChain(hierarchy)) return "";

    QString code = data.value(MAIN_FIELD_STUDIES).toMap().value(study).toMap().value(RDC::StudyField::ABBREVIATION).toString();
    code = code + data.value(MAIN_FIELD_STUDIES).toMap().value(study).toMap().value(RDC::StudyField::CONFIG_CODE).toString();
    return code;

}

QStringList RawDataContainer::getMetaDataDateTime(){
    QStringList hierarchy; hierarchy << MAIN_FIELD_METADATA;
    if (!checkHiearchyChain(hierarchy)) return QStringList();

    QString date = data.value(MAIN_FIELD_METADATA).toMap().value(RDC::MetadataField::DATE).toString();
    QString hour = data.value(MAIN_FIELD_METADATA).toMap().value(RDC::MetadataField::HOUR).toString();
    QDateTime dt = QDateTime::fromString(date + " " + hour,"yyyy-MM-dd HH:mm");
    QStringList ans; ans << dt.toString("dd/MM/yyyy HH:mm");
    ans << date << hour;
    return ans;
}

QVariantMap RawDataContainer::getApplicationUserData(const QString &type) {

    QVariantMap ret;
    QString check = RDC::AppUserType::validate(type);

    if (check != ""){
        error = "Attempting to get application user data: " + check;
        return ret;
    }

    QStringList hierarchy; hierarchy << MAIN_FIELD_APPLICATION_USER << type;
    if (!checkHiearchyChain(hierarchy)) return ret;
    return data.value(MAIN_FIELD_APPLICATION_USER).toMap().value(type).toMap();
    return ret;
}

bool RawDataContainer::isStudyOngoing(const QString &st){

    QString check = RDC::Study::validate(st);

    if (check != ""){
        error = "Attempting to get if study is ongoing: " + check;
        return false;
    }

    QStringList hierarchy; hierarchy << MAIN_FIELD_STUDIES << st << RDC::StudyField::STATUS;
    if (!checkHiearchyChain(hierarchy)) return false;
    return (data.value(MAIN_FIELD_STUDIES).toMap().value(st).toMap().value(RDC::StudyField::STATUS).toString() == STUDY_STATUS_ONGOING);
}

QStringList RawDataContainer::getTrialListTypesForStudy(const QString &st){

    QString check = RDC::Study::validate(st);

    if (check != ""){
        error = "Attempting to get trial list type: " + check;
        return QStringList();
    }

    QStringList hierarchy; hierarchy << MAIN_FIELD_STUDIES << st << RDC::StudyField::TRIAL_LIST;
    if (!checkHiearchyChain(hierarchy)) return QStringList();
    return data.value(MAIN_FIELD_STUDIES).toMap().value(st).toMap().value(RDC::StudyField::TRIAL_LIST).toMap().keys();
}



QVariantMap RawDataContainer::getSubjectData() {
    QStringList hierarchy; hierarchy << MAIN_FIELD_SUBJECT_DATA;
    if (!checkHiearchyChain(hierarchy)) return QVariantMap();
    return data.value(MAIN_FIELD_SUBJECT_DATA).toMap();
}


///////////////////////////////////////////////// SETTING DATA FUNCTIONS /////////////////////////////////////////////////////////////////////////

bool RawDataContainer::setMetadata(const QVariantMap &metadata){
    QStringList mandatory;
    mandatory = RDC::MetadataField::valid;
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
    current_metada[RDC::MetadataField::VERSION] = CURRENT_JSON_STRUCT_VERSION;

    // Saving as metadata
    data[MAIN_FIELD_METADATA] = current_metada;
    return true;
}

void RawDataContainer::addCustomMetadataFields(const QString field_name, const QString field_value){
    QVariantMap current_metada = data.value(MAIN_FIELD_METADATA).toMap();
    current_metada.insert(field_name,field_value);
    data[MAIN_FIELD_METADATA] = current_metada;
}

bool RawDataContainer::setProcessingParameters(const QVariantMap &pp){

    // All processing parameters will be considered mandatory.
    QStringList mandatory;
    mandatory << RDC::ProcessingParameter::MAX_DISPERSION_WINDOW
              << RDC::ProcessingParameter::MIN_FIXATION_DURATION
              << RDC::ProcessingParameter::RESOLUTION_HEIGHT
              << RDC::ProcessingParameter::RESOLUTION_WIDTH;

    QStringList keys = pp.keys();
    for (qint32 i = 0; i < mandatory.size(); i++){
        if (!keys.contains(mandatory.at(i))){
            error = "Cannot set processing parameters withouth field: " + mandatory.at(i);
            return false;
        }
    }

    // There could be keys that are not supposed to be here. We check.
    QString check = RDC::ProcessingParameter::validateList(keys);
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


bool RawDataContainer::setCurrentStudy(const QString &study){
    if (!data.value(MAIN_FIELD_STUDIES).toMap().contains(study)){
        error = study + " has not been configured for this container";
        return false;
    }

    currentlySelectedStudy = study;
    currentDataSetMap.clear();
    currentRawDataList.clear();
    currentTrial.clear();
    currentTrialList.clear();

    return true;

}

bool RawDataContainer::setCurrentTrialListType(const QString &tlt){
    QString check = RDC::TrialListType::validate(tlt);

    if (check != ""){
        error = "Attempting to set current trial list type: " + check;
        return false;
    }

    currentTrialListType = tlt;
    return true;
}

bool RawDataContainer::setSubjectData(const QVariantMap &subject_data){
    QStringList fields = RDC::SubjectField::valid;

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

bool RawDataContainer::setApplicationUserData(const QString &type, const QVariantMap &au_data){


    QString check = RDC::AppUserType::validate(type);
    if (check != ""){
        error = "Attempting to set application user data: " + check;
        return false;
    }

    QStringList fields = RDC::AppUserField::valid;
    QVariantMap map;

    // Will treat all fields as mandatory.
    for (qint32 i = 0; i < fields.size(); i++){
        if (!au_data.contains(fields.at(i))){
            error = QString("Application user field field ") + fields.at(i) + QString(" is required for setting the subject data");
            return false;
        }
        map[fields.at(i)] = au_data.value(fields.at(i));
    }

    QVariantMap users = data.value(MAIN_FIELD_APPLICATION_USER).toMap();
    users[type] = map;
    data[MAIN_FIELD_APPLICATION_USER] = map;

    return true;
}

bool RawDataContainer::addStudy(const QString &study, const QVariantMap &studyConfiguration, const QString &experimentDescription, const QString &version){

    error = "";
    QString check = RDC::Study::validate(study);
    if (check != ""){
        error = "While adding study: " + check;
        return false;
    }

    QStringList parameters_to_check;
    QMap<QString,QStringList> valid_parameter_values;
    valid_parameter_values[RDC::StudyParameter::VALID_EYE] = RDC::Eye::valid;

    // Set the list of the parameters to check depending on study.
    if (study == RDC::Study::BINDING){
        valid_parameter_values[RDC::StudyParameter::BINDING_TYPE] = RDC::Eye::valid;
        valid_parameter_values[RDC::StudyParameter::NUMBER_TARGETS] = RDC::BindingTargetCount::valid;
        valid_parameter_values[RDC::StudyParameter::TARGET_SIZE] = RDC::BindingTargetSize::valid;
    }
    else if (study == RDC::Study::READING){
        valid_parameter_values[ RDC::StudyParameter::LANGUAGE] = RDC::ReadingLanguage::valid;
    }
    else if (study == RDC::Study::GONOGO){

    }
    else if (study == RDC::Study::NBACKMS){

    }
    else if (study == RDC::Study::NBACKRT){

    }
    else if (study == RDC::Study::NBACKVS){
        valid_parameter_values[RDC::StudyParameter::NUMBER_TARGETS] = RDC::NBackVSTargetCount::valid;
    }
    else if (study == RDC::Study::PERCEPTION){
        valid_parameter_values[RDC::StudyParameter::PERCEPTION_PART] = RDC::PerceptionPart::valid;
        valid_parameter_values[RDC::StudyParameter::PERCEPTION_TYPE] = RDC::PerceptionType::valid;
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
    QString eye = finalStudyConfiguration.value(RDC::StudyParameter::VALID_EYE).toString();
    if (eye == RDC::Eye::LEFT) config_code = "L";
    else if (eye == RDC::Eye::RIGHT) config_code = "R";
    else config_code = "B";

    studyToConfigure.insert(RDC::StudyField::EXPERIMENT_DESCRIPTION,experimentDescription);
    studyToConfigure.insert(RDC::StudyField::VERSION,version);
    studyToConfigure.insert(RDC::StudyField::STATUS,RawDataContainer::STUDY_STATUS_ONGOING);

    //studyToConfigure[RDC::StudyParameter::VALID_EYE] = mapStudyValues.value(studyConfiguration[SCP_EYES]);

    // Specific study configurations.
    QString abbreviation;
    if (study == RDC::Study::BINDING){
        config_code = config_code + finalStudyConfiguration.value(RDC::StudyParameter::NUMBER_TARGETS).toString() +
                finalStudyConfiguration.value(RDC::StudyParameter::TARGET_SIZE).toString().mid(0,1).toUpper();
        if (finalStudyConfiguration.value(RDC::StudyParameter::BINDING_TYPE).toString() == RDC::BindingType::BOUND) abbreviation = "bc";
        else abbreviation = "uc";
    }
    else if (study == RDC::Study::READING){
        config_code = config_code + finalStudyConfiguration.value(RDC::StudyParameter::LANGUAGE).toString().mid(0,2).toUpper();
        abbreviation = "rd";
    }
    else if (study == RDC::Study::GONOGO){
        abbreviation = "gn";
    }
    else if (study == RDC::Study::NBACKMS){
        abbreviation = "ms";
    }
    else if (study == RDC::Study::NBACKRT){
        abbreviation = "rt";
    }
    else if (study == RDC::Study::NBACKVS){
        config_code = config_code + finalStudyConfiguration.value(RDC::StudyParameter::NUMBER_TARGETS).toString();
        abbreviation = "vs";
    }
    else if (study == RDC::Study::PERCEPTION){
        valid_parameter_values[RDC::StudyParameter::PERCEPTION_PART] = RDC::PerceptionPart::valid;
        valid_parameter_values[RDC::StudyParameter::PERCEPTION_TYPE] = RDC::PerceptionType::valid;
        config_code = config_code + finalStudyConfiguration.value(RDC::StudyParameter::PERCEPTION_TYPE).toString().mid(0,1).toUpper();
    }


    studyToConfigure.insert(RDC::StudyField::STUDY_CONFIGURATION,finalStudyConfiguration);
    studyToConfigure.insert(RDC::StudyField::CONFIG_CODE,config_code);
    studyToConfigure.insert(RDC::StudyField::ABBREVIATION,abbreviation);
    studies.insert(study,studyToConfigure);
    data[MAIN_FIELD_STUDIES] = studies;

    return true;

}


bool RawDataContainer::markStudyAsFinalized(const QString &studyname){
    if (!data.value(MAIN_FIELD_STUDIES).toMap().contains(studyname)) return false;
    QVariantMap studies = data.value(MAIN_FIELD_STUDIES).toMap();
    QVariantMap study = studies.value(studyname).toMap();
    study[RDC::StudyField::STATUS] = STUDY_STATUS_FINALIZED;
    studies[MAIN_FIELD_STUDIES] = study;
    data[MAIN_FIELD_STUDIES] = studies;
    return true;
}



////////////////////////////////////// CREATING HIEARARCHY //////////////////////////////////////

bool RawDataContainer::addNewTrial(const QString &trial_id, const QString &type){
    currentTrial.clear();
    currentRawDataList.clear();

    // All trials have a data subfield.
    currentTrial.insert(RDC::TrialField::ID,trial_id);
    currentTrial.insert(RDC::TrialField::DATA,QVariantMap());
    currentTrial.insert(RDC::TrialField::TRIAL_TYPE,type);
    currentTrial.insert(RDC::TrialField::RESPONSE,"");

    if (type == "") {
        error = "Trial Type cannot be empty";
        return false;
    }

    return true;
}

bool RawDataContainer::setCurrentDataSet(const QString &data_set_type){
    QString check = RDC::DataSetType::validate(data_set_type);
    if (check != ""){
        error = "While setting the data set type: " + check;
        return false;
    }
    currentDataSetType = data_set_type;
    return true;
}

void RawDataContainer::finalizeDataSet(){

    QVariantMap currentDataSetRawData;

    // Creating the raw data field in the data set where the actual data is stored.
    currentDataSetRawData.insert(RDC::DataStudyField::RAW_DATA,currentRawDataList);

    // Setting the current data set to its defined type.
    currentDataSetMap.insert(currentDataSetType,currentDataSetRawData);

    // Clearing the data that was just added.
    currentRawDataList.clear();

}

void RawDataContainer::finalizeTrial(const QString &response){
    // All trials must contain a response. Even if empty.
    currentTrial[RDC::TrialField::RESPONSE] = response;

    // Saving the data sets to the trial data field.
    currentTrial.insert(RDC::TrialField::DATA,currentDataSetMap);

    // Adding the trial to the trial list.
    currentTrialList << currentTrial;

    // Clearing the trial that was just added.
    currentTrial.clear();
}

bool RawDataContainer::finalizeStudy(){

    QVariantMap trialList;

    // Setting the trial list to a map with the trial list type as the key.
    trialList.insert(currentTrialListType,currentTrialList);

    // Getting the current study structure from the current studies list.
    QString studyName =currentlySelectedStudy;

    if (studyName == ""){
        error = "Trying to finalize an unitialized study";
        return false;
    }

    QVariantMap studies = data.value(MAIN_FIELD_STUDIES).toMap();
    QVariantMap study = studies.value(studyName).toMap();

    // Storing the trial list for the study.
    study.insert(RDC::StudyField::TRIAL_LIST,trialList);

    //qDebug() << "Finalizing Study" << studyName << " of value " << currentlySelectedStudy;

    studies[studyName] = study;
    data[MAIN_FIELD_STUDIES] = studies;

    // Clearing all data.
    currentDataSetMap.clear();
    currentTrial.clear();
    currentRawDataList.clear();
    currentTrialList.clear();

    return true;

}

void RawDataContainer::addNewRawDataVector(const QVariantMap &raw_data_vector){
    // This should be checked, however in order to optimize peformance (just in case) no check is done. As long as
    // the vector is correctly constructed once all checks will always pass.
    currentRawDataList << raw_data_vector;
}


////////////////////////////////////// HIEARCHY CHECK FUNCTION //////////////////////////////////////
bool RawDataContainer::checkHiearchyChain(const QStringList &hieararchy) {
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

QVariantMap RawDataContainer::GenerateReadingRawDataVector(float timestamp, float xr, float yr, float xl, float yl, float pr, float pl, float char_r, float char_l, float word_r, float word_l){
    QVariantMap vector = RawDataContainer::GenerateStdRawDataVector(timestamp,xr,yr,xl,yl,pr,pl);
    vector.insert(RDC::DataVectorField::CHAR_L,char_l);
    vector.insert(RDC::DataVectorField::CHAR_R,char_r);
    vector.insert(RDC::DataVectorField::WORD_L,word_l);
    vector.insert(RDC::DataVectorField::WORD_R,word_r);
    return vector;
}

QVariantMap RawDataContainer::GenerateStdRawDataVector(float timestamp, float xr, float yr, float xl, float yl, float pr, float pl){
    QVariantMap vector;
    vector.insert(RDC::DataVectorField::TIMESTAMP,timestamp);
    vector.insert(RDC::DataVectorField::X_L,xl);
    vector.insert(RDC::DataVectorField::X_R,xr);
    vector.insert(RDC::DataVectorField::Y_L,yl);
    vector.insert(RDC::DataVectorField::Y_R,yr);
    vector.insert(RDC::DataVectorField::PUPIL_L,pl);
    vector.insert(RDC::DataVectorField::PUPIL_R,pr);
    return vector;

}

////////////////////////////////////// STATIC STRING INITIALIZATION //////////////////////////////////////

QString RawDataContainer::MAIN_FIELD_SUBJECT_DATA                = "subject";
QString RawDataContainer::MAIN_FIELD_PROCESSING_PARAMETERS       = "processing_parameters";
QString RawDataContainer::MAIN_FIELD_FREQUENCY_CHECK_PARAMETERS  = "frequency_check_parameters";
QString RawDataContainer::MAIN_FIELD_METADATA                    = "metadata";
QString RawDataContainer::MAIN_FIELD_APPLICATION_USER            = "application_user";
QString RawDataContainer::MAIN_FIELD_STUDIES                     = "studies";

QString RawDataContainer::CURRENT_JSON_STRUCT_VERSION            = "1";

QString RawDataContainer::STUDY_STATUS_FINALIZED                 = "finalized";
QString RawDataContainer::STUDY_STATUS_ONGOING                   = "ongoing";
