#include "phrases.h"

Phrase::Phrase(){

}

void Phrase::setPhrase(const QString &p){
    phrase = p;
    // Generating the space indexes
    qint32 sindex = phrase.indexOf(' ');
    while (sindex != -1){
        spaceIndexes << sindex;
        sindex = phrase.indexOf(' ',sindex+1);
    }
}

QString Phrase::zeroPadID() const{
    QString imageID = QString::number(id);
    imageID = QString().fill('0',PAD_LENGTH_FOR_ID-imageID.size()) + imageID;
    return imageID;
}

QString Phrase::toString() const {
    QString ans;
    ans = zeroPadID() + ":" + phrase + "| ";
    ans = ans + followUpQuestion.join(", ");
    return ans;
}

qint32 Phrase::wordIndexForCharacterIndex(qint32 ci) const{

    if (spaceIndexes.isEmpty()) return 0;

    for (qint32 i = 0; i < spaceIndexes.size(); i++){
        if (ci <= spaceIndexes.at(i)){
            return i;
        }
    }

    return spaceIndexes.size()+1;
}

QString Phrase::getIthWord(qint32 i) const{
    QStringList words = phrase.split(' ');
    if (i > words.size()-1) return "";
    return words.at(i);
}
