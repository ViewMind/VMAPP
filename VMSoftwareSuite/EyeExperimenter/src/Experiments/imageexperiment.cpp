#include "imageexperiment.h"

ImageExperiment::ImageExperiment(bool bound, QWidget *parent):Experiment(parent){

    manager = new BindingManager();
    m = (BindingManager*) manager;
    expHeader = HEADER_IMAGE_EXPERIMENT;
    if (bound) outputDataFile = FILE_OUTPUT_BINDING_BC;
    else outputDataFile = FILE_OUTPUT_BINDING_UC;

    // Connecting the timer time out with the time out function.
    //connect(&stateTimer,&QTimer::timeout,this,&ImageExperiment::onTimeOut);
    connect(&stateTimer,&QTimer::timeout,this,&ImageExperiment::nextState);

    // Binding type.
    isBound = bound;

}

bool ImageExperiment::startExperiment(ConfigurationManager *c){

    outputDataFile = outputDataFile + "_" + QString::number(c->getInt(CONFIG_BINDING_NUMBER_OF_TARGETS));
    if (c->getBool(CONFIG_BINDING_TARGET_SMALL)) outputDataFile = outputDataFile + "_s";
    else outputDataFile = outputDataFile + "_l";

    if (!Experiment::startExperiment(c)) return false;

    // If there were no problems the experiment can begin.
    currentTrial = 0;
    error = "";
    state = STATE_RUNNING;
    atLast = false;
    ignoreData = false;
    newImage(m->getTrial(currentTrial).name,0);
    trialState = TSB_CENTER_CROSS;
    drawCurrentImage();
    stateTimer.setInterval(TIME_START_CROSS);
    stateTimer.start();
    if (!vrEnabled){
        this->show();
        this->activateWindow();
    }
    else updateSecondMonitorORHMD();

    return true;

}

void ImageExperiment::drawCurrentImage(){

    if (state != STATE_RUNNING) return;

    if (trialState == TSB_CENTER_CROSS){
        //qWarning() << "DRAWING: Center Cross" << currentTrial;
        m->drawCenter();
        updateSecondMonitorORHMD();
        return;
    }

    if ((trialState == TSB_TRANSITION) || (trialState == TSB_FINISH)){
        //qWarning() << "DRAWING: Transition" << currentTrial;
        m->drawClear();
        updateSecondMonitorORHMD();
        return;
    }

    if (trialState == TSB_SHOW) {
        //qWarning() << "DRAWING: Show" << currentTrial;
        m->drawTrial(currentTrial,true);
    }
    else if (trialState == TSB_TEST) {
        //qWarning() << "DRAWING: Test" << currentTrial;
        m->drawTrial(currentTrial,false);
    }
    updateSecondMonitorORHMD();

}

bool ImageExperiment::advanceTrial(){

    //qWarning() << currentTrial;

    if (currentTrial < m->size()-1){
        currentTrial++;
        // Adding the entry in the file
        newImage(m->getTrial(currentTrial).name,0);
        return true;
    }
    // This is done.
    state = STATE_STOPPED;
    stateTimer.stop();
    return false;
}

void ImageExperiment::togglePauseExperiment(){
    // This experiment cannot be paused so the implementation here is simply empty.
}

void ImageExperiment::newEyeDataAvailable(const EyeTrackerData &data){
    Experiment::newEyeDataAvailable(data);

    if (state != STATE_RUNNING) return;
    if (ignoreData) return;
    if ((trialState != TSB_SHOW) && (trialState != TSB_TEST)) return;

    if (data.isLeftZero() && data.isRightZero()) return;

    // Adding the data row.
    QVariantList dataS;
    dataS  << data.time
           << data.xRight
           << data.yRight
           << data.xLeft
           << data.yLeft
           << data.pdRight
           << data.pdLeft;
    etData << QVariant(dataS);

}

void ImageExperiment::keyPressHandler(int keyPressed){

    // Making sure the experiment can be aborted
    if (keyPressed == Qt::Key_Escape){
        experimenteAborted();
        return;
    }
    else if(keyPressed == Qt::Key_Right) {
        nextState();
        return;
    }

    // Key presses are only valid in TSB_TEST state
    if (trialState != TSB_TEST) return;

    // If it's an odd image, THEN we see if the letter is S or D
    switch (keyPressed){
    case Qt::Key_S:
        answer = "S";
        nextState();
        break;
    case Qt::Key_D:
        answer = "D";
        nextState();
        break;
    default:
        // If it is not S or D then it is ignored.
        break;
    }


}

void ImageExperiment::nextState(){

    //qWarning() << "TIME INTERVAL: " << mtime.elapsed();
    //mtime.start();
    stateTimer.stop();

    switch (trialState){
    case TSB_CENTER_CROSS:
        trialState = TSB_SHOW;
        stateTimer.setInterval(TIME_IMAGE_1);
        stateTimer.start();
        drawCurrentImage();
        return;
    case TSB_FINISH:
        //qWarning() << "ENTER: FINISH" << currentTrial;
        if (advanceTrial()){
            trialState = TSB_CENTER_CROSS;
            drawCurrentImage();
            stateTimer.setInterval(TIME_START_CROSS);
            stateTimer.start();
        }
        else{
            if (!saveDataToHardDisk()) emit(experimentEndend(ER_FAILURE));
            else emit(experimentEndend(ER_NORMAL));
        }
        return;
    case TSB_SHOW:
        //qWarning() << "ENTER: SHOW" << currentTrial;
        trialState = TSB_TRANSITION;
        drawCurrentImage();
        stateTimer.setInterval(TIME_WHITE_TRANSITION);
        stateTimer.start();
        return;
    case TSB_TEST:
        //qWarning() << "ENTER: TEST" << currentTrial;
        trialState = TSB_FINISH;
        addAnswer(answer);
        drawCurrentImage();
        stateTimer.setInterval(TIME_FINISH);
        stateTimer.start();
        return;
    case TSB_TRANSITION:
        //qWarning() << "ENTER: TRANSITION" << currentTrial;
        trialState = TSB_TEST;
        answer = "N/A";
        newImage(m->getTrial(currentTrial).name,1);
        drawCurrentImage();
        stateTimer.setInterval(TIME_IMAGE_2_TIMEOUT);
        stateTimer.start();
        return;
    }


}

// -------------------- DATA WRITING FUNCTIONS -----------------------

void ImageExperiment::newImage(QString name, qint32 isTrial){

    ignoreData = true;
    QVariantList dataS;
    dataS  << name
           << isTrial; // 1 if it is test and 0 if it is show.
    etData << QVariant(dataS);
    ignoreData = false;

}

void ImageExperiment::addAnswer(QString ans){

    ignoreData = true;
    QVariantList dataS;
    dataS << m->getTrial(currentTrial).name + "->" + ans;
    etData << QVariant(dataS);
    ignoreData = false;

}

ImageExperiment::~ImageExperiment(){
}
