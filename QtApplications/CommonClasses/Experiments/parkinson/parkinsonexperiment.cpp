#include "parkinsonexperiment.h"

ParkinsonExperiment::ParkinsonExperiment(QWidget *parent, const QString &study_type):Experiment(parent,study_type)
{
    manager = new ParkinsonManager();
    m = dynamic_cast<ParkinsonManager*>(manager);
    skipEyeData = true;
    connect(&timer,&QTimer::timeout,this,&ParkinsonExperiment::on_timeOut);

}


bool ParkinsonExperiment::startExperiment(const QString &workingDir, const QString &experimentFile,
                                          const QVariantMap &studyConfig, bool useMouse){

    if (!Experiment::startExperiment(workingDir,experimentFile,studyConfig,useMouse)) return false;
    return false;

//    // Setting the first question.
//    // This window is shown and given focus.
//    this->show();

//    // The current state is running.
//    state = STATE_RUNNING;
//    if (debugMode){
//        emit(updateBackground(m->getImage()));
//    }

//    timer.setInterval(PARKINSON_TIMEOUT);

//    m->drawCurrentMaze();
//    newMaze();

//    if (!vrEnabled){
//        this->show();
//        this->activateWindow();
//    }
//    else updateSecondMonitorORHMD();

//    return true;
}

void ParkinsonExperiment::togglePauseExperiment(){
}

void ParkinsonExperiment::newMaze(){
    // Adding the maze title to the ET data.

//    QList<ParkinsonParser::ParkinsonTarget> targets = m->getCurrentMaze().targets;

//    QStringList titleParts;
//    titleParts << m->getCurrentMaze().name;
//    for (qint32 i = 0; i < targets.size(); i++){
//        titleParts << QString::number(targets.at(i).x) + " " + QString::number(targets.at(i).y);
//    }

//    QVariantList dataS;
//    dataS << titleParts.join(" | ");
//    etData << QVariant(dataS);

//    // Calculating the escape coordinates.
//    xEnd = targets.last().x;
//    yEnd = targets.last().y;

//    // Enabling time out and data saving.
//    timer.start();
//    skipEyeData = false;
}

void ParkinsonExperiment::on_timeOut(){

//    timer.stop();
//    skipEyeData = true;
//    if (!m->drawCurrentMaze()){
//        state = STATE_STOPPED;
//        if (!saveDataToHardDisk()) emit(experimentEndend(ER_FAILURE));
//        else emit(experimentEndend(ER_NORMAL));
//    }
//    else{
//        newMaze();
//        updateSecondMonitorORHMD();
//    }
}

void ParkinsonExperiment::newEyeDataAvailable(const EyeTrackerData &data){
    Experiment::newEyeDataAvailable(data);

//    // Nothing should be done if the state is NOT running.
//    if (state != STATE_RUNNING) return;

//    if (skipEyeData) return;

//    if (data.isLeftZero() && data.isRightZero()) return;

//    // Format: Image ID, time stamp for right and left, word index, character index, sentence length and pupil diameter for left and right eye.
//    QVariantList dataS;
//    dataS  << data.time
//           << data.xRight
//           << data.yRight
//           << data.xLeft
//           << data.yLeft
//           << data.pdRight
//           << data.pdLeft;
//    etData << QVariant(dataS);

//    // Checking if the target exit was reached.
//    qreal diffXr = qAbs(data.xRight - xEnd);
//    qreal diffXl = qAbs(data.xLeft - xEnd);
//    qreal diffYr = qAbs(data.yRight - yEnd);
//    qreal diffYl = qAbs(data.yLeft - yEnd);
//    qreal tolerance = 20;

//    if ( ( (diffXl <= tolerance ) && (diffYl <= tolerance ) ) ||
//         ( (diffXr <= tolerance ) && (diffYr <= tolerance ) ) ){
//        on_timeOut();
//    }

}

void ParkinsonExperiment::keyPressEvent(QKeyEvent *event){
    Q_UNUSED(event)
//    if (event->key() == Qt::Key_Escape){
//        experimenteAborted();
//        return;
//    }
}

QString ParkinsonExperiment::getExperimentDescriptionFile(const QVariantMap &studyConfig){
    Q_UNUSED(studyConfig)
    return ":/parkinson/descriptions/parkinson.dat";
}

ParkinsonExperiment::~ParkinsonExperiment(){
}
