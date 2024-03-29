#include "nbackrtexperiment.h"

const qint32 NBackRTExperiment::DEFAULT_NUMBER_OF_TARGETS =                    3;

// Possible pauses for the fielding experiment
const qint32 NBackRTExperiment::PAUSE_TRIAL_1 =                                32;
const qint32 NBackRTExperiment::PAUSE_TRIAL_2 =                                64;

const qint32 NBackRTExperiment::NBACKVS_MIN_HOLD_TIME =                        50;
const qint32 NBackRTExperiment::NBACKVS_MAX_HOLD_TIME =                        250;
const qint32 NBackRTExperiment::NBACKVS_STEP_HOLD_TIME =                       50;
const qint32 NBackRTExperiment::NBACKVS_START_HOLD_TIME =                      250;
const qint32 NBackRTExperiment::NBACKVS_NTRIAL_FOR_STEP_CHANGE =               2;
const qint32 NBackRTExperiment::NBACKVS_RETRIEVAL_TIMEOUT =                    3000;
const qint32 NBackRTExperiment::NBACKVS_TRANSITION_TIME =                      500;

const QString NBackRTExperiment::MSG_SEQ_HITS  = "studystatus_nback_sequence_hit";
const QString NBackRTExperiment::MSG_SEQ_TOUT  = "studystatus_nback_sequence_timeout";
const QString NBackRTExperiment::MSG_TARGET_VEL = "studystatus_nback_target_speed";

NBackRTExperiment::NBackRTExperiment(QWidget *parent, const QString &study_type):Experiment(parent,study_type){

    manager = new NBackManager();
    m = static_cast<NBackManager*>(manager);

    // Connecting the timer time out with the time out function.
    connect(&stateTimer,&QTimer::timeout,this,&NBackRTExperiment::onTimeOut);

    // Making sure that by default the light up all value is false
    lightUpAll = false;

}

bool NBackRTExperiment::startExperiment(const QString &workingDir, const QString &experimentFile,
                                        const QVariantMap &studyConfig){

    reducedTrialSet = false;

    if (studyType == VMDC::Study::NBACKVS){
        nbackConfig.minHoldTime              = NBACKVS_MIN_HOLD_TIME;
        nbackConfig.maxHoldTime              = NBACKVS_MAX_HOLD_TIME;
        nbackConfig.stepHoldTime             = NBACKVS_STEP_HOLD_TIME;
        nbackConfig.startHoldTime            = NBACKVS_START_HOLD_TIME;
        nbackConfig.numberOfTrialsForChange  = NBACKVS_NTRIAL_FOR_STEP_CHANGE;
        nbackConfig.numberOfTargets          = studyConfig.value(VMDC::StudyParameter::NUMBER_TARGETS).toInt();
        if (!studyConfig.value(VMDC::StudyParameter::NBACK_LIGHT_ALL).toBool()){
            trialRecognitionMachine.lightUpSquares = true;
        }
        else {
            lightUpAll = true;
        }

        timeOutTime = NBACKVS_RETRIEVAL_TIMEOUT;
        timeOutTransition = NBACKVS_TRANSITION_TIME;

    }
    else {
        // Variable Speed configuration for DEFAULT NBACK RT.
        qint32 hold_time = studyConfig.value(VMDC::StudyParameter::NBACK_HOLD_TIME).toInt();
        nbackConfig.minHoldTime              = hold_time;
        nbackConfig.maxHoldTime              = hold_time;
        nbackConfig.stepHoldTime             = 0;
        nbackConfig.startHoldTime            = hold_time;
        nbackConfig.numberOfTrialsForChange  = 0;
        //nbackConfig.numberOfTargets          = DEFAULT_NUMBER_OF_TARGETS;
        nbackConfig.numberOfTargets          = studyConfig.value(VMDC::StudyParameter::NUMBER_TARGETS).toInt();

        // The reduced trial set is used with 3 or 4 targets in the slow (now the new default) version of the study.
        reducedTrialSet = (studyType == VMDC::Study::NBACK);

        if (DBUGBOOL(Debug::Options::LIGHTUP_NBACKRT)) {
            qDebug() << "DBUG: Lighting Up Squares in NBackRT";
            trialRecognitionMachine.lightUpSquares = true;
        }
        else trialRecognitionMachine.lightUpSquares = false;

        timeOutTime = studyConfig.value(VMDC::StudyParameter::NBACK_TIMEOUT).toInt();
        timeOutTransition = studyConfig.value(VMDC::StudyParameter::NBACK_TRANSITION).toInt();

//        qDebug() << "Printing NBack Config Options for Non VS NBack";
//        nbackConfig.debugPrintOptions();
//        qDebug() << "Time out" << timeOutTime;
//        qDebug() << "Time out transition" << timeOutTransition;

    }


    if (!Experiment::startExperiment(workingDir,experimentFile,studyConfig)){
        emit Experiment::experimentEndend(ER_FAILURE);
        return false;
    }

    QVariantMap config;
    config.insert(NBackManager::CONFIG_IS_VS,(studyType == VMDC::Study::NBACKVS));
    config.insert(NBackManager::CONFIG_SLOW_STUDY_REDUCE_TRIAL_NUMBER,reducedTrialSet);
    if (studyConfig.value(VMDC::StudyParameter::LANGUAGE).toString() == VMDC::UILanguage::SPANISH){
        config.insert(NBackManager::CONFIG_PAUSE_TEXT_LANG,NBackManager::LANG_ES);
    }
    else{
        config.insert(NBackManager::CONFIG_PAUSE_TEXT_LANG,NBackManager::LANG_EN);
    }

    config.insert(NBackManager::CONFIG_N_TARGETS,studyConfig.value(VMDC::StudyParameter::NUMBER_TARGETS).toInt());

    m->configure(config);

    // The processing parameters requires the hitboxes that are going to be used, so they are added before they are set.
    processingParameters = addHitboxesToProcessingParameters(processingParameters);
    if (!rawdata.setProcessingParameters(processingParameters)){
        error = "Failed setting processing parameters on Reading: " + rawdata.getError();
        emit Experiment::experimentEndend(ER_FAILURE);
    }

    currentImage = 0;
    currentTrial = 0;
    error = "";
    ignoreData = false;

    nbackConfig.resetVSStateMachine();

    // Setting the eye to the EyeRecognition Machine.
    if ((leftEyeEnabled) && (!rightEyeEnabled)){
        trialRecognitionMachine.useRightEye = false;
    }
    else{
        trialRecognitionMachine.useRightEye = true;
    }

    state = STATE_RUNNING;
    tstate = TSF_START;
    stateTimer.setInterval(timeOutTransition);

    m->drawBackground();
    drawCurrentImage();

    // Resetting measuring variables.
    successfullTrials = false;
    timeoutTrials = false;

    this->renderCurrentStudyExplanationScreen();

    return true;
}

void NBackRTExperiment::onTimeOut(){

    if (tstate == TSF_SHOW_BLANKS){

        if (nbackConfig.wasSequenceCompleted){
            successfullTrials++;
        }
        else{
            timeoutTrials++;
        }
        nbackConfig.adjustSpeed();
        updateFronEndMessages();

        // Before drawing the start of a new trial, we check if a pause is necessary.
        if ( (currentTrial == PAUSE_TRIAL_1) || ( (currentTrial == PAUSE_TRIAL_2) && !reducedTrialSet) ){

            // Paused must be set to zero.
            if (state == STATE_RUNNING){
                rawdata.studyPauseSet(true);
                state = STATE_PAUSED;
                stateTimer.stop();
                m->drawPauseScreen();
                updateDisplay();
                //qDebug() << "Pausing wihth current trial" << currentTrial;
                return;
            }

        }

    }

    stateTimer.stop();
    nextState();
}

void NBackRTExperiment::nextState(){

    switch (tstate){
    case TSF_START:

        // If this is too much of light up box, then it can be removed by setting this as soon as the sequence is over.
        m->ligthOffAllBoxes();

        tstate = TSF_SHOW_TARGET;
        currentImage = 0;
        stateTimer.setInterval(nbackConfig.getCurrentHoldTime());

        if (!addNewTrial()){
            stateTimer.stop();
            emit Experiment::experimentEndend(ER_FAILURE);
            return;
        }
        currentDataSetType = VMDC::DataSetType::ENCODING_1;
        if (studyPhase == SP_EVALUATION) rawdata.setCurrentDataSet(currentDataSetType);

        break;
    case TSF_SHOW_TARGET:

        // End the previous data set.
        if (studyPhase == SP_EVALUATION) {
            finalizeOnlineFixations();
            rawdata.finalizeDataSet();
        }
        currentImage++;
        if (currentImage == nbackConfig.numberOfTargets){

            // Retrieval will begin
            if (studyPhase == SP_EVALUATION) rawdata.setCurrentDataSet(VMDC::DataSetType::RETRIEVAL_1);

            tstate = TSF_SHOW_BLANKS;
            QList<qint32> expectedSequence = m->getExpectedTargetSequenceForTrial(currentTrial, nbackConfig.numberOfTargets);
            m->setDebugSequenceValue(expectedSequence.first());
            trialRecognitionMachine.reset(expectedSequence);

            if (overrideTime != 0) stateTimer.setInterval(overrideTime);
            else stateTimer.setInterval(timeOutTime);
        }
        else {

            // WE compute the next data set type
            if (studyPhase == SP_EVALUATION){
                nextEncodingDataSetType();
                rawdata.setCurrentDataSet(currentDataSetType);
            }

            stateTimer.setInterval(nbackConfig.getCurrentHoldTime());
        }
        break;
    case TSF_SHOW_BLANKS:

        // We can now finalize the dataset and the trial
        if (studyPhase == SP_EVALUATION) {
            finalizeOnlineFixations();
            rawdata.finalizeDataSet();
            rawdata.finalizeTrial("");
        }

        tstate = TSF_START;

        currentTrial++;
        //qDebug() << "NBack Current Trial" << currentTrial;
        if (m->getLoopValue() != -1){
            if (currentTrial > m->getLoopValue()){
                //qDebug() << "NBack Setting current trial to zero";
                currentTrial = 0;
            }
        }

        if (finalizeExperiment()) return;
        stateTimer.setInterval(timeOutTransition);
    }

    if (studyPhase == SP_EVALUATION) stateTimer.start();
    drawCurrentImage();
}

void NBackRTExperiment::drawCurrentImage(){

    //qDebug() << "Drawing Current Image " << currentTrial << currentImage << mtimer.elapsed();
    mtimer.start();

    m->setTargetPosition(currentTrial,currentImage);

    switch (tstate){
    case TSF_SHOW_BLANKS:
        m->setDrawState(NBackManager::DS_TARGET_BOX_ONLY);
        break;
    case TSF_SHOW_TARGET:
        m->setDrawState(NBackManager::DS_CROSS_TARGET);
        break;
    case TSF_START:
        m->setDrawState(NBackManager::DS_CROSS);
        currentImage=-1;
        break;

    }

    updateDisplay();

    //qDebug() << 6;
}

bool NBackRTExperiment::finalizeExperiment(){
    //qDebug() << currentTrial << m->size();
    if (currentTrial < m->size()) return false;
    stateTimer.stop();
    state = STATE_STOPPED;

    ExperimentResult er;
    if (error.isEmpty()) er = ER_NORMAL;
    else er = ER_WARNING;

    // Finalizing the study. NBacks are not multi part.
    if (!rawdata.finalizeStudy()){
        error = "Failed on NBack RT/VS study finalization: " + rawdata.getError();
        emit Experiment::experimentEndend(ER_FAILURE);
        return false;
    }
    rawdata.markFileAsFinalized();

    if (!saveDataToHardDisk()){
        emit Experiment::experimentEndend(ER_FAILURE);
    }
    else emit Experiment::experimentEndend(er);
    return true;
}

void NBackRTExperiment::keyPressHandler(int keyPressed){
    // Making sure the experiment can be aborted, but any other key is ignored.
    if (keyPressed == Qt::Key_Escape){
        experimenteAborted();
        return;
    }
    else{
        //if ((keyPressed == Qt::Key_N) && (state == STATE_RUNNING) && (tstate == TSF_SHOW_BLANKS)){
        if ((keyPressed == Qt::Key_N) && (state == STATE_RUNNING)){
            onTimeOut();
        }
        else if ((state == STATE_PAUSED) && (keyPressed == Qt::Key_G)){
            //qDebug() << "UNPAUSING THE NBACK STUDY. Current state"  << tstate;
            rawdata.studyPauseSet(false);
            m->drawBackground();
            updateDisplay();
            state = STATE_RUNNING;
            nextState();
        }
    }
}

QString NBackRTExperiment::getExperimentDescriptionFile(const QVariantMap &studyConfig){
    Q_UNUSED(studyConfig)
    return ":/nbackfamiliy/descriptions/fielding.dat";
}

void NBackRTExperiment::resetStudy(){

    successfullTrials = 0;
    timeoutTrials = 0;
    currentTrial = 0;
    state = STATE_RUNNING;
    tstate = TSF_START;
    m->drawBackground();
    drawCurrentImage();
    nbackConfig.resetVSStateMachine();

    if (studyPhase == SP_EVALUATION){
        updateFronEndMessages();
        stateTimer.start();
    }

}

void NBackRTExperiment::newEyeDataAvailable(const EyeTrackerData &data){
    Experiment::newEyeDataAvailable(data);

    if (state != STATE_RUNNING) return;
    if (ignoreData) return;

    // Not saving data during transition screens.
    if (tstate == TSF_START) return;
    if (data.isLeftZero(false) && data.isRightZero(false)) return;

    // Format: Image ID, time stamp for right and left, word index, character index, sentence length and pupil diameter for left and right eye.
    if (studyPhase == SP_EVALUATION){
        rawdata.addNewRawDataVector(ViewMindDataContainer::GenerateStdRawDataVector(static_cast<qreal>(data.timestamp()),
                                                                                    data.xr(),
                                                                                    data.yr(),
                                                                                    data.xl(),
                                                                                    data.yl(),
                                                                                    data.pr(),
                                                                                    data.pl()));
    }

    computeOnlineFixations(data);

    if (tstate == TSF_SHOW_BLANKS){

        bool litUp = false;
        bool isOver = trialRecognitionMachine.isSequenceOver(studyLogicFixation,m,&litUp);

        // NOTE. If light up all is true lit up will NEVER be true. As the flag tellign to light up the correct sequence fixation box will be false.
        if (lightUpAll){

            // If lightUpAll is enabled, we use the data gathering eye to figure out if a fixation hast started (or finished) in a target box.
            // And if it has we light it up anyways.

            Fixation fixationToUse;
            if (trialRecognitionMachine.useRightEye) fixationToUse = lastFixationR;
            else fixationToUse = lastFixationL;

            if (fixationToUse.hasStarted() || fixationToUse.hasFinished()){
                qint32 targetBoxToLightUp = m->pointIsInWhichTargetBox(fixationToUse.getX(),fixationToUse.getY());
                if (targetBoxToLightUp != -1){
                    litUp = m->lightUpBox(targetBoxToLightUp);
                }
            }

        }

        if (litUp){
            updateDisplay();
        }

        if (isOver){
            m->setDebugSequenceValue(-1); // If DEBUG mode is enabled this WILL hide the sequence.
            nbackConfig.wasSequenceCompleted = true;
            onTimeOut();
        }
    }


}

QVariantMap NBackRTExperiment::addHitboxesToProcessingParameters(QVariantMap pp){
    QList<QRectF> hitBoxes = m->getHitTargetBoxes();

    QVariantList modHitBoxes;
    for (qint32 i = 0; i < hitBoxes.size(); i++){

        qreal x,y,w,h;
        QVariantList hitbox;

        x = hitBoxes.at(i).x();
        w = hitBoxes.at(i).width();
        y = hitBoxes.at(i).y();
        h = hitBoxes.at(i).height();

        hitbox << x << y << w << h;
        // Casting is necessary otherwise the operation appends a list to the list.
        // Then the JSON is interpreted as one long list instead of serveral lists of 4 values.
        modHitBoxes << static_cast<QVariant>(hitbox);
    }

    //qDebug() << "NBACK HITBOXES" << modHitBoxes;

    // Store them as part of the processing parameters.
    pp.insert(VMDC::ProcessingParameter::NBACK_HITBOXES,modHitBoxes);
    return pp;
}

void NBackRTExperiment::nextEncodingDataSetType(){
    // A very very complicated way of doing a counter due to the way I defined the data set types as enum.
    // But at least is clean.
    if (currentDataSetType ==  VMDC::DataSetType::ENCODING_1){
        currentDataSetType = VMDC::DataSetType::ENCODING_2;
    }
    else if (currentDataSetType ==  VMDC::DataSetType::ENCODING_2){
        currentDataSetType = VMDC::DataSetType::ENCODING_3;
    }
    else if (currentDataSetType ==  VMDC::DataSetType::ENCODING_3){
        currentDataSetType = VMDC::DataSetType::ENCODING_4;
    }
    else if (currentDataSetType ==  VMDC::DataSetType::ENCODING_4){
        currentDataSetType = VMDC::DataSetType::ENCODING_5;
    }
    else if (currentDataSetType ==  VMDC::DataSetType::ENCODING_5){
        currentDataSetType = VMDC::DataSetType::ENCODING_6;
    }
}

bool NBackRTExperiment::addNewTrial(){
    QString type = m->getFullSequenceAsString(currentTrial);
    currentTrialID = QString::number(currentTrial);

    if (studyPhase == SP_EVALUATION){

        QVariantMap metadata;
        metadata[VMDC::TrialMetadataField::NBACK_HOLD_TIME] = nbackConfig.getCurrentHoldTime();

        if (!rawdata.addNewTrial(currentTrialID,type,"",metadata)){
            error = "Creating a new trial for " + currentTrialID + " gave the following error: " + rawdata.getError();
            return false;
        }
    }
    return true;
}

void NBackRTExperiment::updateFronEndMessages(){
    studyMessages[MSG_SEQ_HITS]   = successfullTrials;
    studyMessages[MSG_SEQ_TOUT]   = timeoutTrials;
    studyMessages[MSG_TARGET_VEL] = nbackConfig.getCurrentHoldTime();
    emit Experiment::updateStudyMessages(studyMessages);
}

/////////////////////////////////////////// TRIAL RECOGNITION MACHINE
void NBackRTExperiment::TrialRecognitionMachine::reset(const QList<qint32> &trialRecogSeq){    
    trialRecognitionSequence = trialRecogSeq;
}


bool NBackRTExperiment::TrialRecognitionMachine::isSequenceOver(const Fixation &fixationToUse, NBackManager *m, bool *updateHUD){

    if (trialRecognitionSequence.isEmpty()) return true;

    // Indicataor to see if we need to update the image to the HMD or second screen.
    *updateHUD = false;

    if (fixationToUse.hasFinished() || fixationToUse.hasStarted()){
        if (m->isPointInTargetBox(fixationToUse.getX(),fixationToUse.getY(),trialRecognitionSequence.first())){
            if (lightUpSquares) {
                *updateHUD = m->lightUpBox(trialRecognitionSequence.first());
            }
            if (DBUGBOOL(Debug::Options::DBUG_MSG)){
                qDebug() << "DBUG: NBackRT/VS Right Fixation FINISHED In " << trialRecognitionSequence.first();
            }

            trialRecognitionSequence.removeFirst();

            if (DBUGBOOL(Debug::Options::RENDER_HITBOXES)){
                if (!trialRecognitionSequence.empty()){
                    m->setDebugSequenceValue(trialRecognitionSequence.first());
                    *updateHUD = true;
                }
            }            
        }
    }


    if (trialRecognitionSequence.isEmpty()) return true;

    return false;
}

/////////////////////////////////////////// VARIABLE SPEED STRUCT FUNCTIONS

void NBackRTExperiment::VariableSpeedAndTargetNumberConfig::adjustSpeed(){

    if (stepHoldTime != 0){
        if (wasSequenceCompleted){
            successfulConsecutiveSequences++;
            failedConsecutiveSequences = 0;
            if (successfulConsecutiveSequences == numberOfTrialsForChange){
                successfulConsecutiveSequences = 0;
                if (currentHoldTime > minHoldTime){
                    currentHoldTime = qMax(currentHoldTime-stepHoldTime,minHoldTime);
                    if (DBUGBOOL(Debug::Options::DBUG_MSG)){
                        qDebug() << "CURRENT HOLD TIME IS DECREASED" << currentHoldTime;
                    }
                }
            }
        }
        else{
            successfulConsecutiveSequences = 0;
            failedConsecutiveSequences++;
            if (failedConsecutiveSequences == numberOfTrialsForChange){
                failedConsecutiveSequences = 0;
                if (currentHoldTime < maxHoldTime){
                    currentHoldTime = qMin(currentHoldTime+stepHoldTime,maxHoldTime);
                    if (DBUGBOOL(Debug::Options::DBUG_MSG)){
                        qDebug() << "CURRENT HOLD TIME IS INCREASED" << currentHoldTime;
                    }
                }
            }
        }

    }

    wasSequenceCompleted = false;
}


qint32 NBackRTExperiment::VariableSpeedAndTargetNumberConfig::getCurrentHoldTime() const{
    return currentHoldTime;
}

void NBackRTExperiment::VariableSpeedAndTargetNumberConfig::debugPrintOptions() const {
    qDebug() << "   Number of Targets" << this->numberOfTargets;
    qDebug() << "   Hold Time Range"  << this->minHoldTime << " to " << this->maxHoldTime << " starting at " << this->startHoldTime;
}


void NBackRTExperiment::VariableSpeedAndTargetNumberConfig::resetVSStateMachine(){
    failedConsecutiveSequences = 0;
    successfulConsecutiveSequences = 0;
    currentHoldTime = startHoldTime;
    wasSequenceCompleted = false;
}
