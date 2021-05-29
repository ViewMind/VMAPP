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

QString RawDataContainer::getError() const{
    return error;
}

bool RawDataContainer::setMetadata(const Metadata &metadata){
    QList<MetadataField> mandatory;
    mandatory << MF_DATE << MF_HOUR << MF_INSTITUTION_ID << MF_INSTITUTION_INSTANCE << MF_INSTITUTION_KEY << MF_INSTITUTION_NAME << MF_PROCESSING_PARAMETER_KEY;
    QList<MetadataField> keys = metadata.keys();
    for (qint32 i = 0; i < mandatory.size(); i++){
        if (!keys.contains(mandatory.at(i))){
            error = "Cannot set metadata withouth field: "; error = error + mapMetadataField.value(mandatory.at(i));
            return false;
        }
    }

    // In order to make sure no previously set data is lost.
    QVariantMap current_metada = data.value(MAIN_FIELD_METADATA).toMap();
    for (qint32 i = 0; i < mandatory.size(); i++){
        current_metada[mapMetadataField.value(mandatory.at(i))] = metadata.value(mandatory.at(i));
    }

    // Ensuring the proper version.
    current_metada[mapMetadataField.value(MF_VERSION)] = CURRENT_JSON_STRUCT_VERSION;

    // Saving as metadata
    data[MAIN_FIELD_METADATA] = current_metada;
    return true;
}

void RawDataContainer::addCustomMetadataFields(const QString field_name, const QString field_value){
    QVariantMap current_metada = data.value(MAIN_FIELD_METADATA).toMap();
    current_metada.insert(field_name,field_value);
    data[MAIN_FIELD_METADATA] = current_metada;
}

bool RawDataContainer::setProcessingParameters(const ProcessingParameters &pp){
    QList<ProcessingParameterField> mandatory;
    mandatory << PP_RESOLUTION_HEIGHT << PP_RESOLUTION_WIDTH;
    QList<ProcessingParameterField> keys = pp.keys();
    for (qint32 i = 0; i < mandatory.size(); i++){
        if (!keys.contains(mandatory.at(i))){
            error = "Cannot set processing parameters withouth field: "; error = error + mapProcessingParameterField.value(mandatory.at(i));
            return false;
        }
    }

    QVariantMap ppmap;
    for (qint32 i = 0; i < keys.size(); i++){
        ppmap.insert(mapProcessingParameterField.value(keys.at(i)),pp.value(keys.at(i)));
    }

    data[MAIN_FIELD_PROCESSING_PARAMETERS] = ppmap;
    return true;
}

bool RawDataContainer::setApplicationUserData(const AppUserType &type, const ApplicationUserData &au_data){
    QList<AppUserField> mandatory;
    mandatory << AUF_EMAIL << AUF_LASTNAME << AUF_LASTNAME << AUF_LOCAL_ID << AUF_NAME;
    QList<AppUserField> keys = au_data.keys();
    for (qint32 i = 0; i < mandatory.size(); i++){
        if (!keys.contains(mandatory.at(i))){
            error = "Cannot set application user data withouth field: "; error = error + mapAppUserField.value(mandatory.at(i));
            return false;
        }
    }

    QVariantMap appuser_map = data.value(MAIN_FIELD_APPLICATION_USER).toMap();
    QVariantMap au_struct;
    for (qint32 i = 0; i < keys.size(); i++){
        au_struct.insert(mapAppUserField.value(keys.at(i)),au_data.value(keys.at(i)));
    }
    appuser_map.insert(mapAppUserType.value(type),au_struct);
    data[MAIN_FIELD_APPLICATION_USER] = appuser_map;
    return true;
}

bool RawDataContainer::setSubjectData(const SubjectData &subject_data){
    QList<SubjectField> mandatory;
    mandatory << SF_AGE << SF_BIRHTCOUNTRY << SF_BIRTHDATE << SF_GENDER << SF_INSTITUTION_PROVIDED_ID << SF_LASTNAME << SF_LOCAL_ID << SF_NAME << SF_YEARS_FORMATION;
    QList<SubjectField> keys = subject_data.keys();
    for (qint32 i = 0; i < mandatory.size(); i++){
        if (!keys.contains(mandatory.at(i))){
            error = "Cannot set subject data withouth field: "; error = error + mapSubjectField.value(mandatory.at(i));
            return false;
        }
    }

    QVariantMap subject;
    for (qint32 i = 0; i < keys.size(); i++){
        subject.insert(mapSubjectField.value(keys.at(i)),subject_data.value(keys.at(i)));
    }
    data[MAIN_FIELD_SUBJECT_DATA] = subject;
    return true;
}

bool RawDataContainer::setCurrentStudy(const StudyType &study){
    QString studyName = mapStudyType.value(study);
    if (!data.value(MAIN_FIELD_STUDIES).toMap().contains(studyName)){
        error = studyName + " has not been configured for this container";
        return false;
    }

    currentlySelectedStudy = study;
    currentDataSetMap.clear();
    currentRawDataList.clear();
    currentTrial.clear();
    currentTrialList.clear();

    return true;

}

void RawDataContainer::setCurrentTrialListType(const TrialListType &tlt){
    currentTrialListType = mapTrialListTypes.value(tlt);
}


bool RawDataContainer::addStudy(const StudyType &study, const StudyConfiguration &studyConfiguration, const QString &experimentDescription, const QString &version){

    error = "";

    QVariantMap studies = data.value(MAIN_FIELD_STUDIES,QVariantMap()).toMap();
    QVariantMap studyParameters;
    QVariantMap studyToConfigure;

    // Checking required configuration paramerts.
    if (!studyConfiguration.contains(SCP_EYES)){
        error = "Any study requires eye configuration parameter";
        return false;
    }

    // Checking that the eye parameter is valid.
    QSet<StudyConfigurationValue> valid_eyes;
    valid_eyes << SCV_EYE_LEFT << SCV_EYE_RIGHT << SCV_EYE_BOTH;
    if (!valid_eyes.contains(studyConfiguration.value(SCP_EYES))){
        error = QString("Invalid eye configuration value of ") + mapStudyValues.value(studyConfiguration.value(SCP_EYES));
        return false;
    }

    studyToConfigure.insert(mapStudiesField.value(STF_EXPERIMENT_DESCRIPTION),experimentDescription);
    studyToConfigure.insert(mapStudiesField.value(STF_VERSION),version);
    studyParameters[mapStudyParmeters.value(SCP_EYES)] = mapStudyValues.value(studyConfiguration[SCP_EYES]);

    // Getting the study name.
    QString studyName = mapStudyType.value(study);

    // Configuring for each particular study.
    switch (study){
    case STUDY_READING:        
        studyParameters = configureReadingStudy(studyConfiguration,studyParameters);
        break;
    case STUDY_NBACKRT:
        // Nothing to do
        break;
    case STUDY_BINDING:
        studyParameters = configureBindingStudy(studyConfiguration,studyParameters);
        break;
    case STUDY_GONOGO:
        // Nothing to do.
        break;
    default:
        error = QString("Attempting to configure container for an unsupported study ") + mapStudyType.value(study);
        return false;
    }

    // Checking errors when configuring for a particular experiment.
    if (error != "") return false;

    studyToConfigure.insert(mapStudiesField.value(STF_STUDY_CONFIGURATION),studyParameters);
    studies.insert(studyName,studyToConfigure);
    data[MAIN_FIELD_STUDIES] = studies;

    return true;

}


////////////////////////////////////// CONFIGURING EACH INDIVIDUAL EXPERIMENT //////////////////////////////////////

QVariantMap RawDataContainer::configureReadingStudy(const StudyConfiguration &params, QVariantMap studyConfiguration){
    // Checking required configuration parameters.
    if (!params.contains(SCP_LANGUAGE)){
        error = "Reading study requires language configuration parameter";
        return studyConfiguration;
    }

    // Checking a valid language.
    QSet<StudyConfigurationValue> valid;
    valid << SCV_LANG_IS << SCV_LANG_FR << SCV_LANG_ES << SCV_LANG_DE << SCV_LANG_EN;
    if (!valid.contains(params.value(SCP_LANGUAGE))){
        error = QString("Invalid language configuration value of ") + mapStudyValues.value(params.value(SCP_LANGUAGE));
        return studyConfiguration;
    }

    // Saving the language configuration.
    studyConfiguration[mapStudyParmeters.value(SCP_LANGUAGE)] = mapStudyValues.value(params[SCP_LANGUAGE]);

    return studyConfiguration;
}

QVariantMap RawDataContainer::configureBindingStudy(const StudyConfiguration &params, QVariantMap studyConfiguration){

    QSet<StudyConfigurationValue> valid;

    // Checking required configuration parameters.
    if (!params.contains(SCP_BINDING_TYPE)){
        error = "Binding study requires binding type configuration parameter";
        return studyConfiguration;
    }

    valid.clear();
    valid << SCV_BINDING_TYPE_BOUND << SCV_BINDING_TYPE_UNBOUND;
    if (!valid.contains(params.value(SCP_BINDING_TYPE))){
        error = QString("Invalid biding type of ") + mapStudyValues.value(params.value(SCP_BINDING_TYPE));
        return studyConfiguration;
    }
    //---

    if (!params.contains(SCP_NUMBER_OF_TARGETS)){
        error = "Binding study requires number of targets configuration parameter";
        return studyConfiguration;
    }

    valid.clear();
    valid << SCV_BINDING_TARGETS_2 << SCV_BINDING_TARGETS_3;
    if (!valid.contains(params.value(SCP_NUMBER_OF_TARGETS))){
        error = QString("Invalid number of targets for binding ") + mapStudyValues.value(params.value(SCP_NUMBER_OF_TARGETS));
        return studyConfiguration;
    }

    //---

    if (!params.contains(SCP_TARGET_SIZE)){
        error = "Binding study requires target size configuration parameter";
        return studyConfiguration;
    }

    valid.clear();
    valid << SCV_BINDING_TARGETS_SMALL << SCV_BINDING_TARGETS_LARGE;
    if (!valid.contains(params.value(SCP_TARGET_SIZE))){
        error = QString("Invalid target size for binding ") + mapStudyValues.value(params.value(SCP_TARGET_SIZE));
        return studyConfiguration;
    }

    //---

    // Saving the configuration.
    studyConfiguration[mapStudyParmeters.value(SCP_BINDING_TYPE)] = mapStudyValues.value(params[SCP_BINDING_TYPE]);
    studyConfiguration[mapStudyParmeters.value(SCP_NUMBER_OF_TARGETS)] = mapStudyValues.value(params[SCP_NUMBER_OF_TARGETS]);
    studyConfiguration[mapStudyParmeters.value(SCP_TARGET_SIZE)] = mapStudyValues.value(params[SCP_TARGET_SIZE]);

    return studyConfiguration;
}


////////////////////////////////////// CREATING HIEARARCHY //////////////////////////////////////

bool RawDataContainer::addNewTrial(const QString &trial_id, const TrialConfiguration &trial_configuration){
    currentTrial.clear();
    currentRawDataList.clear();

    // All trials have a data subfield.
    currentTrial.insert(mapTrialFields.value(TF_ID),trial_id);
    currentTrial.insert(mapTrialFields.value(TF_DATA),QVariantMap());

    // Other than the data field, each study has it's own trial structure.
    switch (currentlySelectedStudy){
    case STUDY_READING:
        if (!trial_configuration.contains(TF_SENTENCE)){
            error = "A Reading trial must contain a sentence trial field";
            return false;
        }
        currentTrial.insert(mapTrialFields.value(TF_SENTENCE),trial_configuration.value(TF_SENTENCE));
        currentTrial.insert(mapTrialFields.value(TF_RESPONSE),"");
        break;
    case STUDY_NBACKRT:
        if (!trial_configuration.contains(TF_TRIAL_TYPE)){
            error = "A NBackRT trial must contain a trial type field";
            return false;
        }
        currentTrial.insert(mapTrialFields.value(TF_TRIAL_TYPE),trial_configuration.value(TF_TRIAL_TYPE));
        currentTrial.insert(mapTrialFields.value(TF_RESPONSE),"");
        break;
    case STUDY_BINDING:
        if (!trial_configuration.contains(TF_TRIAL_TYPE)){
            error = "A Binding trial must contain a trial type field";
            return false;
        }
        currentTrial.insert(mapTrialFields.value(TF_TRIAL_TYPE),trial_configuration.value(TF_TRIAL_TYPE));
        currentTrial.insert(mapTrialFields.value(TF_RESPONSE),"");
        break;
    case STUDY_GONOGO:
        if (!trial_configuration.contains(TF_TRIAL_TYPE)){
            error = "A GoNoGo trial must contain a trial type field";
            return false;
        }
        currentTrial.insert(mapTrialFields.value(TF_TRIAL_TYPE),trial_configuration.value(TF_TRIAL_TYPE));
        currentTrial.insert(mapTrialFields.value(TF_RESPONSE),"");
        break;
    default:
        error = "Attempting to configure container for an unsupported study";
        return false;
    }

    return true;
}

void RawDataContainer::setCurrentDataSet(const DataSetType &data_set_type){
    currentDataSetType = mapDataSetTypes.value(data_set_type);
}

void RawDataContainer::finalizeDataSet(){

    QVariantMap currentDataSetRawData;

    // Creating the raw data field in the data set where the actual data is stored.
    currentDataSetRawData.insert(mapDataSetFields.value(DSF_RAW_DATA),currentRawDataList);

    // Setting the current data set to its defined type.
    currentDataSetMap.insert(currentDataSetType,currentDataSetRawData);

    // Clearing the data that was just added.
    currentRawDataList.clear();

}

void RawDataContainer::finalizeTrial(const QString &response){
    // All trials must contain a response. Even if empty.
    currentTrial[mapTrialFields.value(TF_RESPONSE)] = response;

    // Saving the data sets to the trial data field.
    currentTrial.insert(mapTrialFields.value(TF_DATA),currentDataSetMap);

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
    QString studyName = mapStudyType.value(currentlySelectedStudy);

    if (studyName == ""){
        error = "Trying to finalize an unitialized study";
        return false;
    }

    QVariantMap studies = data.value(MAIN_FIELD_STUDIES).toMap();
    QVariantMap study = studies.value(studyName).toMap();

    // Storing the trial list for the study.
    study.insert(mapStudiesField.value(STF_TRIAL_LIST),trialList);

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

////////////////////////////////////// STATIC VECTOR CREATION FUNCTIONS //////////////////////////////////////
QVariantMap RawDataContainer::GenerateReadingRawDataVector(float timestamp, float xr, float yr, float xl, float yl, float pr, float pl, float char_r, float char_l, float word_r, float word_l){
    QVariantMap vector = RawDataContainer::GenerateStdRawDataVector(timestamp,xr,yr,xl,yl,pr,pl);
    vector.insert(mapValueVectorParameters.value(VVP_CHAR_L),char_l);
    vector.insert(mapValueVectorParameters.value(VVP_CHAR_R),char_r);
    vector.insert(mapValueVectorParameters.value(VVP_WORD_L),word_l);
    vector.insert(mapValueVectorParameters.value(VVP_WORD_R),word_r);
    return vector;
}

QVariantMap RawDataContainer::GenerateStdRawDataVector(float timestamp, float xr, float yr, float xl, float yl, float pr, float pl){
    QVariantMap vector;
    vector.insert(mapValueVectorParameters.value(VVP_TIMESTAMP),timestamp);
    vector.insert(mapValueVectorParameters.value(VVP_XL),xl);
    vector.insert(mapValueVectorParameters.value(VVP_XR),xr);
    vector.insert(mapValueVectorParameters.value(VVP_YL),yl);
    vector.insert(mapValueVectorParameters.value(VVP_YR),yr);
    vector.insert(mapValueVectorParameters.value(VVP_PUPIL_L),pl);
    vector.insert(mapValueVectorParameters.value(VVP_PUPIL_R),pr);
    return vector;

}

////////////////////////////////////// STATIC MAP INTIALIZATION FUNCTIONS //////////////////////////////////////


QMap<RawDataContainer::StudyType,const char*> RawDataContainer::initStudyTypeMap(){
    QMap<RawDataContainer::StudyType,const char*> map;
    map.insert(STUDY_BINDING,"Binding");
    map.insert(STUDY_GONOGO,"Go No-Go");
    map.insert(STUDY_NBACKMS,"NBack MS");
    map.insert(STUDY_NBACKRT,"NBack RT");
    map.insert(STUDY_NBACKVS,"NBack VS");
    map.insert(STUDY_PERCEPTION, "Perception");
    map.insert(STUDY_READING, "Reading");
    return map;
}


QMap<RawDataContainer::StudyConfigurationParameters,const char*> RawDataContainer::initStudyParametersMap(){
    QMap<RawDataContainer::StudyConfigurationParameters,const char*> map;
    map.insert(SCP_EYES,"valid_eye");
    map.insert(SCP_LANGUAGE,"language");
    map.insert(SCP_NUMBER_OF_TARGETS,"number_targets");
    map.insert(SCP_TARGET_SIZE,"target_size");
    map.insert(SCP_BINDING_TYPE,"binding_type");
    return map;
}

QMap<RawDataContainer::StudyConfigurationValue,const char*> RawDataContainer::initStudyValuesMap(){
    QMap<RawDataContainer::StudyConfigurationValue,const char*> map;
    map.insert(SCV_BINDING_TARGETS_2,"2");
    map.insert(SCV_BINDING_TARGETS_3,"3");
    map.insert(SCV_BINDING_TARGETS_LARGE,"Large");
    map.insert(SCV_BINDING_TARGETS_SMALL,"Small");
    map.insert(SCV_EYE_BOTH,"Both");
    map.insert(SCV_EYE_LEFT,"Left");
    map.insert(SCV_EYE_RIGHT,"Right");
    map.insert(SCV_LANG_DE,"German");
    map.insert(SCV_LANG_ES,"Spanish");
    map.insert(SCV_LANG_FR,"French");
    map.insert(SCV_LANG_EN,"English");
    map.insert(SCV_LANG_IS,"Iselandic");
    map.insert(SCV_BINDING_TYPE_BOUND,"bound");
    map.insert(SCV_BINDING_TYPE_UNBOUND,"unbound");
    return map;
}

QMap<RawDataContainer::TrialListType,const char*> RawDataContainer::initTrialListTypesMap(){
    QMap<RawDataContainer::TrialListType,const char*> map;
    map.insert(TLT_BOUND,"bound");
    map.insert(TLT_UNBOUND,"unbound");
    map.insert(TLT_UNIQUE,"unique");
    return map;
}

QMap<RawDataContainer::ValueVectorParameter,const char*> RawDataContainer::initValueVectorParameters(){
    QMap<RawDataContainer::ValueVectorParameter,const char*> map;
    map.insert(VVP_TIMESTAMP,"ts");
    map.insert(VVP_CHAR_L,"cl");
    map.insert(VVP_CHAR_R,"cr");
    map.insert(VVP_PUPIL_L,"pl");
    map.insert(VVP_PUPIL_R,"pr");
    map.insert(VVP_WORD_L,"wl");
    map.insert(VVP_WORD_R,"wr");
    map.insert(VVP_XL,"xl");
    map.insert(VVP_XR,"xr");
    map.insert(VVP_YL,"yl");
    map.insert(VVP_YR,"yr");
    return map;
}

QMap<RawDataContainer::DataSetField,const char*> RawDataContainer::initDataSetFieldsMap(){
    QMap<RawDataContainer::DataSetField,const char*> map;
    map.insert(DSF_RAW_DATA,"raw_data");
    return map;
}

QMap<RawDataContainer::DataSetType,const char*> RawDataContainer::initDataSetTypesMap(){
    QMap<RawDataContainer::DataSetType,const char*> map;
    map.insert(DST_ENCODING_1,"encoding_1");
    map.insert(DST_ENCODING_2,"encoding_2");
    map.insert(DST_ENCODING_3,"encoding_3");
    map.insert(DST_ENCODING_4,"encoding_4");
    map.insert(DST_ENCODING_5,"encoding_5");
    map.insert(DST_ENCODING_6,"encoding_6");
    map.insert(DST_RETRIEVAL_1,"reterieval_1");
    map.insert(DST_RETRIEVAL_2,"reterieval_2");
    map.insert(DST_RETRIEVAL_3,"reterieval_3");
    map.insert(DST_UNIQUE,"unique");
    return map;
}


QMap<RawDataContainer::TrialField,const char*> RawDataContainer::initTrialFieldsMap(){
    QMap<RawDataContainer::TrialField,const char*> map;
    map.insert(TF_DATA,"data");
    map.insert(TF_RESPONSE,"response");
    map.insert(TF_SENTENCE,"sentence");
    map.insert(TF_TRIAL_TYPE,"trial_type");
    map.insert(TF_ID,"ID");
    return map;
}

QMap<RawDataContainer::MetadataField,const char*> RawDataContainer::initMetadataFieldMap(){
    QMap<RawDataContainer::MetadataField,const char*> map;
    map.insert(MF_DATE,"date");
    map.insert(MF_HOUR,"hour");
    map.insert(MF_INSTITUTION_ID,"institution_id");
    map.insert(MF_INSTITUTION_INSTANCE,"institution_instance");
    map.insert(MF_INSTITUTION_KEY,"institution_key");
    map.insert(MF_INSTITUTION_NAME,"institution_name");
    map.insert(MF_VERSION,"version");
    map.insert(MF_PROCESSING_PARAMETER_KEY,"processing_parameters_key");
    return map;
}
QMap<RawDataContainer::SubjectField,const char*>  RawDataContainer::initSubjectFieldMap(){
    QMap<RawDataContainer::SubjectField,const char*> map;
    map.insert(SF_BIRHTCOUNTRY,"birth_country");
    map.insert(SF_BIRTHDATE,"birth_date");
    map.insert(SF_YEARS_FORMATION,"years_formation");
    map.insert(SF_GENDER,"gender");
    map.insert(SF_AGE,"age");
    map.insert(SF_INSTITUTION_PROVIDED_ID,"instituion_provided_id");
    map.insert(SF_LASTNAME,"lastname");
    map.insert(SF_LOCAL_ID,"local_id");
    map.insert(SF_NAME,"name");
    return map;
}
QMap<RawDataContainer::AppUserField,const char*>  RawDataContainer::initAppUserFieldMap(){
    QMap<RawDataContainer::AppUserField,const char*> map;
    map.insert(AUF_EMAIL,"email");
    map.insert(AUF_LASTNAME,"lastname");
    map.insert(AUF_NAME,"name");
    map.insert(AUF_LOCAL_ID,"local_id");
    return map;
}
QMap<RawDataContainer::AppUserType,const char*>   RawDataContainer::initAppUserTypeMap(){
    QMap<RawDataContainer::AppUserType,const char*> map;
    map.insert(AT_EVALUATOR,"evaluator");
    map.insert(AT_TEST_TAKER,"test_taker");
    return map;
}

QMap<RawDataContainer::StudiesField,const char*>   RawDataContainer::initStudiesFieldMap(){
    QMap<RawDataContainer::StudiesField,const char*> map;
    map.insert(STF_EXPERIMENT_DESCRIPTION,"experiment_description");
    map.insert(STF_STUDY_CONFIGURATION,"study_configuration");
    map.insert(STF_TRIAL_LIST,"trial_list");
    map.insert(STF_VERSION,"version");
    return map;
}

QMap<RawDataContainer::ProcessingParameterField,const char*> RawDataContainer::initProcessingParametersMap(){
    QMap<RawDataContainer::ProcessingParameterField,const char*> map;
    map.insert(PP_RESOLUTION_HEIGHT,"resolution_height");
    map.insert(PP_RESOLUTION_WIDTH,"resolution_width");
    map.insert(PP_NBACK_HITBOXES,"nback_hitboxes");
    map.insert(PP_GONOGO_HITBOXES,"go_no_go_hitboxes");
    return map;
}

const QMap<RawDataContainer::StudyType,const char*> RawDataContainer::mapStudyType = initStudyTypeMap();
const QMap<RawDataContainer::StudyConfigurationParameters,const char*> RawDataContainer::mapStudyParmeters = RawDataContainer::initStudyParametersMap();
const QMap<RawDataContainer::StudyConfigurationValue,const char*> RawDataContainer::mapStudyValues = RawDataContainer::initStudyValuesMap();
const QMap<RawDataContainer::TrialListType,const char*> RawDataContainer::mapTrialListTypes = RawDataContainer::initTrialListTypesMap();
const QMap<RawDataContainer::DataSetField,const char*> RawDataContainer::mapDataSetFields = RawDataContainer::initDataSetFieldsMap();
const QMap<RawDataContainer::DataSetType,const char*> RawDataContainer::mapDataSetTypes = RawDataContainer::initDataSetTypesMap();
const QMap<RawDataContainer::TrialField,const char*> RawDataContainer::mapTrialFields = RawDataContainer::initTrialFieldsMap();
const QMap<RawDataContainer::ValueVectorParameter,const char*> RawDataContainer::mapValueVectorParameters = RawDataContainer::initValueVectorParameters();
const QMap<RawDataContainer::MetadataField,const char*> RawDataContainer::mapMetadataField = RawDataContainer::initMetadataFieldMap();
const QMap<RawDataContainer::SubjectField,const char*>  RawDataContainer::mapSubjectField  = RawDataContainer::initSubjectFieldMap();   
const QMap<RawDataContainer::AppUserField,const char*>  RawDataContainer::mapAppUserField  = RawDataContainer::initAppUserFieldMap(); 
const QMap<RawDataContainer::AppUserType,const char*>   RawDataContainer::mapAppUserType   = RawDataContainer::initAppUserTypeMap();
const QMap<RawDataContainer::StudiesField,const char*>   RawDataContainer::mapStudiesField = RawDataContainer::initStudiesFieldMap();
const QMap<RawDataContainer::ProcessingParameterField,const char*> RawDataContainer::mapProcessingParameterField = RawDataContainer::initProcessingParametersMap();

const char * RawDataContainer::MAIN_FIELD_SUBJECT_DATA                = "subject";
const char * RawDataContainer::MAIN_FIELD_PROCESSING_PARAMETERS       = "processing_parameters";
const char * RawDataContainer::MAIN_FIELD_FREQUENCY_CHECK_PARAMETERS  = "frequency_check_parameters";
const char * RawDataContainer::MAIN_FIELD_METADATA                    = "metadata";
const char * RawDataContainer::MAIN_FIELD_APPLICATION_USER            = "application_user";
const char * RawDataContainer::MAIN_FIELD_STUDIES                     = "studies";

const char * RawDataContainer::CURRENT_JSON_STRUCT_VERSION            = "1";



