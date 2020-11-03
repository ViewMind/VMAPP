#include "nbackrtexperiment.h"

NBackRTExperiment::NBackRTExperiment(QWidget *parent, NBackType nbt):Experiment(parent){

    manager = new FieldingManager();
    m = dynamic_cast<FieldingManager*>(manager);
    nbackType = nbt;

    switch(nbt){
    case NBT_DEFAULT:
        expHeader = HEADER_NBACKRT_EXPERIMENT;
        outputDataFile = FILE_OUTPUT_NBACKRT;
        break;
    case NBT_VARIABLE_SPEED:
        expHeader = HEADER_NBACK_VARIABLE_SPEED_EXPERIMENT;
        outputDataFile = FILE_OUTPUT_NBACK_VARIABLE_SPEED;
        break;
    }

    // Connecting the timer time out with the time out function.
    connect(&stateTimer,&QTimer::timeout,this,&NBackRTExperiment::onTimeOut);
}

bool NBackRTExperiment::startExperiment(ConfigurationManager *c){

    switch(nbackType){
    case NBT_VARIABLE_SPEED:
        nbackConfig.minHoldTime              = c->getInt(CONFIG_NBACKVS_MIN_HOLD_TIME);
        nbackConfig.maxHoldTime              = c->getInt(CONFIG_NBACKVS_MAX_HOLD_TIME);
        nbackConfig.stepHoldTime             = c->getInt(CONFIG_NBACKVS_STEP_HOLD_TIME);
        nbackConfig.startHoldTime            = c->getInt(CONFIG_NBACKVS_START_HOLD_TIME);
        nbackConfig.numberOfTrialsForChange  = c->getInt(CONFIG_NBACKVS_NTRIAL_FOR_STEP_CHANGE);
        nbackConfig.numberOfTargets          = c->getInt(CONFIG_NBACKVS_SEQUENCE_LENGTH);
        outputDataFile = outputDataFile + "_" + QString::number(nbackConfig.numberOfTargets);
        break;
    case NBT_DEFAULT:
        // Variable Speed configuration for DEFAULT NBACK RT.
        nbackConfig.minHoldTime              = TIME_TARGET;
        nbackConfig.maxHoldTime              = TIME_TARGET;
        nbackConfig.stepHoldTime             = 0;
        nbackConfig.startHoldTime            = TIME_TARGET;
        nbackConfig.numberOfTrialsForChange  = 0;
        nbackConfig.numberOfTargets          = DEFAULT_NUMBER_OF_TARGETS;
        break;
    }

    if (!Experiment::startExperiment(c)) return false;

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
    mwp.sampleFrequency = c->getReal(CONFIG_SAMPLE_FREQUENCY);
    mwp.minimumFixationLength = c->getReal(CONFIG_MIN_FIXATION_LENGTH);
    mwp.maxDispersion = c->getReal(CONFIG_MOVING_WINDOW_DISP);
    mwp.calculateWindowSize();

    qDebug() << "MWP" << mwp.sampleFrequency << mwp.minimumFixationLength << mwp.maxDispersion << mwp.getStartWindowSize();

    trialRecognitionMachine.configure(mwp);
    //qDebug() << "Miniminum number of data points computed" << mwp.getStartWindowSize();

    m->drawBackground();
    drawCurrentImage();

    if (!vrEnabled){
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
        addTrialHeader();
        stateTimer.setInterval(nbackConfig.getCurrentHoldTime());
        break;
    case TSF_SHOW_TARGET:
        currentImage++;
        if (currentImage == nbackConfig.numberOfTargets){
            tstate = TSF_SHOW_BLANKS;
            trialRecognitionMachine.reset(m->getExpectedTargetSequenceForTrial(currentTrial, nbackConfig.numberOfTargets));
            qDebug() << "EXPECTED SEQUENCE FOR TRIAL" << m->getExpectedTargetSequenceForTrial(currentTrial, nbackConfig.numberOfTargets);
            addTrialHeader();
            stateTimer.setInterval(TIME_OUT_BLANKS);
        }
        else {
            addTrialHeader();
            stateTimer.setInterval(nbackConfig.getCurrentHoldTime());
        }
        break;
    case TSF_SHOW_BLANKS:
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

    qDebug() << 5;

    m->setTargetPosition(currentTrial,currentImage);

    qDebug() << 7;

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
                qDebug() << "Pausing wihth current trial" << currentTrial;
                return;
            }

        }
        m->setDrawState(FieldingManager::DS_CROSS);
        currentImage=-1;
        break;
    }

    updateSecondMonitorORHMD();

    qDebug() << 6;
}

bool NBackRTExperiment::finalizeExperiment(){
    //qDebug() << currentTrial << m->size();
    if (currentTrial < m->size()) return false;
    stateTimer.stop();
    state = STATE_STOPPED;
    ExperimentResult er;
    if (error.isEmpty()) er = ER_NORMAL;
    else er = ER_WARNING;
    if (!saveDataToHardDisk()){
        emit(experimentEndend(ER_FAILURE));
    }
    else emit(experimentEndend(er));
    return true;
}

void NBackRTExperiment::togglePauseExperiment(){
    // Pause is not managed here, for this experiment.
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

void NBackRTExperiment::addTrialHeader(){

    ignoreData = true;
    qint32 imageInTrialID = currentImage+1;

//    if (tstate == TSF_SHOW_BLANKS) imageInTrialID = 4;
//    else imageInTrialID = currentImage+1;

    // Saving the current image.
    QVariantList dataS;
    dataS  << m->getTrial(currentTrial).id
           << imageInTrialID;
           //<< currentImage;

    if (nbackType == NBT_VARIABLE_SPEED){
        dataS << nbackConfig.getCurrentHoldTime();
    }

    etData << QVariant(dataS);

    ignoreData = false;

}

void NBackRTExperiment::newEyeDataAvailable(const EyeTrackerData &data){

    Experiment::newEyeDataAvailable(data);

    if (state != STATE_RUNNING) return;
    if (ignoreData) return;

    // Not saving data during transition screens.
    if (tstate == TSF_START) return;
    if (data.isLeftZero() && data.isRightZero()) return;

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
