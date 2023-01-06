#include "bindingexperiment.h"

const QString BindingExperiment::MSG_CORRECT = "studystatus_binding_correct";
const QString BindingExperiment::MSG_INCORRECT = "studystatus_binding_incorrect";

BindingExperiment::BindingExperiment(QWidget *parent, const QString &study_type):Experiment(parent,study_type){

    manager = new BindingManager();
    m = static_cast<BindingManager*>(manager);

    // Connecting the timer time out with the time out function.
    connect(&stateTimer,&QTimer::timeout,this,&BindingExperiment::nextState);

    metaStudyType = VMDC::MultiPartStudyBaseName::BINDING;

}

bool BindingExperiment::startExperiment(const QString &workingDir, const QString &experimentFile, const QVariantMap &studyConfig){

    // We need to set up the binding type to know which of the explanation set of strings will be used.
    QVariantMap config;
    config.insert(BindingManager::CONFIG_IS_BC,(studyType == VMDC::Study::BINDING_BC));
    config.insert(BindingManager::CONFIG_N_TARGETS,studyConfig.value(VMDC::StudyParameter::NUMBER_TARGETS));

    m->configure(config);


    if (!Experiment::startExperiment(workingDir,experimentFile,studyConfig)) return false;

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
        emit Experiment::experimentEndend(ER_FAILURE);
        return false;
    }

    correctResponses = 0;
    incorrectResponses = 0;

    drawCurrentImage();
    stateTimer.setInterval(TIME_START_CROSS);
    //stateTimer.start();
    updateDisplay();

    return true;

}

void BindingExperiment::drawCurrentImage(){

    if (state != STATE_RUNNING) return;

    if (trialState == TSB_CENTER_CROSS){
        //qWarning() << "DRAWING: Center Cross" << currentTrial;
        m->drawCenter();
        updateDisplay();
        return;
    }

    if ((trialState == TSB_TRANSITION) || (trialState == TSB_FINISH)){
        //qWarning() << "DRAWING: Transition" << currentTrial;
        m->drawClear();
        updateDisplay();
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
    updateDisplay();

}

bool BindingExperiment::advanceTrial(){

    //qWarning() << currentTrial;

    if (currentTrial < m->size()-1){
        currentTrial++;
        if (m->getLoopValue() > 0){
            if (currentTrial > m->getLoopValue()){
                currentTrial = 0;
            }
        }
        return true;
    }

    state = STATE_STOPPED;
    stateTimer.stop();
    return false;
}

void BindingExperiment::newEyeDataAvailable(const EyeTrackerData &data){
    Experiment::newEyeDataAvailable(data);

    if (studyPhase != SP_EVALUATION) return;

    if (state != STATE_RUNNING) return;
    if (ignoreData) return;
    if ((trialState != TSB_SHOW) && (trialState != TSB_TEST)) return;

    if (data.isLeftZero(false) && data.isRightZero(false)) return;

    rawdata.addNewRawDataVector(ViewMindDataContainer::GenerateStdRawDataVector(static_cast<qreal>(data.timestamp()),
                                                                                data.xr(),
                                                                                data.yr(),
                                                                                data.xl(),
                                                                                data.yl(),
                                                                                data.pr(),
                                                                                data.pl()));
    computeOnlineFixations(data);

}

void BindingExperiment::resetStudy(){

    // Resettign the stats.
    incorrectResponses = 0;
    correctResponses = 0;
    updateStudyMessages();

    currentTrial = 0;
    error = "";
    state = STATE_RUNNING;
    atLast = false;
    ignoreData = false;
    trialState = TSB_CENTER_CROSS;

    drawCurrentImage();
    stateTimer.setInterval(TIME_START_CROSS);
    if (studyPhase == SP_EVALUATION) stateTimer.start();

    if (Globals::EyeTracker::IS_VR) updateDisplay();

}

void BindingExperiment::keyPressHandler(int keyPressed){

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

QString BindingExperiment::getExperimentDescriptionFile(const QVariantMap &studyConfig){
    if (studyType == VMDC::Study::BINDING_BC){
        if (studyConfig.value(VMDC::StudyParameter::NUMBER_TARGETS).toString() == VMDC::BindingTargetCount::TWO){
            return ":/binding/descriptions/bc.dat";
        }
        else{
            return ":/binding/descriptions/bc_3.dat";
        }
    }
    else{
        if (studyConfig.value(VMDC::StudyParameter::NUMBER_TARGETS).toString() == VMDC::BindingTargetCount::TWO){
            return ":/binding/descriptions/uc.dat";
        }
        else{
            return ":/binding/descriptions/uc_3.dat";
        }
    }
}

void BindingExperiment::nextState(){

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
        if (studyPhase == SP_EVALUATION) stateTimer.start();
        drawCurrentImage();
        return;
    case TSB_FINISH:
        //qWarning() << "ENTER: FINISH" << currentTrial;
        if (advanceTrial()){

            trialState = TSB_CENTER_CROSS;
            drawCurrentImage();
            stateTimer.setInterval(TIME_START_CROSS);
            if (studyPhase == SP_EVALUATION) stateTimer.start();

        }
        else{

            // This is done.
            if (!rawdata.finalizeStudy()){
                error = "Failed on Binding study finalization: " + rawdata.getError();
                emit Experiment::experimentEndend(ER_FAILURE);
                return;
            }

            // We need to check if both binding bc AND bingin UC are part of this file, otherwise it is ongoing.
            QStringList studylist = rawdata.getStudies();
            if (studylist.contains(VMDC::Study::BINDING_BC) && studylist.contains(VMDC::Study::BINDING_UC)){
                rawdata.markFileAsFinalized();
            }


            if (!saveDataToHardDisk()) emit Experiment::experimentEndend(ER_FAILURE);
            else emit Experiment::experimentEndend(ER_NORMAL);
        }
        return;
    case TSB_SHOW:
        //qWarning() << "ENTER: SHOW" << currentTrial;

        // Encoding Ends.
        if (studyPhase == SP_EVALUATION){
            finalizeOnlineFixations();
            rawdata.finalizeDataSet();
        }

        trialState = TSB_TRANSITION;
        drawCurrentImage();

        stateTimer.setInterval(TIME_WHITE_TRANSITION);
        if (studyPhase == SP_EVALUATION) stateTimer.start();

        return;
    case TSB_TEST:
        //qWarning() << "ENTER: TEST" << currentTrial;
        trialState = TSB_FINISH;

        // Retrieval Ends.
        if (studyPhase == SP_EVALUATION){
            finalizeOnlineFixations();
            rawdata.finalizeDataSet();
            rawdata.finalizeTrial(answer);
            if (answer != expectedResponse){
                incorrectResponses++;
            }
            else{
                correctResponses++;
            }
            updateStudyMessages();
        }

        drawCurrentImage();

        stateTimer.setInterval(TIME_FINISH);
        if (studyPhase == SP_EVALUATION) stateTimer.start();

        return;
    case TSB_TRANSITION:
        //qWarning() << "ENTER: TRANSITION" << currentTrial;
        trialState = TSB_TEST;

        // Retrieval begins. We start the new dataset.
        if (studyPhase == SP_EVALUATION) rawdata.setCurrentDataSet(VMDC::DataSetType::RETRIEVAL_1);

        answer = "N/A";
        drawCurrentImage();
        stateTimer.setInterval(TIME_IMAGE_2_TIMEOUT);
        if (studyPhase == SP_EVALUATION) stateTimer.start();
        return;
    }


}

bool BindingExperiment::addNewTrial(){
    QString type = "";
    currentTrialID = m->getTrial(currentTrial).name;

    if (currentTrialID.toUpper().contains("SAME")) type = "S";
    else if (currentTrialID.toUpper().contains("DIFFERENT")) type = "D";

    expectedResponse = type;
    if (DBUGBOOL(Debug::Options::DBUG_MSG)){
        qDebug() << "Expected response " << type;
    }

    if (studyPhase == SP_EVALUATION) {
        // The type of trial is the right answer in this case.
        if (!rawdata.addNewTrial(currentTrialID,type,type)){
            error = "Creating a new trial for " + currentTrialID + " gave the following error: " + rawdata.getError();
            return false;
        }
        rawdata.setCurrentDataSet(VMDC::DataSetType::ENCODING_1);
    }
    return true;
}

void BindingExperiment::updateStudyMessages(){
    studyMessages.clear();
    studyMessages[MSG_CORRECT] = correctResponses;
    studyMessages[MSG_INCORRECT] = incorrectResponses;
    emit Experiment::updateStudyMessages(studyMessages);
}



BindingExperiment::~BindingExperiment(){
}
