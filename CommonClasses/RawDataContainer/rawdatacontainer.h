#ifndef RAWDATACONTAINER_H
#define RAWDATACONTAINER_H

/**
 * @brief The RawDataContainer class
 * @details Class meant the raw data obtained from an study as well as the the data required all data required to analyze it and generate a report.
 */

#include <QVariantMap>
#include <QJsonDocument>
#include <QFile>
#include <QTextStream>
#include "../DatFileInfo/datfileinfoindir.h"

//// The top level fields of the JSON structure for ANY experiment.
//#define MAIN_FIELD_EXPERIMENT_DESCRIPTION      "experiment_description"
//#define MAIN_FIELD_RAW_DATA_SETS               "raw_data_sets"
//#define MAIN_FIELD_SUBJECT_DATA                "subject_data"
//#define MAIN_FIELD_PROCESSING_PARAMETERS       "processing_parameters"
//#define MAIN_FIELD_FREQUENCY_CHECK_PARAMETERS  "frequency_check_parameters"
//#define MAIN_FIELD_METADATA                    "meta_data";

//// Common subfields of the JSON Structure for ANY experiments.
//#define FIELD_XR                               "xr"
//#define FIELD_YR                               "yr"
//#define FIELD_XL                               "xl"
//#define FIELD_YL                               "yl"
//#define FIELD_PUPIL_L                          "pl"
//#define FIELD_PUPIL_R                          "pr"

//#define FIELD_STUDY_TYPE                       "study_type"
//#define FIELD_VERSION                          "version"
//#define FIELD_RESOLUTION                       "resolution"
//#define FIELD_DATE                             "date"
//#define FIELD_HOUR                             "hour"
//#define FIELD_VALID_EYE                        "valid_eye";
//#define FIELD_STUDY_CONFIGURATION              "study_configuration"
//#define FIELD_FREQUENCY_CHECK_FLAG             "frequecy_check_flag"
//#define FIELD_FREQUENCY_CHECK_REPORT           "frequecy_check_reports"

//// Specific fields for individual studies.
//#define FIELD_READING_LANGUAGE                 "language"

//// Specific Parameters
//#define CURRENT_JSON_VERSION_STRUCTURE         "1";
//#define VALID_EYE_LEFT                         "left"
//#define VALID_EYE_RIGHT                        "right"
//#define VALID_EYE_BOTH                         "both";

//// Study types
//#define  STUDY_READING                         "Reading"
//#define  STUDY_BINDING                         "Binding"
//#define  STUDY_NBACKMS                         "NBack MS"
//#define  STUDY_NBACKRT                         "NBack RT"
//#define  STUDY_NBACKVS                         "NBack VS"
//#define  STUDY_GONOGO                          "Go-No Go"
//#define  STUDY_PERCEPTION                      "Perception"

// Fields specific for each experiment.
class RawDataContainer
{
public:
    RawDataContainer();

    /**
     * @brief StudyType The possible studies for which to setup the data.
     */
    typedef enum {
        STUDY_READING,
        STUDY_BINDING,
        STUDY_GONOGO,
        STUDY_NBACKMS,
        STUDY_NBACKRT,
        STUDY_NBACKVS,
        STUDY_PERCEPTION } StudyType;

    typedef enum {
        SCP_EYES,
        SCP_LANGUAGE,
        SCP_NUMBER_OF_TARGETS,
        SCP_TARGET_SIZE,
        SCP_BINDING_TYPE,
    } StudyConfigurationParameters;

    typedef enum {
        SCV_EYE_LEFT,
        SCV_EYE_RIGHT,
        SCV_EYE_BOTH,
        SCV_LANG_ES,
        SCV_LANG_FR,
        SCV_LANG_DE,
        SCV_LANG_IS,
        SCV_LANG_EN,
        SCV_BINDING_TARGETS_3,
        SCV_BINDING_TARGETS_2,
        SCV_NBACKVS_TARGETS_3,
        SCV_NBACKVS_TARGETS_4,
        SCV_NBACKVS_TARGETS_5,
        SCV_NBACKVS_TARGETS_6,
        SCV_PERCEPTION_STUDY_PART_1,
        SCV_PERCEPTION_STUDY_PART_2,
        SCV_PERCEPTION_STUDY_PART_3,
        SCV_PERCEPTION_STUDY_PART_4,
        SCV_PERCEPTION_STUDY_PART_5,
        SCV_PERCEPTION_STUDY_PART_6,
        SCV_PERCEPTION_STUDY_PART_7,
        SCV_PERCEPTION_STUDY_PART_8,
        SCV_PERCEPTION_TYPE_TRAINING,
        SCV_PERCEPTION_TYPE_REHAB,
        SCV_BINDING_TARGETS_LARGE,
        SCV_BINDING_TARGETS_SMALL,
        SCV_BINDING_TYPE_BOUND,
        SCV_BINDING_TYPE_UNBOUND
    } StudyConfigurationValue;

    typedef enum {
        MF_VERSION,
        MF_HOUR,
        MF_DATE,
        MF_INSTITUTION_ID,
        MF_INSTITUTION_NAME,
        MF_INSTITUTION_KEY,
        MF_INSTITUTION_INSTANCE,
        MF_INSTITUTION_SECRET,
        MF_STUDY_CONFIGURATION,
    } MetadataField;

    typedef enum {
        SF_NAME,
        SF_LASTNAME,
        SF_YEARS_FORMATION,
        SF_INSTITUTION_PROVIDED_ID,
        SF_BIRTHDATE,
        SF_BIRHTCOUNTRY,
        SF_GENDER,
        SF_LOCAL_ID
    } SubjectField;

    typedef enum {
        SF_NAME,
        SF_LASTNAME,
        SF_EMAIL,
        SF_LOCAL_ID
    } AppUserField;

    typedef enum {
        AT_EVALUATOR,
        AT_TEST_TAKER
    } AppUserType;

    typedef enum {
        VVP_TIMESTAMP,
        VVP_XR,
        VVP_YR,
        VVP_XL,
        VVP_YL,
        VVP_PUPIL_R,
        VVP_PUPIL_L,
        VVP_WORD_R,
        VVP_CHAR_R,
        VVP_WORD_L,
        VVP_CHAR_L,
    } ValueVectorParameter;

    typedef enum {
        TLT_UNIQUE,
        TLT_BOUND,
        TLT_UNBOUND
    } TrialListType;

    typedef enum {
        DST_UNIQUE,
        DST_ENCODING_1,
        DST_ENCODING_2,
        DST_ENCODING_3,
        DST_ENCODING_4,
        DST_ENCODING_5,
        DST_ENCODING_6,
        DST_RETRIEVAL_1,
        DST_RETRIEVAL_2,
        DST_RETRIEVAL_3,
    } DataSetType;

    typedef enum {
        TF_DATA,
        TF_ID,
        TF_SENTENCE,
        TF_RESPONSE,
        TF_TRIAL_TYPE
    } TrialField;

    typedef enum {
        DSF_RAW_DATA
    } DataSetField;

    typedef QMap<StudyConfigurationParameters,StudyConfigurationValue> StudyConfiguration;
    typedef QMap<TrialField,QString> TrialConfiguration;

    /**
     * @brief getJSONString
     * @return The UTF-8 JSON enconded string representing all the stored internal data
     */
    QString getJSONString() const;

    /**
     * @brief getError
     * @return The last error message from any operation done. Empty if no error.
     */
    QString getError() const;

    bool setStudy(const StudyType &study, const StudyConfiguration &studyConfiguration);

    bool addNewTrial(const QString &trial_id, const TrialConfiguration &trial_configuration);

    void addNewDataSet(const DataSetType &data_set_type);

    void finalizeDataSet();

    void finalizeTrial(const QString &reponse = "");

    void finalizeStudy();

    void addNewRawDataVector(const QVariantMap &raw_data_vector);

    /**
     * @brief GenerateStdRawDataVector Geneates a valid vector that can be inserted with add NewRawDataVector. Ensuring coherence. Standar all EyeTracking values.
     * @param xr Right Eye X Coordinate
     * @param yr Righe Eye Y Cooordinate
     * @param xl Left Eye X Coordinate
     * @param yl Left Eye Y Coordinate
     * @param pr Right Eye Pupil size
     * @param pl Left Eye Pupil size.
     * @return A Raw data vector in the form of a map.
     */
    static QVariantMap GenerateStdRawDataVector(float xr, float yr, float xl, float yl, float pr, float pl);
    /**
     * @brief GenerateReadingRawDataVector Geneates a valid vector that can be inserted with add NewRawDataVector. Ensuring coherence. Standar all EyeTracking values. Includes specific Reading study data.
     * @param xr Right Eye X Coordinate
     * @param yr Righe Eye Y Cooordinate
     * @param xl Left Eye X Coordinate
     * @param yl Left Eye Y Coordinate
     * @param pr Right Eye Pupil size
     * @param pl Left Eye Pupil size.
     * @param char_r Character index for the right eye
     * @param char_l Character index for the left eye
     * @param word_r Word index for the right eye
     * @param word_l Word index for the left eye.
     * @return
     */
    static QVariantMap GenerateReadingRawDataVector(float xr, float yr, float xl, float yl, float pr, float pl, float char_r, float char_l, float word_r, float word_l);

private:
    QVariantMap data;
    QString error;

    // Variables required to know what to create.
    QVariantList currentTrialList;
    QVariantMap currentTrial;
    QVariantMap currentDataSet;

    // Where the actuall values will be stored.
    QVariantList currentRawDataList;

    QString currentTrialListType;
    QString currentDataSetType;

    StudyType currentlySelectedStudy;
    StudyConfiguration currentStudyConfiguration;

    /**
     * @brief configureReadingStudy Individual study configuration functions
     * @param params The map for the study parameters to be configured.
     * @return True if there was no error. False otherwise. Error variable contains the error
     */
    bool configureReadingStudy(const StudyConfiguration &params);

    // All initialization functions
    static QMap<StudyType,const char*> initStudyTypeMap();
    static QMap<StudyConfigurationParameters,const char*> initStudyParametersMap();
    static QMap<StudyConfigurationValue,const char*> initStudyValuesMap();
    static QMap<TrialListType,const char*> initDataSetListTypesMap();
    static QMap<ValueVectorParameter,const char*> initValueVectorParameters();
    static QMap<DataSetType,const char*> initDataSetTypesMap();
    static QMap<TrialField,const char*> initTrialFieldsMap();
    static QMap<DataSetField,const char*> initDataSetFieldsMap();


    // These maps map each enum type to string
    static const QMap<StudyType,const char*> mapStudyType;
    static const QMap<StudyConfigurationParameters,const char*> mapStudyParmeters;
    static const QMap<StudyConfigurationValue,const char*> mapStudyValues;
    static const QMap<TrialListType,const char*> mapDataSetListTypes;
    static const QMap<ValueVectorParameter,const char*> mapValueVectorParameters;
    static const QMap<DataSetType,const char*> mapDataSetTypes;
    static const QMap<TrialField,const char*> mapTrialFields;
    static const QMap<DataSetField,const char*> mapDataSetFields;


    // The main fields in ANY study container.
    static const char *MAIN_FIELD_EXPERIMENT_DESCRIPTION;
    static const char *MAIN_FIELD_TRIAL_LIST;
    static const char *MAIN_FIELD_SUBJECT_DATA;
    static const char *MAIN_FIELD_PROCESSING_PARAMETERS;
    static const char *MAIN_FIELD_FREQUENCY_CHECK_PARAMETERS;
    static const char *MAIN_FIELD_METADATA;

    // Constant strings to be used in all structures.
    static const char *FIELD_VERSION;
    static const char *CURRENT_JSON_STRUCT_VERSION;



};

#endif // RAWDATACONTAINER_H
