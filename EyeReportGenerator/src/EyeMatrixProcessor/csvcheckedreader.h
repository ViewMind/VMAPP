#ifndef CSVCHECKEDREADER_H
#define CSVCHECKEDREADER_H

#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <QSet>
#include <QHash>
#include <QDebug>

typedef QList< QHash<qint32,QVariant> > CSVData;

class CSVCheckedReader
{
public:

    typedef enum {CT_INT, CT_STRING, CT_REAL, CT_BOOL, CT_STRING_SET, CT_IGNORE} ColumnType;

    struct ColumnChecker{
        ColumnType type;
        qreal max;
        qreal min;
        QSet<QString> set;

        void clear(){
            type = CT_STRING;
            max = 0;
            min = 0;
            set.clear();
        }

        bool isWithinRange(qreal v) const {
            if (max == min) return true;
            if ((v <= max) && (v >= min)) return true;
            else return false;
        }
    };

    typedef QList<ColumnChecker> ColumnChecks;

    CSVCheckedReader();    

    CSVData loadCSV(const QString &file, const ColumnChecks & cc, bool hasHeader);

    QString getError() const {return error;}

private:
    QList<QStringList> data;
    qint32 currentRow;

    // If there was an error
    QString error;

    // The set of valid true or false values
    QSet<QString> stringsTrue;
    QSet<QString> stringsFalse;

    QVariant checkColumn(const QString &value, const ColumnChecker &cc);

};

#endif // CSVCHECKEDREADER_H
