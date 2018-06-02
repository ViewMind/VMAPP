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

    if (m->getUsesNumbers()) timeCountForStart = TIME_START_NUMBER;
    else timeCountForStart = TIME_START_CROSS;

    // If there were no problems the experiment can begin.
    currentTrial = 0;
    error = "";
    state = STATE_RUNNING;
    atLast = false;
    ignoreData = false;
    newImage(m->getTrial(currentTrial).name,0);
    trialState = TSB_CENTER_CROSS;
    drawCurrentImage();
    stateTimer->setInterval(1);
    stateTimer->start();
    timerCounter = timeCountForStart;
    this->show();
    this->activateWindow();

    return true;

}

void ImageExperiment::drawCurrentImage(){

    if (trialState == TSB_CENTER_CROSS){
        //qWarning() << "DRAWING: Center Cross" << currentTrial;
        m->drawCenter(currentTrial);
        emit(updateBackground(m->getImage()));
        return;
    }

    if ((trialState == TSB_TRANSITION) || (trialState == TSB_FINISH) || (trialState == TSB_NUMBER_TRANSITION)){
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

void ImageExperiment::advanceTrial(){

    //qWarning() << currentTrial;

    if (currentTrial < m->size()-1){
        currentTrial++;
        // Adding the entry in the file
        newImage(m->getTrial(currentTrial).name,0);
        return;
    }
    // This is done.
    state = STATE_STOPPED;

    stateTimer->stop();

    if (!error.isEmpty()) {
        emit(experimentEndend(ER_WARNING));
    }
    else {
        emit(experimentEndend(ER_NORMAL));
    }
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
        //qWarning() << "ENTER: Center cross" << currentTrial;
        if (!m->getUsesNumbers()){
            trialState = TSB_SHOW;
            timerCounter = TIME_IMAGE_1;
            drawCurrentImage();
        }
        else{
            trialState = TSB_NUMBER_TRANSITION;
            timerCounter = TIME_NUMBER_TRANSITION;
            drawCurrentImage();
        }
        return;
    case TSB_NUMBER_TRANSITION:
        //qWarning() << "ENTER: NTransition" << currentTrial;
        trialState = TSB_SHOW;
        drawCurrentImage();
        timerCounter = TIME_IMAGE_1;
        break;
    case TSB_FINISH:
        //qWarning() << "ENTER: FINISH" << currentTrial;
        advanceTrial();
        trialState = TSB_CENTER_CROSS;
        drawCurrentImage();
        timerCounter = timeCountForStart;
        return;
    case TSB_SHOW:
        //qWarning() << "ENTER: SHOW" << currentTrial;
        trialState = TSB_TRANSITION;
        drawCurrentImage();
        timerCounter = TIME_WHITE_TRANSITION;
        return;
    case TSB_TEST:
        //qWarning() << "ENTER: TEST" << currentTrial;
        trialState = TSB_FINISH;
        addAnswer(answer);
        drawCurrentImage();
        timerCounter = TIME_FINISH;
        return;
    case TSB_TRANSITION:
        //qWarning() << "ENTER: TRANSITION" << currentTrial;
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
