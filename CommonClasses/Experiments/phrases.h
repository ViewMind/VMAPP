#ifndef PHRASES_H
#define PHRASES_H

#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <QDebug>

#define   PAD_LENGTH_FOR_ID                             4

class Phrase{

public:

    Phrase();

    bool hasQuestion() const {return !followUpQuestion.isEmpty();}

    void setID(qint32 ID) {id = ID;}
    void setPhrase(const QString &p);
    void appendToFollowUp(const QString &s) {followUpQuestion << s;}

    // Accessing and convenience functions
    qint32 getID() const {return id;}
    qint32 getSizeInWords() const {return spaceIndexes.size()+1;}
    QString getPhrase() const {return phrase;}
    QString getFollowUpQuestion() const {return followUpQuestion.first();}
    QString getFollowUpAt(qint32 i) const {return followUpQuestion.at(i);}
    QString zeroPadID() const;
    QString toString() const;
    qint32 wordIndexForCharacterIndex(qint32 ci) const;
    QString getIthWord(qint32 i) const;
    qint32 getNofOptions() const {return followUpQuestion.size()-1;}
    qint32 size() const {return followUpQuestion.size();}

private:
    QString phrase;
    qint32 id;
    QStringList followUpQuestion;
    QList<qint32> spaceIndexes;
};

typedef QList<Phrase> Phrases;


#endif // PHRASES_H
