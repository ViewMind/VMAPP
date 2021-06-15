#ifndef VIEWMINDDATACONTAINER_H
#define VIEWMINDDATACONTAINER_H

/**
 * @brief The RawDataContainer class
 * @details Class meant the raw data obtained from an study as well as the the data required all data required to analyze it and generate a report.
 */

#include <QVariantMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QSet>
#include <QDebug>
#include "VMDC.h"
#include "../debug.h"

// Fields specific for each experiment.
class ViewMindDataContainer
{
public:
    ViewMindDataContainer();

    /**
     * @brief saveJSONFile The UTF-8 JSON enconded string representing all the stored internal data, is saved to the file passed as a parameter.
     * @param file_name Where to save the file
     * @param pretty_print If true prints a human readable JSON (use only for debugging).
     * @return True if the file was generated, false otherwise. Error will contain the error message.
     */
    bool saveJSONFile(const QString &file_name, bool pretty_print = false);

    bool loadFromJSONFile(const QString &file_name);

    /**
     * @brief getError
     * @return The last error message from any operation done. Empty if no error.
     */
    QString getError() const;

    ////////////////////////// ATTACHMENT DATA RELATED FUNCTIONS
    bool setMetadata(const QVariantMap &metadata);
    bool setApplicationUserData(const QString &type, const QVariantMap &au_data);
    bool setSubjectData(const QVariantMap &subject_data);
    void setQCParameters(const QVariantMap &qcparams);

    ////////////////////////// CUSTOM FIELDS
    void addCustomMetadataFields(const QString field_name, const QString field_value);

    bool setProcessingParameters(const QVariantMap &pp);

    bool setQCVector(const QString &study, const QString &qcfield ,const QVariantList &vector);

    ////////////////////////// FUNCTIONS FOR ADDING STUDY DATA.

    bool addStudy(const QString &study, const QVariantMap &studyConfiguration, const QString &experimentDescription, const QString &version);

    bool setCurrentStudy(const QString &study);

    bool addNewTrial(const QString &trial_id, const QString &type);

    bool setCurrentDataSet(const QString &data_set_type);

    void addNewRawDataVector(const QVariantMap &raw_data_vector);

    void addFixationVectorL(const QVariantMap &fixation_vector);

    void addFixationVectorR(const QVariantMap &fixation_vector);

    void finalizeTrial(const QString &reponse = "");

    void finalizeDataSet();

    bool finalizeStudy();

    void markFileAsFinalized();

    ////////////////////////// FUNCTIONS FOR GETTING STUDY DATA.
    QVariantList getStudyTrialList(const QString &study);



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

    ////////////////////////// FUNCTION FOR READING DATA. ONLY WHAT IS ABSOLUTELY NECESSARY.
    QStringList getStudies() ;
    QString getStudyStatus(const QString &study);
    QString getMetadaStatus() ;
    QVariantMap getStudyConfiguration(const QString study_type) ;
    QString getStudyCode(const QString &study) ;
    QStringList getMetaDataDateTime() ;  // Returns a display string and then a string that cna be used for sorting. The list always has two values.
    QVariantMap getApplicationUserData(const QString &type);
    QVariantMap getSubjectData();
    QVariantMap getProcessingParameters() const;
    QVariantMap getQCParameters() const;


private:
    QVariantMap data;
    QString error;

    // Variables where information is stored as it is being gerated . On finalization they are passed on to the permanent structure.
    QVariantList currentTrialList;
    QVariantMap currentTrial;
    QVariantMap currentDataSetMap;
    QVariantList currentRawDataList;
    QVariantList currentLFixationVectorL;
    QVariantList currentLFixationVectorR;

    // Names and identification for currently selected study.
    QString currentDataSetType;
    QString currentlySelectedStudy;

    /**
     * @brief checkHiearchyChain Checks that the specific hiearchy of fields exists in the data QVariantMap.
     * @param hieararchy A list of the fields to check in hiearchical order. So a list containg Field1, Field2 will expect data to have a map named Field1 and that map should have an entry named Field2.
     * @return True if all the keys exist in the hiearchy provided, false otherwise.
     */
    bool checkHiearchyChain(const QStringList &hieararchy);


    // The main fields in ANY study container.
    static QString MAIN_FIELD_SUBJECT_DATA;
    static QString MAIN_FIELD_PROCESSING_PARAMETERS;
    static QString MAIN_FIELD_FREQUENCY_CHECK_PARAMETERS;
    static QString MAIN_FIELD_APPLICATION_USER;
    static QString MAIN_FIELD_METADATA;
    static QString MAIN_FIELD_STUDIES;

    // Constant strings to be used in all structures.
    static QString CURRENT_JSON_STRUCT_VERSION;


};

#endif // VIEWMINDDATACONTAINER_H
