#include "rawdatacontainer.h"

RawDataContainer::RawDataContainer()
{
    error = "";
}

QString RawDataContainer::getJSONString() const{
    QJsonDocument json = QJsonDocument::fromVariant(data);
    return QString(json.toJson());
}

QString RawDataContainer::getError() const{
    return error;
}

bool RawDataContainer::setStudy(const StudyType &study, const StudyConfiguration &studyConfiguration){
    // Resetting just in case.
    data.clear();
    QVariantMap metadata;
    metadata[FIELD_VERSION] = CURRENT_JSON_STRUCT_VERSION; // Version 1 of the JSON Data.
    data[MAIN_FIELD_METADATA] = metadata;
    data[MAIN_FIELD_EXPERIMENT_DESCRIPTION] = "";
    data[MAIN_FIELD_FREQUENCY_CHECK_PARAMETERS] = QVariantMap();
    data[MAIN_FIELD_PROCESSING_PARAMETERS] = QVariantMap();
    data[MAIN_FIELD_TRIAL_LIST] = QVariantMap();
    data[MAIN_FIELD_SUBJECT_DATA] = QVariantMap();

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


    // Saving the eye configuration.
    metadata = data.value(MAIN_FIELD_METADATA).toMap();
    metadata[mapStudyParmeters.value(SCP_EYES)] = mapStudyValues.value(studyConfiguration[SCP_EYES]);
    data[MAIN_FIELD_METADATA] = metadata;

    // Configuring for each particular study.
    switch (study){
    case STUDY_READING:
        return configureReadingStudy(studyConfiguration);
    default:
        error = "Attempting to configure container for an unsupported study";
        return false;
    }

    return true;

}


////////////////////////////////////// CONFIGURING EACH INDIVIDUAL EXPERIMENT //////////////////////////////////////

bool RawDataContainer::configureReadingStudy(const StudyConfiguration &params){
    // Checking required configuration parameters.
    if (!params.contains(SCP_LANGUAGE)){
        error = "Reading study requires language configuration parameter";
        return false;
    }

    currentlySelectedStudy = STUDY_READING;

    // Checking a valid language.
    QSet<StudyConfigurationValue> valid;
    valid << SCV_LANG_IS << SCV_LANG_FR << SCV_LANG_ES << SCV_LANG_DE << SCV_LANG_EN;
    if (!valid.contains(params.value(SCP_LANGUAGE))){
        error = QString("Invalid eye configuration value of ") + mapStudyValues.value(params.value(SCP_EYES));
        return false;
    }

    // Saving the language configuration.
    QVariantMap metadata = data[MAIN_FIELD_METADATA].toMap();
    metadata[mapStudyParmeters.value(SCP_LANGUAGE)] = mapStudyValues.value(params[SCP_LANGUAGE]);
    data[MAIN_FIELD_METADATA] = metadata;

    // Creating a "Unique" Data Set List
    currentTrialListType = mapDataSetListTypes.value(TLT_UNIQUE);

    return true;
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
    default:
        error = "Attempting to configure container for an unsupported study";
        return false;
    }

    return true;
}

void RawDataContainer::addNewDataSet(const DataSetType &data_set_type){
    currentDataSetType = mapDataSetTypes.value(data_set_type);
}

void RawDataContainer::finalizeDataSet(){

    QVariantMap currentDataSetRawData;

    // Creating the raw data field in the data set where the actual data is stored.
    currentDataSetRawData.insert(mapDataSetFields.value(DSF_RAW_DATA),currentRawDataList);

    // Setting the current data set to its defined type.
    currentDataSet.insert(currentDataSetType,currentDataSetRawData);

    // Clearing the data that was just added.
    currentRawDataList.clear();

}

void RawDataContainer::finalizeTrial(const QString &response){
    if (currentTrial.contains(mapTrialFields.value(TF_RESPONSE))){
        currentTrial[mapTrialFields.value(TF_RESPONSE)] = response;
    }

    // Saving the data sets to the trial data field.
    currentTrial.insert(mapTrialFields.value(TF_DATA),currentDataSet);

    // Adding the trial to the trial list.
    currentTrialList << currentTrial;

    // Clearing the trial that was just added.
    currentTrial.clear();
}

void RawDataContainer::finalizeStudy(){

    QVariantMap trialList;
    // Setting the trial list to a map with the trial list type as the key.
    trialList.insert(currentTrialListType,currentTrialList);

    // Storing the map in the main structure for trial lists.
    data.insert(MAIN_FIELD_TRIAL_LIST,trialList);

    // Clearing all data.
    currentDataSet.clear();
    currentTrial.clear();
    currentRawDataList.clear();
    currentTrialList.clear();

}

void RawDataContainer::addNewRawDataVector(const QVariantMap &raw_data_vector){
   // This should be checked, however in order to optimize peformance (just in case) no check is done. As long as
   // the vector is correctly constructed once all checks will always pass.
   currentRawDataList << raw_data_vector;
}

////////////////////////////////////// STATIC VECTOR CREATION FUNCTIONS //////////////////////////////////////
QVariantMap RawDataContainer::GenerateReadingRawDataVector(float xr, float yr, float xl, float yl, float pr, float pl, float char_r, float char_l, float word_r, float word_l){
    QVariantMap vector = RawDataContainer::GenerateStdRawDataVector(xr,yr,xl,yl,pr,pl);
    vector.insert(mapValueVectorParameters.value(VVP_CHAR_L),char_l);
    vector.insert(mapValueVectorParameters.value(VVP_CHAR_R),char_r);
    vector.insert(mapValueVectorParameters.value(VVP_WORD_L),word_l);
    vector.insert(mapValueVectorParameters.value(VVP_WORD_R),word_r);
    return vector;
}

QVariantMap RawDataContainer::GenerateStdRawDataVector(float xr, float yr, float xl, float yl, float pr, float pl){
    QVariantMap vector = RawDataContainer::GenerateStdRawDataVector(xr,yr,xl,yl,pr,pl);
    vector.insert(mapValueVectorParameters.value(VVP_XL),xl);
    vector.insert(mapValueVectorParameters.value(VVP_XR),xr);
    vector.insert(mapValueVectorParameters.value(VVP_YL),yl);
    vector.insert(mapValueVectorParameters.value(VVP_YL),yl);
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

QMap<RawDataContainer::TrialListType,const char*> RawDataContainer::initDataSetListTypesMap(){
    QMap<RawDataContainer::TrialListType,const char*> map;
    map.insert(TLT_BOUND,"bound");
    map.insert(TLT_UNBOUND,"unbound");
    map.insert(TLT_UNIQUE,"unique");
    return map;
}

QMap<RawDataContainer::ValueVectorParameter,const char*> RawDataContainer::initValueVectorParameters(){
    QMap<RawDataContainer::ValueVectorParameter,const char*> map;
    map.insert(VVP_TIMESTAMP,"timestamp");
    map.insert(VVP_CHAR_L,"char_l");
    map.insert(VVP_CHAR_R,"char_r");
    map.insert(VVP_PUPIL_L,"pupil_l");
    map.insert(VVP_PUPIL_R,"pupil_r");
    map.insert(VVP_WORD_L,"word_l");
    map.insert(VVP_WORD_R,"word_r");
    map.insert(VVP_XL,"x_l");
    map.insert(VVP_XR,"x_r");
    map.insert(VVP_YL,"y_l");
    map.insert(VVP_YR,"y_r");
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


const QMap<RawDataContainer::StudyType,const char*> RawDataContainer::mapStudyType = initStudyTypeMap();
const QMap<RawDataContainer::StudyConfigurationParameters,const char*> RawDataContainer::mapStudyParmeters = RawDataContainer::initStudyParametersMap();
const QMap<RawDataContainer::StudyConfigurationValue,const char*> RawDataContainer::mapStudyValues = RawDataContainer::initStudyValuesMap();
const QMap<RawDataContainer::TrialListType,const char*> RawDataContainer::mapDataSetListTypes = RawDataContainer::initDataSetListTypesMap();
const QMap<RawDataContainer::DataSetField,const char*> RawDataContainer::mapDataSetFields = RawDataContainer::initDataSetFieldsMap();
const QMap<RawDataContainer::DataSetType,const char*> RawDataContainer::mapDataSetTypes = RawDataContainer::initDataSetTypesMap();
const QMap<RawDataContainer::TrialField,const char*> RawDataContainer::mapTrialFields = RawDataContainer::initTrialFieldsMap();
const QMap<RawDataContainer::ValueVectorParameter,const char*> RawDataContainer::mapValueVectorParameters = RawDataContainer::initValueVectorParameters();

const char * RawDataContainer::MAIN_FIELD_EXPERIMENT_DESCRIPTION      = "experiment_description";
const char * RawDataContainer::MAIN_FIELD_TRIAL_LIST                  = "trial_list";
const char * RawDataContainer::MAIN_FIELD_SUBJECT_DATA                = "subject_data";
const char * RawDataContainer::MAIN_FIELD_PROCESSING_PARAMETERS       = "processing_parameters";
const char * RawDataContainer::MAIN_FIELD_FREQUENCY_CHECK_PARAMETERS  = "frequency_check_parameters";
const char * RawDataContainer::MAIN_FIELD_METADATA                    = "metadata";

const char * RawDataContainer::FIELD_VERSION                          = "version";
const char * RawDataContainer::CURRENT_JSON_STRUCT_VERSION            = "1";



