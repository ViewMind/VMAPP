#include "fieldingexperiment.h"

FieldingExperiment::FieldingExperiment(QWidget *parent):Experiment(parent){

    manager = new FieldingManager();
    m = dynamic_cast<FieldingManager*>(manager);
    expHeader = HEADER_FIELDING_EXPERIMENT;
    outputDataFile = FILE_OUTPUT_FIELDING;


    // Connecting the timer time out with the time out function.
    connect(&stateTimer,&QTimer::timeout,this,&FieldingExperiment::onTimeOut);
}

bool FieldingExperiment::startExperiment(ConfigurationManager *c){
    if (!Experiment::startExperiment(c)) return false;

    currentImage = 0;
    currentTrial = 0;
    error = "";
    ignoreData = false;

    state = STATE_RUNNING;
    tstate = TSF_START;
    stateTimer.setInterval(TIME_TRANSITION);
    stateTimer.start();

    m->drawBackground();
    drawCurrentImage();

    if (!vrEnabled){
        this->show();
        this->activateWindow();
    }

    return true;

}

void FieldingExperiment::nextState(){

    switch (tstate){
    case TSF_SHOW_BLANK_1:
        currentTrial++;
        if (finalizeExperiment()) return;
        tstate = TSF_START;
        stateTimer.setInterval(TIME_TRANSITION);
        break;
    case TSF_SHOW_BLANK_2:
        tstate = TSF_SHOW_BLANK_1;
        currentImage = 0;
        addTrialHeader();
        stateTimer.setInterval(TIME_CONDITION);
        break;
    case TSF_SHOW_BLANK_3:
        tstate = TSF_SHOW_BLANK_2;
        currentImage = 1;
        addTrialHeader();
        stateTimer.setInterval(TIME_CONDITION);
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
        tstate = TSF_TRANSITION;
        stateTimer.setInterval(TIME_TRANSITION);
        break;
    case TSF_START:
        tstate = TSF_SHOW_DOT_1;
        currentImage = 0;
        addTrialHeader();
        stateTimer.setInterval(TIME_TARGET);
        break;
    case TSF_TRANSITION:
        tstate = TSF_SHOW_BLANK_3;
        currentImage = 2;
        addTrialHeader();
        stateTimer.setInterval(TIME_CONDITION);
        break;
    }

    stateTimer.start();
    drawCurrentImage();
}

void FieldingExperiment::onTimeOut(){
    stateTimer.stop();
    nextState();
}

void FieldingExperiment::drawCurrentImage(){

    m->setTargetPosition(currentTrial,currentImage);

    switch (tstate){
    case TSF_SHOW_BLANK_1:
        m->setDrawState(FieldingManager::DS_1);
        break;
    case TSF_SHOW_BLANK_2:
        m->setDrawState(FieldingManager::DS_2);
        break;
    case TSF_SHOW_BLANK_3:
        m->setDrawState(FieldingManager::DS_3);
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
    case TSF_TRANSITION:
        m->setDrawState(FieldingManager::DS_CROSS);
        currentImage=3;
        break;
    }

    updateSecondMonitorORHMD();
}

bool FieldingExperiment::finalizeExperiment(){
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

void FieldingExperiment::togglePauseExperiment(){
    // Pause is not managed here, for this experiment.
}

void FieldingExperiment::keyPressHandler(int keyPressed){
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

void FieldingExperiment::newEyeDataAvailable(const EyeTrackerData &data){
    Experiment::newEyeDataAvailable(data);

    if (state != STATE_RUNNING) return;
    if (ignoreData) return;

    // Not saving data during transition screens.
    if ((tstate == TSF_START) || (tstate == TSF_TRANSITION)) return;

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

}

void FieldingExperiment::addTrialHeader(){

    ignoreData = true;

    QPoint currentTarget = m->getTargetPoint(currentTrial,currentImage);

    qint32 imageInTrialID = currentImageToImageIndex();

    // Saving the current image.
    QVariantList dataS;
    dataS  << m->getTrial(currentTrial).id
           << imageInTrialID
           << currentTarget.x() + TARGET_R + TARGET_OFFSET_X
           << currentTarget.y() + TARGET_R + TARGET_OFFSET_Y;
    etData << QVariant(dataS);

    ignoreData = false;
}

qint32 FieldingExperiment::currentImageToImageIndex(){
    qint32 imageInTrialID;

    switch (tstate){
    case TSF_SHOW_BLANK_1:
        imageInTrialID = 7; // This is the sixth image in the sequence
        break;
    case TSF_SHOW_BLANK_2:
        imageInTrialID = 6; // This is the fifth image in the sequence
        break;
    case TSF_SHOW_BLANK_3:
        imageInTrialID = 5; // This is the fourth image in the sequence
        break;
    default:
        imageInTrialID = currentImage+1;
        break;
    }

    return imageInTrialID;
}
