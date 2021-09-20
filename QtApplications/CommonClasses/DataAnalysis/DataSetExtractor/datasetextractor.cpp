#include "datasetextractor.h"

DataSetExtractor::DataSetExtractor()
{

}

DataSetExtractor::RawDataInfo DataSetExtractor::separateRawDataFile(const QString &fileName){

    RawDataInfo rdi;
    rdi.error = "";

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)){
        rdi.error = "Could not open " + fileName + " for reading";
        return rdi;
    }

    QTextStream reader(&file);
    QStringList content = reader.readAll().split("\n");
    file.close();

    // First line contains the header and experiment version.
    QString firstLine = content.first();
    QStringList tokens = firstLine.split(" ");
    if (tokens.size() != 2){
        rdi.error = "Expected 2 tokens in the first line, but found " + QString::number(tokens.size()) + ": " + firstLine;
        return rdi;
    }
    QString header = tokens.first();
    rdi.expVersion = tokens.last();

    // Separating the experiment description from the rest of the file.
    int lineCounter = 1;
    while (!content.at(lineCounter).contains(header)){
        lineCounter++;
    }
    // Line counter now contains the line that contains the second header.
    rdi.expDescription = content.mid(1,lineCounter-1).join("\n");  // The -1 is to avoid the repeated header to be be addded.

    // Now we find the resolution. Should be the next nonempty line.
    lineCounter++;
    while (content.at(lineCounter).trimmed().isEmpty()){
        lineCounter++;
    }

    tokens = content.at(lineCounter).split(" ");
    if (tokens.size() != 2){
        rdi.error = "Expected 2 tokens in the resolution line: " + content.at(lineCounter);
        return rdi;
    }
    rdi.resolution.setWidth(tokens.first().toInt());
    rdi.resolution.setHeight(tokens.last().toInt());

    // From line coutner to the end fo the file it is then the experiment data.
    rdi.expData = content.mid(lineCounter+1,-1).join("\n");

    return rdi;


}

bool DataSetExtractor::extractDataSetFromFile(const QString &fileName){

    // Extracting the data info.
    RawDataInfo rdi = separateRawDataFile(fileName);

    if (!rdi.error.isEmpty()){
        error = "Separating Raw Data File: " + rdi.error;
        return false;
    }

    // Setting the original file name and path
    QFileInfo info(fileName);
    data.originalFileName = info.baseName();
    data.originalFilePath = info.path();
    data.version = "";
    data.dataSetType = -1;
    data.dataSets.clear();
    data.dataSetIDs.clear();
    data.resolution.setWidth(0);
    data.resolution.setHeight(0);
    data.extraSetInfo.clear();
    data.experimentDescription = "";
    data.version = rdi.expVersion;
    data.experimentDescription = rdi.expDescription;
    data.resolution = rdi.resolution;

    // Setting the lines for data extraction.
    content = rdi.expData.split("\n");

    //for (qint32 i = 0; i < content.size(); i++) qDebug() << content.at(i);

    if (data.originalFileName.startsWith(FILE_OUTPUT_READING)){
        return dataSetFromReadingFile();
    }

    error = "Unknow file type";
    return false;
}

bool DataSetExtractor::dataSetFromReadingFile(){

    QString id = "";
    DataMatrix dm;

    data.dataSetType = EXP_READING;

    for (int i = 0; i < content.size(); i++){
        QString line = content.at(i);

        if (line.trimmed().isEmpty()) continue;

        QStringList tokens = line.split(" ",QString::SkipEmptyParts);
        if (tokens.size() != READING_TOKEN_LIST_EXPECTED_SIZE){
            error = "Badly formatted line for reading file: " + line + ". Token Size: " + QString::number(tokens.size()) + ". Expecting: " + QString::number(READING_TOKEN_LIST_EXPECTED_SIZE);
            return false;
        }
        if (tokens.first() != id){            
            if (id.isEmpty()){
                id = tokens.first();
            }
            else{
                data.dataSets << dm;
                data.dataSetIDs << id;
                id = tokens.first();
                data.extraSetInfo << "";
            }
            dm.clear();
        }

        // Adding the data row.
        DataRow dr;
        for (qint32 i = 1; i < READING_TOKEN_LIST_EXPECTED_SIZE; i++){
            dr << tokens.at(i).toDouble();
        }
        dm << dr;
    }

    // When all is said and done, we still need have the last data matrix.
    data.dataSets << dm;
    data.dataSetIDs << id;
    data.extraSetInfo << "";

    return true;

}

