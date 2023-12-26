#include "processrecognizer.h"

ProcessRecognizer::ProcessRecognizer()
{
    lastTable = "";
}

TableOutputParser::TableColumnList ProcessRecognizer::getCurrentProcesses() const {
    return this->processesSnapShot;
}

QStringList ProcessRecognizer::getErrors() const {
    return this->errors;
}

QString ProcessRecognizer::getLastTaskTable() const {
    return this->lastTable;
}

bool ProcessRecognizer::refreshProcessList(){

    this->errors.clear();
    this->processesSnapShot.clear();

    TableOutputParser top;

    QProcess process;
    process.start(TASKLIST);
    bool ranOK = process.waitForFinished();

    if (!ranOK){
        QString error = "Failed to run command 'tasklist'. Reason: " + process.errorString();
        error = error + "\nSTDERR:\n" + process.readAllStandardError();
        this->errors << error;
        return false;
    }

    lastTable = process.readAllStandardOutput();
    lastTable = lastTable.trimmed();

    ranOK = top.parseCMDTableOutputTableWithDivider(lastTable);
    if (!ranOK){
        this->errors << "Errors Parsing the Output of the TaskList Table" << top.getErrors();
        return false;
    }

    this->processesSnapShot = top.getParsedOutputAsTableRowList();

    return true;

}

qint32 ProcessRecognizer::getNumberOfInstancesRunningOf(const QString &processName){

    QString colPname = QString(COLUMN_PNAME);

    if (!this->processesSnapShot.contains(colPname)){
        this->errors << "The process snapshot table does not contain the expected column of '" + colPname  + "'. Columns are: " + this->processesSnapShot.keys().join(",");
        return -1;
    }

    QStringList list = this->processesSnapShot.value(colPname);
    QString comp = processName;
    comp = comp.toLower();
    qint32 counter = 0;

    for (qint32 i = 0; i < list.size(); i++){
        if (list.at(i).toLower().trimmed() == comp){
            counter++;
        }
    }

    return counter;

}
