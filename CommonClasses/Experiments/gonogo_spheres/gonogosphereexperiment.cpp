#include "gonogosphereexperiment.h"

const QString GoNoGoSphereExperiment::MSG_GREEN_HIT      = "studystatus_gonogo3D_green_hit";
const QString GoNoGoSphereExperiment::MSG_RED_HIT        = "studystatus_gonogo3D_red_hit";


GoNoGoSphereExperiment::GoNoGoSphereExperiment(QObject *parent, const QString &study_type) : Experiment(parent,study_type) {

    manager = new GoNoGoSpereManager();
    m = static_cast<GoNoGoSpereManager*>(manager);
    this->studyType = studyType;
    this->isStudy3D = true;

}

bool GoNoGoSphereExperiment::startExperiment(const QString &workingDir, const QString &experimentFile,
                                       const QVariantMap &studyConfig){

    if (!Experiment::startExperiment(workingDir,experimentFile,studyConfig)){
        emit Experiment::experimentEndend(ER_FAILURE);
        return false;
    }

    // Starting the study means sending the description packet.
    QVariantMap map = m->getGNGSphereParser().getStudyDescriptionPayloadContent();

    // Which hands have been enabled come form the study configuration. However, the speeds are for now constnats of this class.
    QVariantList speed, hands;
    QString handToUse = studyConfig.value(VMDC::StudyParameter::HAND_TO_USE).toString();

    if (handToUse == VMDC::Hand::BOTH){
        hands << true << true;
    }
    else if (handToUse == VMDC::Hand::LEFT){
        hands << true << false;
    }
    else if (handToUse == VMDC::Hand::RIGHT){
        hands << false << true;
    }
    else {
        this->error = "Invalid hand configuration: " + handToUse;
        emit Experiment::experimentEndend(ER_FAILURE);
        return false;
    }

    qreal min_speed = studyConfig.value(VMDC::StudyParameter::MIN_SPEED).toReal();
    qreal max_speed = studyConfig.value(VMDC::StudyParameter::MAX_SPEED).toReal();
    qreal initial_speed = studyConfig.value(VMDC::StudyParameter::INITIAL_SPEED).toReal();
    if (min_speed > max_speed){
        error = "Specified minimum speed (" + QString::number(min_speed) + ") is lower than the specified maxiumum speed (" + QString::number(max_speed) + ")";
        emit Experiment::experimentEndend(ER_FAILURE);
        return false;
    }
    if (initial_speed < min_speed){
        error = "Specified minimum speed (" + QString::number(min_speed) + ") is higher than the specified initial speed (" + QString::number(initial_speed) + ")";
        emit Experiment::experimentEndend(ER_FAILURE);
        return false;

    }
    if (initial_speed > max_speed){
        error = "Specified maximum speed (" + QString::number(max_speed) + ") is lower than the specified initial speed (" + QString::number(initial_speed) + ")";
        emit Experiment::experimentEndend(ER_FAILURE);
        return false;
    }

    speed << min_speed << max_speed << initial_speed;

    map[GoNoGoSphereParser::SPEED_LIMIT] = speed;
    map[GoNoGoSphereParser::HAND_CONTROL] = hands;

    // The base setup is done by the start experiment. We just fill in the particulars here: the study description and the command argument with the name.
    rrsControlPacket.setPayloadField(Packet3DStudyControl::COMMAND_ARG, Study3DNames::GO_NO_GO_SPHERE);
    rrsControlPacket.setPayloadField(Packet3DStudyControl::DESCRIPTION, map);

    study3DState = S3S_EXPECTING_STUDY_DESCRIPTION;
    studyPhase = SP_EXPLANATION;

    StaticThreadLogger::log("GoNoGoSphereExperiment::startExperiment","Sending request study explanation packet");
    emit Experiment::remoteRenderServerPacketAvailable();

    return true;
}

void GoNoGoSphereExperiment::keyPressHandler(int keyPressed){
    // Making sure the experiment can be aborted, but any other key is ignored.
    if (keyPressed == Qt::Key_Escape){
        //experimenteAborted();
        StaticThreadLogger::error("GoNoGoSphereExperiment::keyPressHandler","Aborting 3D Studies is not yet implemented");
        return;
    }
    else if (keyPressed == Qt::Key_N){
        if (studyPhase == SP_EXAMPLE){
            // Advances to the next example.
            rrsControlPacket.resetForRX();
            rrsControlPacket.setPacketType(RenderServerPacketType::TYPE_3DSTUDY_CONTROL);
            rrsControlPacket.setPayloadField(Packet3DStudyControl::COMMAND,Study3DControlCommands::CMD_NEXT_EXAMPLES);
            emit Experiment::remoteRenderServerPacketAvailable();
        }
        return;
    }
}

void GoNoGoSphereExperiment::newDataForStudyMessageFor3DStudies(const QVariantList &msg){

    if (msg.size() != 2){
        StaticThreadLogger::error("GoNoGoSphereExperiment::newDataForStudyMessageFor3DStudies","Message for Go No Go Sphere 3D Study is a list of size: " + QString::number(msg.size()));
        return;
    }

    studyMessages.clear();
    studyMessages[MSG_GREEN_HIT] = msg.at(0);
    studyMessages[MSG_RED_HIT] = msg.at(1);

    ///qDebug() << "STUDY MESSAGES" << studyMessages;

    emit Experiment::updateStudyMessages(studyMessages);

}

QString GoNoGoSphereExperiment::getExperimentDescriptionFile(const QVariantMap &studyConfig){
    Q_UNUSED(studyConfig)
    return ":/gonogo_spheres/descriptions/gonogo_spheres.dat";
}
