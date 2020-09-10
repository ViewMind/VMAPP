#include "gonogoexperiment.h"

GoNoGoExperiment::GoNoGoExperiment(QWidget *parent):Experiment(parent)
{
    manager = new GoNoGoManager();
    m = dynamic_cast<GoNoGoManager*>(manager);
    expHeader = HEADER_GONOGO_EXPERIMENT;
    outputDataFile = FILE_OUTPUT_GONOGO;

    // Connecting the timer time out with the time out function.
    connect(&stateTimer,&QTimer::timeout,this,&GoNoGoExperiment::onTimeOut);
}


void GoNoGoExperiment::onTimeOut(){

    stateTimer.stop();

    switch(gngState){
    case GNGS_CROSS:
        //qDebug() << "DRAWING TRIAL";
        m->drawCurrentTrial();        
        stateTimer.setInterval(GONOGO_TIME_ESTIMULUS);
        stateTimer.start();
        gngState = GNGS_ESTIMULUS;
        break;
    case GNGS_ESTIMULUS:
        //qDebug() << "DRAWING CROSS";
        if (!m->drawCross()){
            // Experiment is finished.
            stateTimer.stop();
            state = STATE_STOPPED;
            ExperimentResult er;
            if (error.isEmpty()) er = ER_NORMAL;
            else er = ER_WARNING;
            if (!saveDataToHardDisk()){
                emit(experimentEndend(ER_FAILURE));
            }
            else emit(experimentEndend(er));
            return;
        }
        else{
            addTrialHeader();
            stateTimer.setInterval(GONOGO_TIME_CROSS);
            stateTimer.start();
            gngState = GNGS_CROSS;
        }
        break;
    }
    updateSecondMonitorORHMD();
}

bool GoNoGoExperiment::startExperiment(ConfigurationManager *c){
    if (!Experiment::startExperiment(c)) return false;

    ignoreData = false;

    state = STATE_RUNNING;

    MovingWindowParameters mwp;
    mwp.sampleFrequency = c->getReal(CONFIG_SAMPLE_FREQUENCY);
    mwp.minimumFixationLength = c->getReal(CONFIG_MIN_FIXATION_LENGTH);
    mwp.maxDispersion = c->getReal(CONFIG_MOVING_WINDOW_DISP);
    mwp.calculateWindowSize();
    rMWA.parameters = mwp;
    lMWA.parameters = mwp;

    m->drawCross();
    addTrialHeader();
    stateTimer.setInterval(GONOGO_TIME_CROSS);
    stateTimer.start();
    gngState = GNGS_CROSS;
    rMWA.finalizeOnlineFixationCalculation();
    lMWA.finalizeOnlineFixationCalculation();

    if (!vrEnabled){
        this->show();
        this->activateWindow();
    }

    updateSecondMonitorORHMD();

    return true;
}

void GoNoGoExperiment::newEyeDataAvailable(const EyeTrackerData &data){
    Experiment::newEyeDataAvailable(data);

    if (state != STATE_RUNNING) return;

    if (ignoreData) return;
    if (gngState != GNGS_ESTIMULUS) return;

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

    // Checking if there is a fixation inside the correct target.
    //qDebug() << data.toString();
    Fixation r = rMWA.calculateFixationsOnline(data.xRight,data.yRight,static_cast<qreal>(data.time));
    Fixation l = lMWA.calculateFixationsOnline(data.xLeft,data.xLeft,static_cast<qreal>(data.time));

    if (r.isValid()){
        if (m->isPointInSideCorrectTargetForCurrentTrial(r.x,r.y)){
            onTimeOut();
            return;
        }
    }

    if (l.isValid()){
        if (m->isPointInSideCorrectTargetForCurrentTrial(l.x,l.y)){
            onTimeOut();
            return;
        }
    }

}

void GoNoGoExperiment::addTrialHeader(){

    ignoreData = true;

    // Saving the current image.
    QVariantList dataS;
    dataS  << m->getCurrentTrialHeader();
    etData << QVariant(dataS);

    ignoreData = false;
}

void GoNoGoExperiment::keyPressHandler(int keyPressed){
    // Making sure the experiment can be aborted, but any other key is ignored.
    if (keyPressed == Qt::Key_Escape){
        experimenteAborted();
        return;
    }
    else if (keyPressed == Qt::Key_N){
        //qDebug() << "KEY N";
        onTimeOut();
        return;
    }
//    else{
//        if ((state == STATE_PAUSED) && (keyPressed == Qt::Key_G)){
//            state = STATE_RUNNING;
//            m->drawCross();
//            addTrialHeader();
//            stateTimer.setInterval(GONOGO_TIME_CROSS);
//            stateTimer.start();
//            updateSecondMonitorORHMD();
//        }
//    }
}


void GoNoGoExperiment::togglePauseExperiment(){
    // The pause is automatic or non existant.
}
