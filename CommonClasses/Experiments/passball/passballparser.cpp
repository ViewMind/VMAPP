#include "passballparser.h"

const QString PassBallParser::ORIGIN_TARGETS  = "origin_targets";
const QString PassBallParser::MIDDLE_TARGETS  = "middle_targets";
const QString PassBallParser::LAST_TARGETS    = "last_targets";
const QString PassBallParser::PASS_TARGETS    = "pass_targets";
const QString PassBallParser::HAND_CONTROL    = "hand_control";
const QString PassBallParser::SPEED_LIMIT     = "speed_limits";

PassBallParser::PassBallParser() {

}

bool PassBallParser::generatePassBallSequence(){

    study_description.clear();
    QVariantList origins;
    QVariantList middles;
    QVariantList finals;
    QVariantList pass_targets;

    error = "";


    QList< QList<qint32> > sequence = BallPathGenerator::GenerateBallPathSequence(NROWS,NCOLS,
                                                                                  TIMES_PER_HOLE,N_PASSES,
                                                                                  MIN_DISTANCE,MEAN_DISTANCE);

    for (qint32 i = 0; i < sequence.size(); i++){

        if (sequence.at(i).size() != 4){
            error = "Sequece " + QString::number(i) + " of the ball path generator conatined " + QString::number(sequence.size()) + " values instead of 4";
            return false;
        }

        origins << sequence.at(i).at(0);
        middles << sequence.at(i).at(1);
        finals << sequence.at(i).at(2);
        pass_targets << sequence.at(i).at(3);
    }

    study_description[PassBallParser::ORIGIN_TARGETS] = origins;
    study_description[PassBallParser::MIDDLE_TARGETS] = middles;
    study_description[PassBallParser::LAST_TARGETS] = finals;
    study_description[PassBallParser::PASS_TARGETS] = pass_targets;

    return true;

}

QVariantMap PassBallParser::getStudyDescriptionPayloadContent() const {
    return study_description;
}

qint32 PassBallParser::getSize() const {
    return static_cast<qint32>(study_description.value(PassBallParser::ORIGIN_TARGETS).toList().size());
}

QString PassBallParser::getError() const {
    return error;
}
