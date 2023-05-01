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


    namespace MultiPartStudyBaseName {
        static const QString BINDING      = "Binding";
        static const QStringList valid{BINDING};
        static QString getMetaStudy(const QString & study){
            for (qint32 i = 0; i < valid.size(); i++){
                if (study.contains(valid.at(i))) return valid.at(i);
            }
            return study;
        }
    }

    namespace Study{
       static const QString READING          = "Reading";
       static const QString BINDING_BC       = MultiPartStudyBaseName::BINDING + " BC";
       static const QString BINDING_UC       = MultiPartStudyBaseName::BINDING + " UC";
       static const QString NBACKMS          = "NBack MS";
       static const QString NBACKRT          = "NBack RT";
       static const QString NBACKVS          = "NBack VS";
       static const QString NBACK            = "NBack";
       static const QString GONOGO           = "Go No-Go";
       static const QString GONOGO_SPHERE    = "Go No-Go Sphere";
       static const QString PASSBALL         = "PassBall";
       static const QStringList valid {READING,BINDING_BC,BINDING_UC,NBACKMS,NBACKRT,NBACKVS,NBACK,GONOGO,GONOGO_SPHERE,PASSBALL};
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
       static const QString NBACK_TIMEOUT      = "nback_timeout";
       static const QString NBACK_TRANSITION   = "nback_transition";
       static const QStringList valid{VALID_EYE,LANGUAGE,NUMBER_TARGETS,TARGET_SIZE,NBACK_LIGHT_ALL,DEFAULT_EYE,IS_3D_STUDY,HAND_TO_USE,MIN_SPEED,
                   MAX_SPEED,NBACK_TIMEOUT,NBACK_TRANSITION};
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
       static const QString TIMESTAMP = "ts";
       static const QString CHAR_L = "cl";
       static const QString CHAR_R = "cr";
       static const QString PUPIL_L = "pl";
       static const QString PUPIL_R = "pr";
       static const QString WORD_L = "wl";
       static const QString WORD_R = "wr";
       static const QString X_L = "xl";
       static const QString X_R = "xr";
       static const QString Y_L = "yl";
       static const QString Y_R = "yr";
       static const QStringList valid{TIMESTAMP,CHAR_L,CHAR_R,PUPIL_L,PUPIL_R,WORD_L,WORD_R,X_L,X_R,Y_L,Y_R};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Data Vector Field"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }   
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }       
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
       static const QString STATUS = "status";
       static const QString INSTITUTION_ID = "institution_id";
       static const QString INSTITUTION_INSTANCE = "institution_instance";
       static const QString INSTITUTION_NAME = "institution_name";
       static const QString VERSION = "version";
       static const QString PROC_PARAMETER_KEY = "processing_parameters_key";
       static const QString MOUSE_USED = "mouse_used";
       static const QString PROTOCOL = "protocol";
       static const QString COMMENTS = "comments";
       static const QString DISCARD_REASON = "discard_reason";
       static const QString APP_VERSION = "app_version";
       static const QStringList valid{DATE,HOUR,INSTITUTION_ID,INSTITUTION_INSTANCE,INSTITUTION_NAME,VERSION,PROC_PARAMETER_KEY,MOUSE_USED,PROTOCOL,STATUS,COMMENTS,DISCARD_REASON,APP_VERSION};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Metadata Field"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }   
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }       
    }
    
    namespace SubjectField {
       static const QString BIRTH_COUNTRY = "birth_country";
       static const QString BIRTH_DATE = "birth_date";
       static const QString YEARS_FORMATION = "years_formation";
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

       static const QStringList valid{VALIDATION_POINT_ACCEPTANCE_THRESHOLD,LEFT_EYE_DATA,RIGHT_EYE_DATA, COFICIENT_OF_DETERMINATION,MATH_ISSUES_FOR_CALIBRATION,
                   CALIBRATION_POINT_GATHERTIME,CALIBRATION_POINT_WAITTIME, CALIBRATION_PTS_NO_DATA,CALIBRATION_TARGET_PERCENTS,IS_3D,
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
//       static const QString DATE                   = "date";
//       static const QString HOUR                   = "hour";
       static const QString START_TIME             = "start_time";
       static const QString END_TIME               = "end_time";
       static const QString STUDY_DURATION         = "study_duration";
       static const QString EXPERIMENT_DESCRIPTION = "experiment_description";
       static const QString STUDY_CONFIGURATION    = "study_configuration";
       static const QString TRIAL_LIST             = "trial_list";
       static const QString STUDY_DATA             = "study_data";
       static const QString VERSION                = "version";
       static const QString STATUS                 = "status";
       static const QString ABBREVIATION           = "abbreviation";
       static const QString CONFIG_CODE            = "config_code";
       static const QString QUALITY_CONTROL        = "quality_control";
       static const QStringList valid{START_TIME,END_TIME,STUDY_DURATION,EXPERIMENT_DESCRIPTION,STUDY_CONFIGURATION,TRIAL_LIST,VERSION,STATUS,ABBREVIATION,CONFIG_CODE,QUALITY_CONTROL};
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
       static const QStringList valid{ELEMENT_SAMPLES,EYE_LEFT,EYE_RIGHT,TIME_VECTOR};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Study 3D Data Field"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }
    }

    
    namespace ProcessingParameter{
       static const QString RESOLUTION_HEIGHT = "resolution_height";
       static const QString RESOLUTION_WIDTH = "resolution_width";
       static const QString NBACK_HITBOXES = "nback_hitboxes";
       static const QString GONOGO_HITBOXES = "go_no_go_hitboxes";
       static const QString MIN_FIXATION_DURATION = "minimum_fixation_length";
       static const QString MAX_DISPERSION_WINDOW = "max_dispersion_window";
       static const QString MAX_DISPERSION_WINDOW_PX = "max_disp_window_in_px";
       static const QString SAMPLE_FREQUENCY = "sample_frequency";
       static const QString GAZE_ON_CENTER_RADIOUS = "gaze_on_center_threshold_radius";
       static const QString LATENCY_ESCAPE_RADIOUS = "latency_escape_radious";
       static const QString HAND_CALIB_RESULTS     = "hand_calibration_results";
       static const QStringList valid{RESOLUTION_HEIGHT,RESOLUTION_WIDTH,NBACK_HITBOXES,GONOGO_HITBOXES,MIN_FIXATION_DURATION,MAX_DISPERSION_WINDOW,MAX_DISPERSION_WINDOW_PX,SAMPLE_FREQUENCY,LATENCY_ESCAPE_RADIOUS, GAZE_ON_CENTER_RADIOUS, HAND_CALIB_RESULTS};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Processing Parameter"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }   
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }       
    }

    namespace QCGlobalParameters {
        static const QString MIN_TIMESTAMP_DIFF                     = "min_successive_timestamp_difference";     // Results computed but not used.
        static const QString MAX_TIMESTAMP_DIFF                     = "max_successive_timestamp_difference";     // Results computed but not used.
        static const QString MAX_GLITCHES                           = "max_recommended_glitches";                // Basically DEPRACATED
        static const QString ALLOWED_SAMPLING_FREQ_VARIATION_PLUS   = "allowed_f_plus_variation";
        static const QString ALLOWED_SAMPLING_FREQ_VARIATION_MINUS  = "allowed_f_minus_variation";
        static const QString THRESHOLD_NUM_TRIALS_VALID_F           = "threshold_trials_with_valid_f";
        static const QString ICI_TRIAL_THRESHOLD                    = "information_completion_index_trial_threshold";
        static const QString ICI_VALID_N_TRIAL_THRESHOLD            = "information_completion_index_valid_trial_threshold";
        static const QString ICI_3D_MIN_BIN_N_POINTS                = "information_completion_index_minimum_number_of_data_points_per_bin_in_3D_study";
        static const QString ICI_3D_TARGET_NUMBER_OF_BINS           = "information_completion_index_target_number_of_bins_in_3D_study";

        static const QStringList valid{MIN_TIMESTAMP_DIFF,MAX_TIMESTAMP_DIFF,MAX_GLITCHES,ALLOWED_SAMPLING_FREQ_VARIATION_MINUS,ALLOWED_SAMPLING_FREQ_VARIATION_PLUS,
                    THRESHOLD_NUM_TRIALS_VALID_F, ICI_TRIAL_THRESHOLD, ICI_VALID_N_TRIAL_THRESHOLD,ICI_3D_MIN_BIN_N_POINTS,ICI_3D_TARGET_NUMBER_OF_BINS};
        static QString validate(const QString &str) { return VMDC::validate(str,valid,"Global QC Parameter"); }
        static qint32 toInt(const QString &str) {return valid.indexOf(str); }
        static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }
        static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }
    }

    namespace QCStudyParameters {
        static const QString MIN_POINTS_PER_TRIAL = "min_points_per_trial";   // DEPRACATED. To be removed.
        static const QString MIN_FIXS_PER_TRIAL                     = "min_fixations_per_trial";
        static const QString THRESHOLD_VALID_NUM_FIXATIONS          = "threshold_valid_num_fixations";
        static const QString MIN_TIME_DURATION_PER_TRIAL            = "min_time_duration_per_trial";
        static const QString THRESHOLD_VALID_NUM_DATA_POINTS        = "threshold_valid_num_datapoints";

        static const QStringList valid{MIN_POINTS_PER_TRIAL,MIN_FIXS_PER_TRIAL,THRESHOLD_VALID_NUM_FIXATIONS,MIN_TIME_DURATION_PER_TRIAL,THRESHOLD_VALID_NUM_DATA_POINTS};
        static QString validate(const QString &str) { return VMDC::validate(str,valid,"Study QC Parameter"); }
        static qint32 toInt(const QString &str) {return valid.indexOf(str); }
        static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }
        static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }
    }

    namespace QCFields {
        static const QString POINTS = "points_per_trial";
        static const QString FIXATIONS = "fixations_per_trial";
        static const QString GLITCHES = "glitches_per_trial";
        static const QString AVG_FREQ = "avg_freq_per_trial";
        static const QString ICI = "index_of_information_completion";
        static const QString N_DATASETS = "number_of_datasets";
        static const QString QC_POINT_INDEX = "qc_data_point_index";
        static const QString QC_POINT_INDEX_OK = "qc_data_point_index_ok";
        static const QString QC_FIX_INDEX = "qc_fix_index";
        static const QString QC_FIX_INDEX_OK = "qc_fix_index_ok";
        static const QString QC_FREQ_INDEX = "qc_freq_index";
        static const QString QC_FREQ_INDEX_OK = "qc_freq_index_ok";
        static const QString QC_ICI_INDEX = "qc_ici_index";
        static const QString QC_ICI_INDEX_OK = "qc_ici_index_ok";
        static const QStringList valid{ICI,FIXATIONS,POINTS,AVG_FREQ,GLITCHES,N_DATASETS,QC_POINT_INDEX,QC_POINT_INDEX_OK,QC_FIX_INDEX,
                    QC_FIX_INDEX_OK,QC_FREQ_INDEX,QC_FREQ_INDEX_OK,QC_ICI_INDEX, QC_ICI_INDEX_OK};
        static QString validate(const QString &str) { return VMDC::validate(str,valid,"QC Computed Values"); }
        static qint32 toInt(const QString &str) {return valid.indexOf(str); }
        static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }
        static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }
    }

}

#endif // RDC_H
