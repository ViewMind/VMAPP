#include "passballexperiment.h"

const QString PassBallExperiment::MSG_PASSES_COMPLETED      = "studystatus_passball_completed";
const QString PassBallExperiment::MSG_PASSES_MISSED         = "studystatus_passball_missed";

PassBallExperiment::PassBallExperiment(QObject *parent, const QString &study_type) : Experiment(parent,study_type)
{
    manager = new PassBallManager();
    m = static_cast<PassBallManager*>(manager);
    this->studyType = studyType;
    this->isStudy3D = true;

}


bool PassBallExperiment::startExperiment(const QString &workingDir, const QString &experimentFile, const QVariantMap &studyConfig){

    if (!Experiment::startExperiment(workingDir,experimentFile,studyConfig)){
        emit Experiment::experimentEndend(ER_FAILURE);
        return false;
    }

    // Starting the study means sending the description packet.
    QVariantMap map = m->getPBParser().getStudyDescriptionPayloadContent();

    // Which hands have been enabled come form the study configuration. However, the speeds are for now constnats of this class.
    QVariantList hands;
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

    map[PassBallParser::HAND_CONTROL] = hands;

    // The base setup is done by the start experiment. We just fill in the particulars here: the study description and the command argument with the name.
    rrsControlPacket.setPayloadField(Packet3DStudyControl::COMMAND_ARG, Study3DNames::PASS_BALL);
    rrsControlPacket.setPayloadField(Packet3DStudyControl::DESCRIPTION, map);

//    qDebug() << "Pass Ball Map Being Sent to RRS";
//    Debug::prettpPrintQVariantMap(map);

    study3DState = S3S_EXPECTING_STUDY_DESCRIPTION;
    studyPhase = SP_EXPLANATION;

    StaticThreadLogger::log("PassBallExperiment::startExperiment","Sending request study explanation packet");
    emit Experiment::remoteRenderServerPacketAvailable();

    return true;


}

void PassBallExperiment::keyPressHandler(int keyPressed){
    // Making sure the experiment can be aborted, but any other key is ignored.
    if (keyPressed == Qt::Key_Escape){
        //experimenteAborted();
        StaticThreadLogger::error("PassBallExperiment::keyPressHandler","Aborting 3D Studies is not yet implemented");
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

void PassBallExperiment::newDataForStudyMessageFor3DStudies(const QVariantList &msg){

    if (msg.size() != 2){
        StaticThreadLogger::error("PassBallExperiment::newDataForStudyMessageFor3DStudies","Message for Pass Ball 3D Study is a list of size: " + QString::number(msg.size()));
        return;
    }

    studyMessages.clear();
    studyMessages[MSG_PASSES_COMPLETED] = msg.at(0);
    studyMessages[MSG_PASSES_MISSED] = msg.at(1);

    ///qDebug() << "STUDY MESSAGES" << studyMessages;

    emit Experiment::updateStudyMessages(studyMessages);

}

QString PassBallExperiment::getExperimentDescriptionFile(const QVariantMap &studyConfig){
    Q_UNUSED(studyConfig)
    return "";
}

