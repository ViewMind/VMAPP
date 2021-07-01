#include "readingexperiment.h"

const qreal  ReadingExperiment::K_READING_HIT_TOLERANCE = 0.042;
const qint32 ReadingExperiment::TIME_TO_HOLD_FOR_SELECTION_IN_SECONDS = 1;

ReadingExperiment::ReadingExperiment(QWidget *parent, const QString &study_type):Experiment(parent,study_type)
{
    manager = new ReadingManager();
    m = static_cast<ReadingManager*>(manager);
    this->studyType = VMDC::Study::READING;
}

bool ReadingExperiment::startExperiment(const QString &workingDir, const QString &experimentFile,
                                        const QVariantMap &studyConfig, bool useMouse){


    if (!Experiment::startExperiment(workingDir,experimentFile,studyConfig,useMouse)) return false;

    // Configure for VR or no values.
    QVariantMap config;
    config.insert(ReadingManager::CONFIG_IS_USING_VR,(Globals::EyeTracker::IS_VR && (!useMouse)));
    m->configure(config);

    // Computing the tolerance for the reading experiment.
    readingHitTolerance = processingParameters.value(VMDC::ProcessingParameter::RESOLUTION_WIDTH).toReal()*K_READING_HIT_TOLERANCE;

    eyeSelector.setTargetCountForSelection(processingParameters.value(VMDC::ProcessingParameter::SAMPLE_FREQUENCY).toInt()*TIME_TO_HOLD_FOR_SELECTION_IN_SECONDS);
    eyeSelector.setBehaviour(EyeSelector::EBS_CUMULATIVE);
    eyeSelector.reset();

    // Setting the first question.
    currentQuestion = 0;

    // Allways start drawing the point.
    qstate = ReadingManager::ReadingManager::QS_POINT;
    currentTrialID = "";

    // This window is shown and given focus.
    //qDebug() << useMouse << Globals::EyeTracker::IS_VR;
    if (!Globals::EyeTracker::IS_VR || (useMouse)){
        this->show();
        this->activateWindow();
    }

    // The current state is running.
    state = STATE_RUNNING;
    m->drawPhrase(qstate,currentQuestion);

    // We add the new trial and dataset (in reading they are the same thing)
    ReadingParser::Phrase phrase = m->getPhrase(currentQuestion);
    currentTrialID = QString::number(phrase.getID());
    if (!rawdata.addNewTrial(currentTrialID,phrase.getPhrase(),phrase.getRightAns())){
        error = "Failed to add new trial " + currentTrialID + ": " + rawdata.getError();
        emit(experimentEndend(ER_FAILURE));
        return false;
    }

    // Now we set the unique dataset.
    rawdata.setCurrentDataSet(VMDC::DataSetType::UNIQUE);

    updateSecondMonitorORHMD();

    return true;

}

void ReadingExperiment::newEyeDataAvailable(const EyeTrackerData &data){

    // Nothing should be done if the state is NOT running.
    if (state != STATE_RUNNING) return;

    Experiment::newEyeDataAvailable(data);

    //qDebug() << "READING DATA: " << data.toString();

    // Determining what character the user is looking at.
    qint32 x,y;
    QList<qint32> indL, indR;
    qreal rx, ry, lx, ly;
    if (data.isLeftZero()){
        x = data.xRight;
        y = data.yRight;
        indR = calculateWordAndCharacterPostion(x,y);
        indL << -1 << -1;
        lx = 0; ly = 0;
        rx = x; ry = y;
    }
    else if (data.isRightZero()){
        x = data.xLeft;
        y = data.yLeft;
        indL = calculateWordAndCharacterPostion(x,y);
        indR << -1 << -1;
        lx = x; ly = y;
        rx = 0; ry = 0;
    }
    else{
        x = static_cast<qint32>(data.avgX());
        y = static_cast<qint32>(data.avgY());
        indL = calculateWordAndCharacterPostion(data.xLeft,data.yLeft);
        indR = calculateWordAndCharacterPostion(data.xRight,data.yRight);
        lx = data.xLeft; ly = data.yLeft;
        rx = data.xRight; ry = data.yRight;
    }

    // Data is ONLY saved when looking at a phrase.
    if ((qstate == ReadingManager::QS_PHRASE) || (qstate == ReadingManager::QS_INFORMATION)){
        appendEyeTrackerData(data,indR.first(),indR.last(),indL.first(),indL.last());
    }

    // On a question a test needs to be done to know if the option has been selected with the gaze.
    response = "";
    if (qstate == ReadingManager::QS_QUESTION){
        m->highlightOption(QPoint(x,y));
        qint32 selected = eyeSelector.isSelectionMade(x,y);
        if (selected != -1){

            // Saving the answer.
            ReadingParser::Phrase p = m->getPhrase(currentQuestion);
            response = p.getFollowUpAt(selected+1);

            // Advancing to next phrase or information.
            advanceToTheNextPhrase();
        }
        return;
    }

    if (isPointWithInTolerance(x,y)){
        advanceToTheNextPhrase();
    }
}

void ReadingExperiment::advanceToTheNextPhrase(){

    if ((qstate == ReadingManager::QS_QUESTION) || (qstate == ReadingManager::QS_PHRASE)){

        // Reset the selection state machine.
        eyeSelector.reset();

        // Finalize both the data set and the trial.
        rawdata.finalizeDataSet();
        rawdata.finalizeTrial(response);
        finalizeOnlineFixations();

        // I move on to the next question only I'm in a question or phrase qstate.        
        if (currentQuestion < m->size()-1){
            currentQuestion++;

            // We just finished a question so now we advance.
            ReadingParser::Phrase phrase = m->getPhrase(currentQuestion);
            currentTrialID = QString::number(phrase.getID());

            if (!rawdata.addNewTrial(currentTrialID,phrase.getPhrase(),phrase.getRightAns())){
                error = "Failed to add new trial " + currentTrialID + ": " + rawdata.getError();
                emit(experimentEndend(ER_FAILURE));
                return;
            }


        }
        else{
            state = STATE_STOPPED;

            // Finalizing the study and marking it as finalized. Reading has no other parts.
            if (!rawdata.finalizeStudy()){
                error = "Failed on Reading study finalization: " + rawdata.getError();
                emit (experimentEndend(ER_FAILURE));
                return;
            }
            rawdata.markFileAsFinalized();

            if (!saveDataToHardDisk()){
                emit (experimentEndend(ER_FAILURE));
                return;
            }
            if (error.isEmpty()) emit(experimentEndend(ER_NORMAL));
            else emit (experimentEndend(ER_WARNING));
            return;
        }
    }

    // The state machine is advacned.
    determineQuestionState();

    // And the next phrase is drawn.
    m->drawPhrase(qstate,currentQuestion);
    if (qstate == ReadingManager::QS_QUESTION){
        eyeSelector.setTargetBoxes(m->getOptionTargetBoxes());
    }

    updateSecondMonitorORHMD();
}

bool ReadingExperiment::isPointWithInTolerance(int x, int y){

    if (qAbs(x-m->getCurrentTargetX()) < readingHitTolerance){
        if (qAbs(y-m->getCurrentTargetY()) < readingHitTolerance){
            return true;
        }
    }
    return false;
}

void ReadingExperiment::determineQuestionState(){

    switch (qstate){
    case ReadingManager::QS_POINT:
        if (m->getPhrase(currentQuestion).hasQuestion()) qstate = ReadingManager::QS_INFORMATION;
        else qstate = ReadingManager::QS_PHRASE;
        break;
    case ReadingManager::QS_PHRASE:
        qstate = ReadingManager::QS_POINT;
        break;
    case ReadingManager::QS_INFORMATION:
        qstate = ReadingManager::QS_QUESTION;
        break;
    case ReadingManager::QS_QUESTION:
        qstate = ReadingManager::QS_POINT;
        break;
    }

}

void ReadingExperiment::keyPressHandler(int keyPressed){

    if (keyPressed == Qt::Key_Escape){
        experimenteAborted();
        return;
    }

    // In the question state, the advance must be with the mouse click.
    // Otherwise the space bar advances the question.
    if ((qstate != ReadingManager::QS_QUESTION) && (keyPressed == Qt::Key_N)){
        advanceToTheNextPhrase();
    }

}

QString ReadingExperiment::getExperimentDescriptionFile(const QVariantMap &studyConfig){
    QString lang_code = "";
    if (studyConfig.value(VMDC::StudyParameter::LANGUAGE).toString() == VMDC::ReadingLanguage::GERMAN){
        lang_code = "de";
    }
    else if (studyConfig.value(VMDC::StudyParameter::LANGUAGE).toString() == VMDC::ReadingLanguage::FRENCH){
        lang_code = "fr";
    }
    else if (studyConfig.value(VMDC::StudyParameter::LANGUAGE).toString() == VMDC::ReadingLanguage::SPANISH){
        lang_code = "es";
    }
    else if (studyConfig.value(VMDC::StudyParameter::LANGUAGE).toString() == VMDC::ReadingLanguage::ENGLISH){
        lang_code = "en";
    }
    else if (studyConfig.value(VMDC::StudyParameter::LANGUAGE).toString() == VMDC::ReadingLanguage::ISELANDIC){
        lang_code = "is";
    }

    return QString(":/reading/descriptions/Reading_") + lang_code + ".dat";

}

void ReadingExperiment::appendEyeTrackerData(const EyeTrackerData &data,
                                             qint32 wordIndexR,
                                             qint32 characterIndexR,
                                             qint32 wordIndexL,
                                             qint32 characterIndexL){

    if (data.isLeftZero() && data.isRightZero()) return;

    computeOnlineFixations(data,characterIndexL,wordIndexL,characterIndexR,wordIndexR);

    // Format: Image ID, time stamp for right and left, word index, character index, sentence length and pupil diameter for left and right eye.
    rawdata.addNewRawDataVector(ViewMindDataContainer::GenerateReadingRawDataVector(data.time,
                                                                               data.xRight,data.yRight ,data.xLeft,data.yLeft,
                                                                               data.pdRight,data.pdLeft,
                                                                               characterIndexR,characterIndexL,
                                                                               wordIndexR,wordIndexL));

}


QList<qint32> ReadingExperiment::calculateWordAndCharacterPostion(qint32 x, qint32 y){

    // Character and word position are determined ONLY through the x value. In case
    // the y value is taken into account in the future, it was also added as a parameter, although unused.
    Q_UNUSED(y)

    qint32 charIndex = m->getCharIndex(x);
    qint32 wordIndex = -1;
    QString currentWord = "";

    if (charIndex != -1){
        ReadingParser::Phrase p = m->getPhrase(currentQuestion);
        wordIndex = p.wordIndexForCharacterIndex(charIndex);
        currentWord = p.getIthWord(wordIndex);
    }
    return QList<qint32>() << wordIndex << charIndex;
}

ReadingExperiment::~ReadingExperiment(){
}


