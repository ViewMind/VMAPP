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

bool FieldingParser::parseFieldingExperiment(const QString &contents){

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

    return true;

}
