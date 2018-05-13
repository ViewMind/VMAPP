#include "imageexperiment.h"

ImageExperiment::ImageExperiment(bool bound, QWidget *parent):Experiment(parent){

    manager = new BindingManager();
    m = (BindingManager*) manager;
    expHeader = HEADER_IMAGE_EXPERIMENT;
    if (bound) outputDataFile = FILE_OUTPUT_BINDING_BC;
    else outputDataFile = FILE_OUTPUT_BINDING_UC;

    // Connecting the timer time out with the time out function.
    stateTimer = new QTimer();
    connect(stateTimer,&QTimer::timeout,this,&ImageExperiment::onTimeOut);

    // Binding type.
    isBound = bound;
}

bool ImageExperiment::startExperiment(ConfigurationManager *c){

    if (!c->getBool(CONFIG_BINDING_DEFAULT)){
        outputDataFile = outputDataFile + "_" + QString::number(c->getInt(CONFIG_BINDING_NUM_TARGETS)) + "_";
        if (c->getBool(CONFIG_BINDING_USE_NUMBERS)) outputDataFile = outputDataFile + "n";
        else outputDataFile = outputDataFile + "x";
    }

    if (!Experiment::startExperiment(c)) return false;

    // If there were no problems the experiment can begin.
    currentTrial = -1;
    error = "";
    state = STATE_RUNNING;
    ignoreData = false;
    trialState = TSB_CENTER_CROSS;
    drawCurrentImage();
    stateTimer->setInterval(1);
    stateTimer->start();
    timerCounter = TIME_START_CROSS;
    this->show();
    this->activateWindow();

    return true;

}

void ImageExperiment::drawCurrentImage(){
    if (trialState == TSB_CENTER_CROSS){
        m->drawCenter(currentTrial);
        return;
    }

    if ((trialState == TSB_TRANSITION) || (trialState == TSB_FINISH)){
        gview->scene()->clear();
        return;
    }

    if (trialState == TSB_SHOW) m->drawTrial(currentTrial,true);
    else if (trialState == TSB_TEST) m->drawTrial(currentTrial,false);

    emit(updateBackground(m->getImage()));

}

void ImageExperiment::advanceTrial(){

    if (currentTrial < m->getNumberOfTrials() - 1){
        currentTrial++;

        // Adding the entry in the file
        newImage(m->getTrial(currentTrial).name,0);

        drawCurrentImage();
        return;
    }

    // This is done.
    state = STATE_STOPPED;
    stateTimer->stop();
    if (!error.isEmpty()) emit(experimentEndend(ER_WARNING));
    else emit(experimentEndend(ER_NORMAL));
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
        stateTimer->stop();
        state = STATE_STOPPED;
        emit(experimentEndend(ER_ABORTED));
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

void ImageExperiment::onTimeOut(){
    timerCounter--;
    if (timerCounter == 0) {
        nextState();
    }
}

void ImageExperiment::nextState(){

    timerCounter = 0;

    switch (trialState){
    case TSB_CENTER_CROSS:
        //qWarning() << "ENTER: Center cross";
        trialState = TSB_SHOW;
        advanceTrial();
        timerCounter = TIME_IMAGE_1;
        return;
    case TSB_FINISH:
        //qWarning() << "ENTER: FINISH";
        trialState = TSB_CENTER_CROSS;
        drawCurrentImage();
        timerCounter = TIME_START_CROSS;
        return;
    case TSB_SHOW:
        //qWarning() << "ENTER: SHOW";
        trialState = TSB_TRANSITION;
        drawCurrentImage();
        timerCounter = TIME_WHITE_TRANSITION;
        return;
    case TSB_TEST:
        //qWarning() << "ENTER: TEST";
        trialState = TSB_FINISH;
        addAnswer(answer);
        drawCurrentImage();
        timerCounter = TIME_FINISH;
        return;
    case TSB_TRANSITION:
        //qWarning() << "ENTER: TRANSITION";
        trialState = TSB_TEST;
        answer = "N/A";
        newImage(m->getTrial(currentTrial).name,1);
        drawCurrentImage();
        timerCounter = TIME_IMAGE_2_TIMEOUT;
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
    delete stateTimer;
}
