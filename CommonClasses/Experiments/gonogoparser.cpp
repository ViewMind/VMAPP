#include "gonogoparser.h"

GoNoGoParser::GoNoGoParser()
{

}

QList<GoNoGoParser::Trial> GoNoGoParser::getTrials() const {
    return trials;
}

QString GoNoGoParser::getError() const{
    return error;
}

QString GoNoGoParser::getVersionString() const{
    return versionString;
}

QList<QRectF> GoNoGoParser::getTargetBoxes() const{
    return targetBoxes;
}

QRectF GoNoGoParser::getArrowTargetBox() const{
    return arrowTargetBox;
}

QList<qint32> GoNoGoParser::getCorrectAnswerArray() const{
    return answerArray;
}

bool GoNoGoParser::parseGoNoGoExperiment(const QString &contents, const qreal &resolutionWidth, const qreal &resolutionHeight){


    QStringList lines = contents.split('\n',QString::SkipEmptyParts);
    if (lines.isEmpty()){
        error = "No lines in experiment description";
        return false;
    }
    trials.clear();

    // The first line should be the version string.
    versionString = lines.first();
    versionString = versionString.trimmed();

    // Making sure the name is unique.
    QSet<QString> uniqueIDs;
    QSet<int> test;

    // Possible trial type descriptions
    QStringList trialTypes;
    trialTypes << "R<-" << "G<-" << "R->" <<  "G->";

//    leftTarget->setPos(ScreenResolutionWidth*GONOGO_SIDE_MARGIN,centerY-target_radious);
//    rightTarget->setPos(ScreenResolutionWidth*(1-GONOGO_SIDE_MARGIN)-diameter,centerY-target_radious);

    // Computing target boxes
    qreal diameter = (resolutionWidth*GONOGO_TARGET_TOL);
    qreal targetDiamteter = (resolutionWidth*GONOGO_TARGET_SIZE);

    // The offset is computed to center the target box with the actual target.
    qreal xOffset = (diameter - targetDiamteter)/2;

    qreal r = diameter/2;
    qreal cY = resolutionHeight/2;
    QRectF leftTarget(resolutionWidth*GONOGO_SIDE_MARGIN - xOffset,cY-r,diameter,diameter);
    QRectF rightTarget(resolutionWidth*(1-GONOGO_SIDE_MARGIN) - diameter + xOffset,cY-r,diameter,diameter);
    targetBoxes.clear();
    targetBoxes << leftTarget << rightTarget;

    // Computing the arrow target box.
    qreal centerX = resolutionWidth/2;
    qreal centerY = resolutionHeight/2;
    qreal line_length = resolutionWidth*GONOGO_CROSS_LINE_LENGTH;
    qreal indicator_line_length = GONOGO_INDICATOR_LINE_LENGTH*line_length;

    // Arrow x coordinates of the main trunk
    qreal left_x0 = centerX-line_length/2;
    qreal right_x0 = centerX+line_length/2;

    // Offset to build indicators (45 degree line of the arrow ends)
    qreal ka = qSqrt(2)*indicator_line_length/2;

    // Using information above to actually compute the arrow target box.
    qreal arrowWidth = (right_x0 - left_x0);
    qreal arrowHeight = 2*ka;
    qreal arrowTargetBoxW = arrowWidth*GONOGO_ARROW_TARGET_BOX_WMARGIN;
    qreal arrowTargetBoxH = arrowHeight*GONOGO_ARROW_TARGET_BOX_HMARGIN;

    //qDebug() << arrowTargetBoxH << arrowHeight;

    arrowTargetBox = QRectF(left_x0 - ((arrowTargetBoxW - arrowWidth)/2),
                            centerY - ka - ((arrowTargetBoxH - arrowHeight)/2),
                            arrowTargetBoxW,
                            arrowTargetBoxH);

    answerArray.clear();
    answerArray << 1;  // Answer to Red Left is to Look Right.
    answerArray << 0;  // Answer to Green Left is to Look Left.
    answerArray << 0;  // Answer to Red Right is to Look Left.
    answerArray << 1;  // Answer to Green Right is to Look Right.


    for (qint32 i = 1; i < lines.size(); i++){
        QStringList tokens = lines.at(i).split(" ",QString::SkipEmptyParts);
        if (tokens.size() != 2){
            error = "Invalid number of tokens in line " + lines.at(i) + " it should only have 2 parts";
            trials.clear();
            return false;
        }

        QString name = tokens.first().trimmed();
        QString trialDesc = tokens.last().trimmed();

        if (uniqueIDs.contains(name)){
            error = "Repeated trial ID " + name;
            trials.clear();
            return false;
        }

        Trial t;
        t.id = name;

        qint32 trialTypeID = trialTypes.indexOf(trialDesc);
        if (trialTypeID == -1){
            error = "Unrecognized trial type " + trialDesc + " in line " + lines.at(i);
            trials.clear();
            return false;
        }

        t.type = trialTypeID;

        trials << t;
    }

    return true;

}
