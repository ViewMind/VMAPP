#include "nbackrtexperiment.h"

const qint32 NBackRTExperiment::TIME_TRANSITION =                              500;
const qint32 NBackRTExperiment::TIME_TARGET =                                  250;
const qint32 NBackRTExperiment::TIME_OUT_BLANKS =                              3000;
const qint32 NBackRTExperiment::DEFAULT_NUMBER_OF_TARGETS =                    3;

// Possible pauses for the fielding experiment
const qint32 NBackRTExperiment::PAUSE_TRIAL_1 =                                32;
const qint32 NBackRTExperiment::PAUSE_TRIAL_2 =                                64;

const qint32 NBackRTExperiment::NBACKVS_MIN_HOLD_TIME =                        50;
const qint32 NBackRTExperiment::NBACKVS_MAX_HOLD_TIME =                        250;
const qint32 NBackRTExperiment::NBACKVS_STEP_HOLD_TIME =                       50;
const qint32 NBackRTExperiment::NBACKVS_START_HOLD_TIME =                      250;
const qint32 NBackRTExperiment::NBACKVS_NTRIAL_FOR_STEP_CHANGE =               2;


NBackRTExperiment::NBackRTExperiment(QWidget *parent, const QString &studyType):Experiment(parent,studyType){

    manager = new FieldingManager();
    m = dynamic_cast<FieldingManager*>(manager);

    // Connecting the timer time out with the time out function.
    connect(&stateTimer,&QTimer::timeout,this,&NBackRTExperiment::onTimeOut);


}

bool NBackRTExperiment::startExperiment(const QString &workingDir, const QString &experimentFile,
                                        const QVariantMap &studyConfig, bool useMouse,
                                        QVariantMap pp){

    if (studyType == RDC::Study::NBACKVS){
        nbackConfig.minHoldTime              = NBACKVS_MIN_HOLD_TIME;
        nbackConfig.maxHoldTime              = NBACKVS_MAX_HOLD_TIME;
        nbackConfig.stepHoldTime             = NBACKVS_STEP_HOLD_TIME;
        nbackConfig.startHoldTime            = NBACKVS_START_HOLD_TIME;
        nbackConfig.numberOfTrialsForChange  = NBACKVS_NTRIAL_FOR_STEP_CHANGE;
        nbackConfig.numberOfTargets          = studyConfig.value(RDC::StudyParameter::NUMBER_TARGETS).toInt();
    }
    else{
        // Variable Speed configuration for DEFAULT NBACK RT.
        nbackConfig.minHoldTime              = TIME_TARGET;
        nbackConfig.maxHoldTime              = TIME_TARGET;
        nbackConfig.stepHoldTime             = 0;
        nbackConfig.startHoldTime            = TIME_TARGET;
        nbackConfig.numberOfTrialsForChange  = 0;
        nbackConfig.numberOfTargets          = DEFAULT_NUMBER_OF_TARGETS;

    }

    if (!Experiment::startExperiment(workingDir,experimentFile,studyConfig,useMouse,pp)){
        emit(experimentEndend(ER_FAILURE));
        return false;
    }

    QVariantMap config;
    config.insert(FieldingManager::CONFIG_IS_VR_BEING_USED,(Globals::EyeTracker::IS_VR && (!useMouse)));
    if (studyConfig.value(RDC::StudyParameter::LANGUAGE).toString() == RDC::UILanguage::SPANISH){
        config.insert(FieldingManager::CONFIG_PAUSE_TEXT_LANG,FieldingManager::LANG_ES);
    }
    else{
        config.insert(FieldingManager::CONFIG_PAUSE_TEXT_LANG,FieldingManager::LANG_ES);
    }
    m->configure(config);

    // The processing parameters requires the hitboxes that are going to be used, so they are added before they are set.
    pp = addHitboxesToProcessingParameters(pp);
    if (!rawdata.setProcessingParameters(pp)){
        error = "Failed setting processing parameters on Reading: " + rawdata.getError();
        emit(experimentEndend(ER_FAILURE));
    }

    currentImage = 0;
    currentTrial = 0;
    error = "";
    ignoreData = false;

    nbackConfig.resetVSStateMachine();

    state = STATE_RUNNING;
    tstate = TSF_START;
    stateTimer.setInterval(TIME_TRANSITION);
    stateTimer.start();

    // Computing the minimum number of hits to count as a fixation.
    MovingWindowParameters mwp;
    mwp.sampleFrequency = pp.value(RDC::ProcessingParameter::SAMPLE_FREQUENCY).toReal();
    mwp.minimumFixationLength =  pp.value(RDC::ProcessingParameter::MIN_FIXATION_DURATION).toReal();
    mwp.maxDispersion =  pp.value(RDC::ProcessingParameter::MAX_DISPERSION_WINDOW).toReal();
    mwp.calculateWindowSize();

    //qDebug() << "MWP" << mwp.sampleFrequency << mwp.minimumFixationLength << mwp.maxDispersion << mwp.getStartWindowSize();

    trialRecognitionMachine.configure(mwp);
    //qDebug() << "Miniminum number of data points computed" << mwp.getStartWindowSize();

    m->drawBackground();
    drawCurrentImage();

    if (!Globals::EyeTracker::IS_VR || (useMouse)){
        this->show();
        this->activateWindow();
    }

    return true;

}

void NBackRTExperiment::onTimeOut(){

    if (tstate == TSF_SHOW_BLANKS){
        nbackConfig.adjustSpeed();
    }

    stateTimer.stop();
    nextState();
}

void NBackRTExperiment::nextState(){

    switch (tstate){
    case TSF_START:
        tstate = TSF_SHOW_TARGET;
        currentImage = 0;
        stateTimer.setInterval(nbackConfig.getCurrentHoldTime());

        if (!addNewTrial()){
            stateTimer.stop();
            emit(experimentEndend(ER_FAILURE));
            return;
        }
        currentDataSetType = RDC::DataSetType::ENCODING_1;
        rawdata.setCurrentDataSet(currentDataSetType);

        break;
    case TSF_SHOW_TARGET:

        // End the previous data set.
        rawdata.finalizeDataSet();
        currentImage++;
        if (currentImage == nbackConfig.numberOfTargets){

            // Retrieval will begin
            rawdata.setCurrentDataSet(RDC::DataSetType::RETRIEVAL_1);

            tstate = TSF_SHOW_BLANKS;
            trialRecognitionMachine.reset(m->getExpectedTargetSequenceForTrial(currentTrial, nbackConfig.numberOfTargets));
            //qDebug() << "EXPECTED SEQUENCE FOR TRIAL" << m->getExpectedTargetSequenceForTrial(currentTrial, nbackConfig.numberOfTargets);
            stateTimer.setInterval(TIME_OUT_BLANKS);
        }
        else {

            // WE compute the next data set type
            nextEncodingDataSetType();
            rawdata.setCurrentDataSet(currentDataSetType);

            stateTimer.setInterval(nbackConfig.getCurrentHoldTime());
        }
        break;
    case TSF_SHOW_BLANKS:

        // We can now finalize the dataset and the trial
        rawdata.finalizeDataSet();
        rawdata.finalizeTrial("");

        tstate = TSF_START;
        currentTrial++;
        if (finalizeExperiment()) return;
        stateTimer.setInterval(TIME_TRANSITION);
    }

#ifdef MANUAL_TRIAL_PASS
    if (tstate != TSF_SHOW_BLANKS) stateTimer.start();
#else
    stateTimer.start();
#endif
    drawCurrentImage();
}

void NBackRTExperiment::drawCurrentImage(){

    //qDebug() << 5;

    m->setTargetPosition(currentTrial,currentImage);

    //qDebug() << 7;

    switch (tstate){
    case TSF_SHOW_BLANKS:
        m->setDrawState(FieldingManager::DS_TARGET_BOX_ONLY);
        break;
    case TSF_SHOW_TARGET:
        m->setDrawState(FieldingManager::DS_CROSS_TARGET);
        break;
    case TSF_START:
        // Before drawing the start of a new trial, we check if a pause is necessary.
        if ( (currentTrial == PAUSE_TRIAL_1) || (currentTrial == PAUSE_TRIAL_2)){

            // Paused must be set to zero.
            if (state == STATE_RUNNING){
                state = STATE_PAUSED;
                stateTimer.stop();
                m->drawPauseScreen();
                updateSecondMonitorORHMD();
                //qDebug() << "Pausing wihth current trial" << currentTrial;
                return;
            }

        }
        m->setDrawState(FieldingManager::DS_CROSS);
        currentImage=-1;
        break;
    }

    updateSecondMonitorORHMD();

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
        emit (experimentEndend(ER_FAILURE));
        return false;
    }
    rawdata.markFileAsFinalized();

    if (!saveDataToHardDisk()){
        emit(experimentEndend(ER_FAILURE));
    }
    else emit(experimentEndend(er));
    return true;
}

void NBackRTExperiment::keyPressHandler(int keyPressed){
    // Making sure the experiment can be aborted, but any other key is ignored.
    if (keyPressed == Qt::Key_Escape){
        experimenteAborted();
        return;
    }
    else{
        if ((keyPressed == Qt::Key_N) && (state == STATE_RUNNING) && (tstate == TSF_SHOW_BLANKS)){
            onTimeOut();
        }
        else if ((state == STATE_PAUSED) && (keyPressed == Qt::Key_G)){
            state = STATE_RUNNING;
            m->drawBackground();
            updateSecondMonitorORHMD();
            nextState();
        }
    }
}


void NBackRTExperiment::newEyeDataAvailable(const EyeTrackerData &data){

    Experiment::newEyeDataAvailable(data);

    if (state != STATE_RUNNING) return;
    if (ignoreData) return;

    // Not saving data during transition screens.
    if (tstate == TSF_START) return;
    if (data.isLeftZero() && data.isRightZero()) return;

    // Format: Image ID, time stamp for right and left, word index, character index, sentence length and pupil diameter for left and right eye.
    rawdata.addNewRawDataVector(RawDataContainer::GenerateStdRawDataVector(data.time,data.xRight,data.yRight,data.xLeft,data.yLeft,data.pdRight,data.pdLeft));

    //qDebug() << "X,Y" << data.xRight << data.xLeft;

    if (tstate == TSF_SHOW_BLANKS){
        bool isOver = trialRecognitionMachine.isSequenceOver(data.xRight,data.yRight,data.xLeft,data.yLeft,m,data.time);
        qint32 rx, ry, lx, ly; rx = -1; ry = -1; lx = -1; ly = -1;
        if (trialRecognitionMachine.leftLastFixation.isValid()){
            lx = static_cast<qint32>(trialRecognitionMachine.leftLastFixation.x);
            ly = static_cast<qint32>(trialRecognitionMachine.leftLastFixation.y);
        }
        if (trialRecognitionMachine.rightLastFixation.isValid()){
            rx = static_cast<qint32>(trialRecognitionMachine.rightLastFixation.x);
            ry = static_cast<qint32>(trialRecognitionMachine.rightLastFixation.y);
        }
        emit(addFixations(rx,ry,lx,ly));
        emit(addDebugMessage(trialRecognitionMachine.getMessages(),false));
        if (isOver){
#ifdef MANUAL_TRIAL_PASS
            emit(addDebugMessage(trialRecognitionMachine.getMessages() + "\nEND",false));
#else
            emit(addDebugMessage(trialRecognitionMachine.getMessages(),false));
            nbackConfig.wasSequenceCompleted = true;
            onTimeOut();
#endif
        }
        else{
            emit(addDebugMessage(trialRecognitionMachine.getMessages(),false));
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
        modHitBoxes << (QVariant) hitbox;
    }

    // Store them as part of the processing parameters.
    pp.insert(RDC::ProcessingParameter::NBACK_HITBOXES,modHitBoxes);
    return pp;
}


void NBackRTExperiment::nextEncodingDataSetType(){
    // A very very complicated way of doing a counter due to the way I defined the data set types as enum.
    // But at least is clean.
    if (currentDataSetType ==  RDC::DataSetType::ENCODING_1){
        currentDataSetType = RDC::DataSetType::ENCODING_2;
    }
    else if (currentDataSetType ==  RDC::DataSetType::ENCODING_2){
        currentDataSetType = RDC::DataSetType::ENCODING_3;
    }
    else if (currentDataSetType ==  RDC::DataSetType::ENCODING_3){
        currentDataSetType = RDC::DataSetType::ENCODING_4;
    }
    else if (currentDataSetType ==  RDC::DataSetType::ENCODING_4){
        currentDataSetType = RDC::DataSetType::ENCODING_5;
    }
    else if (currentDataSetType ==  RDC::DataSetType::ENCODING_5){
        currentDataSetType = RDC::DataSetType::ENCODING_6;
    }
}

bool NBackRTExperiment::addNewTrial(){
    QString type = m->getFullSequenceAsString(currentTrial);
    currentTrialID = QString::number(currentTrial);

    if (!rawdata.addNewTrial(currentTrialID,type)){
        error = "Creating a new trial for " + currentTrialID + " gave the following error: " + rawdata.getError();
        return false;
    }
    return true;
}


/////////////////////////////////////////// TRIAL RECOGNITION MACHINE
void NBackRTExperiment::TrialRecognitionMachine::reset(const QList<qint32> &trialRecogSeq){    
    rTrialRecognitionSequence = trialRecogSeq;
    lTrialRecognitionSequence = trialRecogSeq;
    rMWA.finalizeOnlineFixationCalculation();
    lMWA.finalizeOnlineFixationCalculation();
    messages.clear();
    messages << "MWS: " + QString::number(rMWA.parameters.getStartWindowSize());
    QString seq = "SEQ: ";
    for (qint32 i = 0; i < trialRecogSeq.size(); i++){
        seq = seq + QString::number(trialRecogSeq.at(i)) + " ";
    }
    messages << seq;
}

void NBackRTExperiment::TrialRecognitionMachine::configure(MovingWindowParameters mwp){
    mwp.calculateWindowSize();
    rMWA.parameters = mwp;
    lMWA.parameters = mwp;
}

bool NBackRTExperiment::TrialRecognitionMachine::isSequenceOver(qreal rX, qreal rY, qreal lX, qreal lY, FieldingManager *m, qreal timeStamp){

    Fixation r = rMWA.calculateFixationsOnline(rX,rY,timeStamp);
    Fixation l = lMWA.calculateFixationsOnline(lX,lY,timeStamp);

    rightLastFixation = r;
    leftLastFixation = l;

    if (rTrialRecognitionSequence.isEmpty()) return true;
    if (lTrialRecognitionSequence.isEmpty()) return true;

    if (r.isValid()){
        if (m->isPointInTargetBox(r.x,r.y,rTrialRecognitionSequence.first())){
            //qDebug() << "Fixation" << r.toString() << "inside box" << rTrialRecognitionSequence.first();
            messages << "RFIX IN: " + QString::number(rTrialRecognitionSequence.first());
            rTrialRecognitionSequence.removeFirst();
        }
    }


    if (l.isValid()){
        if (m->isPointInTargetBox(l.x,l.y,lTrialRecognitionSequence.first())){
            messages << "LFIX IN: " + QString::number(lTrialRecognitionSequence.first());
            lTrialRecognitionSequence.removeFirst();
        }
    }

    if (rTrialRecognitionSequence.isEmpty()) return true;
    if (lTrialRecognitionSequence.isEmpty()) return true;

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
                    qDebug() << "CURRENT HOLD TIME IS DECREASED" << currentHoldTime;
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
                    qDebug() << "CURRENT HOLD TIME IS INCREASED" << currentHoldTime;
                }
            }
        }

    }

    wasSequenceCompleted = false;
}



qint32 NBackRTExperiment::VariableSpeedAndTargetNumberConfig::getCurrentHoldTime() const{
    return currentHoldTime;
}


void NBackRTExperiment::VariableSpeedAndTargetNumberConfig::resetVSStateMachine(){
    failedConsecutiveSequences = 0;
    successfulConsecutiveSequences = 0;
    currentHoldTime = startHoldTime;
    wasSequenceCompleted = false;
}
