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

    namespace MultiPartStudyBaseName {
        static const QString BINDING      = "Binding";
        static const QString PERCEPTION   = "Perception";
        static const QStringList valid{BINDING, PERCEPTION};
        static QString getMetaStudy(const QString & study){
            for (qint32 i = 0; i < valid.size(); i++){
                if (study.contains(valid.at(i))) return valid.at(i);
            }
            return study;
        }
    }

    namespace Study{
       static const QString READING      = "Reading";
       static const QString BINDING_BC   = MultiPartStudyBaseName::BINDING + " BC";
       static const QString BINDING_UC   = MultiPartStudyBaseName::BINDING + " UC";
       static const QString NBACKMS      = "NBack MS";
       static const QString NBACKRT      = "NBack RT";
       static const QString NBACKVS      = "NBack VS";
       static const QString GONOGO       = "Go No-Go";
       static const QString PERCEPTION_1 = MultiPartStudyBaseName::PERCEPTION + " 1";
       static const QString PERCEPTION_2 = MultiPartStudyBaseName::PERCEPTION + " 2";
       static const QString PERCEPTION_3 = MultiPartStudyBaseName::PERCEPTION + " 3";
       static const QString PERCEPTION_4 = MultiPartStudyBaseName::PERCEPTION + " 4";
       static const QString PERCEPTION_5 = MultiPartStudyBaseName::PERCEPTION + " 5";
       static const QString PERCEPTION_6 = MultiPartStudyBaseName::PERCEPTION + " 6";
       static const QString PERCEPTION_7 = MultiPartStudyBaseName::PERCEPTION + " 7";
       static const QString PERCEPTION_8 = MultiPartStudyBaseName::PERCEPTION + " 8";
       static const QStringList valid{READING,BINDING_BC,BINDING_UC,NBACKMS,NBACKRT,NBACKVS,GONOGO,PERCEPTION_1,PERCEPTION_2,PERCEPTION_3,PERCEPTION_4,PERCEPTION_5,PERCEPTION_6,PERCEPTION_7,PERCEPTION_8};
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
       static const QString PERCEPTION_TYPE    = "perception_type";
       static const QString PERCEPTION_PART    = "perception_part";
       static const QStringList valid{VALID_EYE,LANGUAGE,NUMBER_TARGETS,TARGET_SIZE,PERCEPTION_TYPE,PERCEPTION_PART};
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
       static const QString FIXATION_L = "fixation_l";
       static const QString FIXATION_R = "fixation_r";
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
       static const QString RETRIEVAL_1 = "reterieval_1";
       static const QString RETRIEVAL_2 = "reterieval_2";
       static const QString RETRIEVAL_3 = "reterieval_3";
       static const QString UNIQUE      = "unique";
       static const QStringList valid{ENCODING_1,ENCODING_2,ENCODING_3,ENCODING_4,ENCODING_5,ENCODING_6,RETRIEVAL_1,RETRIEVAL_2,RETRIEVAL_3,UNIQUE};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Data Set Type"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }   
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }       
    }
    
    
    namespace TrialField {
       static const QString DATA       = "data";
       static const QString RESPONSE   = "response";
       static const QString TRIAL_TYPE = "trial_type";
       static const QString ID         = "ID";
       static const QStringList valid{DATA,RESPONSE,TRIAL_TYPE,ID};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Trial Field"); }
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
       static const QStringList valid{DATE,HOUR,INSTITUTION_ID,INSTITUTION_INSTANCE,INSTITUTION_NAME,VERSION,PROC_PARAMETER_KEY,MOUSE_USED,PROTOCOL,STATUS};
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
       static const QString AGE = "age";
       static const QString INSTITUTION_PROVIDED_ID = "institution_provided_id";
       static const QString LASTNAME = "lastname";
       static const QString LOCAL_ID = "local_id";
       static const QString NAME = "name";
       static const QStringList valid{NAME,BIRTH_COUNTRY,BIRTH_DATE,YEARS_FORMATION,GENDER,AGE,INSTITUTION_PROVIDED_ID,LASTNAME,LOCAL_ID};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Subject Field"); }
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
       static const QString EXPERIMENT_DESCRIPTION = "experiment_description";
       static const QString STUDY_CONFIGURATION    = "study_configuration";
       static const QString TRIAL_LIST             = "trial_list";
       static const QString VERSION                = "version";
       static const QString STATUS                 = "status";
       static const QString ABBREVIATION           = "abbreviation";
       static const QString CONFIG_CODE            = "config_code";
       static const QString QUALITY_CONTROL        = "quality_control";
       static const QStringList valid{DATE,HOUR,EXPERIMENT_DESCRIPTION,STUDY_CONFIGURATION,TRIAL_LIST,VERSION,STATUS,ABBREVIATION,CONFIG_CODE,QUALITY_CONTROL};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Study Field"); }
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
       static const QString SAMPLE_FREQUENCY = "sample_frequency";
       static const QString LATENCY_ESCAPE_RADIOUS = "latency_escape_radious";
       static const QStringList valid{RESOLUTION_HEIGHT,RESOLUTION_WIDTH,NBACK_HITBOXES,GONOGO_HITBOXES,MIN_FIXATION_DURATION,MAX_DISPERSION_WINDOW,SAMPLE_FREQUENCY,LATENCY_ESCAPE_RADIOUS};
       static QString validate(const QString &str) { return VMDC::validate(str,valid,"Processing Parameter"); }
       static qint32 toInt(const QString &str) {return valid.indexOf(str); }
       static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }   
       static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }       
    }

    namespace QCGlobalParameters {
        static const QString MIN_TIMESTAMP_DIFF = "min_successive_timestamp_difference";
        static const QString MAX_TIMESTAMP_DIFF = "max_successive_timestamp_difference";
        static const QString MAX_GLITCHES       = "max_recommended_glitches";
        static const QStringList valid{MIN_TIMESTAMP_DIFF,MAX_TIMESTAMP_DIFF,MAX_GLITCHES};
        static QString validate(const QString &str) { return VMDC::validate(str,valid,"Global QC Parameter"); }
        static qint32 toInt(const QString &str) {return valid.indexOf(str); }
        static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }
        static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }
    }

    namespace QCStudyParameters {
        static const QString MIN_POINTS_PER_TRIAL = "min_points_per_trial";
        static const QString MIN_FIXS_PER_TRIAL = "min_fixations_per_trial";
        static const QStringList valid{MIN_POINTS_PER_TRIAL,MIN_FIXS_PER_TRIAL};
        static QString validate(const QString &str) { return VMDC::validate(str,valid,"Study QC Parameter"); }
        static qint32 toInt(const QString &str) {return valid.indexOf(str); }
        static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }
        static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }
    }

    namespace QCFields {
        static const QString POINTS = "points_per_trial";
        static const QString FIXATIONS_L = "fixations_per_trial_l";
        static const QString FIXATIONS_R = "fixations_per_trial_r";
        static const QString GLITCHES = "glitches_per_trial";
        static const QString AVG_FREQ = "avg_freq_per_trial";
        static const QString N_DATASETS = "number_of_datasets";
        static const QStringList valid{POINTS,FIXATIONS_L,FIXATIONS_R,GLITCHES,AVG_FREQ,N_DATASETS};
        static QString validate(const QString &str) { return VMDC::validate(str,valid,"QC Computed Values"); }
        static qint32 toInt(const QString &str) {return valid.indexOf(str); }
        static QString fromInt(qint32 index ) { if ((index >= 0) && (index < valid.size())) return valid.at(index); else return ""; }
        static QString validateList(const QStringList &totest) { for (qint32 i = 0; i < totest.size(); i++) { QString ans = validate(totest.at(i)); if (ans != "") return ans; } return ""; }
    }

}

#endif // RDC_H
