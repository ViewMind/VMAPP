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
        MF_PROCESSING_PARAMETER_KEY
    } MetadataField;

    typedef enum {
        SF_NAME,
        SF_LASTNAME,
        SF_YEARS_FORMATION,
        SF_INSTITUTION_PROVIDED_ID,
        SF_BIRTHDATE,
        SF_BIRHTCOUNTRY,
        SF_GENDER,
        SF_AGE,
        SF_LOCAL_ID
    } SubjectField;

    typedef enum {
        AUF_NAME,
        AUF_LASTNAME,
        AUF_EMAIL,
        AUF_LOCAL_ID
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
        STF_TRIAL_LIST,
        STF_STUDY_CONFIGURATION,
        STF_EXPERIMENT_DESCRIPTION,
        STF_VERSION
    } StudiesField;

    typedef enum {
        PP_RESOLUTION_WIDTH,
        PP_RESOLUTION_HEIGHT,
        PP_NBACK_HITBOXES,
        PP_GONOGO_HITBOXES
    } ProcessingParameterField;

    typedef enum {
        DSF_RAW_DATA
    } DataSetField;

    typedef QMap<StudyConfigurationParameters,StudyConfigurationValue> StudyConfiguration;
    typedef QMap<TrialField,QString> TrialConfiguration;
    typedef QMap<MetadataField,QString> Metadata;
    typedef QMap<SubjectField,QString> SubjectData;
    typedef QMap<AppUserField,QString> ApplicationUserData;
    typedef QMap<ProcessingParameterField,QVariant> ProcessingParameters;

    /**
     * @brief saveJSONFile The UTF-8 JSON enconded string representing all the stored internal data, is saved to the file passed as a parameter.
     * @param file_name Where to save the file
     * @param pretty_print If true prints a human readable JSON (use only for debugging).
     * @return True if the file was generated, false otherwise. Error will contain the error message.
     */
    bool saveJSONFile(const QString &file_name, bool pretty_print = false);

    /**
     * @brief getError
     * @return The last error message from any operation done. Empty if no error.
     */
    QString getError() const;

    ////////////////////////// ATTACHMENT DATA RELATED FUNCTIONS
    bool setMetadata(const Metadata &metadata);
    bool setApplicationUserData(const AppUserType &type, const ApplicationUserData &au_data);
    bool setSubjectData(const SubjectData & subject_data);

    ////////////////////////// CUSTOM FIELDS
    void addCustomMetadataFields(const QString field_name, const QString field_value);

    bool setProcessingParameters(const ProcessingParameters &pp);

    ////////////////////////// STUDY SETUP RELATED FUNCTIONS

    bool addStudy(const StudyType &study, const StudyConfiguration &studyConfiguration, const QString &experimentDescription, const QString &version);

    bool setCurrentStudy(const StudyType &study);

    void setCurrentTrialListType(const TrialListType &tlt);

    bool addNewTrial(const QString &trial_id, const TrialConfiguration &trial_configuration);

    void setCurrentDataSet(const DataSetType &data_set_type);

    void addNewRawDataVector(const QVariantMap &raw_data_vector);

    void finalizeTrial(const QString &reponse = "");

    void finalizeDataSet();

    bool finalizeStudy();

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
    static QVariantMap GenerateStdRawDataVector(float timestamp, float xr, float yr, float xl, float yl, float pr, float pl);
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
    static QVariantMap GenerateReadingRawDataVector(float timestamp, float xr, float yr, float xl, float yl, float pr, float pl, float char_r, float char_l, float word_r, float word_l);

private:
    QVariantMap data;
    QString error;

    // Variables where information is stored as it is being gerated . On finalization they are passed on to the permanent structure.
    QVariantList currentTrialList;
    QVariantMap currentTrial;
    QVariantMap currentDataSetMap;
    QVariantList currentRawDataList;

    // Names and identification for currently selected study.
    QString currentTrialListType;
    QString currentDataSetType;
    StudyType currentlySelectedStudy;

    /**
     * @brief configureReadingStudy Sets all necessary configuration values for a reading experiment
     * @param params The map for the study parameters to be configured.
     * @param studyConfiguration The current configuration map which will be modifed and returned with the actual configuration parameters.
     * @return The studyConfiguration map with values ready to be inserted in general studies structure. Error will not be empty if there was an error.
     */
    QVariantMap configureReadingStudy(const StudyConfiguration &params, QVariantMap studyConfiguration);
    QVariantMap configureBindingStudy(const StudyConfiguration &params, QVariantMap studyConfiguration);

    // All initialization functions
    static QMap<StudyType,const char*> initStudyTypeMap();
    static QMap<StudyConfigurationParameters,const char*> initStudyParametersMap();
    static QMap<StudyConfigurationValue,const char*> initStudyValuesMap();
    static QMap<TrialListType,const char*> initTrialListTypesMap();
    static QMap<ValueVectorParameter,const char*> initValueVectorParameters();
    static QMap<DataSetType,const char*> initDataSetTypesMap();
    static QMap<TrialField,const char*> initTrialFieldsMap();
    static QMap<DataSetField,const char*> initDataSetFieldsMap();    
    static QMap<MetadataField,const char*> initMetadataFieldMap();
    static QMap<SubjectField,const char*> initSubjectFieldMap();
    static QMap<AppUserField,const char*> initAppUserFieldMap();
    static QMap<AppUserType,const char*> initAppUserTypeMap();
    static QMap<StudiesField,const char*> initStudiesFieldMap();
    static QMap<ProcessingParameterField,const char*>initProcessingParametersMap();


    // These maps map each enum type to string
    static const QMap<StudyType,const char*> mapStudyType;
    static const QMap<StudyConfigurationParameters,const char*> mapStudyParmeters;
    static const QMap<StudyConfigurationValue,const char*> mapStudyValues;
    static const QMap<TrialListType,const char*> mapTrialListTypes;
    static const QMap<ValueVectorParameter,const char*> mapValueVectorParameters;
    static const QMap<DataSetType,const char*> mapDataSetTypes;
    static const QMap<TrialField,const char*> mapTrialFields;
    static const QMap<DataSetField,const char*> mapDataSetFields;    
    static const QMap<MetadataField,const char *> mapMetadataField;
    static const QMap<SubjectField,const char *>  mapSubjectField;
    static const QMap<AppUserField,const char *>  mapAppUserField;
    static const QMap<AppUserType,const char *>   mapAppUserType;
    static const QMap<StudiesField,const char*>   mapStudiesField;
    static const QMap<ProcessingParameterField,const char*> mapProcessingParameterField;

    // The main fields in ANY study container.
    static const char *MAIN_FIELD_SUBJECT_DATA;
    static const char *MAIN_FIELD_PROCESSING_PARAMETERS;
    static const char *MAIN_FIELD_FREQUENCY_CHECK_PARAMETERS;
    static const char *MAIN_FIELD_APPLICATION_USER;
    static const char *MAIN_FIELD_METADATA;
    static const char *MAIN_FIELD_STUDIES;

    // Constant strings to be used in all structures.
    static const char *CURRENT_JSON_STRUCT_VERSION;



};

#endif // RAWDATACONTAINER_H
