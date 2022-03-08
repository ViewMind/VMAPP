#ifndef READINGPARSER_H
#define READINGPARSER_H

#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include "../../eyetracker_defines.h"

class ReadingParser
{
public:

    class Phrase{

    public:

        Phrase();

        bool hasQuestion() const {return !followUpQuestion.isEmpty();}

        void setID(qint32 ID) {id = ID;}
        void setStringID(const QString &sid) { stringID = sid; }
        void setPhrase(const QString &p);
        void appendToFollowUp(const QString &s, bool isRightAns);

        // Accessing and convenience functions
        qint32 getID() const {return id;}
        QString getStringID() const {return stringID;}
        qint32 getSizeInWords() const {return spaceIndexes.size()+1;}
        QString getPhrase() const {return phrase;}
        QString getRightAns() const {return rightAnswer;}
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
        QString stringID;
        QString rightAnswer;
        QStringList followUpQuestion;
        QList<qint32> spaceIndexes;
    };

    typedef QList<Phrase> Phrases;

    ReadingParser();
    bool parseReadingDescription(const QString &contents);

    // Getting the results;
    QString getDescription() const {return description;}
    QString getVersion() const {return versionString;}
    QString getError() const {return error;}
    Phrases getPhrases() const {return phrases; }
    QList<QStringList> getExpectedIDs() const { return expectedIDs; }

private:
    QList<QStringList> expectedIDs;
    QString description;
    QString versionString;
    Phrases phrases;
    QString error;

    static const char * READING_COMMENT_STRING;
    static const qint32 PAD_LENGTH_FOR_ID;


};

#endif // READINGPARSER_H
