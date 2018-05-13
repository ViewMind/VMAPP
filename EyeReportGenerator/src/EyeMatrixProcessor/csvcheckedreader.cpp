#include "csvcheckedreader.h"

CSVCheckedReader::CSVCheckedReader()
{
    stringsTrue << "true" << "t" << "v" << "1";
    stringsFalse << "false" << "f"  << "0";
}


CSVData CSVCheckedReader::loadCSV(const QString &file, const ColumnChecks &cc, bool hasHeader){

    QFile csv(file);

    CSVData data;

    if (!csv.open(QFile::ReadOnly)){
        error = "Could not open " + file + " for reading";
        return data;
    }

    QTextStream reader(&csv);

    qint32 rowCounter = 0;

    // Skipping header
    if (hasHeader) {
        reader.readLine();
        rowCounter++;
    }

    while (!reader.atEnd()){

        //QStringList columns = reader.readLine().split(",");

        QString line = reader.readLine();
        QStringList columns = line.split(",");

        if (columns.size() != cc.size()){
            csv.close();
            error =  "Row " + QString::number(rowCounter) + " should have " + QString::number(cc.size()) + " fields insted of " + QString::number(columns.size());
            return data;
        }

        QHash<qint32,QVariant> row;

        for (qint32 i = 0; i < columns.size(); i++){

            if (cc.at(i).type == CT_IGNORE) continue;

            QString col = columns.at(i).trimmed();
            QVariant value = checkColumn(col,cc.at(i));

            if (!error.isEmpty()){
                csv.close();
                error = "On Row " + QString::number(rowCounter+1) + ", Column "  + QString::number(i) + ": " + error;
                return data;
            }

            row[i] = value;

        }

        data << row;

        rowCounter++;

    }

    return data;

}


QVariant CSVCheckedReader::checkColumn(const QString &value, const ColumnChecker &cc){

    QString temp;
    bool ok;
    qint32 an_int;
    qreal a_real;
    QVariant ans;

    switch (cc.type){
    case CT_BOOL:
        temp = value.toLower();
        if (stringsFalse.contains(temp)) ans = false;
        else if (stringsTrue.contains(temp)) ans = true;
        else{
            error = value + " is not a valid true or false value";
        }
        break;
    case CT_INT:
        an_int = value.toInt(&ok);
        if (!ok){
            error = value + " is not a valid integer";
        }
        if (!cc.isWithinRange(an_int)){
            error = value + " should be a number between " + QString::number(cc.min) + " and " + QString::number(cc.max) ;
        }
        ans = an_int;
        break;
    case CT_REAL:
        a_real = value.toDouble(&ok);
        if (!ok){
            error =  value + " is not a valid real number";
        }
        if (!cc.isWithinRange(a_real)){
            error =  value + " should be a number between " + QString::number(cc.min) + " and " + QString::number(cc.max) ;
        }
        ans = a_real;
        break;
    case CT_STRING:
        ans = value;
        return ans;
    case CT_STRING_SET:        
        if (!cc.set.contains(value)){
            error =  "Value:" + value + " should be one of the following: " + cc.set.toList().join(",");
        }
        else ans = value;
        break;
    case CT_IGNORE:
        return ans;
    }

    return ans;
}
