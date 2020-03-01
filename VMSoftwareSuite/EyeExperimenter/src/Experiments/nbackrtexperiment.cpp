#include "nbackrtexperiment.h"

NBackRTExperiment::NBackRTExperiment(QWidget *parent):Experiment(parent){

    manager = new FieldingManager();
    m = dynamic_cast<FieldingManager*>(manager);
    expHeader = HEADER_NBACKRT_EXPERIMENT;
    outputDataFile = FILE_OUTPUT_NBACKRT;

    // Connecting the timer time out with the time out function.
    connect(&stateTimer,&QTimer::timeout,this,&NBackRTExperiment::onTimeOut);
}

bool NBackRTExperiment::startExperiment(ConfigurationManager *c){
    if (!Experiment::startExperiment(c)) return false;

    currentImage = 0;
    currentTrial = 0;
    error = "";
    ignoreData = false;

    state = STATE_RUNNING;
    tstate = TSF_START;
    stateTimer.setInterval(TIME_TRANSITION);
    stateTimer.start();

    // Computing the minimum number of hits to count as a fixation.
    MovingWindowParameters mwp;
    mwp.sampleFrequency = c->getReal(CONFIG_SAMPLE_FREQUENCY);
    mwp.minimumFixationLength = c->getReal(CONFIG_MIN_FIXATION_LENGTH);
    mwp.calculateWindowSize();
    trialRecognitionMachine.setNumberOfDataHit(mwp.getStartWindowSize());
    qDebug() << "Miniminum number of data points computed" << mwp.getStartWindowSize();

    drawCurrentImage();

    if (!vrEnabled){
        this->show();
        this->activateWindow();
    }

    return true;

}

void NBackRTExperiment::onTimeOut(){
    stateTimer.stop();
    nextState();
}

void NBackRTExperiment::nextState(){

    switch (tstate){
    case TSF_START:
        tstate = TSF_SHOW_DOT_1;
        currentImage = 0;
        addTrialHeader();
        stateTimer.setInterval(TIME_TARGET);
        break;
    case TSF_SHOW_DOT_1:
        tstate = TSF_SHOW_DOT_2;
        currentImage = 1;
        addTrialHeader();
        stateTimer.setInterval(TIME_TARGET);
        break;
    case TSF_SHOW_DOT_2:
        tstate = TSF_SHOW_DOT_3;
        currentImage = 2;
        addTrialHeader();
        stateTimer.setInterval(TIME_TARGET);
        break;
    case TSF_SHOW_DOT_3:
        tstate = TSF_SHOW_BLANKS;
        trialRecognitionMachine.reset(m->getExpectedTargetSequenceForTrial(currentTrial));
        addTrialHeader();
        stateTimer.setInterval(TIME_OUT_BLANKS);
        break;
    case TSF_SHOW_BLANKS:
        tstate = TSF_START;
        currentTrial++;
        if (finalizeExperiment()) return;
        stateTimer.setInterval(TIME_TRANSITION);
    }

    stateTimer.start();
    drawCurrentImage();
}

void NBackRTExperiment::drawCurrentImage(){

    m->setTargetPosition(currentTrial,currentImage);

    switch (tstate){
    case TSF_SHOW_BLANKS:
        m->setDrawState(FieldingManager::DS_TARGET_BOX_ONLY);
        break;
    case TSF_SHOW_DOT_1:
        m->setDrawState(FieldingManager::DS_CROSS_TARGET);
        break;
    case TSF_SHOW_DOT_2:
        m->setDrawState(FieldingManager::DS_CROSS_TARGET);
        break;
    case TSF_SHOW_DOT_3:
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
        if (state == STATE_PAUSED){
            state = STATE_RUNNING;
            m->drawBackground();
            updateSecondMonitorORHMD();
            nextState();
        }
    }
}

void NBackRTExperiment::addTrialHeader(){

    ignoreData = true;
    qint32 imageInTrialID;
    if (tstate == TSF_SHOW_BLANKS) imageInTrialID = 4;
    else imageInTrialID = currentImage+1;

    // Saving the current image.
    QVariantList dataS;
    dataS  << m->getTrial(currentTrial).id
           << imageInTrialID;
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

    if (tstate == TSF_SHOW_BLANKS){
        if (trialRecognitionMachine.isSequenceOver(data.xRight,data.yRight,data.xLeft,data.yLeft,m)){
            onTimeOut();
        }
    }

}


/////////////////////////////////////////// TRIAL RECOGNITION MACHINE
void NBackRTExperiment::TrialRecognitionMachine::reset(const QList<qint32> &trialRecogSeq){
    rTrialRecognitionSequence = trialRecogSeq;
    lTrialRecognitionSequence = trialRecogSeq;
    rHitCount = 0;
    lHitCount = 0;
}

void NBackRTExperiment::TrialRecognitionMachine::setNumberOfDataHit(qint32 n){
    numberOfDataHits = n;
}

bool NBackRTExperiment::TrialRecognitionMachine::isSequenceOver(qreal rX, qreal rY, qreal lX, qreal lY, FieldingManager *m){

    if (rTrialRecognitionSequence.isEmpty()) return true;
    if (lTrialRecognitionSequence.isEmpty()) return true;

    if (m->isPointInTargetBox(rX,rY,rTrialRecognitionSequence.first())){
        rHitCount++;
        if (rHitCount == numberOfDataHits){
            rTrialRecognitionSequence.removeFirst();
            rHitCount = 0;
            if (rTrialRecognitionSequence.isEmpty()) return true;
        }
    }

    if (m->isPointInTargetBox(lX,lY,lTrialRecognitionSequence.first())){
        lHitCount++;
        if (lHitCount == numberOfDataHits){
            lTrialRecognitionSequence.removeFirst();
            lHitCount = 0;
            if (lTrialRecognitionSequence.isEmpty()) return true;
        }
    }


    return false;
}
