#include "hwrecognizer.h"

HWRecognizer::HWRecognizer()
{

    // Initialize the specs to all empty.
    specs[HWKeys::CPU_BRAND] = "";
    specs[HWKeys::CPU_MODEL] = "";
    specs[HWKeys::GPU_BRAND] = "";
    specs[HWKeys::GPU_MODEL] = "";
    specs[HWKeys::PC_BRAND] = "";
    specs[HWKeys::PC_MODEL] = "";
    specs[HWKeys::PC_SN] = "";

    // We parse the sytem info.
    parseSystemInfo();

    // Copy the relevant information.
    QMap<QString,QString> keys_to_copy;
    keys_to_copy[SYSINFO_KEY_SYS_MANUFACTURER] = HWKeys::PC_BRAND;
    keys_to_copy[SYSINFO_KEY_SYS_MODEL] = HWKeys::PC_MODEL;
    QStringList keys = keys_to_copy.keys();
    for (qint32 i = 0; i < keys.size(); i++){
        QString sysinfo_key = keys.at(i);
        QString spec_key = keys_to_copy.value(sysinfo_key);
        if (systemInfo.contains(sysinfo_key)){
            specs[spec_key] = systemInfo.value(sysinfo_key);
        }
        else {
            lastErrors.append("Could not find in system info key '" + sysinfo_key + "'");
        }
    }


    // Then we run the commands for each of the wmic commands.
    WMICParseInfo wmic;

    // Serial Number
    wmic.n_words_for_brand = 0;
    wmic.command = CMD_GET_DEVICE_SERIAL_NUMBER;
    wmic.onlyFirstLine = true;
    wmic.brand.clear(); wmic.model.clear();
    parseWMICOutput(&wmic);
    if (!wmic.model.empty()) specs[HWKeys::PC_SN] = wmic.model.first();

    // CPU Brand And Model
    wmic.n_words_for_brand = 1;
    wmic.command = CMD_GET_CPU_NAME;
    wmic.onlyFirstLine = true;
    wmic.brand.clear(); wmic.model.clear();
    parseWMICOutput(&wmic);
    if (!wmic.model.empty()) specs[HWKeys::CPU_MODEL] = wmic.model.first();
    if (!wmic.brand.empty()) specs[HWKeys::CPU_BRAND] = wmic.brand.first();

    // GPU Brand And Model.
    wmic.n_words_for_brand = 1;
    wmic.command = CMD_GET_GPU_NAME;
    wmic.onlyFirstLine = true;
    wmic.brand.clear(); wmic.model.clear();
    parseWMICOutput(&wmic);
    if (!wmic.model.empty()) specs[HWKeys::GPU_BRAND] = wmic.model.first();
    if (!wmic.brand.empty()) specs[HWKeys::GPU_MODEL] = wmic.brand.first();

}

HWRecognizer::HardwareMap HWRecognizer::getHardwareSpecs() const{
    return specs;
}

QStringList HWRecognizer::getErrors() const{
    return lastErrors;
}

QString HWRecognizer::runCommand(const QString &command, const QStringList &args ,bool *ranOK){

    QProcess process;
    if (args.empty()) process.start(command);
    else process.start(command,args);

    *ranOK = process.waitForFinished();

    if (!*ranOK){
        QString error = "Failed to run command '" + command + "'. Reason: " + process.errorString();
        error = error + "\nSTDERR:\n" + process.readAllStandardError();
        this->lastErrors << error;
        return "";
    }

    else return process.readAllStandardOutput();

}

bool HWRecognizer::parseWMICOutput(WMICParseInfo *wmic_struct){

    QStringList args = wmic_struct->command.split(" ");
    QString command = args.first();
    args.removeFirst();
    bool ok = true;

    QString cmd_output = runCommand(command,args,&ok);

    if (!ok) return false;

    cmd_output = cmd_output.remove('\r');

    QStringList lines = cmd_output.split("\n",Qt::SkipEmptyParts);

    if (lines.size() > 1) lines.removeFirst(); // The very first line is usually the tile.


    for (qint32 i = 0; i < lines.size(); i++){

        QStringList words = lines.at(i).split(" ",Qt::SkipEmptyParts);
        QString brand;
        QString model;

        QStringList temp;
        if (words.size() > wmic_struct->n_words_for_brand){
            for (qint32 j = 0; j < wmic_struct->n_words_for_brand; j++){
                temp << words.at(j);
            }
            brand = temp.join(" ");

            temp.clear();
            for (qint32 j = wmic_struct->n_words_for_brand; j < words.size(); j++){
                temp << words.at(j);
            }
            model = temp.join(" ");
        }
        else {
            // Default in case of un expected value in th word cout is to just treat the entire line as a model.
            model = lines.at(i);
        }

        wmic_struct->brand << brand;
        wmic_struct->model << model;

        if ((i == 0) && (wmic_struct->onlyFirstLine)){
            // We ignore all lines other than the first one, after the header.
            break;
        }

    }

    return true;

}


void HWRecognizer::parseSystemInfo() {

    bool ok = true;
    QString sysinfoOutput = runCommand(CMD_SYSTEMINFO,QStringList(),&ok);
    if (!ok) return;

    //sysinfoOutput = sysinfoOutput.replace('\t',' ');
    //sysinfoOutput = sysinfoOutput.replace('\r','');
    sysinfoOutput =  sysinfoOutput.remove('\r');

    systemInfo.clear();
    QString key = "";
    QString value = "";

    SystemInfoParserState state = ST_KEY;

    for (qint32 i = 0; i < sysinfoOutput.size(); i++){

        QString c = sysinfoOutput.at(i);

        switch (state){

        case ST_KEY:
            if (c != ":") key = key + c;
            else state = ST_WAIT_VALUE;
            break;

        case ST_WAIT_VALUE:
            if (c == " ") continue;
            else {
                value = c;
                state = ST_VALUE;
            }
            break;

        case ST_VALUE:
            if (c != "\n") value = value + c;
            else {
                state = ST_KEY_OR_VALUE;
            }
            break;

        case ST_KEY_OR_VALUE:
            if (c == " "){
                value = value + "\n ";
                state = ST_VALUE;
            }
            else {
                // Value was finished being parsed and this is the first letter of a new command.
                systemInfo[key] = value;
                key = c;
                value = "";
                state = ST_KEY;
            }
            break;
        }

    }

    // At the very end we add the last key value pair.
    systemInfo[key] = value;

    //    QStringList keys = systemInfo.keys();
    //    for (qint32 i = 0; i < keys.size(); i++){
    //        qDebug().noquote() << "KEY" << keys.at(i);
    //        qDebug().noquote() << "Value" << systemInfo.value(keys.at(i));
    //    }

    //qDebug() << systemInfo;

}
