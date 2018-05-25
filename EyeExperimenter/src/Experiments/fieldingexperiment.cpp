#include "fieldingexperiment.h"

FieldingExperiment::FieldingExperiment(QWidget *parent):Experiment(parent){

    manager = new FieldingManager();
    m = (FieldingManager*)manager;
    expHeader = HEADER_IMAGE_EXPERIMENT;
    outputDataFile = FILE_OUTPUT_FIELDING;

    // Used for the pause in the middle.
    Paused=0;

    // Connecting the timer time out with the time out function.
    stateTimer = new QTimer();
    connect(stateTimer,&QTimer::timeout,this,&FieldingExperiment::onTimeOut);
}

bool FieldingExperiment::startExperiment(ConfigurationManager *c){
    if (!Experiment::startExperiment(c)) return false;

    currentImage = 0;
    currentTrial = 0;
    error = "";
    ignoreData = false;

    state = STATE_RUNNING;
    tstate = TSF_START;
    stateTimer->setInterval(TIMER_TIME_INTERVAL);
    stateTimer->start();
    timerCounter = TIME_TRANSITION;

    drawCurrentImage();

    this->show();
    this->activateWindow();

    return true;

}

void FieldingExperiment::nextState(){

    switch (tstate){
    case TSF_SHOW_BLANK_1:
        currentTrial++;
        if (finalizeExperiment()) return;
        tstate = TSF_START;
        timerCounter = TIME_TRANSITION;
        break;
    case TSF_SHOW_BLANK_2:
        tstate = TSF_SHOW_BLANK_1;
        currentImage = 0;
        timerCounter = TIME_CONDITION;
        addTrialHeader();
        break;
    case TSF_SHOW_BLANK_3:
        tstate = TSF_SHOW_BLANK_2;
        currentImage = 1;
        timerCounter = TIME_CONDITION;
        addTrialHeader();
        break;
    case TSF_SHOW_DOT_1:
        tstate = TSF_SHOW_DOT_2;
        currentImage = 1;
        timerCounter = TIME_TARGET;
        addTrialHeader();
        break;
    case TSF_SHOW_DOT_2:
        tstate = TSF_SHOW_DOT_3;
        currentImage = 2;
        timerCounter = TIME_TARGET;
        addTrialHeader();
        break;
    case TSF_SHOW_DOT_3:
        tstate = TSF_TRANSITION;
        timerCounter = TIME_TRANSITION;
        break;
    case TSF_START:
        tstate = TSF_SHOW_DOT_1;
        currentImage = 0;
        timerCounter = TIME_TARGET;
        addTrialHeader();
        break;
    case TSF_TRANSITION:
        tstate = TSF_SHOW_BLANK_3;
        currentImage = 2;
        timerCounter = TIME_CONDITION;
        addTrialHeader();
        break;
    }

    drawCurrentImage();
}


// Function that draws pause text
void FieldingExperiment::drawPauseImage(){

    gview->scene()->clear();
    QString mainPhrase="Press any key to continue";
    qint32 xpos, ypos;

    // Chaging the current target point to escape point
    QGraphicsSimpleTextItem *phraseToShow = gview->scene()->addSimpleText(mainPhrase,QFont());
    phraseToShow->setPen(QPen(Qt::black));
    phraseToShow->setBrush(QBrush(Qt::black));
    xpos = (this->geometry().width() - phraseToShow->boundingRect().width())/2;
    ypos = (this->geometry().height() - phraseToShow->boundingRect().height())/2;
    phraseToShow->setPos(xpos,ypos);

    // Nothing more to do.
    return;

}


void FieldingExperiment::onTimeOut(){
    timerCounter--;
    if (timerCounter == 0) nextState();
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
                timerCounter = -1;
                state = STATE_PAUSED;
                drawPauseImage();
                emit(updateBackground(m->getImage()));
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

    emit(updateBackground(m->getImage()));
}

bool FieldingExperiment::finalizeExperiment(){
    if (currentTrial < m->size()) return false;
    stateTimer->stop();
    state = STATE_STOPPED;
    if (error.isEmpty()) emit(experimentEndend(ER_NORMAL));
    else emit(experimentEndend(ER_WARNING));
    return true;
}

void FieldingExperiment::togglePauseExperiment(){
    // Pause is not managed here, for this experiment.
}

void FieldingExperiment::keyPressEvent(QKeyEvent *event){
    // Making sure the experiment can be aborted, but any other key is ignored.
    if (event->key() == Qt::Key_Escape){
        stateTimer->stop();
        state = STATE_STOPPED;
        emit(experimentEndend(ER_ABORTED));
    }
	else{
        if (state == STATE_PAUSED){
            state = STATE_RUNNING;
            gview->scene()->clear();
            m->drawBackground();
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

void FieldingExperiment::addTrialHeader(){

    ignoreData = true;

    QFile file(dataFile);
    if (!file.open(QFile::Append)){
        error = "Could not open data file " + dataFile + " for appending data.";
        state = STATE_STOPPED;
        emit(experimentEndend(ER_FAILURE));
    }

    QTextStream writer(&file);
    writer.setCodec(COMMON_TEXT_CODEC);

    QPoint currentTarget = m->getTargetPoint(currentTrial,currentImage);

    qint32 imageInTrialID = currentImageToImageIndex();

    // Saving the trial id, the image number and the center location of the target.
    writer << m->getTrial(currentTrial).id << " "
           << imageInTrialID << " "
           << currentTarget.x() + TARGET_R + TARGET_OFFSET_X << " "
           << currentTarget.y() + TARGET_R + TARGET_OFFSET_Y << "\n";

    file.close();

    // Saving the current image.

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
