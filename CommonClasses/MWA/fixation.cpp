#include "fixation.h"

const qreal Fixation::PUPIL_ZERO_TOL = 1e-6;

Fixation::Fixation()
{
    status = FIX_INVALID;
}

void Fixation::construct(const QList<DataPoint> &onlinePointsForFixation, bool is_done){

    x = 0;
    y = 0;
    pupil = 0;
    pupilZeroCount = 0;

    for (qint32 i = 0; i < onlinePointsForFixation.size(); i++){
        x = x + onlinePointsForFixation.at(i).x;
        y = y + onlinePointsForFixation.at(i).y;
        pupil = pupil + onlinePointsForFixation.at(i).pupil;

        if (qAbs(pupil) < PUPIL_ZERO_TOL){
            pupilZeroCount++;
        }
    }
    qreal size = static_cast<qreal>(onlinePointsForFixation.size());
    x = x/size;
    y = y/size;
    pupil = pupil/size;

    fixStart = onlinePointsForFixation.first().timestamp;
    fixEnd   = onlinePointsForFixation.last().timestamp;

    duration = fixEnd - fixStart;
    time = (fixStart + fixEnd)/2;

    // The parameter flag indicates whether the fixation started or finished.
    if (is_done){
        status = FIX_FINISHED;
    }
    else{
        status = FIX_STARTED;
    }

}

// Getting the status
bool Fixation::hasFinished() const {
    return status == FIX_FINISHED;
}

bool Fixation::hasStarted() const {
    return status == FIX_STARTED;
}

bool Fixation::isInvalid() const {
    return status == FIX_INVALID;
}

qreal Fixation::getX() const{
    return x;
}
qreal Fixation::getY() const{
    return y;
}
qreal Fixation::getDuration() const{
    return duration;
}
qreal Fixation::getStartTime() const{
    return fixStart;
}
qreal Fixation::getEndTime() const{
    return fixEnd;
}
qreal Fixation::getPupil() const{
    return pupil;
}
qreal Fixation::getTime() const{
    return time;
}
qreal Fixation::getBlinks() const{
    return pupilZeroCount;
}
qreal Fixation::getChar() const{
    return sentence_char;
}
qreal Fixation::getWord() const{
    return sentence_word;
}

// Checks if the fixation falls in a given range.
bool Fixation::isIn(qreal minX, qreal maxX, qreal minY, qreal maxY) const {
    bool ans = (minX <= x);
    ans = ans && (x <= maxX);
    ans = ans && (minY <= y);
    ans = ans && (y <= maxY);
    return ans;
}


QString Fixation::toString() const {
    return "(" + QString::number(x) + "," + QString::number(y) + "). "
            + QString::number(fixEnd) + " - " + QString::number(fixStart) + " = " + QString::number(duration);
}
