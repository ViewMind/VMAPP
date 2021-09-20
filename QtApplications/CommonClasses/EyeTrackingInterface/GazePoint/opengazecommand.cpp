#include "opengazecommand.h"

OpenGazeCommand::OpenGazeCommand()
{
    clear();
}

void OpenGazeCommand::clear(){
    id = "";
    cmdType = "";
    fields.clear();
}

void OpenGazeCommand::setEnableCommand(const QString &cmd, bool enable){
    cmdType = GPCT_SET;
    id = cmd;
    if (enable) fields[GPF_STATE] = "1";
    else fields[GPF_STATE] = "0";
}

QString OpenGazeCommand::getField(const QString &fieldName) const{
    if (fields.contains(fieldName)) return fields.value(fieldName);
    else return "";
}

QString OpenGazeCommand::fromString(const QString &fullcmd){

    clear();
    QString cleanCommand;

    // Get only the internal command
    qint32 foundStartEnd = 0;

    for (qint32 i = 0; i < fullcmd.size(); i++){
        if (foundStartEnd == 0){
            if (fullcmd.at(i) == '<'){
                foundStartEnd++;
                continue;
            }
        }
        else {
            if (fullcmd.at(i) == '/' && (fullcmd.at(i+1) == '>')){
                foundStartEnd++;
                break;
            }
        }
        if (foundStartEnd > 0) cleanCommand = cleanCommand + fullcmd.at(i);
    }

    // If not ok, nothing more to do.
    if (foundStartEnd != 2) return "Badly formatted command: " + fullcmd;

    // Split by spaces.
    QStringList parts = cleanCommand.split(" ",Qt::SkipEmptyParts);

    // There should AT LEAST be a cmd type AND an ID
    if (parts.size() < 2) return "There should at least be a command type and an ID: " + fullcmd;

    // First part should be the command type.
    cmdType = parts.first();
    QSet<QString> allowedCommnds; allowedCommnds << GPCT_ACK << GPCT_GET << GPCT_NACK << GPCT_SET << GPCT_REC << GPCT_CAL;
    if (!allowedCommnds.contains(cmdType)) return "Unknown command type: " + cmdType + " in: " + fullcmd;

    // Then the ID, unless it is a rec field
    qint32 start = 2;
    if (cmdType != GPCT_REC){
        QString idstr = parts.at(1);
        QStringList idandvalue = idstr.split("=",Qt::SkipEmptyParts);
        if (idandvalue.size() != 2) return "ID field wrongly formatted: " + idstr + " in " + fullcmd;
        if (idandvalue.first() != "ID") return "ID field wrongly formatted: " + idstr + " in " + fullcmd;
        id = idandvalue.last();
        id = id.replace("\"","");
        id = id.trimmed();
    }
    else{
        // It is a REC field so there is no ID.
        id = "";
        start = 1;
    }

    // Now all the fields;
    for (qint32 i = start; i < parts.size(); i++){
        QString str = parts.at(i);
        QStringList fieldandvalue = str.split("=",Qt::SkipEmptyParts);
        if (fieldandvalue.size() != 2) return "Field wrongly formatted: " + str + " in " + fullcmd;
        QString value = fieldandvalue.last();
        value = value.replace("\"","");
        value = value.trimmed();
        QString field = fieldandvalue.first();
        field = field.replace("\"","");
        field = field.trimmed();
        fields[field] = value;
    }

    return "";
}

QByteArray OpenGazeCommand::prepareCommandToSend() const{

    QString cmd = "<" + cmdType + " ID=\"" + id + "\" ";
    if (fields.size() > 0){
        QHashIterator<QString,QString> i(fields);
        while (i.hasNext()){
            i.next();
            cmd = cmd + i.key() + "=\"" + i.value() + "\" ";
        }
    }
    cmd = cmd + "/>\r\n";
    return cmd.toLatin1();

}

void OpenGazeCommand::prettyPrint() const{
    qWarning() << "TYPE:" << cmdType;
    qWarning() << "ID:" << id;
    QHashIterator<QString,QString> i(fields);
    while (i.hasNext()) {
        i.next();
        qWarning() << "  " << i.key() << "=" << i.value();
    }
}
