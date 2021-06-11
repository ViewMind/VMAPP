<?php

include_once ("../common/TypedString.php");

abstract class StudyTypes extends TypedString
{
   const BINDING                                 = "Binding";
   const GONOGO                                  = "Go No-Go";
   const NBACKMS                                 = "NBack MS";
   const NBACKRT                                 = "NBack RT";
   const NBACKVS                                 = "NBack VS";
   const PERCEPTION                              = "Perception";
   const READING                                 = "Reading";
}

abstract class EyeType extends TypedString
{
   const BOTH                                    = "Both";
   const LEFT                                    = "Left";
   const RIGHT                                   = "Right";
}



abstract class ProcessingParameter extends TypedString
{
   const SAMPLE_FREQUENCY                        = "sample_frequency";
   const MAX_DISPERSION_WINDOW                   = "max_dispersion_window";
   const MINIMUM_FIXATION_LENGTH                 = "minimum_fixation_length";
   const LATENCY_ESCAPE_RADIOUS                  = "latency_escape_radious";
   const RESOLUTION_WIDTH                        = "resolution_width";
   const RESOLUTION_HEIGHT                       = "resolution_height";
   const NBACK_HITBOXES                          = "nback_hitboxes";
   const GONOGO_HITBOXES                         = "go_no_go_hitboxes";
}

abstract class RawDataVectorFields
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

abstract class FixationVectorFields
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

abstract class MainFields extends TypedString
{
   const STUDIES                                 = "studies";
   const PROCESSING_PARAMETERS                   = "processing_parameters";
   const METADATA                                = "metadata";
   const SUBJECT                                 = "subject";
}

abstract class DataSetFields extends TypedString
{
   const RAW_DATA                                = "raw_data";
   const FIXATIONS_L                             = "fixations_l";
   const FIXATIONS_R                             = "fixations_r";
   const DATA_SET_VALUES                         = "data_set_values";
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


abstract class StudyFields extends TypedString
{
   const TRIAL_LIST                              = "trial_list";
   const FINALIZED_RESULTS                       = "finalized_results";
   const VALID_EYE                               = "valid_eye";
   const STUDY_CONFIGURATION                     = "study_configuration";
}

abstract class StudyConfigurationFields extends TypedString{
   const VALID_EYE                               = "valid_eye";
}

abstract class TrialFields extends TypedString
{
   const DATA                                    = "data";
   const ID                                      = "ID";
   const RESPONSE                                = "response";
   const TRIAL_TYPE                              = "trial_type";
   const TRIAL_VALUES                            = "trial_values";
}

abstract class TrialListTypeFields extends TypedString
{
   const LIST                                    = "list";
   const DATA                                    = "data";   
}

abstract class TrialListTypeDataField extends TypedString {
   const DURATION                                = "duration";
   const BINDING_SCORE                           = "binding_score";
}

abstract class BindingScore extends TypedString {
   const TEST_CORRECT                            = "test_correct";
   const TEST_WRONG                              = "test_wrong";
   const CORRECT                                 = "correct";
   const WRONG                                   = "wrong";
}

abstract class MetadaFields extends TypedString
{
   const RES_WIDTH                               = "resolution_width";
   const RES_HEIGHT                              = "resolution_height";
   const DATE                                    = "date";
   const HOUR                                    = "hour";
   const SUBJECT_IDENTIFIER                      = "subject_id";
   const SUBJECT_AGE                             = "age";
   const INSTITUTION_ID                          = "institution_id";
   const INSTITUTION_INSTANCE                    = "institution_instance";
   const INSTITUTION_KEY                         = "institution_key";
   const INSTITUTION_NAME                        = "institution_name";
   const VERSION                                 = "version";
   const PROCESSING_PARAMETER_KEY                = "processing_parameters_key";   
}

abstract class SubjectFields extends TypedString {
   const NAME                                    = "name";
   const LASTNAME                                = "lastname";
   const YEARS_FORMATION                         = "years_formation";
   const INSTITUTION_PROVIDED_ID                 = "institution_provided_id";
   const BIRTHDATE                               = "birthdate";
   const BIRTH_COUNTRY                           = "birth_country";
   const GENDER                                  = "gender";
   const AGE                                     = "age";
   const LOCAL_ID                                = "local_id";
}
   
abstract class DataSetTypes extends TypedString
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

abstract class GoNoGoTargetBoxes {
   const ARROW  = 0;
   const LEFT   = 1;
   const RIGHT  = 2;
}

?>
