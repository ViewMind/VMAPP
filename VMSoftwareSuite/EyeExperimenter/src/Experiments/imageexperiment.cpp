#include "imageexperiment.h"

ImageExperiment::ImageExperiment(QWidget *parent, const QString &studyType):Experiment(parent,studyType){

    manager = new BindingManager();
    m = (BindingManager*) manager;

    // Connecting the timer time out with the time out function.
    connect(&stateTimer,&QTimer::timeout,this,&ImageExperiment::nextState);

    metaStudyType = VMDC::MultiPartStudyBaseName::BINDING;

}

bool ImageExperiment::startExperiment(const QString &workingDir, const QString &experimentFile, const QVariantMap &studyConfig, bool useMouse, QVariantMap pp){

    // We need to set up the target size before parsing.
    QVariantMap config;
    config.insert(BindingManager::CONFIG_USE_SMALL_TARGETS,
                  (studyConfig.value(VMDC::StudyParameter::TARGET_SIZE).toString() == VMDC::BindingTargetSize::SMALL));
    m->configure(config);

    if (!Experiment::startExperiment(workingDir,experimentFile,studyConfig,useMouse,pp)) return false;

    // No extra processing parameters required so we set it.
    if (!rawdata.setProcessingParameters(pp)){
        error = "Failed setting processing parameters on Reading: " + rawdata.getError();
        emit(experimentEndend(ER_FAILURE));
    }

    // Setup data gathering.
    // If there were no problems the experiment can begin.
    currentTrial = 0;
    error = "";
    state = STATE_RUNNING;
    atLast = false;
    ignoreData = false;
    trialState = TSB_CENTER_CROSS;

    // Adding a new trial for the first time.
    if (!addNewTrial()){
        emit(experimentEndend(ER_FAILURE));
        return false;
    }

    drawCurrentImage();
    stateTimer.setInterval(TIME_START_CROSS);
    stateTimer.start();
    if (!Globals::EyeTracker::IS_VR || (useMouse)){
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
        return true;
    }

    state = STATE_STOPPED;
    stateTimer.stop();
    return false;
}

void ImageExperiment::newEyeDataAvailable(const EyeTrackerData &data){
    Experiment::newEyeDataAvailable(data);

    if (state != STATE_RUNNING) return;
    if (ignoreData) return;
    if ((trialState != TSB_SHOW) && (trialState != TSB_TEST)) return;

    if (data.isLeftZero() && data.isRightZero()) return;

    rawdata.addNewRawDataVector(ViewMindDataContainer::GenerateStdRawDataVector(data.time,data.xRight,data.yRight,data.xLeft,data.yLeft,data.pdRight,data.pdLeft));
    computeOnlineFixations(data);

}

void ImageExperiment::keyPressHandler(int keyPressed){

    // Making sure the experiment can be aborted
    if (keyPressed == Qt::Key_Escape){
        experimenteAborted();
        return;
    }
    else if(keyPressed == Qt::Key_N) {
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

        // Enconding begins.
        if (!addNewTrial()){
            emit(experimentEndend(ER_FAILURE));
            return;
        }

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

            // This is done.
            if (!rawdata.finalizeStudy()){
                error = "Failed on Binding study finalization: " + rawdata.getError();
                emit(experimentEndend(ER_FAILURE));
                return;
            }

            // We need to check if both binding bc AND bingin UC are part of this file, otherwise it is ongoing.
           QStringList studylist = rawdata.getStudies();
            if (studylist.contains(VMDC::Study::BINDING_BC) && studylist.contains(VMDC::Study::BINDING_UC)){
                rawdata.markFileAsFinalized();
            }


            if (!saveDataToHardDisk()) emit(experimentEndend(ER_FAILURE));
            else emit(experimentEndend(ER_NORMAL));
        }
        return;
    case TSB_SHOW:
        //qWarning() << "ENTER: SHOW" << currentTrial;        

        // Encoding Ends.
        rawdata.finalizeDataSet();
        finalizeOnlineFixations();

        trialState = TSB_TRANSITION;
        drawCurrentImage();
        stateTimer.setInterval(TIME_WHITE_TRANSITION);
        stateTimer.start();
        return;
    case TSB_TEST:
        //qWarning() << "ENTER: TEST" << currentTrial;
        trialState = TSB_FINISH;

        // Retrieval Ends.
        rawdata.finalizeDataSet();
        finalizeOnlineFixations();

        rawdata.finalizeTrial(answer);

        drawCurrentImage();
        stateTimer.setInterval(TIME_FINISH);
        stateTimer.start();
        return;
    case TSB_TRANSITION:
        //qWarning() << "ENTER: TRANSITION" << currentTrial;
        trialState = TSB_TEST;

        // Retrieval begins. We start the new dataset.
        rawdata.setCurrentDataSet(VMDC::DataSetType::RETRIEVAL_1);

        answer = "N/A";
        drawCurrentImage();
        stateTimer.setInterval(TIME_IMAGE_2_TIMEOUT);
        stateTimer.start();
        return;
    }


}

bool ImageExperiment::addNewTrial(){
    QString type = "";
    currentTrialID = m->getTrial(currentTrial).name;

    if (currentTrialID.toUpper().contains("SAME")) type = "S";
    else if (currentTrialID.toUpper().contains("DIFFERENT")) type = "D";

    if (!rawdata.addNewTrial(currentTrialID,type)){
        error = "Creating a new trial for " + currentTrialID + " gave the following error: " + rawdata.getError();
        return false;
    }
    rawdata.setCurrentDataSet(VMDC::DataSetType::ENCODING_1);
    return true;
}



ImageExperiment::~ImageExperiment(){
}
