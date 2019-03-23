#include "dataextractor.h"

DataExtractor::DataExtractor()
{

}

DataExtractor::DataPoints DataExtractor::analyzeFile(const QString &fname){
    fileToAnalyze = fname;
    QFileInfo info(fileToAnalyze);
    QStringList parts = info.baseName().split("_");

    DataPointsExtractParameters dpep;

    if (parts.first() == FILE_BINDING){
        dpep.trialChangeBehaviour = TCB_LINE_SIZE_CHANGE;
        dpep.dataLineSize = 7;
        dpep.colsWithData << 1 << 2 << 3 << 4;
        dpep.valueChangeColumn = 0;
        dpep.header = HEADER_IMAGE_EXPERIMENT;
        dpep.colWithID = 0;
        return extractDataPoints(dpep);
    }
    else if (parts.first() == FILE_OUTPUT_READING){
        dpep.trialChangeBehaviour = TCB_LINE_VALUE_CHANGE;
        dpep.dataLineSize = 13;
        dpep.valueChangeColumn = 0;
        dpep.colsWithData << 2 << 3 << 4 << 5;
        dpep.header = HEADER_READING_EXPERIMENT;
        dpep.colWithID = 0;
        return extractDataPoints(dpep);
    }
    else{
        DataPoints dp;
        dp.error = "Unrecognized file name. Cannot perform frequency analysis";
        return dp;
    }
}

QStringList DataExtractor::getFileLines(QString *error){
    QFile file(fileToAnalyze);
    if (!file.open(QFile::ReadOnly)){
        *error = "Could not open file: " + fileToAnalyze + " for reading";
        return QStringList();
    }

    QTextStream reader(&file);
    reader.setCodec(COMMON_TEXT_CODEC);
    QString content = reader.readAll();
    file.close();

    return content.split("\n");
}

QStringList DataExtractor::removeHeader(const QStringList &lines, const QString &header){
    qint32 lineid = 0;
    qint32 counter = 0;

    // Removing the header
    while (lineid < lines.size()){
        if (lines.at(lineid).startsWith(header)) counter++;
        lineid++;
        if (counter == 2) break;
    }

    if (lineid < lines.size())  return lines.mid(lineid);
    else return QStringList();
}

DataExtractor::DataPoints DataExtractor::extractDataPoints(const DataExtractor::DataPointsExtractParameters &dpep){


    QString error;
    QStringList content = getFileLines(&error);
    DataPoints ans;
    if (!error.isEmpty()) {
        ans.error = error;
        return ans;
    }
    if (content.isEmpty()){
        ans.error = "No content";
        return ans;
    }

    content = removeHeader(content,dpep.header);
    if (content.isEmpty()){
        ans.error == "Header segment for binding was not found";
        return ans;
    }

    DataMatrix matrix;

    bool trialChange;
    bool ok;
    QString previousValue = "";

    QStringList res = content.first().split(" ");
    ans.width = res.first().toInt();
    ans.height = res.last().toInt();

    for (qint32 i = 1; i < content.size(); i++){

        QStringList parts = content.at(i).split(" ",QString::SkipEmptyParts);

        trialChange = false;
        if (dpep.trialChangeBehaviour == TCB_LINE_SIZE_CHANGE){
            trialChange = (parts.size() != dpep.dataLineSize);
        }
        else if (dpep.trialChangeBehaviour == TCB_LINE_VALUE_CHANGE){
            // For simplicity all lines that are not the size of dataLineSize are ignored.
            if (dpep.dataLineSize != parts.size()) continue;
            trialChange = (parts.at(dpep.valueChangeColumn) != previousValue);
            previousValue = parts.at(dpep.valueChangeColumn);

        }

        if (trialChange){
            if (matrix.size() > 0){
                ans.datapoints << matrix;
                matrix.clear();
            }
        }
        else if (parts.size() == dpep.dataLineSize){
            ok = true;
            QList<qreal> row;
            for (qint32 i = 0; i < dpep.colsWithData.size(); i++){
               qreal value =  parts.at(dpep.colsWithData.at(i)).toDouble(&ok);
               if (!ok){
                   row << 0;
               }
               else{
                   row << value;
               }
            }
            //qWarning() << "Adding row" << row;
            matrix << row;
        }
    }

    if (matrix.size() > 0){
        ans.datapoints << matrix;
    }

    return ans;

}



