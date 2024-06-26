#ifndef RDC_H
#define RDC_H

#include <QVariantMap>
#include <QStringList>
#include <iostream>

namespace VMDC {

    static QString validate(const QString &str, const QStringList &possibilities, const char * type){
        if (possibilities.contains(str)){
            return "";
        }
        else{
            return str + " is not a valid value of type " + QString(type) + ". Possibilities are: " + possibilities.join(",");
        }
    }

    namespace Eye {
       static const QString LEFT = "left";
       static const QString RIGHT = "right";
       static const QString BOTH = "both";
       static const QStringList valid{LEFT, RIGHT, BOTH};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Eye"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }
    }

    namespace Hand {
       static const QString LEFT = "left";
       static const QString RIGHT = "right";
       static const QString BOTH = "both";
       static const QStringList valid{LEFT, RIGHT, BOTH};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Hand"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }
    }


    namespace Study{
       static const QString BINDING_BC_2_SHORT               = "binding_bc_2_short";
       static const QString BINDING_UC_2_SHORT               = "binding_uc_2_short";
       static const QString BINDING_BC_3_SHORT               = "binding_bc_3_short";
       static const QString BINDING_UC_3_SHORT               = "binding_uc_3_short";
       static const QString NBACK_3                          = "nback_3";
       static const QString NBACK_4                          = "nback_4";
       static const QString GONOGO                           = "gonogo";
       static const QString SPHERES                          = "spheres";
       static const QString MOVING_DOT                       = "moving_dot";
       static const QString SPHERES_VS                       = "spheres_vs";
       static const QStringList valid {BINDING_BC_2_SHORT,BINDING_UC_2_SHORT,BINDING_BC_3_SHORT,BINDING_UC_3_SHORT,NBACK_3,NBACK_4,GONOGO,SPHERES,SPHERES_VS,MOVING_DOT};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Study"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }       
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }
    }


    namespace StatusType {
       static const QString FINALIZED  = "finalized";
       static const QString ONGOING    = "ongoing";
       static const QStringList valid{FINALIZED,ONGOING};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Study Status"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }
    }
 
    namespace StudyParameter {
       static const QString VALID_EYE          = "valid_eye";
       static const QString LANGUAGE           = "language";
       static const QString NUMBER_TARGETS     = "number_targets";
       static const QString TARGET_SIZE        = "target_size";
       static const QString NBACK_LIGHT_ALL    = "nback_light_all";
       static const QString NBACK_HOLD_TIME    = "nback_rt_hold_time";
       static const QString DEFAULT_EYE        = "default_eye";
       static const QString IS_3D_STUDY        = "is_3d_study";
       static const QString HAND_TO_USE        = "hand_to_use";
       static const QString MIN_SPEED          = "min_speed";
       static const QString MAX_SPEED          = "max_speed";
       static const QString INITIAL_SPEED      = "initial_speed";
       static const QString SPEED              = "speed";
       static const QString NBACK_TIMEOUT      = "nback_timeout";
       static const QString NBACK_TRANSITION   = "nback_transition";
       static const QString WAIT_MESSAGE       = "wait_message";
       static const QString NUM_TRIALS         = "number_of_trials";
       static const QStringList valid{VALID_EYE,LANGUAGE,NUMBER_TARGETS,TARGET_SIZE,NBACK_LIGHT_ALL,DEFAULT_EYE,IS_3D_STUDY,HAND_TO_USE,MIN_SPEED,
                   MAX_SPEED,NBACK_TIMEOUT,NBACK_TRANSITION,WAIT_MESSAGE,NUM_TRIALS,SPEED};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Study Parameter"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }
    }
    
    namespace BindingTargetCount {
       static const QString TWO    = "2";
       static const QString THREE  = "3";
       static const QStringList valid{TWO,THREE};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Binding Target Count"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }       
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }
    }
    
    namespace BindingTargetSize {
       static const QString LARGE = "large";
       static const QString SMALL = "small";
       static const QStringList valid{LARGE,SMALL};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Binding Target Size"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }       
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }
    }

    namespace UILanguage {
       static const QString SPANISH   = "Spanish";
       static const QString ENGLISH   = "English";
       static const QStringList valid{SPANISH,ENGLISH};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"UI Language"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }
    }
    
    namespace ReadingLanguage {
       static const QString GERMAN    = "German";
       static const QString SPANISH   = "Spanish";
       static const QString FRENCH    = "French";
       static const QString ENGLISH   = "English";
       static const QString ISELANDIC = "Iselandic";
       static const QStringList valid{GERMAN,SPANISH,FRENCH,ENGLISH,ISELANDIC};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Reading Language"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }       
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }
    }
        
    namespace NBackVSTargetCount {
       static const QString THREE = "3";
       static const QString FOUR  = "4";
       static const QString FIVE  = "5";
       static const QString SIX   = "6";
       static const QStringList valid{THREE,FOUR,FIVE,SIX};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"NBackVS Target Count"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }       
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }
    }
    
    namespace PerceptionType {
       static const QString TRAINING = "training";
       static const QString REHAB    = "rehab";
       static const QStringList valid{TRAINING,REHAB};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Perception Type"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }       
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }
    }
    

    namespace DataVectorField {
       static const int F_XR     = 0;
       static const int F_YR     = 1;
       static const int F_PR     = 2;
       static const int F_XL     = 3;
       static const int F_YL     = 4;
       static const int F_PL     = 5;
       static const int F_X_ET_L = 6;
       static const int F_Y_ET_L = 7;
       static const int F_Z_ET_L = 8;
       static const int F_X_ET_R = 9;
       static const int F_Y_ET_R = 10;
       static const int F_Z_ET_R = 11;

       static const int L_TS     = 0;
       static const int L_CNT    = 1;

       static const QString INT_VECTOR = "l";
       static const QString FLOAT_VECTOR = "f";
    }

    namespace FixationVectorField {
       static const QString X = "x";
       static const QString Y = "y";
       static const QString DURATION = "duration";
       static const QString START_TIME = "start_time";
       static const QString END_TIME = "end_time";
       static const QString START_INDEX = "start_index";
       static const QString END_INDEX = "end_index";
       static const QString TIME = "time";
       static const QString WORD = "word";
       static const QString CHAR = "char";
       static const QString PUPIL = "pupil";
       static const QString ZERO_PUPIL = "zero_pupil";
       static const QStringList valid{X,Y,DURATION,START_TIME,END_TIME,START_INDEX,END_INDEX,TIME,WORD,CHAR,PUPIL,ZERO_PUPIL};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Fixation Vector Field"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }
    }
    
    namespace DataSetField {
       static const QString RAW_DATA = "raw_data";
       static const QString FIXATION_L = "fixations_l";
       static const QString FIXATION_R = "fixations_r";
       static const QStringList valid{RAW_DATA, FIXATION_L, FIXATION_R};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Data Study Field"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }   
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }       
    }
    
    
    namespace DataSetType {
       static const QString ENCODING_1 = "encoding_1";
       static const QString ENCODING_2 = "encoding_2";
       static const QString ENCODING_3 = "encoding_3";
       static const QString ENCODING_4 = "encoding_4";
       static const QString ENCODING_5 = "encoding_5";
       static const QString ENCODING_6 = "encoding_6";
       static const QString RETRIEVAL_1 = "retrieval_1";
       static const QString RETRIEVAL_2 = "retrieval_2";
       static const QString RETRIEVAL_3 = "retrieval_3";
       static const QString UNIQUE      = "unique";
       static const QStringList valid{ENCODING_1,ENCODING_2,ENCODING_3,ENCODING_4,ENCODING_5,ENCODING_6,RETRIEVAL_1,RETRIEVAL_2,RETRIEVAL_3,UNIQUE};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Data Set Type"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }   
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }       
    }
    
    
    namespace TrialField {
       static const QString DATA              = "data";
       static const QString RESPONSE          = "response";
       static const QString CORRECT_RESPONSE  = "correct_response";
       static const QString TRIAL_TYPE        = "trial_type";
       static const QString ID                = "ID";
       static const QString METADATA          = "metadata";
       static const QStringList valid{DATA,RESPONSE,TRIAL_TYPE,ID,METADATA};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Trial Field"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }   
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }       
    }

    namespace TrialMetadataField {
       static const QString NBACK_HOLD_TIME   = "nback_hold_time";
       static const QStringList valid{NBACK_HOLD_TIME};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Trial Metadata Field"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }
    }

    
    namespace MetadataField {
       static const QString DATE = "date";
       static const QString HOUR = "hour";
       static const QString INSTITUTION_ID = "institution_id";
       static const QString INSTITUTION_INSTANCE = "institution_instance";
       static const QString INSTITUTION_NAME = "institution_name";
       static const QString VERSION = "version";
       static const QString PROC_PARAMETER_KEY = "processing_parameters_key";
       static const QString PROTOCOL = "protocol";
       static const QString COMMENTS = "comments";
       static const QString DISCARD_REASON = "discard_reason";
       static const QString APP_VERSION = "app_version";
       static const QString EVALUATION_ID = "evaluation_id";
       static const QString EVALUATION_TYPE = "evaluation_type";
       static const QStringList valid{DATE,HOUR,INSTITUTION_ID,INSTITUTION_INSTANCE,INSTITUTION_NAME,VERSION,PROC_PARAMETER_KEY,
                   PROTOCOL,COMMENTS,DISCARD_REASON,APP_VERSION,EVALUATION_ID,EVALUATION_TYPE};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Metadata Field"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }   
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }       
    }
    
    namespace SubjectField {
       static const QString BIRTH_COUNTRY = "birth_country";
       static const QString BIRTH_DATE = "birth_date";
       static const QString YEARS_FORMATION = "years_formation";
       static const QString AGE_AT_EVALUATION = "age";
       static const QString GENDER = "gender";
       static const QString INSTITUTION_PROVIDED_ID = "institution_provided_id";
       static const QString LASTNAME = "lastname";
       static const QString LOCAL_ID = "local_id";
       static const QString NAME = "name";
       static const QString EMAIL = "email";
       static const QStringList valid{NAME,BIRTH_COUNTRY,BIRTH_DATE,YEARS_FORMATION,GENDER,INSTITUTION_PROVIDED_ID,LASTNAME,LOCAL_ID,EMAIL};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Subject Field"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }   
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }       
    }
    
    namespace CalibrationFields {
       static const QString CONFIG_PARAMS                         = "configuration_parameters";
       static const QString CALIBRATION_ATTEMPTS                  = "calibration_attempts";
       static const QStringList valid{CONFIG_PARAMS,CALIBRATION_ATTEMPTS};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Calibration Field"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }
    }


    namespace CalibrationAttemptFields {

       static const QString LEFT_EYE_DATA                         = "left_eye_data";
       static const QString RIGHT_EYE_DATA                        = "right_eye_data";
       static const QString SUCCESSFUL                            = "successful";
       static const QString COFICIENT_OF_DETERMINATION            = "coefficient_of_determination";
       static const QString CORRECTION_COEFICIENTS                = "correction_coefficients";
       static const QString CALIBRATION_TARGET_PERCENTS           = "calibration_target_percents";
       static const QString VALIDATION_POINT_ACCEPTANCE_THRESHOLD = "validation_point_acceptance_threshold";
       static const QString CALIBRATION_POINT_GATHERTIME          = "calib_pt_gather_time";
       static const QString CALIBRATION_POINT_WAITTIME            = "calib_pt_wait_time";
       static const QString CALIBRATION_DATA_USE_START_INDEX      = "calibration_data_use_start_index";
       static const QString MATH_ISSUES_FOR_CALIBRATION           = "math_issues_for_calibration";
       static const QString CALIBRATION_PTS_NO_DATA               = "calibration_points_with_too_few_data_points";
       static const QString TIMESTAMP                             = "timestamp";
       static const QString IS_3D                                 = "is_3d";
       static const QString RAW_DATA                              = "raw_et_vectors";
       static const QString PUPIL_DATA                            = "pupil_data";

       static const QStringList valid{VALIDATION_POINT_ACCEPTANCE_THRESHOLD,LEFT_EYE_DATA,RIGHT_EYE_DATA, COFICIENT_OF_DETERMINATION,MATH_ISSUES_FOR_CALIBRATION,
                   CALIBRATION_POINT_GATHERTIME,CALIBRATION_POINT_WAITTIME, CALIBRATION_PTS_NO_DATA,CALIBRATION_TARGET_PERCENTS,IS_3D, RAW_DATA, PUPIL_DATA,
                   CORRECTION_COEFICIENTS, TIMESTAMP};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Calibration Attempt Field"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }
    }

    namespace CalibrationConfigurationFields {

       static const QString REQ_NUMBER_OF_ACCEPTED_POINTS         = "required_number_of_passing_points";
       static const QString VALIDATION_POINT_HIT_TOLERANCE        = "validation_point_hit_tolerance";
       static const QString NUMBER_OF_CALIBRAION_POINTS           = "number_of_calibration_points";
       static const QString CALIBRATION_TARGET_LOCATION           = "calibration_target_location";
       static const QString CALIBRATION_TARGET_DIAMETER           = "calibration_target_diameter";
       static const QString CALIBRATION_NON_NORM_VECS             = "calibration_non_normalized_vectors";
       static const QString CALIBRATION_VALIDATION_R              = "calibration_validation_radious";
       static const QString CALIBRATION_UID                       = "calibration_uid";
       static const QStringList valid{REQ_NUMBER_OF_ACCEPTED_POINTS,VALIDATION_POINT_HIT_TOLERANCE,NUMBER_OF_CALIBRAION_POINTS,CALIBRATION_TARGET_LOCATION,
                   CALIBRATION_TARGET_DIAMETER, CALIBRATION_NON_NORM_VECS, CALIBRATION_VALIDATION_R, CALIBRATION_UID};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Calibration Configuration Field"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }

    }

    
    namespace AppUserField {
       static const QString EMAIL = "email";
       static const QString LASTNAME = "lastname";
       static const QString NAME = "name";
       static const QString LOCAL_ID = "local_id";
       static const QString VIEWMIND_ID ="viewmind_id";
       static const QStringList valid{NAME,LASTNAME,LOCAL_ID,EMAIL,VIEWMIND_ID};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"App User Field"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }   
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }       
    }
    
    
    namespace AppUserType {
       static const QString EVALUATOR = "evaluator";
       static const QString MEDIC = "medic";
       static const QStringList valid{MEDIC,EVALUATOR};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"App User Type"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }   
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }       
    }
    
    
    namespace StudyField {
       static const QString DATE                   = "date";
       static const QString HOUR                   = "hour";
       static const QString START_TIME             = "start_time";
       static const QString EXPLANATION_TIME       = "explanation_time";
       static const QString EXAMPLE_TIME           = "example_time";
       static const QString END_TIME               = "end_time";
       static const QString STUDY_DURATION         = "study_duration";
       static const QString PAUSE_DURATION         = "pause_duration";
       static const QString EXPERIMENT_DESCRIPTION = "experiment_description";
       static const QString STUDY_CONFIGURATION    = "study_configuration";
       static const QString TRIAL_LIST             = "trial_list";
       static const QString STUDY_DATA             = "study_data";
       static const QString VERSION                = "version";
       static const QString STATUS                 = "status";
       static const QString ABBREVIATION           = "abbreviation";
       static const QString CONFIG_CODE            = "config_code";
       static const QString QUALITY_CONTROL        = "quality_control";
       static const QStringList valid{START_TIME,END_TIME,STUDY_DURATION,EXPERIMENT_DESCRIPTION,STUDY_CONFIGURATION,TRIAL_LIST,VERSION,STATUS,ABBREVIATION,
                   CONFIG_CODE,QUALITY_CONTROL,DATE,HOUR,EXPLANATION_TIME,EXAMPLE_TIME};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Study Field"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }   
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }       
    }

    namespace Study3DDataFields {
       static const QString ELEMENT_SAMPLES        = "element_samples";
       static const QString EYE_LEFT               = "eye_left";
       static const QString EYE_RIGHT              = "eye_right";
       static const QString TIME_VECTOR            = "time_vector";
       static const QString FRAME_COUNTER          = "frame_counter";
       static const QStringList valid{ELEMENT_SAMPLES,EYE_LEFT,EYE_RIGHT,TIME_VECTOR,FRAME_COUNTER};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Study 3D Data Field"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }
    }

    
    namespace ProcessingParameter{
       static const QString RESOLUTION_HEIGHT        = "resolution_height";
       static const QString RESOLUTION_WIDTH         = "resolution_width";
       static const QString NBACK_HITBOXES           = "nback_hitboxes";
       static const QString GONOGO_HITBOXES          = "go_no_go_hitboxes";
       static const QString DOT_FOLLOW_HITBOXES      = "dot_follow_hitboxes";
       static const QString BINDING_HITBOXES         = "binding_hitboxes";
       static const QString MIN_FIXATION_DURATION    = "minimum_fixation_length";
       static const QString MAX_DISPERSION_WINDOW    = "max_dispersion_window";
       static const QString MAX_DISPERSION_WINDOW_PX = "max_disp_window_in_px";
       static const QString SAMPLE_FREQUENCY         = "sample_frequency";
       static const QString GAZE_ON_CENTER_RADIOUS   = "gaze_on_center_threshold_radius";
       static const QString LATENCY_ESCAPE_RADIOUS   = "latency_escape_radious";
       static const QString HAND_CALIB_RESULTS       = "hand_calibration_results";
       static const QString FOV_3D                   = "field_of_view";
       static const QString MESH_FILES               = "mesh_files";
       static const QString MESH_STRUCT              = "mesh_struct";
       static const QString ORIGIN_POINTS            = "origin_points";
       static const QString SPHERE_RADIOUS           = "sphere_radious";
       static const QStringList valid{RESOLUTION_HEIGHT,RESOLUTION_WIDTH,NBACK_HITBOXES,GONOGO_HITBOXES,MIN_FIXATION_DURATION,
                   MAX_DISPERSION_WINDOW,MAX_DISPERSION_WINDOW_PX,SAMPLE_FREQUENCY,LATENCY_ESCAPE_RADIOUS,
                   GAZE_ON_CENTER_RADIOUS, HAND_CALIB_RESULTS,FOV_3D,MESH_FILES,MESH_STRUCT,ORIGIN_POINTS,SPHERE_RADIOUS,
                   DOT_FOLLOW_HITBOXES,BINDING_HITBOXES};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Processing Parameter"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }   
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }       
    }

    namespace QCGlobalParameters {
        static const QString DQI_THRESHOLD                          = "data_quality_index_threshold";

        static const QStringList valid{DQI_THRESHOLD};
        static QString validate(const QString &str) { return VMDC::validate(str,valid,"Global QC Parameter"); }
        static qint32 toInt(const QString &str) {return valid.indexOf(str); }
        static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }
        static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }
    }

    namespace QCFields {
        static const QString DQI = "data_quality_index";
        static const QString DQI_OK = "data_quality_index_ok";
        static const QStringList valid{DQI, DQI_OK};
        static QString validate(const QString &str) { return VMDC::validate(str,valid,"QC Computed Values"); }
        static qint32 toInt(const QString &str) {return valid.indexOf(str); }
        static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }
        static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }
    }

}

#endif // RDC_H
