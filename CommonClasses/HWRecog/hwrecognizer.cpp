#include "hwrecognizer.h"

HWRecognizer::HWRecognizer()
{

    // Initialize the specs to all empty.
    specs[HWKeys::CPU_MODEL] = "";
    specs[HWKeys::GPU_MODEL] = "";
    specs[HWKeys::GPU_BRAND] = "";
    specs[HWKeys::PC_BRAND] = "";
    specs[HWKeys::PC_MODEL] = "";
    specs[HWKeys::PC_SN] = "";
    specs[HWKeys::DISK_MODEL] = "";
    specs[HWKeys::DISK_SIZE] = "";
    specs[HWKeys::DISK_SN] = "";

    // We parse the sytem info.
    parseSystemInfo();

    // And the PNP info.
    parsePNPUtilInfo();

    // Copy the relevant information.
    QMap<QString,QString> keys_to_copy;
    keys_to_copy[SYSINFO_KEY_SYS_MANUFACTURER] = HWKeys::PC_BRAND;
    keys_to_copy[SYSINFO_KEY_SYS_MODEL] = HWKeys::PC_MODEL;
    keys_to_copy[SYSINFO_KEY_RAM] = HWKeys::TOTAL_RAM;
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

    QMap<QString,QStringList> info;

    info = parseWMICOutputAsTable(CMD_GET_DEVICE_SERIAL_NUMBER);
    if (!info.isEmpty()){
        QStringList list = info.value(WMIC_KEY_SN);
        if (!list.empty()){
            specs[HWKeys::PC_SN] = list.first();
        }
    }

    // CPU Brand And Model
    info = parseWMICOutputAsTable(CMD_GET_CPU_NAME);
    if (!info.isEmpty()){
        QStringList list = info.value(WMIC_KEY_NAME);
        if (!list.empty()){
            specs[HWKeys::CPU_MODEL] = list.first();
        }
    }

    // We search for the entry of the CPU in the PNP Info.
    QList< QMap<QString,QString> > search_results = searchPNPInfo(PNP_KEY_DESC,specs.value(HWKeys::CPU_MODEL));
    if (search_results.size() > 0){
        specs[HWKeys::CPU_BRAND] = search_results.first().value(PNP_KEY_BRAND);
    }

    // GPU Brand And Model.
    info = parseWMICOutputAsTable(CMD_GET_GPU_NAME);
    if (!info.isEmpty()){
        QStringList list = info.value(WMIC_KEY_NAME);
        if (!list.empty()){
            qDebug() << "MODELO";
            specs[HWKeys::GPU_MODEL] = list.first();
        }
    }

    search_results = searchPNPInfo(PNP_KEY_DESC,specs.value(HWKeys::GPU_MODEL));
    if (search_results.size() > 0){
        specs[HWKeys::GPU_BRAND] = search_results.first().value(PNP_KEY_BRAND);
    }

    // HDD Info
    info = parseWMICOutputAsTable(CMD_GET_HDD_INFO);

    if (!info.isEmpty()){

        QStringList list = info.value(WMIC_KEY_MODEL);
        if (!list.empty()){
            specs[HWKeys::DISK_MODEL] = list.first();
        }
        list = info.value(WMIC_KEY_SN);
        if (!list.empty()){
            specs[HWKeys::DISK_SN] = list.first();
        }
        list = info.value(WMIC_KEY_SIZE);
        if (!list.empty()){
            specs[HWKeys::DISK_SIZE] = list.first();
        }

    }

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

QMap<QString, QStringList> HWRecognizer::parseWMICOutputAsTable(const QString &cmd){

    QMap<QString, QStringList> parsedInformation;
    QStringList args = cmd.split(" ");
    QString command = args.first();
    args.removeFirst();
    bool ok = true;

    QString cmd_output = runCommand(command,args,&ok);

    if (!ok) return parsedInformation;

    cmd_output = cmd_output.remove('\r');

    QStringList lines = cmd_output.split("\n",Qt::SkipEmptyParts);

    if (lines.size() < 2) {
        this->lastErrors.append("Number of lines obtained in output when running command '" + cmd + "' is too low");
        return parsedInformation;
    }

    // The first line is a table. The starting position of it's Name and It's value provide the formatting.
    QString header = lines.first().trimmed();

    QStringList keys;
    QList<qint32> key_start_positions;
    QString key = "";
    qint32 position_start = 0;
    bool searching_for_key = false;



    for (qint32 i = 0; i < header.size(); i++){
        QChar c = header.at(i);
        if (isspace(c.toLatin1())){

            if (!searching_for_key){
                if (key == ""){
                    this->lastErrors.append("Could not parse header. Found a space with empty key. Header: '" + header + "'");
                    parsedInformation.clear();
                    return parsedInformation;
                }
                else {
                    keys << key;
                    key_start_positions << position_start;
                    parsedInformation[key] = QStringList();
                    key = "";
                    searching_for_key = true;
                }
            }

        }
        else {

            if (searching_for_key){
                // This is a character when we are searching for the start of a key name.
                key = QString("") + c;
                searching_for_key = false;
                position_start = i;
            }
            else {
                key = key + c;
            }

        }
    }

    // When we reached the end, we need to check if key is non empty.
    if (key != ""){
        keys << key;
        key_start_positions << position_start;
        parsedInformation[key] = QStringList();
    }



    // Now armed with this information we can process all other lines.
    for (qint32 i = 1; i < lines.size(); i++){
        QString line = lines.at(i);

        QString acc = "";
        qint32 pos_in_str = 0;

        for (qint32 j = 0; j < key_start_positions.size(); j++){
            qint32 end;

            if ( j < key_start_positions.size()-1) {
                end = key_start_positions.at(j+1);
            }
            else {
                end = static_cast<qint32>(line.size());
            }

            while (pos_in_str < end){
                acc = acc + line.at(pos_in_str);
                pos_in_str++;
            }

            // The key is actually indexed by the previous value.
            parsedInformation[keys.at(j)] << acc.trimmed();
            acc = "";
        }


    }

    return parsedInformation;

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

void HWRecognizer::parseStringIntoMap(const QString &line, const QStringList &keys, const QList<qint32> count){

    QStringList words = line.split(" ",Qt::SkipEmptyParts);

    if (keys.size()-1 != count.size()) return;

    // Special case of a single word.
    if (keys.size() == 1){
        specs.insert(keys.first(),line);
        return;
    }

    qint32 cnt = count.first();
    qint32 cnt_idx = 0;
    QStringList temp;
    qint32 i = 0;
    for (i = 0; i < words.size(); i++){
        temp << words.at(i);
        if (temp.size() == cnt){
            specs.insert(keys.at(i),temp.join(" "));
            temp.clear();
            cnt_idx++;
            if (cnt_idx < count.size()){
                cnt = count.at(cnt_idx);
            }
            else {
                cnt = -1; // The if will never be true and so temp will have all the remaining word.s
            }
        }
    }

    // Temp has all the remaining words.
    specs.insert(keys.last(),temp.join(" "));

}

QString HWRecognizer::toString(bool prettyPrint) const{

    QStringList keys = specs.keys();

    QStringList ans;

    for (qint32 i = 0; i < keys.size(); i++){
        QString value = specs.value(keys.at(i));
        if (prettyPrint){
            ans << keys.at(i) + ": " + value;
        }
        else {
            ans << keys.at(i) + "|" + value;
        }
    }

    if (prettyPrint){
        return "\n   " + ans.join("\n   ");
    }
    else {
        return ans.join("||");
    }

}

QList< QMap<QString,QString> > HWRecognizer::searchPNPInfo(const QString &key, const QString &value){

    QList< QMap<QString,QString> > ans;

    for (qint32 i = 0; i < pnputilinfo.size(); i++){
        if (pnputilinfo.value(i).value(key) == value){
            ans << pnputilinfo.value(i);
        }
    }

    return ans;

}


void HWRecognizer::parsePNPUtilInfo(){

    QStringList args = CMD_PNPINFO.split(" ");
    QString command = args.first();
    args.removeFirst();
    bool ok = true;

    QString cmd_output = runCommand(command,args,&ok);

    if (!ok) return;

    cmd_output = cmd_output.remove('\r');

    QStringList lines = cmd_output.split("\n");
    QStringList entry;

    pnputilinfo.clear();

    for (qint32 i = 0; i < lines.size(); i++){
        if (lines.at(i).trimmed() == ""){
            if (!entry.empty()){
                pnputilinfo << parseSinglePNPInfoEntry(entry);
                entry.clear();
            }
        }
        else {
            entry << lines.at(i);
        }
    }

}

QMap<QString,QString> HWRecognizer::parseSinglePNPInfoEntry(const QStringList &lines){

    QMap<QString,QString> parsed;

    for (qint32 i = 0; i < lines.size(); i++){
        QStringList parts = lines.at(i).split(":",Qt::SkipEmptyParts);
        if (parts.size() < 2) continue;
        QString key = parts.first();
        parts.removeFirst();
        QString value = parts.join(" ").trimmed();
        parsed[key] = value;
    }

    return parsed;

}
