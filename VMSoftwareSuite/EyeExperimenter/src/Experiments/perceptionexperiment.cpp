#include "perceptionexperiment.h"

PerceptionExperiment::PerceptionExperiment(QWidget *parent):Experiment(parent)
{
    manager = new PerceptionManager();
    m = dynamic_cast<PerceptionManager*>(manager);

    expHeader = HEADER_PERCEPTION_EXPERIMENT;
    outputDataFile = FILE_OUTPUT_PERCEPTION;

    // Connecting the timer time out with the time out function.
    connect(&stateTimer,&QTimer::timeout,this,&PerceptionExperiment::onTimeOut);

}


void PerceptionExperiment::onTimeOut(){
    stateTimer.stop();

    if ( m->getPerceptionTrialState() == PerceptionManager::PTS_YES_OR_NO){
        if (!wasAnswerSet){
            // Time out with no answer.
            QVariantList dataS;
            dataS  << "ANS -1";
            etData << QVariant(dataS);
        }
    }

    if (m->drawNextTrialState()){
        stateTimer.stop();
        state = STATE_STOPPED;
        ExperimentResult er;
        if (error.isEmpty()) er = ER_NORMAL;
        else er = ER_WARNING;

        // Adding the part identifier to the file.
        QString mp_id = MULTI_PART_FILE_STUDY_IDENTIFIER;
        mp_id = mp_id + " " + multiPartIdentifier;

        QVariantList dataS;
        dataS  << mp_id;
        etData << QVariant(dataS);

        if (!saveDataToHardDisk()){
            emit(experimentEndend(ER_FAILURE));
        }
        else {

           if (multiPartIdentifier == PERCEPTION_STUDY_LAST_MULTI_PART_IDENTIFIER){
               // The study has finished. We need to finalize it.
               QString temp = finalizeMultiPartStudyFile(dataFile);
               if (!temp.isEmpty()){
                   error = "[Perception Stuydy] Failed to finalize study: " + temp;
                   er = ER_FAILURE;
               }
           }

           emit(experimentEndend(er));

        }
    }
    else{
        PerceptionManager::PerceptionTrialState pts = m->getPerceptionTrialState();
        QVariantList dataS;

        switch (pts) {
        case PerceptionManager::PTS_CROSS_ONLY:

            // Saving the header.
            dataS  << m->getCurrentTrialHeader();
            etData << QVariant(dataS);

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
        QVariantList dataS;
        dataS << data.time
              << data.xRight
              << data.yRight
              << data.xLeft
              << data.yLeft
              << data.pdRight
              << data.pdLeft;
        etData << QVariant(dataS);
    }
    else if (pts == PerceptionManager::PTS_YES_OR_NO){
       m->highlightSelection(data.avgX(),data.avgY());
       qint32 selectedAns = eyeSelector.isSelectionMade(data.avgX(),data.avgY());
       if (selectedAns != -1){
           // Saving the answer
           QVariantList dataS;
           dataS  << "ANS " + QString::number(selectedAns);
           etData << QVariant(dataS);
           wasAnswerSet = true;
           onTimeOut();
       }
    }

}

bool PerceptionExperiment::startExperiment(ConfigurationManager *c){

    // Adding to the file the parameter for perception study type.
    if (c->getBool(CONFIG_PERCEPTION_IS_TRAINING)){
        outputDataFile = outputDataFile + "_" + CONFIG_P_PERCEPTION_TRAINING;
    }
    else{
        outputDataFile = outputDataFile + "_" + CONFIG_P_PERCEPTION_REHAB;
    }

    bool isTraining = c->getBool(CONFIG_PERCEPTION_IS_TRAINING);

    if (!Experiment::startExperiment(c)) return false;

    // We need to check if this is the first part of the multi part file
    if (multiPartIdentifier.isEmpty()){
        QString temp = makeStudyMultiPart(&dataFile);
        if (!temp.isEmpty()){
            error = "[Perception Study] Making the Study Multi File got error: " + temp;
            return false;
        }
        multiPartIdentifier = "0";
    }
    else{

        // In this case we need to make sure that type (Training or Rehab) is the same regardless of configuration. We get that from file name.
        // Which was over written in the Experiment::startExperiment
        QFileInfo info(dataFile);
        QStringList name_parts = info.baseName().split("_",QString::SkipEmptyParts);
        //qDebug() << name_parts;
        QString type = "";

        if (name_parts.size() > 2){
            type = name_parts.at(1);
        }
        else{
            error = "Could not determine perception study type from file name "  + dataFile;
            return false;
        }

        if (type == CONFIG_P_PERCEPTION_REHAB){
            isTraining = false;
        }
        else if (type == CONFIG_P_PERCEPTION_TRAINING){
            isTraining = true;
        }
        else{
            error = "Could not determine the perception study from filename with: "  + type;
            return false;
        }

        // Current part is the last part +1
        bool ok = false;
        qint32 temp = multiPartIdentifier.toInt(&ok);
        if (!ok){
            error = "[Perception Study] Unexpected multi part identifier: " + multiPartIdentifier;
            return false;
        }
        temp++;
        multiPartIdentifier = QString::number(temp);
    }

    error = "";

    state = STATE_RUNNING;

    m->prepareForTrialSet();
    m->drawNextTrialState();

    // Adding the header for the first trial.
    QVariantList dataS;
    dataS  << m->getCurrentTrialHeader();
    etData << QVariant(dataS);

    eyeSelector.reset();
    eyeSelector.setTargetCountForSelection(HOLD_TIME_FOR_ANS_SELECTION*c->getReal(CONFIG_SAMPLE_FREQUENCY));
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

    if (!vrEnabled){
        this->show();
        this->activateWindow();
    }

    return true;

}


void PerceptionExperiment::togglePauseExperiment(){
    // Pause is not managed here, for this experiment.
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
