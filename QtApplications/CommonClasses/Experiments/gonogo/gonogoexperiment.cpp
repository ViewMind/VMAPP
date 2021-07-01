#include "gonogoexperiment.h"

GoNoGoExperiment::GoNoGoExperiment(QWidget *parent, const QString &study_type):Experiment(parent,study_type)
{
    manager = new GoNoGoManager();
    m = static_cast<GoNoGoManager*>(manager);

    // Connecting the timer time out with the time out function.
    connect(&stateTimer,&QTimer::timeout,this,&GoNoGoExperiment::onTimeOut);

    this->studyType = study_type;
}


void GoNoGoExperiment::onTimeOut(){

    stateTimer.stop();

    switch(gngState){
    case GNGS_CROSS:

        if (!addNewTrial()){
            emit(experimentEndend(ER_FAILURE));
            stateTimer.stop();
            return;
        }
        rawdata.setCurrentDataSet(VMDC::DataSetType::UNIQUE);

        //qDebug() << "DRAWING TRIAL";
        m->drawCurrentTrial();        
        rMWA.finalizeOnlineFixationCalculation();
        lMWA.finalizeOnlineFixationCalculation();
        stateTimer.setInterval(GONOGO_TIME_ESTIMULUS);
        stateTimer.start();
        gngState = GNGS_ESTIMULUS;
#ifdef DBUG_FIX_TO_MOVE
        mtimer.start();
#endif
        break;
    case GNGS_ESTIMULUS:
#ifdef DBUG_FIX_TO_MOVE
        qDebug() << "Timeout @ " << mtimer.elapsed();
        qDebug() << "================================";
#endif
        rawdata.finalizeDataSet();
        finalizeOnlineFixations();
        rawdata.finalizeTrial("");

        if (!m->drawCross()){

            // Experiment is finished and marking as such. No ongoing for go no go.
            if (!rawdata.finalizeStudy()){
                error = "Failed on GoNoGo finalization because: " + rawdata.getError();
                emit(experimentEndend(ER_FAILURE));
                stateTimer.stop();
                return;
            }
            rawdata.markFileAsFinalized();

            stateTimer.stop();
            state = STATE_STOPPED;
            ExperimentResult er;
            if (error.isEmpty()) er = ER_NORMAL;
            else er = ER_WARNING;
            if (!saveDataToHardDisk()){
                emit(experimentEndend(ER_FAILURE));
            }
            else emit(experimentEndend(er));
#ifdef DBUG_FIX_TO_MOVE
            qDebug() << "TOTAL TIME" << totalTimer.elapsed();
#endif
            return;
        }
        else{
            stateTimer.setInterval(GONOGO_TIME_CROSS);
            stateTimer.start();
            gngState = GNGS_CROSS;
        }
        break;
    }
    updateSecondMonitorORHMD();
}

bool GoNoGoExperiment::startExperiment(const QString &workingDir, const QString &experimentFile,
                                       const QVariantMap &studyConfig, bool useMouse){

    if (!Experiment::startExperiment(workingDir,experimentFile,studyConfig,useMouse)){
        emit(experimentEndend(ER_FAILURE));
        return false;
    }

    // Go No Go only has 1 type of trial list.
    processingParameters = setGoNoGoTargetBoxes(processingParameters);
    if (!rawdata.setProcessingParameters(processingParameters)){
        error = "Failed to set processing parameters when addign gonogo targetboxes";
        emit(experimentEndend(ER_FAILURE));
        return false;
    }

    ignoreData = false;

    state = STATE_RUNNING;

    m->drawCross();
    stateTimer.setInterval(GONOGO_TIME_CROSS);
    stateTimer.start();
    gngState = GNGS_CROSS;

    if (!Globals::EyeTracker::IS_VR || (useMouse)){
        this->show();
        this->activateWindow();
    }

    rMWA.setMinimalFixations(true);
    lMWA.setMinimalFixations(true);

    updateSecondMonitorORHMD();

#ifdef DBUG_FIX_TO_MOVE
    totalTimer.start();
#endif

    return true;
}

void GoNoGoExperiment::newEyeDataAvailable(const EyeTrackerData &data){
    Experiment::newEyeDataAvailable(data);

    if (state != STATE_RUNNING) return;

    if (ignoreData) return;
    if (gngState != GNGS_ESTIMULUS) return;

    if (data.isLeftZero() && data.isRightZero()) return;

    // Format: Image ID, time stamp for right and left, word index, character index, sentence length and pupil diameter for left and right eye.
    rawdata.addNewRawDataVector(ViewMindDataContainer::GenerateStdRawDataVector(data.time,data.xRight,data.yRight,data.xLeft,data.yLeft,data.pdRight,data.pdLeft));

    // Checking if there is a fixation inside the correct target.
    //qDebug() << data.toString();
    computeOnlineFixations(data);

    if (lastFixationR.isValid()){
        //qDebug() << "RFIX" << r.toString();
        if (m->isPointInSideCorrectTargetForCurrentTrial(lastFixationR.x,lastFixationR.y)){
#ifdef DBUG_FIX_TO_MOVE
            qDebug() << "END TRIAL " <<  m->getCurrentTrialHeader()  << " RIGHT FIX @"  << data.time << ". DUR" << lastFixationR.duration;
#endif
            onTimeOut();
            return;
        }
    }

    if (lastFixationL.isValid()){
        //qDebug() << "LFIX" << l.toString();
        if (m->isPointInSideCorrectTargetForCurrentTrial(lastFixationL.x,lastFixationL.y)){
#ifdef DBUG_FIX_TO_MOVE
            qDebug() << "END TRIAL " <<  m->getCurrentTrialHeader()  << " LEFT FIX @"  << data.time << ". DUR" << lastFixationL.duration;
#endif
            onTimeOut();
            return;
        }
    }

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
}

QString GoNoGoExperiment::getExperimentDescriptionFile(const QVariantMap &studyConfig){
    Q_UNUSED(studyConfig)
    return ":/gonogo/descriptions/go_no_go.dat";
}

bool GoNoGoExperiment::addNewTrial(){
    QString temp = m->getCurrentTrialHeader();
    QStringList parts = temp.split(" ");
    if (parts.size() > 2){
        error = "Got more than two parts when splitting the trial header: " + temp + " for GoNOGo";
        return false;
    }

    qint32 trial_id = parts.last().toInt();
    QString trial_type = GoNoGoParser::TrialTypeList.at(trial_id);

    if (!rawdata.addNewTrial(parts.first(),trial_type,"")){
        error = "Failed in creating go no for trial for header " + temp + ":  " + rawdata.getError();
        return false;
    }
    return true;
}

QVariantMap GoNoGoExperiment::setGoNoGoTargetBoxes(QVariantMap pp){
    QRectF arrow = m->getArrowBox();
    QList<QRectF> left_and_right = m->getLeftAndRightHitBoxes();

    QVariantList target_box_vector;
    QVariantList target_boxes;
    target_box_vector << arrow.x() << arrow.y() << arrow.width() << arrow.height();
    target_boxes << static_cast<QVariant>(target_box_vector);
    target_box_vector.clear();
    target_box_vector << left_and_right.at(0).x() << left_and_right.at(0).y() << left_and_right.at(0).width() << left_and_right.at(0).height();
    target_boxes << static_cast<QVariant>(target_box_vector);
    target_box_vector.clear();
    target_box_vector << left_and_right.at(1).x() << left_and_right.at(1).y() << left_and_right.at(1).width() << left_and_right.at(1).height();
    target_boxes << static_cast<QVariant>(target_box_vector);

    pp.insert(VMDC::ProcessingParameter::GONOGO_HITBOXES,target_boxes);
    return pp;

}

