#include "perceptionparser.h"

PerceptionParser::PerceptionParser()
{

}


bool PerceptionParser::parsePerceptionExperiment(const QString &contents){
    trials.clear();

    QStringList lines = contents.split("\n",QString::SkipEmptyParts);
    qint32 gettingTrialLines = -1;

    // Very first line should be the version string
    versionString = lines.first();

    PerceptionTrial pt;

    for (qint32 i = 1; i < lines.size(); i++){
        QString line = lines.at(i);

        if (gettingTrialLines < 0){
            // This should be a header.
            QStringList header_tokens = line.split(" ",QString::SkipEmptyParts);
            if (header_tokens.size() != 4){
                error = "Parsing line " + line + ", expecting a header with 4 tokens, but got: " + QString::number(header_tokens.size()) + " instead.";
                return false;
            }
            if (!pt.trialDescription.isEmpty()) trials << pt;
            pt.name = header_tokens.at(0);
            pt.trialType = stringCharToTriangleType(header_tokens.at(1));
            pt.uniqueRow = header_tokens.at(2).toInt();
            pt.uniqueCol = header_tokens.at(3).toInt();
            pt.trialDescription.clear();
            gettingTrialLines = 4;
        }
        else{
            QList<TriangleType> row;
            QStringList desc = line.split(" ",QString::SkipEmptyParts);
            if (desc.size() != 5){
                error = "Parsing line " + line + ", was expecting trial description line, but number of tokes was wrong: " + QString::number(desc.size());
                return false;
            }
            for (qint32 j = 0; j < desc.size(); j++){
                TriangleType tt =  stringCharToTriangleType(desc.at(j));
                if (tt == TT_NONE){
                    error = "Unrecognized triangle type in " + line + " of type: " + desc.at(j);
                    return false;
                }
                if ( (tt == TT_X) && !((j == 2) && (gettingTrialLines == 2)) ){
                    error = "Found X on line " + line + " however the X is not in the middle row or it's not in the middle column";
                    return false;
                }
                row << tt;
            }
            gettingTrialLines--;
            pt.trialDescription << row;
        }

    }

    // If I don't do this the very last trial is not added.
    if (!pt.trialDescription.isEmpty()) trials << pt;

    return true;

}

QList<PerceptionParser::PerceptionTrial> PerceptionParser::getTrials() const{
    return trials;
}

QString PerceptionParser::getError() const {
    return error;
}

QString PerceptionParser::getVersionString() const {
    return versionString;
}

PerceptionParser::TriangleType PerceptionParser::stringCharToTriangleType(const QString &tt) const{
    if (tt == "U") return TT_UP;
    else if (tt == "D") return TT_DOWN;
    else if (tt == "L") return TT_LEFT;
    else if (tt == "R") return TT_RIGHT;
    else if (tt == "+") return TT_X;
    else return TT_NONE;
}

QString PerceptionParser::PerceptionTrial::toString() const {
    QString ans = name;
    switch(trialType){
    case PerceptionParser::TT_DOWN:
        ans = ans + " D ";
        break;
    case PerceptionParser::TT_UP:
        ans = ans + " U ";
        break;
    case PerceptionParser::TT_LEFT:
        ans = ans + " L ";
        break;
    case PerceptionParser::TT_RIGHT:
        ans = ans + " R ";
        break;
    case PerceptionParser::TT_NONE:
        ans = ans + " N ";
        break;
    default:
        ans = ans + " X ";
        break;
    }
    ans = ans + QString::number(uniqueRow) + " " + QString::number(uniqueCol);
    return ans;
}
