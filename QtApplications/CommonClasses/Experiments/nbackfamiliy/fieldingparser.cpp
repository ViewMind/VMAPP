#include "fieldingparser.h"

//#ifdef EYETRACKER_HTCVIVEPRO
//const qreal FieldingParser::RECT_WIDTH =                               203.75; //163/4;
//const qreal FieldingParser::RECT_HEIGHT =                              193.75; //155/4;

//const qreal FieldingParser::K_HORIZONAL_MARGIN =                         0.06;
//const qreal FieldingParser::K_SPACE_BETWEEN_BOXES =                      0.09;
//const qreal FieldingParser::K_VERTICAL_MARGIN =                          0.06;

//const qreal FieldingParser::TARGET_BOX_EX_W =                            0.25;  //1.5/6.0;
//const qreal FieldingParser::TARGET_BOX_EX_H =                            0.268; //1.5/5.6;
//#endif

//#ifdef EYETRACKER_GAZEPOINT
//const qreal FieldingParser::RECT_WIDTH =                                163.0; //163/4;
//const qreal FieldingParser::RECT_HEIGHT =                               155.0; //155/4;

//const qreal FieldingParser::K_HORIZONAL_MARGIN =                         0.06;
//const qreal FieldingParser::K_SPACE_BETWEEN_BOXES =                      0.09;
//const qreal FieldingParser::K_VERTICAL_MARGIN =                          0.06;

//const qreal FieldingParser::TARGET_BOX_EX_W =                            0.25;  //1.5/6.0;
//const qreal FieldingParser::TARGET_BOX_EX_H =                            0.268; //1.5/5.6;
//#endif

const qreal FieldingParser::K_RECT_WIDTH =                               0.085;
const qreal FieldingParser::K_RECT_HEIGHT =                              0.143;

const qreal FieldingParser::K_HORIZONAL_MARGIN =                         0.06;
const qreal FieldingParser::K_SPACE_BETWEEN_BOXES =                      0.09;
const qreal FieldingParser::K_VERTICAL_MARGIN =                          0.06;

const qreal FieldingParser::TARGET_BOX_EX_W =                            0.25;  //1.5/6.0;
const qreal FieldingParser::TARGET_BOX_EX_H =                            0.268; //1.5/5.6;

FieldingParser::FieldingParser()
{

}

QString FieldingParser::getError() const{
    return error;
}

QList<FieldingParser::Trial> FieldingParser::getParsedTrials() const{
    return fieldingTrials;
}

QString FieldingParser::getVersionString() const{
    return versionString;
}

QList<QRectF> FieldingParser::getHitTargetBoxes() const{
    return hitTargetBoxes;
}

QList<QRectF> FieldingParser::getDrawTargetBoxes() const{
    return drawTargetBoxes;
}

qint32 FieldingParser::getTargetBoxForImageNumber(const QString &trialID, qint32 imgNum) const{
    qint32 ans = -1;
    for (qint32 i = 0; i < fieldingTrials.size(); i++){
        if (fieldingTrials.at(i).id == trialID){
            switch (imgNum) {
            case 1:
                ans = fieldingTrials.at(i).sequence.at(0);
                break;
            case 2:
                ans = fieldingTrials.at(i).sequence.at(1);
                break;
            case 3:
                ans = fieldingTrials.at(i).sequence.at(2);
                break;
            case 5:
                ans = fieldingTrials.at(i).sequence.at(2);
                break;
            case 6:
                ans = fieldingTrials.at(i).sequence.at(1);
                break;
            case 7:
                ans = fieldingTrials.at(i).sequence.at(0);
                break;
            default:
                break;
            }
            return ans;
        }
    }
    return ans;
}

bool FieldingParser::isHitInTargetBox(const QList<QRectF> &hitTargetBoxes, qint32 targetBox, qreal x, qreal y){
    if ((targetBox < 0) || (targetBox >= hitTargetBoxes.size())) return  false;
    //qDebug() << "CHECKING IF TARGET BOX" << targetBoxes.at(targetBox) << "contains" << x << y;

    // The only acurate cumputation of target hit will be done with boxes 5 and 2.
    if ((targetBox == TARGET_BOX_5) || (targetBox == TARGET_BOX_2)){
        return hitTargetBoxes.at(targetBox).contains(x,y);
    }

    // All other boxes get half a squre of leeway.
    if (hitTargetBoxes.at(targetBox).contains(x,y)) return true;

    qreal leeway = hitTargetBoxes.at(targetBox).height()/2;

    if (hitTargetBoxes.at(targetBox).contains(x,y+leeway)) return true;
    if (hitTargetBoxes.at(targetBox).contains(x,y-leeway)) return true;

    return false;
}


QList<qint32> FieldingParser::getSequenceForTrial(const QString &trialID){
    for (qint32 i = 0; i < fieldingTrials.size(); i++){
        if (fieldingTrials.at(i).id == trialID){
            return fieldingTrials.at(i).sequence;
        }
    }
    return QList<qint32>();
}

bool FieldingParser::parseFieldingExperiment(const QString &contents, qreal resolutionWidth, qreal resolutionHeight){

    // Generating the contents from the phrases
    QStringList lines = contents.split('\n',QString::KeepEmptyParts);

    fieldingTrials.clear();

    // Needed to check for unique ids.
    QSet<QString> uniqueIDs;

    // Checking the size of the first line to see if it is a version string.
    qint32 startI = 0;
    QString possibleHeader = lines.first();
    QStringList headerParts = possibleHeader.split(" ",QString::SkipEmptyParts);
    if (headerParts.size() == 1){
        // Version string pesent
        startI = 1;
        versionString = headerParts.first().trimmed();
    }

    qint32 sizeCheck = 0;
    if (versionString == "v1"){
        sizeCheck = OLD_SEQUENCE_LENGTH+1;
    }
    else if (versionString == "v2"){
        sizeCheck = MAX_SEQUENCE_LENGTH+1;
    }
    else{
        error = "Unknown fielding description version: " + versionString;
        return false;
    }


    for (qint32 i = startI; i < lines.size(); i++){

        if (lines.at(i).isEmpty()) continue;

        QStringList tokens = lines.at(i).split(' ',QString::SkipEmptyParts);


        if (tokens.size() != sizeCheck){
            error = "Invalid line: " + lines.at(i) + " should only have " + QString::number(sizeCheck) + " items separated by space, the id and "
                    + QString::number(MAX_SEQUENCE_LENGTH) +  " numbers. Version: " + versionString;
            return false;
        }

        Trial t;
        t.id = tokens.first();

        // If the id was defined before, its an error. Otherwise we add it to the ids found list.
        if (uniqueIDs.contains(t.id)){
            error = "Cannot use the same id twice: " + t.id + " has already been found";
            return false;
        }
        uniqueIDs << t.id;

        for (qint32 j =1; j < tokens.size(); j++){
            bool ok;
            qint32 value = tokens.at(j).toInt(&ok);
            if (!ok){
                error = "In line: " + lines.at(i) + ", " + tokens.at(j) + " is not a valid integer";
                return false;
            }
            if ((value < 0) || (value > 5)){
                error = "In line: " + lines.at(i) + ", " + tokens.at(j) + " is not an integer between 0 and 5";
                return false;
            }
            t.sequence << value;
        }

        fieldingTrials << t;
    }

    // Computing the drawn target box locations and the hit target boxes
    DrawingConstantsCalculator dcc;
    dcc.setTargetResolution(resolutionWidth,resolutionHeight);

    qreal targetBoxWidth = (resolutionWidth*dcc.getHorizontalRatio(K_RECT_WIDTH));
    qreal targetBoxHeight = (resolutionHeight*dcc.getVerticalRatio(K_RECT_HEIGHT));

    qreal x0, x1, x2, x3, x4, x5;
    qreal y0, y1, y2, y3, y4, y5;

    // Computing the boxes's coordinates.
    x5 = resolutionWidth*dcc.getHorizontalRatio(K_HORIZONAL_MARGIN);
    x2 = resolutionWidth*(1- dcc.getHorizontalRatio(K_HORIZONAL_MARGIN)) - targetBoxWidth;
    x0 = x5 + targetBoxWidth + resolutionWidth*dcc.getHorizontalRatio(K_SPACE_BETWEEN_BOXES);
    x4 = x0;
    x1 = x2 - targetBoxWidth - resolutionWidth*dcc.getHorizontalRatio(K_SPACE_BETWEEN_BOXES);
    x3 = x1;
    y2 = resolutionHeight/2.0 - targetBoxWidth/2.0;
    y5 = y2;
    y0 = resolutionHeight*dcc.getVerticalRatio(K_VERTICAL_MARGIN);
    y1 = y0;
    y3 = resolutionHeight*(1-dcc.getVerticalRatio(K_VERTICAL_MARGIN)) - targetBoxHeight;
    y4 = y3;



    // Rectangle origins in order, in order
    QList<QPoint> rectangleLocations;
    rectangleLocations.clear();
    rectangleLocations << QPoint(static_cast<qint32>(x0),static_cast<qint32>(y0));
    rectangleLocations << QPoint(static_cast<qint32>(x1),static_cast<qint32>(y1));
    rectangleLocations << QPoint(static_cast<qint32>(x2),static_cast<qint32>(y2));
    rectangleLocations << QPoint(static_cast<qint32>(x3),static_cast<qint32>(y3));
    rectangleLocations << QPoint(static_cast<qint32>(x4),static_cast<qint32>(y4));
    rectangleLocations << QPoint(static_cast<qint32>(x5),static_cast<qint32>(y5));


    // Adding the rectangles to the scene and computing the target boxes, including the error margin.
    qreal targetBoxIncreasedMarginWidth = targetBoxWidth*TARGET_BOX_EX_W;
    qreal targetBoxIncreasedMarginHeight = targetBoxHeight*TARGET_BOX_EX_H;
    qreal targetBoxWidthEX = targetBoxWidth + 2*targetBoxIncreasedMarginWidth;
    qreal targetBoxHeightEX = targetBoxHeight + 2*targetBoxIncreasedMarginHeight;

    for (qint32 i = 0; i < rectangleLocations.size(); i++){
        drawTargetBoxes << QRectF(rectangleLocations.at(i).x(),rectangleLocations.at(i).y(),targetBoxWidth,targetBoxHeight);
        //hitTargetBoxes << QRectF(rectangleLocations.at(i).x()-targetBoxIncreasedMarginWidth,rectangleLocations.at(i).y()-targetBoxIncreasedMarginHeight,targetBoxWidthEX,targetBoxHeightEX);
    }

    for (qint32 i = 0; i < rectangleLocations.size(); i++){
        qreal x = rectangleLocations.at(i).x()-targetBoxIncreasedMarginWidth;
        qreal y = rectangleLocations.at(i).y()-targetBoxIncreasedMarginHeight;
        qreal w = targetBoxWidthEX;
        qreal h = targetBoxHeightEX;

        /// PATCH: All boxes except 2 and 5 were to have the target box increased by half it's size.
        if ((i == TARGET_BOX_5) || (i == TARGET_BOX_2)){
            hitTargetBoxes << QRectF(x,y,w,h);
        }
        else{
            qreal leeway = h/2;
            h = 2*h;
            y = y - leeway;
            hitTargetBoxes << QRectF(x,y,w,h);
        }
    }


    return true;

}
