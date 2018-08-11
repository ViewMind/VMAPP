#include "imageexperiment.h"

ImageExperiment::ImageExperiment(bool bound, bool use3Targets, QWidget *parent):Experiment(parent){

    manager = new BindingManager();
    m = (BindingManager*) manager;
    expHeader = HEADER_IMAGE_EXPERIMENT;
    if (bound) outputDataFile = FILE_OUTPUT_BINDING_BC;
    else outputDataFile = FILE_OUTPUT_BINDING_UC;

    if (use3Targets){
        outputDataFile = outputDataFile + "_3";
    }

    // Connecting the timer time out with the time out function.
    //connect(&stateTimer,&QTimer::timeout,this,&ImageExperiment::onTimeOut);
    connect(&stateTimer,&QTimer::timeout,this,&ImageExperiment::nextState);

    // Binding type.
    isBound = bound;

}

bool ImageExperiment::startExperiment(ConfigurationManager *c){

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
    this->show();
    this->activateWindow();

    return true;

}

void ImageExperiment::drawCurrentImage(){

    if (state != STATE_RUNNING) return;

    if (trialState == TSB_CENTER_CROSS){
        //qWarning() << "DRAWING: Center Cross" << currentTrial;
        m->drawCenter();
        emit(updateBackground(m->getImage()));
        return;
    }

    if ((trialState == TSB_TRANSITION) || (trialState == TSB_FINISH)){
        //qWarning() << "DRAWING: Transition" << currentTrial;
        gview->scene()->clear();
        emit(updateBackground(m->getImage()));
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
    emit(updateBackground(m->getImage()));

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

    QFile file(dataFile);
    if (!file.open(QFile::Append)){
        error = "Could not open data file " + dataFile + " for appending data.";
        state = STATE_STOPPED;
        emit(experimentEndend(ER_FAILURE));
    }

    QTextStream writer(&file);
    writer.setCodec(COMMON_TEXT_CODEC);

    // Adding the data row.
    writer << data.time << " "
           << data.xRight << " "
           << data.yRight << " "
           << data.xLeft << " "
           << data.yLeft << " "
           << data.pdRight << " "
           << data.pdLeft << "\n";


    file.close();

}

void ImageExperiment::keyPressEvent(QKeyEvent *event){

    // Making sure the experiment can be aborted
    if (event->key() == Qt::Key_Escape){
        stateTimer.stop();
        state = STATE_STOPPED;
        emit(experimentEndend(ER_ABORTED));
    }
    else if(event->key() == Qt::Key_M) {
        nextState();
        return;
    }

    // Key presses are only valid in TSB_TEST state
    if (trialState != TSB_TEST) return;

    // If it's an odd image, THEN we see if the letter is S or D
    switch (event->key()){
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
            emit(experimentEndend(ER_NORMAL));
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

    QFile file(dataFile);
    if (!file.open(QFile::Append)){
        error = "Could not open data file " + dataFile + " for appending data.";
        state = STATE_STOPPED;
        emit(experimentEndend(ER_FAILURE));
    }

    QTextStream writer(&file);
    writer.setCodec(COMMON_TEXT_CODEC);

    writer << name << " "
           << isTrial // 1 if it is test and 0 if it is show.
           << "\n";

    file.close();

    ignoreData = false;

}

void ImageExperiment::addAnswer(QString ans){

    ignoreData = true;

    QFile file(dataFile);
    if (!file.open(QFile::Append)){
        error = "Could not open data file " + dataFile + " for appending data.";
        state = STATE_STOPPED;
        emit(experimentEndend(ER_FAILURE));
    }

    QTextStream writer(&file);
    writer.setCodec(COMMON_TEXT_CODEC);

    writer << m->getTrial(currentTrial).name << "->" << ans << "\n";
    file.close();

    ignoreData = false;

}

ImageExperiment::~ImageExperiment(){
}
