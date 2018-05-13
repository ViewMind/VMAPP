#include "readingexperiment.h"

ReadingExperiment::ReadingExperiment(QWidget *parent):Experiment(parent)
{
    manager = new ReadingManager();
    m = (ReadingManager*) manager;
    expHeader = HEADER_READING_EXPERIMENT;
    outputDataFile = FILE_OUTPUT_READING;
}

bool ReadingExperiment::startExperiment(ConfigurationManager *c){
    if (!Experiment::startExperiment(c)) return false;

    // Setting the first question.
    currentQuestion = 0;

    // Allways start drawing the point.
    qstate = QS_POINT;

    // This window is shown and given focus.
    this->show();

    // The current state is running.
    state = STATE_RUNNING;
    m->drawPhrase(qstate,currentQuestion);

    if (debugMode){
        emit(updateBackground(m->getImage()));
    }

    return true;

}

void ReadingExperiment::togglePauseExperiment(){
    Experiment::togglePauseExperiment();

    // Hiding the window, if the experiment was paused.
    if (state != STATE_RUNNING){
        this->hide();
    }
    else{
        this->show();
    }
}

void ReadingExperiment::newEyeDataAvailable(const EyeTrackerData &data){
    Experiment::newEyeDataAvailable(data);

    // Nothing should be done if the state is NOT running.
    if (state != STATE_RUNNING) return;

    // Determining what character the user is looking at.
    qint32 x,y;
    QList<qint32> indL, indR;
    if (data.isLeftZero()){
        x = data.xRight;
        y = data.yRight;
        indR = calculateWordAndCharacterPostion(x,y);
        indL << -1 << -1;
    }
    else if (data.isRightZero()){
        x = data.xLeft;
        y = data.yLeft;        
        indL = calculateWordAndCharacterPostion(x,y);
        indR << -1 << -1;
    }
    else{
        x = data.avgX();
        y = data.avgY();
        indL = calculateWordAndCharacterPostion(data.xLeft,data.yLeft);
        indR = calculateWordAndCharacterPostion(data.xRight,data.yRight);
    }

    // Data is ONLY saved when looking at a phrase.
    if (qstate == QS_PHRASE){
        appendDataToFile(data,indR.first(),indR.last(),indL.first(),indL.last(),
                         m->getPhrase(currentQuestion).getSizeInWords());
    }

    if (qstate == QS_QUESTION) return;

    if (isPointWithInTolerance(x,y)){
        advanceToTheNextPhrase();
    }

}

void ReadingExperiment::advanceToTheNextPhrase(){

    if ((qstate == QS_QUESTION) || (qstate == QS_PHRASE)){
        // I move on to the next question only I'm in a question or phrase qstate.
        if (currentQuestion < m->size()-1){
            currentQuestion++;
        }
        else{
            state = STATE_STOPPED;
            if (error.isEmpty()) emit(experimentEndend(ER_NORMAL));
            else emit (experimentEndend(ER_WARNING));
            return;
        }
    }

    // The state machine is advacned.
    determineQuestionState();

    // And the next phrase is drawn.
    m->drawPhrase(qstate,currentQuestion);
    if (debugMode){
        emit(updateBackground(m->getImage()));
    }
}

bool ReadingExperiment::isPointWithInTolerance(int x, int y){

    qint32 tol = config->getInt(CONFIG_READING_PX_TOL);
    if (qAbs(x-m->getCurrentTargetX()) < tol){
        if (qAbs(y-m->getCurrentTargetY()) < tol){
            return true;
        }
    }
    return false;
}

void ReadingExperiment::determineQuestionState(){

    switch (qstate){
    case QS_POINT:
        if (m->getPhrase(currentQuestion).hasQuestion()) qstate = QS_INFORMATION;
        else qstate = QS_PHRASE;
        break;
    case QS_PHRASE:
        qstate = QS_POINT;
        break;
    case QS_INFORMATION:
        qstate = QS_QUESTION;
        break;
    case QS_QUESTION:
        qstate = QS_POINT;
        break;
    }

}

void ReadingExperiment::mousePressEvent(QMouseEvent *event){

    // The logic here should only be run if this a question and the experiment is running.
    if ((state != STATE_RUNNING) || (qstate != QS_QUESTION)) return;

    if (event->button() == Qt::LeftButton){
        qint32 where = m->isPointContainedInAClickArea(event->pos());
        if (where != -1){
            // Saving the answer.
            saveAnswer(where+1);

            // Advancing to next phrase or information.
            advanceToTheNextPhrase();
        }
    }
}

void ReadingExperiment::keyPressEvent(QKeyEvent *event){
    Experiment::keyPressEvent(event);

    // In the question state, the advance must be with the mouse click.
    // Otherwise the space bar advances the question.
    if ((qstate != QS_QUESTION) && (event->key() == Qt::Key_Space)){
        advanceToTheNextPhrase();
    }
}

void ReadingExperiment::appendDataToFile(const EyeTrackerData &data,
                                         qint32 wordIndexR,
                                         qint32 characterIndexR,
                                         qint32 wordIndexL,
                                         qint32 characterIndexL,
                                         qint32 sentenceLength){

    if (data.isLeftZero() && data.isRightZero()) return;

    QFile file(dataFile);
    if (!file.open(QFile::Append)){
        error = "Could not open data file " + dataFile + " for appending data.";
        state = STATE_STOPPED;
        emit(experimentEndend(ER_FAILURE));
    }

    QTextStream writer(&file);
    writer.setCodec(COMMON_TEXT_CODEC);

    // Format: Image ID, time stamp for right and left, word index, character index, sentence length and pupil diameter for left and right eye.
    writer << m->getPhrase(currentQuestion).zeroPadID() << " "
           << data.time << " "
           << data.xRight << " "
           << data.yRight << " "
           << data.xLeft << " "
           << data.yLeft  << " "
           << wordIndexR+1 << " "
           << characterIndexR+1 << " "
           << wordIndexL+1 << " "
           << characterIndexL+1 << " "
           << sentenceLength << " "
           << data.pdRight << " "
           << data.pdLeft << "\n";
    file.close();
}

void ReadingExperiment::saveAnswer(qint32 selected){

    QFile file(dataFile);
    if (!file.open(QFile::Append)){
        error = "Could not open data file " + dataFile + " for saving the answer to a question.";
        state = STATE_STOPPED;
        emit(experimentEndend(ER_FAILURE));
    }

    QTextStream writer(&file);
    writer.setCodec(COMMON_TEXT_CODEC);

    // Format: Image ID, time stamp for right and left.
    Phrase p = m->getPhrase(currentQuestion);
    writer << p.zeroPadID() << " "
           << p.getFollowUpQuestion() << " -> "
           << p.getFollowUpAt(selected) << "\n";

    file.close();
}


QList<qint32> ReadingExperiment::calculateWordAndCharacterPostion(qint32 x, qint32 y){

    // Character and word position are determined ONLY through the x value. In case
    // the y value is taken into account in the future, it was also added as a parameter, although unused.
    Q_UNUSED(y);

    qint32 charIndex = m->getCharIndex(x);
    qint32 wordIndex = -1;
    QString currentWord = "";

    if (charIndex != -1){
        Phrase p = m->getPhrase(currentQuestion);
        wordIndex = p.wordIndexForCharacterIndex(charIndex);
        currentWord = p.getIthWord(wordIndex);
    }
    return QList<qint32>() << wordIndex << charIndex;
}
