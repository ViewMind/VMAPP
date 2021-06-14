#include "perceptionexperiment.h"


const qint32 PerceptionExperiment::HOLD_TIME_FOR_ANS_SELECTION = 1;     // Seconds
const qint32 PerceptionExperiment::HOLD_TIME_CROSS_ONLY        = 500;   // ms.
const qint32 PerceptionExperiment::HOLD_TIME_YES_NO_REHAB      = 3000;  // ms.
const qint32 PerceptionExperiment::HOLD_TIME_YES_NO_TRAIN      = 2000;  // ms.
const qint32 PerceptionExperiment::HOLD_TIME_TRI_REHAB         = 700;   // ms
const qint32 PerceptionExperiment::HOLD_TIME_TRI_TRAIN         = 300;   // ms

PerceptionExperiment::PerceptionExperiment(QWidget *parent, const QString &studyType):Experiment(parent,studyType)
{
    manager = new PerceptionManager();
    m = dynamic_cast<PerceptionManager*>(manager);

    // Connecting the timer time out with the time out function.
    connect(&stateTimer,&QTimer::timeout,this,&PerceptionExperiment::onTimeOut);

    metaStudyType = VMDC::MultiPartStudyBaseName::PERCEPTION;

}


void PerceptionExperiment::onTimeOut(){
    stateTimer.stop();

    if ( m->getPerceptionTrialState() == PerceptionManager::PTS_YES_OR_NO){
        if (!wasAnswerSet){
            // Time out with no answer.
            rawdata.finalizeDataSet();
            finalizeOnlineFixations();
            rawdata.finalizeTrial("-1");
        }
    }

    if (m->drawNextTrialState()){
        stateTimer.stop();
        state = STATE_STOPPED;
        ExperimentResult er;
        if (error.isEmpty()) er = ER_NORMAL;
        else er = ER_WARNING;

        bool ans;
        ans = rawdata.finalizeStudy();

        // We check that ALL parts are present to finalize study.
        QStringList all_parts;
        all_parts << VMDC::Study::PERCEPTION_1 << VMDC::Study::PERCEPTION_2 << VMDC::Study::PERCEPTION_3 << VMDC::Study::PERCEPTION_4
                     << VMDC::Study::PERCEPTION_5 << VMDC::Study::PERCEPTION_6 << VMDC::Study::PERCEPTION_7 << VMDC::Study::PERCEPTION_8;

        QStringList present_parts = rawdata.getStudies();

        bool not_finished = false;
        for (qint32 i = 0; i < all_parts.size(); i++){
            if (!present_parts.contains(all_parts.at(i))){
                not_finished = true;
                break;
            }
        }

        if (!not_finished){
            rawdata.markFileAsFinalized();
        }

        if (!ans){
            error = "Failed on Reading study finalization: " + rawdata.getError();
            emit (experimentEndend(ER_FAILURE));
            return;
        }

        if (!saveDataToHardDisk()){
            emit(experimentEndend(ER_FAILURE));
        }
        else{
            emit(experimentEndend(er));
        }
    }
    else{
        PerceptionManager::PerceptionTrialState pts = m->getPerceptionTrialState();

        switch (pts) {
        case PerceptionManager::PTS_CROSS_ONLY:

            // Creating the new trial.
            if (!addNewTrial()){
                stateTimer.stop();
                emit(experimentEndend(ER_FAILURE));
                return;
            }
            rawdata.setCurrentDataSet(VMDC::DataSetType::UNIQUE);


            stateTimer.start(HOLD_TIME_CROSS_ONLY);
            break;
        case PerceptionManager::PTS_YES_OR_NO:
            eyeSelector.reset();
            wasAnswerSet = false;
            stateTimer.start(holdTimeYesAndNo);
            break;
        case PerceptionManager::PTS_TRIANGLES:
            stateTimer.start(holdTimeYesAndNo);
            break;
        default:
            break;
        }
    }
}

void PerceptionExperiment::newEyeDataAvailable(const EyeTrackerData &data){

    Experiment::newEyeDataAvailable(data);
    if (state != STATE_RUNNING) return;
    if (data.isLeftZero() && data.isRightZero()) return;

    PerceptionManager::PerceptionTrialState pts = m->getPerceptionTrialState();

    if (pts == PerceptionManager::PTS_TRIANGLES){
        // Format: Image ID, time stamp for right and left, word index, character index, sentence length and pupil diameter for left and right eye.
        rawdata.addNewRawDataVector(ViewMindDataContainer::GenerateStdRawDataVector(data.time,data.xRight,data.yRight,data.xLeft,data.yLeft,data.pdRight,data.pdLeft));
        computeOnlineFixations(data);
    }
    else if (pts == PerceptionManager::PTS_YES_OR_NO){
        m->highlightSelection(data.avgX(),data.avgY());
        qint32 selectedAns = eyeSelector.isSelectionMade(data.avgX(),data.avgY());
        if (selectedAns != -1){
            // Saving the answer
            rawdata.finalizeDataSet();
            finalizeOnlineFixations();
            rawdata.finalizeTrial(QString::number(selectedAns));
            wasAnswerSet = true;
            onTimeOut();
        }
    }

}

bool PerceptionExperiment::startExperiment(const QString &workingDir, const QString &experimentFile,
                                           const QVariantMap &studyConfig, bool useMouse,
                                           QVariantMap pp){

    bool isTraining = (studyConfig.value(VMDC::StudyParameter::PERCEPTION_TYPE) == VMDC::PerceptionType::TRAINING);

    if (!Experiment::startExperiment(workingDir,experimentFile,studyConfig,useMouse,pp)) return false;

    QVariantMap configuration;
    if (studyConfig.value(VMDC::StudyParameter::LANGUAGE).toString() == VMDC::UILanguage::SPANISH){
        configuration.insert(PerceptionManager::CONFIGURE_YES_WORD,"SI");
    }
    else{
        configuration.insert(PerceptionManager::CONFIGURE_YES_WORD,"YES");
    }
    m->configure(configuration);

    error = "";

    state = STATE_RUNNING;

    m->prepareForTrialSet();
    m->drawNextTrialState();

    // Adding the header for the first trial.

    eyeSelector.reset();
    eyeSelector.setTargetCountForSelection(HOLD_TIME_FOR_ANS_SELECTION*pp.value(VMDC::ProcessingParameter::SAMPLE_FREQUENCY).toReal());
    eyeSelector.setTargetBoxes(m->getYesAndNoTargetBoxes());

    if (isTraining){
        //qDebug() << "Training times";
        holdTimeTriangles = HOLD_TIME_TRI_TRAIN;
        holdTimeYesAndNo  = HOLD_TIME_YES_NO_TRAIN;
    }
    else{
        //qDebug() << "Rehab times";
        holdTimeTriangles = HOLD_TIME_TRI_REHAB;
        holdTimeYesAndNo  = HOLD_TIME_YES_NO_REHAB;
    }

    stateTimer.start(HOLD_TIME_CROSS_ONLY);

    if (!Globals::EyeTracker::IS_VR || (useMouse)){
        this->show();
        this->activateWindow();
    }

    return true;

}



bool PerceptionExperiment::addNewTrial(){
    QString type = "";
    QString temp = m->getCurrentTrialHeader();
    QStringList parts = temp.split(" ");
    // Header is the ID TYPE COL ROW
    if (parts.size() != 4){
        error = "Expected the trial header " + temp + " to have 4 tokens and not " + QString::number(parts.size());
        return false;
    }


    // The first token is the id and the rest are the type.
    currentTrialID = parts.first();
    parts.removeFirst();
    type = parts.join(" ");

    if (!rawdata.addNewTrial(currentTrialID,type)){
        error = "Creating a new trial for " + currentTrialID + " gave the following error: " + rawdata.getError();
        return false;
    }
    return true;
}

void PerceptionExperiment::keyPressHandler(int keyPressed){
    // Making sure the experiment can be aborted, but any other key is ignored.
    if (keyPressed == Qt::Key_Escape){
        experimenteAborted();
        return;
    }
    else{
        if ((keyPressed == Qt::Key_N) && (state == STATE_RUNNING)){
            onTimeOut();
        }
    }
}
