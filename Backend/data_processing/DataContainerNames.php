<?php

include_once ("../common/TypedString.php");

abstract class Eye extends TypedString
{
   const BOTH                                    = "both";
   const LEFT                                    = "left";
   const RIGHT                                   = "right";
}

abstract class MultiPartStudyBaseName extends TypedString {
   const BINDING                                 = "Binding";
   const PERCEPTION                              = "Perception";
}

abstract class Study extends TypedString {
   const BINDING_BC                              = MultiPartStudyBaseName::BINDING . " BC";
   const BINDING_UC                              = "Binding UC";
   const GONOGO                                  = "Go No-Go";
   const NBACKMS                                 = "NBack MS";
   const NBACKRT                                 = "NBack RT";
   const NBACKVS                                 = "NBack VS";   
   const READING                                 = "Reading";
   const PERCEPTION_1                            = MultiPartStudyBaseName::PERCEPTION  . " 1";
   const PERCEPTION_2                            = MultiPartStudyBaseName::PERCEPTION  . " 2";
   const PERCEPTION_3                            = MultiPartStudyBaseName::PERCEPTION  . " 3";
   const PERCEPTION_4                            = MultiPartStudyBaseName::PERCEPTION  . " 4";
   const PERCEPTION_5                            = MultiPartStudyBaseName::PERCEPTION  . " 5";
   const PERCEPTION_6                            = MultiPartStudyBaseName::PERCEPTION  . " 6";
   const PERCEPTION_7                            = MultiPartStudyBaseName::PERCEPTION  . " 7";
   const PERCEPTION_8                            = MultiPartStudyBaseName::PERCEPTION  . " 8";
}

abstract class StatusType extends TypedString {
   const FINALIZED                               = "finalized";
   const ONGOING                                 = "ongoing";   
}

abstract class StudyParameter extends TypedString{
   const VALID_EYE                               = "valid_eye";  
   const LANGUAGE                                = "language";
   const NUMBER_TARGETS                          = "number_targets";
   const TARGET_SIZE                             = "target_size";
   const PERCEPTION_TYPE                         = "perception_type";
   const PERCEPTION_PART                         = "perception_part";   
}

abstract class BindingTargetCount extends TypedString {
   const TWO                                     = "2";
   const THREE                                   = "3";
}

abstract class BindingTargetSize extends TypedString {
   const  LARGE                                  = "large";
   const  SMALL                                  = "small";
}

abstract class ReadingLanguage extends TypedString{
   const GERMAN    = "German";
   const SPANISH   = "Spanish";
   const FRENCH    = "French";
   const ENGLISH   = "English";
   const ISELANDIC = "Iselandic";
}
    
abstract class NBackVSTargetCount extends TypedString{
   const THREE = "3";
   const FOUR  = "4";
   const FIVE  = "5";
   const SIX   = "6";
}

abstract class PerceptionType extends TypedString {
   const TRAINING = "training";
   const REHAB    = "rehab";
}

abstract class DataVectorField extends TypedString
{
   // Names are short as the are repeated for every value vector. This allows compression of the resulting file
   const TIMESTAMP                               = "ts";
   const CHAR_L                                  = "cl";
   const CHAR_R                                  = "cr";
   const PUPIL_L                                 = "pl";
   const PUPIL_R                                 = "pr";
   const WORD_L                                  = "wl";
   const WORD_R                                  = "wr";
   const XL                                      = "xl";
   const XR                                      = "xr";
   const YL                                      = "yl";
   const YR                                      = "yr";   
}

abstract class FixationVectorField
{
   const X                                       = "x";
   const Y                                       = "y";
   const DURATION                                = "duration";
   const START_TIME                              = "start_time";
   const END_TIME                                = "end_time";
   const START_INDEX                             = "start_index";
   const END_INDEX                               = "end_index";
   const TIME                                    = "time";
   const SAC_AMP                                 = "sac_amp";
   const WORD                                    = "word";
   const CHAR                                    = "char";
   const PUPIL                                   = "pupil";
   const ZERO_PUPIL                              = "zero_pupil";
   const TIMELINE                                = "timeline";
   const NBACK                                   = "nback";
   const IS_IN                                   = "is_in";
   const TARGET_HIT                              = "target_hit";
}

abstract class DataSetField extends TypedString
{
   const RAW_DATA                                = "raw_data";
   const FIXATIONS_L                             = "fixations_l";
   const FIXATIONS_R                             = "fixations_r";
   const DATA_SET_VALUES                         = "computed_data_set_values";
}

abstract class DataSetType extends TypedString
{
   const ENCODING_1                             = "encoding_1";
   const ENCODING_2                             = "encoding_2";
   const ENCODING_3                             = "encoding_3";
   const ENCODING_4                             = "encoding_4";
   const ENCODING_5                             = "encoding_5";
   const ENCODING_6                             = "encoding_6";
   const RETRIEVAL_1                            = "reterieval_1";
   const RETRIEVAL_2                            = "reterieval_2";
   const RETRIEVAL_3                            = "reterieval_3";
   const UNIQUE                                 = "unique";   
}

abstract class TrialField extends TypedString
{
   const DATA                                    = "data";
   const ID                                      = "ID";
   const RESPONSE                                = "response";
   const TRIAL_TYPE                              = "trial_type";
   const TRIAL_VALUES                            = "computed_trial_values";
}

abstract class MetadataField extends TypedString
{
   const DATE                                    = "date";
   const HOUR                                    = "hour";
   const STATUS                                  = "status";
   const SUBJECT_AGE                             = "age";
   const INSTITUTION_ID                          = "institution_id";
   const INSTITUTION_INSTANCE                    = "institution_instance";
   const INSTITUTION_KEY                         = "institution_key";
   const INSTITUTION_NAME                        = "institution_name";
   const VERSION                                 = "version";
   const PROCESSING_PARAMETER_KEY                = "processing_parameters_key";   
   const PROCESSING_MOUSE_USED                   = "mouse_used";   
   const PROCESSING_PROTOCOL                     = "protocol";   
}

abstract class SubjectField extends TypedString {
   const NAME                                    = "name";
   const LASTNAME                                = "lastname";
   const YEARS_FORMATION                         = "years_formation";
   const INSTITUTION_PROVIDED_ID                 = "institution_provided_id";
   const BIRTHDATE                               = "birth_date";
   const BIRTH_COUNTRY                           = "birth_country";
   const GENDER                                  = "gender";
   const AGE                                     = "age";
   const LOCAL_ID                                = "local_id";
}


abstract class AppUserField extends TypedString {
   const EMAIL                                   = "email";
   const LASTNAME                                = "lastname";
   const NAME                                    = "name";
   const LOCAL_ID                                = "local_id";
   const VIEWMIND_ID                             = "viewmind_id";
}
   

abstract class AppUserType extends TypedString {
   const EVALUATOR                               = "evaluator";
   const MEDIC                                   = "medic";
}

abstract class StudyField extends TypedString
{
   const DATE                                    = "date";
   const HOUR                                    = "hour";
   const EXPERIMENT_DESCRIPTION                  = "experiment_description";
   const STUDY_CONFIGURATION                     = "study_configuration";
   const TRIAL_LIST                              = "trial_list";
   const VERSION                                 = "version";
   const STATUS                                  = "status";
   const ABBREVIATION                            = "abbreviation";
   const CONFIG_CODE                             = "config_code";
   const QUALITY_CONTROL                         = "quality_control";
}


abstract class ProcessingParameter extends TypedString
{
   const SAMPLE_FREQUENCY                        = "sample_frequency";
   const MAX_DISPERSION_WINDOW                   = "max_dispersion_window";
   const MINIMUM_FIXATION_LENGTH                 = "minimum_fixation_length";
   const RESOLUTION_WIDTH                        = "resolution_width";
   const RESOLUTION_HEIGHT                       = "resolution_height";
   const NBACK_HITBOXES                          = "nback_hitboxes";
   const GONOGO_HITBOXES                         = "go_no_go_hitboxes";
   const LATENCY_ESCAPE_RADIOUS                  = "latency_escape_radious";
}

abstract class QualityControlParamterGlobal extends TypedString 
{
   const MIN_SUCCESSIVE_TIMESTAMP_DIFFERENCE    = "min_successive_timestamp_difference";
   const MAX_SUCCESSIVE_TIMESTAMP_DIFFERENCE    = "max_successive_timestamp_difference";
   const MAX_GLITCHES                           = "max_recommended_glitches" ;
}

abstract class QualityControlParamterStudy extends TypedString
{
   const MIN_POINTS_PER_TRIAL                   = "min_points_per_trial";
   const MIN_FIX_PER_TRIAL                      = "min_fixations_per_trial";
}

abstract class QCFields extends TypedString
{
    const POINTS                                = "points_per_trial";
    const FIXATIONS_L                           = "fixations_per_trial_l";
    const FIXATIONS_R                           = "fixations_per_trial_r";
    const GLITCHES                              = "glitches_per_trial";
    const AVG_FREQ                              = "avg_freq_per_trial";
    const N_DATASETS                            = "number_of_datasets";
}

abstract class MainFields extends TypedString
{
   const STUDIES                                 = "studies";
   const PROCESSING_PARAMETERS                   = "processing_parameters"; 
   const METADATA                                = "metadata";
   const SUBJECT                                 = "subject";
   const QC_PARAMETERS                           = "qc_parameters";
   const APPLICATION_USER                        = "application_user";
}


abstract class DataSetComputedValues extends TypedString {
   const GAZE_L                                  = "gaze_l";
   const GAZE_R                                  = "gaze_r";
   const SAC_LAT_L                               = "sac_lat_l";
   const SAC_LAT_R                               = "sac_lat_r";
   const DURATION                                = "duration";
   const FIX_IN_CENTER_L                         = "fix_in_center_l";
   const FIX_IN_CENTER_R                         = "fix_in_center_r";
   const RESPONSE_TIME_L                         = "response_time_l";
   const RESPONSE_TIME_R                         = "response_time_r";
}

abstract class TrialComputedValueTypes extends TypedString {
   const COMPLETE_SEQUENCE_FIXATION_L            = "seq_complete_l";
   const COMPLETE_SEQUENCE_FIXATION_R            = "seq_complete_r";
}

abstract class StudyComputedValue extends TypedString {
   const DURATION                                = "duration";
   const BINDING_SCORE                           = "binding_score";
}

abstract class BindingScore extends TypedString {
   const TEST_CORRECT                            = "test_correct";
   const TEST_WRONG                              = "test_wrong";
   const CORRECT                                 = "correct";
   const WRONG                                   = "wrong";
}





abstract class GoNoGoTargetBoxes {
   const ARROW  = 0;
   const LEFT   = 1;
   const RIGHT  = 2;
}

?>
