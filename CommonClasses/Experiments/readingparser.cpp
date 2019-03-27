#include "readingparser.h"

ReadingParser::ReadingParser()
{

}

bool ReadingParser::parseReadingDescription(const QString &contents){

    // If a version string is present it is in the very first line

    // Getting the description if available.
    QString experimentData = contents;
    qint32 commentToken = contents.indexOf(READING_COMMENT_STRING);

    if (commentToken != -1){
        qint32 end = commentToken+QString(READING_COMMENT_STRING).length();
        description = contents.mid(0,commentToken);
        experimentData = experimentData.mid(end+1);
    }

    // Generating the contents from the phrases
    QStringList lines = experimentData.split('\n',QString::SkipEmptyParts);

    // Checking if the first line is a version string with the presence of ":".
    qint32 startI = 0;
    if (!lines.first().contains(":")){
        startI = 1;
        versionString = lines.first();
    }

    for (qint32 i = startI; i < lines.size(); i++){

        QString line = lines.at(i);

        line = line.trimmed();
        if (line.isEmpty()) continue;

        Phrase p;

        QStringList idAndQuestion = line.split(':',QString::SkipEmptyParts);
        if (idAndQuestion.size() != 2){
            error = "There should only be 1 instance of : in the question " + line + " @ " + QString::number(i+1);
            return false;
        }

        bool ok;
        p.setID(idAndQuestion.first().toInt(&ok));
        QString sid = idAndQuestion.first();
        while (sid.length() < PAD_LENGTH_FOR_ID) sid = "0" + sid;
        p.setStringID(sid);
        if (!ok){
            error = "Invalid id (it is not an integer) " + idAndQuestion.first() + " @ " + QString::number(i+1);
            return false;
        }

        QStringList qAndA = idAndQuestion.last().split("|",QString::SkipEmptyParts);
        p.setPhrase(qAndA.first().trimmed());

        if (qAndA.size() == 2){
            QStringList options = qAndA.last().split(',',QString::SkipEmptyParts);
            for (qint32 j = 0; j < options.size(); j++){
                p.appendToFollowUp(options.at(j).trimmed());
            }
        }
        else if (qAndA.size() != 1){
            error = "There should only at most 1 instance of | in the question " + line + " @ " + QString::number(i+1);
            return false;
        }

        phrases << p;
    }

    return true;
}

//************************************* PHRASE FUNCTIONS ************************************

ReadingParser::Phrase::Phrase(){

}

void ReadingParser::Phrase::setPhrase(const QString &p){
    phrase = p;
    // Generating the space indexes
    qint32 sindex = phrase.indexOf(' ');
    while (sindex != -1){
        spaceIndexes << sindex;
        sindex = phrase.indexOf(' ',sindex+1);
    }
}

QString ReadingParser::Phrase::zeroPadID() const{
    QString imageID = QString::number(id);
    imageID = QString().fill('0',PAD_LENGTH_FOR_ID-imageID.size()) + imageID;
    return imageID;
}

QString ReadingParser::Phrase::toString() const {
    QString ans;
    ans = zeroPadID() + ":" + phrase + "| ";
    ans = ans + followUpQuestion.join(", ");
    return ans;
}

qint32 ReadingParser::Phrase::wordIndexForCharacterIndex(qint32 ci) const{

    if (spaceIndexes.isEmpty()) return 0;

    for (qint32 i = 0; i < spaceIndexes.size(); i++){
        if (ci <= spaceIndexes.at(i)){
            return i;
        }
    }

    return spaceIndexes.size()+1;
}

QString ReadingParser::Phrase::getIthWord(qint32 i) const{
    QStringList words = phrase.split(' ');
    if (i > words.size()-1) return "";
    return words.at(i);
}
