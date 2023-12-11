#ifndef STUDYCONTROL_H
#define STUDYCONTROL_H

#include <QObject>

#include "../RenderServerClient/RenderServerPackets/RenderServerStrings.h"
#include "../RenderServerClient/RenderServerPackets/renderserverpacket.h"
#include "../RawDataContainer/VMDC.h"
#include "../RawDataContainer/viewminddatacontainer.h"
#include "../debug.h"
#include "eyexperimenter_defines.h"

#include "../StudyControl/nback/nbackconfigurator.h"
#include "../StudyControl/gng3D/gngspheresconfigurator.h"
#include "../StudyControl/binding/bindingconfigurator.h"
#include "../StudyControl/gng/gngconfigurator.h"



namespace StudyGlobals {
   static const QString SUBJECT_DIR_ABORTED     = "exp_aborted";
   static const QString SUBJECT_DIR_SENT        = "sent";
}

class StudyControl: public QObject
{
    Q_OBJECT

public:
    StudyControl();

    typedef enum {SES_OK, SES_FAILED, SES_ABORTED} StudyEndStatus;
    typedef enum {ISC_ABORT, ISC_BINDING_SAME, ISC_BINDING_DIFF, ISC_CONTINUE} InStudyCommand;
    typedef enum {ST_3D, ST_2D, ST_NOT_SET} StudyType;
    typedef enum { SS_NONE, SS_EXPLAIN, SS_EXAMPLE, SS_EVAL, SS_WAITING_FOR_STUDY_DATA } StudyState;

    /**
     * @brief startStudy
     * @details Intializes the VMDC structure required for the file. Then calls the study specific configuration function and
     * creates the packet to study initialization. This will start the study in the first evaluation screen.
     * @param workingDir - The path to the subject study directory
     * @param studyFile - The full absolute path to the file where where the study information will be stored.
     * @param studyConfig - A map of configuration variables used to determine everthing that is not set about the study.
     * @param studyName - The study name which we are trying to start.
     */
    void startStudy(const QString &workingDir,
                    const QString &studyFile,
                    const QVariantMap &studyConfig,
                    const QString &studyName);

    /**
     * @brief getControlPacket
     * @return The Render Server Packet that contains the communication to the server.
     */
    RenderServerPacket getControlPacket() const;

    /**
     * @brief getStudyEndStatus
     * @return The study end status. If it the study ended, something failed or was aborted.
     */
    StudyEndStatus getStudyEndStatus() const;

    /**
     * @brief getStudyPhase
     * @return The current study phase: none, evaluation, examples or explanation.
     */
    StudyState getStudyPhase() const;

    /**
     * @brief receiveRenderServerPacket
     * @details Process an incomming packet from the render server and takes action appropiately.
     * it can be anything from updating texts in the UI to receiving an ACK or an error or the final study data.
     * @param control - The packet received.
     */
    void receiveRenderServerPacket(const RenderServerPacket &control);

    /**
     * @brief startExamplesPhase - Prepares packet to start the Examples Phase of the study.
     */
    void startExamplesPhase();

    /**
     * @brief nextExplanationSlide - Prepares packet to move to the next explanation slide.
     */
    void nextExplanationSlide();

    /**
     * @brief previousExplanationSlide - Prepares packet to move to the previous explanation slide.
     */
    void previousExplanationSlide();

    /**
     * @brief nextExampleSlide - Prepares packet to move to the next example slide.
     */
    void nextExampleSlide();

    /**
     * @brief startEvaluation - Prepares packet to start evaluation.
     */
    void startEvaluation();

    /**
     * @brief sendInStudyCommand - Sends a command required by the study while it's in the evaluation phase.
     * @param cmd - The in-study command to send.
     */
    void sendInStudyCommand(InStudyCommand cmd);

    /**
     * @brief getCurrentStudyType - Simple way to check whether study is 3D or not.
     * @return The study type
     */
    StudyType getCurrentStudyType() const;

    /**
     * @brief setCalibrationValidationData - Stores the calibration history in the current study file.
     * @param calib_data - The calibration history as obtained from a Calibration Manager.
     */
    void setCalibrationValidationData(const QVariantMap &calib_data);

    /**
     * @brief getDataFilesLocation - Gets a list of the study and idx file locations. Full path.
     * @return A list of file locations. First item is the study file. Second item is the idx file.
     */
    QStringList getDataFilesLocation() const;

    /**
     * @brief requestStudyData - Simply sends the request study data control packet.
     */
    void requestStudyData();

    /**
     * @brief isStudyInDataTransfer
     * @return True if the study has finished and the study data transfer has started. False otherwise. WARNING: it's not reset until the NEXT Study start.
     */
    bool isStudyInDataTransfer() const;

    /**
     * @brief FillNumberOfSlidesInExplanations - Fills a dictionary with the number of slides in each key of explanation.
     * @param language - The configuration manager with the language keys.
     */
    static void FillNumberOfSlidesInExplanations(ConfigurationManager *language);

signals:

    /**
     * @brief newPacketAvailable - A new packet needs to be sent to the Remote Render Server.
     */
    void newPacketAvailable();

    /**
     * @brief updateStudyMessages
     * @param string_value_map - The string -> string map used to update the UI. During the evaluation phase.
     */
    void updateStudyMessages(const QVariantMap &string_value_map);

    /**
     * @brief studyEnd - Signals that the study has finished. It is emitted again when the study data has been received.
     */
    void studyEnd();

private:

    // Packet to send to render server.
    RenderServerPacket rspacket;

    // The initial study configuration augmented, with other stuff if necessary.
    QVariantMap studyConfiguration;

    // The full set of strings to access all directory and files touched by a given study.
    QString workingDirectory;
    QString currentStudyFile;
    QString fullPathCurrentStudyFile;
    QString fullPathCurrentIDXFile;
    QString evaluationStartTime;

    // Flag used to determine when then next ACK is an abort request.
    bool expectingAbortACK;

    // Flag to determine whether the study end signal was sent during the study or during data transfer.
    bool studyInDataTransfer;

    // Where the data will be stored.
    ViewMindDataContainer rawdata;

    // The current state of the study control.
    StudyState studyState;

    // Store the study end status in order to retrieve it.
    StudyEndStatus studyEndStatus;

    // Whether the study is 2D or 3D.
    StudyType studyType;

    // Index that determines which explanation screen we should be writing.
    qint32 currentStudyExplanationScreen;
    QString studyExplanationLanguageKey;

    /**
     * @brief saveDataToHardDisk - Saves raw data file to disk.
     * @return true if successfull, false otherwise.
     */
    bool saveDataToHardDisk();

    /**
     * @brief studyFinalizationLogic - Figures out if a study can be finalized or not.
     * @details Finalized studies are the only one that appear ready to be sent for procesing.
     * Multi part studies (i.e binding) require two complete studies in order to finalize.
     * @return True if it can be finalized. False otherwise.
     */
    bool studyFinalizationLogic();

    /**
     * @brief emitFailState - Shorcut function to coreccty set variables and emit the study end signal on failure of some kind.
     * @param wasAborted - If true the study was aborted, else it was a failure.
     */
    void emitFailState(bool wasAborted = false);

    /**
     * @brief emitNewExplanationMessage - Shortcut function to emit the index of the current explanation screen that should be displayed.
     */
    void emitNewExplanationMessage();


    // Different text explanation keys based on different studies.
    static inline const char * STUDY_TEXT_KEY_BINDING_UC     = "explanation_phase_list_binding_uc";
    static inline const char * STUDY_TEXT_KEY_BINDING_BC     = "explanation_phase_list_binding_bc";
    static inline const char * STUDY_TEXT_KEY_GONOGO         = "explanation_phase_list_gonogo";
    static inline const char * STUDY_TEXT_KEY_GONOGO_3D      = "explanation_phase_list_gonogo_3D";
    static inline const char * STUDY_TEXT_KEY_GONOGO_3D_VS   = "explanation_phase_list_gonogo_3D_vs";
    static inline const char * STUDY_TEXT_KEY_PASS_BALL      = "explanation_phase_list_pass_ball";
    static inline const char * STUDY_TEXT_KEY_NBACKRT        = "explanation_phase_list_nbackrt";
    static inline const char * STUDY_TEXT_KEY_NBACK_3        = "explanation_phase_list_nback_3";
    static inline const char * STUDY_TEXT_KEY_NBACK_4        = "explanation_phase_list_nback_4";
    static inline const char * STUDY_TEXT_KEY_NBACKVS        = "explanation_phase_list_nbackvs";

    // The normal, slow and variable speed values fo rthe GONOGO are codified in the numbers 0,1 and 2 respectively.
    // We need to detect the 2 specifically to change the explanantion language key.
    const qint32 CODE_FOR_GNG3D_VS = 2;

    static QMap<QString,int> NumberOfExplanationSlides;
};

#endif // STUDYCONTROL_H
