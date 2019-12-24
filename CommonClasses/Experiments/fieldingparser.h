#ifndef FIELDINGPARSER_H
#define FIELDINGPARSER_H

#include <QString>
#include <QList>

class FieldingParser
{
public:

    // Definition of structures used to load the sequences for each trial.
    struct Trial{
        QString id;
        QList<qint32> sequence;
    };

    FieldingParser();
    bool parseFieldingExperiment(const QString &contents);
    QList<Trial> getParsedTrials() const;
    QString getError() const;
    QString getVersionString() const;

private:
    QString error;
    QList<Trial> fieldingTrials;
    QString versionString;

};

#endif // FIELDINGPARSER_H
