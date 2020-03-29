#include "fieldingparser.h"

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


QList<qint32> FieldingParser::getSequenceForTrial(const QString &trialID){
    for (qint32 i = 0; i < fieldingTrials.size(); i++){
        if (fieldingTrials.at(i).id == trialID){
            return fieldingTrials.at(i).sequence;
        }
    }
    return QList<qint32>();
}

bool FieldingParser::parseFieldingExperiment(const QString &contents,
                                             qreal resolutionWidth, qreal resolutionHeight,
                                             qreal x_px_2_mm, qreal y_px_2_mm){

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
        versionString = headerParts.first();
    }

    for (qint32 i = startI; i < lines.size(); i++){

        if (lines.at(i).isEmpty()) continue;

        QStringList tokens = lines.at(i).split(' ',QString::SkipEmptyParts);
        if (tokens.size() != 4){
            error = "Invalid line: " + lines.at(i) + " should only have 4 items separated by space, the id and three numbers";
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
    qreal kx = x_px_2_mm;
    qreal ky = y_px_2_mm;

    qreal targetBoxWidth = (RECT_WIDTH/kx);
    qreal targetBoxHeight = (RECT_HEIGHT/ky);

    qreal x0, x1, x2, x3, x4, x5;
    qreal y0, y1, y2, y3, y4, y5;

    // Computing the boxes's coordinates.
    x5 = resolutionWidth*K_HORIZONAL_MARGIN;
    x2 = resolutionWidth*(1- K_HORIZONAL_MARGIN) - targetBoxWidth;
    x0 = x5 + targetBoxWidth + resolutionWidth*K_SPACE_BETWEEN_BOXES;
    x4 = x0;
    x1 = x2 - targetBoxWidth - resolutionWidth*K_SPACE_BETWEEN_BOXES;
    x3 = x1;
    y2 = resolutionHeight/2.0 - targetBoxWidth/2.0;
    y5 = y2;
    y0 = resolutionHeight*K_VERTICAL_MARGIN;
    y1 = y0;
    y3 = resolutionHeight*(1-K_VERTICAL_MARGIN) - targetBoxHeight;
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
        hitTargetBoxes << QRectF(rectangleLocations.at(i).x()-targetBoxIncreasedMarginWidth,rectangleLocations.at(i).y()-targetBoxIncreasedMarginHeight,
                              targetBoxWidthEX,targetBoxHeightEX);
    }

    return true;

}
