#include "finishedstudyfileloader.h"

FinishedStudyFileLoader::FinishedStudyFileLoader(QObject *parent) : QThread{parent}
{

}

void FinishedStudyFileLoader::setRawDataData(const ViewMindDataContainer &vmdc, const QString &study_file){
    this->rawdata = vmdc;
    this->study_data_filename = study_file;
}

QString FinishedStudyFileLoader::getError() const {
    return this->error;
}

void FinishedStudyFileLoader::clear(){
    this->rawdata.clearAllData();
}

void FinishedStudyFileLoader::run(){

    this->error = "";

    // First we check we got the file.
    if (study_data_filename == ""){
        this->error = "Study has ended but no file with stored study data is provided. Aborting";
        return;
    }

    // Now we check the file exists.
    if (!QFile(study_data_filename).exists()){
        this->error = "Study has ended but no file with storoed study data is provided. Aborting";
        return;
    }

    StaticThreadLogger::log("StudyControl::finishedStudyFileLoader","Loading JSON Data ....");

    QString error = "";
    QVariantMap data = Globals::LoadJSONFileToVariantMap(study_data_filename,&error);
    if (error != ""){
        this->error = "Unable to load study file '" + study_data_filename + "' for reading. Reason: " + error;
        return;
    }

    StaticThreadLogger::log("StudyControl::finishedStudyFileLoader","Processing loaded data ....");

    // Process and store the data.
    if (this->processAndStoreStudyData(data)){
        // If all is OK we delete the file.
        QFile(study_data_filename).remove();
    }
    else {
        StaticThreadLogger::error("StudyControl::finishedStudyFileLoader","Something went wrong while processing file '" + study_data_filename + "'. Not deleting");
        return;
    }

    StaticThreadLogger::log("StudyControl::finishedStudyFileLoader","Finished");

}


bool FinishedStudyFileLoader::processAndStoreStudyData(const QVariantMap &control) {

    QVariantList trialList = control.value(RRS::PacketStudyData::TRIAL_LIST_2D).toList();
    QVariantMap  studyData = control.value(RRS::PacketStudyData::ELEMENTS_3D).toMap();
    qint32 explanationDuration = control.value(RRS::PacketStudyData::EXPLAIN_LENGTH).toInt();
    qint32 exampleDuration = control.value(RRS::PacketStudyData::EXAMPLE_LENGTH).toInt();
    qint32 evalDuration = control.value(RRS::PacketStudyData::EVAL_LENGTH).toInt();
    qint32 pauseDuration = control.value(RRS::PacketStudyData::PAUSE_LEGTH).toInt();

    /**
     * The list of study data fields that need to be added as processing parameters.
     * The values are required for actually processing each study file. Whether they have content or not
     * will depend on the study.
     */
    QVariantMap addToPP;
    addToPP[RRS::PacketStudyData::FOV]          = VMDC::ProcessingParameter::FOV_3D;
    addToPP[RRS::PacketStudyData::MESH_FILES]   = VMDC::ProcessingParameter::MESH_FILES;
    addToPP[RRS::PacketStudyData::MESH_STRUCTS] = VMDC::ProcessingParameter::MESH_STRUCT;
    addToPP[RRS::PacketStudyData::ORIGIN_PTS]   = VMDC::ProcessingParameter::ORIGIN_POINTS;
    addToPP[RRS::PacketStudyData::SPHERE_R]     = VMDC::ProcessingParameter::SPHERE_RADIOUS;
    addToPP[RRS::PacketStudyData::RES_W]        = VMDC::ProcessingParameter::RESOLUTION_WIDTH;
    addToPP[RRS::PacketStudyData::RES_H]        = VMDC::ProcessingParameter::RESOLUTION_HEIGHT;

    // The hitboxes can be go no go hitboxes or NBAck. depending on the study.
    if ( (rawdata.getCurrentStudy() == VMDC::Study::NBACK) ||
         (rawdata.getCurrentStudy() == VMDC::Study::NBACKVS) ||
         (rawdata.getCurrentStudy() == VMDC::Study::NBACKRT) ){
        addToPP[RRS::PacketStudyData::HITBOXES] = VMDC::ProcessingParameter::NBACK_HITBOXES;
    }
    else if (rawdata.getCurrentStudy() == VMDC::Study::GONOGO){
        addToPP[RRS::PacketStudyData::HITBOXES] = VMDC::ProcessingParameter::GONOGO_HITBOXES;
    }

    // To do a bit of control and that is it.
    QString ndata_points = control.value(RRS::PacketStudyData::N_DP_DURING_EVAL).toString();
    StaticThreadLogger::log("StudyControl::receiveRenderServerPacket","Finalized evaluation. Number of datapoints gathered is: " + ndata_points);

    // First we set the processing parameter missing fields.
    QVariantMap pp = rawdata.getProcessingParameters();

    // We now add all fields.
    QStringList packet_keys = addToPP.keys();
    for (qint32 i = 0; i < packet_keys.size(); i++){
        QString packetKey = packet_keys.at(i);
        QString ppKey     = addToPP.value(packetKey).toString();
        pp[ppKey] = control.value(packetKey);
    }

    if (!rawdata.setProcessingParameters(pp)){
        this->error = "StudyControl::receiveRenderServerPacket","Failed to set processing parameters in raw data. Reason: " + rawdata.getError();
        return false;
    }

    // We need to figure out if this is file we need to finalize.
    bool shouldBeFinalized = this->studyFinalizationLogic();

    // If the trial list field is a list that is NOT empty, this a 2D study file.
    if (trialList.size() > 0){
        // We have a 2D file.
        if (!rawdata.setFullTrialList(trialList,
                                      explanationDuration,exampleDuration,pauseDuration,evalDuration,
                                      evaluationStartTime)){
            this->error = "Failed to set the full trial list in raw data. Reason: " + rawdata.getError();
            return false;
        }
    }
    else {
        // This should be a 3D study.
        if (!rawdata.setFullElement3D(studyData,
                                      exampleDuration,exampleDuration,pauseDuration,evalDuration,
                                      evaluationStartTime)){
            this->error = "Failed to set the full 3d element data in raw data. Reason: " + rawdata.getError();
            return false;
        }
    }

    // Now that we are ready, then we mark the file as finalized if it is.
    if (shouldBeFinalized){
        rawdata.markFileAsFinalized();
    }

    return true;

}



