#include "fieldingexperiment.h"

const qint32 FieldingExperiment::TIME_TRANSITION =                              500;
const qint32 FieldingExperiment::TIME_TARGET =                                  250;
const qint32 FieldingExperiment::TIME_CONDITION =                               500;

// Possible pauses for the fielding experiment
const qint32 FieldingExperiment::PAUSE_TRIAL_1 =                                32;
const qint32 FieldingExperiment::PAUSE_TRIAL_2 =                                64;


FieldingExperiment::FieldingExperiment(QWidget *parent, const QString &study_type):Experiment(parent,study_type){

    manager = new FieldingManager();
    m = static_cast<FieldingManager*>(manager);

    // Connecting the timer time out with the time out function.
    connect(&stateTimer,&QTimer::timeout,this,&FieldingExperiment::onTimeOut);
}

bool FieldingExperiment::startExperiment(const QString &workingDir, const QString &experimentFile,
                                         const QVariantMap &studyConfig, bool useMouse){

    if (!Experiment::startExperiment(workingDir,experimentFile,studyConfig,useMouse)) return false;

    QVariantMap config;
    config.insert(FieldingManager::CONFIG_IS_VR_BEING_USED,(Globals::EyeTracker::IS_VR && (!useMouse)));
    if (studyConfig.value(VMDC::StudyParameter::LANGUAGE).toString() == VMDC::UILanguage::SPANISH){
        config.insert(FieldingManager::CONFIG_PAUSE_TEXT_LANG,FieldingManager::LANG_ES);
    }
    else{
        config.insert(FieldingManager::CONFIG_PAUSE_TEXT_LANG,FieldingManager::LANG_ES);
    }
    m->configure(config);


    currentImage = 0;
    currentTrial = 0;
    error = "";
    ignoreData = false;

    state = STATE_RUNNING;
    tstate = TSF_START;
    stateTimer.setInterval(TIME_TRANSITION);
    stateTimer.start();

    if (!addNewTrial()){
        stateTimer.stop();
        emit(experimentEndend(ER_FAILURE));
        return false;
    }

    // Start enconding 1
    rawdata.setCurrentDataSet(VMDC::DataSetType::ENCODING_1);

    m->drawBackground();
    drawCurrentImage();

    if (!Globals::EyeTracker::IS_VR || (useMouse)){
        this->show();
        this->activateWindow();
    }

    return true;

}

void FieldingExperiment::nextState(){

    switch (tstate){
    case TSF_SHOW_BLANK_1:

        // End retrieval 3. And trial.
        finalizeOnlineFixations();
        rawdata.finalizeDataSet();        
        rawdata.finalizeTrial("");

        currentTrial++;
        if (finalizeExperiment()) return;
        tstate = TSF_START;
        stateTimer.setInterval(TIME_TRANSITION);
        break;
    case TSF_SHOW_BLANK_2:

        // End retrieval 2
        finalizeOnlineFixations();
        rawdata.finalizeDataSet();
        // Start retrieval 3
        rawdata.setCurrentDataSet(VMDC::DataSetType::RETRIEVAL_3);

        tstate = TSF_SHOW_BLANK_1;
        currentImage = 0;
        stateTimer.setInterval(TIME_CONDITION);
        break;
    case TSF_SHOW_BLANK_3:

        // End Retrieval 1
        finalizeOnlineFixations();
        rawdata.finalizeDataSet();
        // Start Retrieval 2
        rawdata.setCurrentDataSet(VMDC::DataSetType::RETRIEVAL_2);

        tstate = TSF_SHOW_BLANK_2;
        currentImage = 1;
        stateTimer.setInterval(TIME_CONDITION);
        break;
    case TSF_SHOW_DOT_1:

        // End Enconding 1
        finalizeOnlineFixations();
        rawdata.finalizeDataSet();
        // Start enconding 2
        rawdata.setCurrentDataSet(VMDC::DataSetType::ENCODING_2);

        tstate = TSF_SHOW_DOT_2;
        currentImage = 1;
        stateTimer.setInterval(TIME_TARGET);
        break;

    case TSF_SHOW_DOT_2:

        // End Enconding 2
        finalizeOnlineFixations();
        rawdata.finalizeDataSet();

        // Start enconding 3
        rawdata.setCurrentDataSet(VMDC::DataSetType::ENCODING_3);

        tstate = TSF_SHOW_DOT_3;
        currentImage = 2;
        stateTimer.setInterval(TIME_TARGET);
        break;
    case TSF_SHOW_DOT_3:

        // End Encondinng 3.
        finalizeOnlineFixations();
        rawdata.finalizeDataSet();

        tstate = TSF_TRANSITION;
        stateTimer.setInterval(TIME_TRANSITION);
        break;
    case TSF_START:

        if (!addNewTrial()){
            stateTimer.stop();
            emit(experimentEndend(ER_FAILURE));
            return;
        }

        // Start enconding 1
        rawdata.setCurrentDataSet(VMDC::DataSetType::ENCODING_1);

        tstate = TSF_SHOW_DOT_1;
        currentImage = 0;
        stateTimer.setInterval(TIME_TARGET);
        break;
    case TSF_TRANSITION:

        // Start Retrieval 1
        rawdata.setCurrentDataSet(VMDC::DataSetType::RETRIEVAL_1);

        tstate = TSF_SHOW_BLANK_3;
        currentImage = 2;
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
                //qDebug() << "Pausing wihth current trial" << currentTrial;
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

    if (!rawdata.finalizeStudy()){
        error = "Failed on NBackMS study finalization: " + rawdata.getError();
        emit (experimentEndend(ER_FAILURE));
        return false;
    }
    rawdata.markFileAsFinalized();

    ExperimentResult er;
    if (error.isEmpty()) er = ER_NORMAL;
    else er = ER_WARNING;



    if (!saveDataToHardDisk()){
        emit(experimentEndend(ER_FAILURE));
    }
    else emit(experimentEndend(er));
    return true;
}


void FieldingExperiment::keyPressHandler(int keyPressed){
    // Making sure the experiment can be aborted, but any other key is ignored.
    if (keyPressed == Qt::Key_Escape){
        experimenteAborted();
        return;
    }
    else{
        if ((state == STATE_PAUSED) && (keyPressed == Qt::Key_G)){
            state = STATE_RUNNING;
            m->drawBackground();
            updateSecondMonitorORHMD();
            nextState();
        }
    }
}

QString FieldingExperiment::getExperimentDescriptionFile(const QVariantMap &studyConfig){
    Q_UNUSED(studyConfig)
    return ":/nbackfamiliy/descriptions/fielding.dat";
}

void FieldingExperiment::newEyeDataAvailable(const EyeTrackerData &data){
    Experiment::newEyeDataAvailable(data);

    if (state != STATE_RUNNING) return;
    if (ignoreData) return;

    // Not saving data during transition screens.
    if ((tstate == TSF_START) || (tstate == TSF_TRANSITION)) return;

    if (data.isLeftZero() && data.isRightZero()) return;

    // Format: Image ID, time stamp for right and left, word index, character index, sentence length and pupil diameter for left and right eye.
    rawdata.addNewRawDataVector(ViewMindDataContainer::GenerateStdRawDataVector(data.time,data.xRight,data.yRight,data.xLeft,data.yLeft,data.pdRight,data.pdLeft));

    computeOnlineFixations(data);

}


QVariantMap FieldingExperiment::addHitboxesToProcessingParameters(QVariantMap pp){
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

    // Store them as part of the processing parameters.
    pp.insert(VMDC::ProcessingParameter::NBACK_HITBOXES,modHitBoxes);
    return pp;
}


bool FieldingExperiment::addNewTrial(){
    QString type = m->getFullSequenceAsString(currentTrial);
    currentTrialID = QString::number(currentTrial);

    if (!rawdata.addNewTrial(currentTrialID,type,"")){
        error = "Creating a new trial for " + currentTrialID + " gave the following error: " + rawdata.getError();
        return false;
    }
    return true;
}
