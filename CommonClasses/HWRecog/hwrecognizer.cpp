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
    specs[HWKeys::HMD_SN] = "";
    specs[HWKeys::HMD_BRAND] = "";
    specs[HWKeys::OS_NAME] = "";
    specs[HWKeys::OS_VERSION] = "";

    // We parse the sytem info.
    parseSystemInfo();

    // And the PNP info.
    parsePNPUtilInfo();

    // We now attempt to get the HP Omnicept SN
    QString hp_omnicept_sn = findHPOmniceptSN();
    if (hp_omnicept_sn != ""){
        specs[HWKeys::HMD_SN] = hp_omnicept_sn;
        specs[HWKeys::HMD_BRAND] = HP_DEVICE_DESCRIPTION;
    }
    else {
       QString varjo_sn = findVarjoAeroSN();
       if (varjo_sn != ""){
           specs[HWKeys::HMD_SN] = varjo_sn;
           specs[HWKeys::HMD_BRAND] = VARJO_MODEL;
       }
    }


    // Copy the relevant information.
    QMap<QString,QString> keys_to_copy;
    keys_to_copy[SYSINFO_KEY_SYS_MANUFACTURER] = HWKeys::PC_BRAND;
    keys_to_copy[SYSINFO_KEY_SYS_MODEL] = HWKeys::PC_MODEL;
    keys_to_copy[SYSINFO_KEY_RAM] = HWKeys::TOTAL_RAM;
    keys_to_copy[SYSINFO_KEY_OS_NAME] = HWKeys::OS_NAME;
    keys_to_copy[SYSINFO_KEY_OS_VERSION] = HWKeys::OS_VERSION;

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
    QList< QMap<QString,QString> > search_results = searchPNPInfo(PNP_KEY_DESC,specs.value(HWKeys::CPU_MODEL),true);
    if (search_results.size() > 0){
        specs[HWKeys::CPU_BRAND] = search_results.first().value(PNP_KEY_BRAND);
    }

    // GPU Brand And Model.
    info = parseWMICOutputAsTable(CMD_GET_GPU_NAME);
    if (!info.isEmpty()){
        QStringList list = info.value(WMIC_KEY_NAME);
        if (!list.empty()){

            // We might have more than ONE GPU. If this is the case we ONLY want to NON intel ones.
            if (list.size() > 1){
                QStringList temp;
                for (qint32 i = 0; i < list.size(); i++){
                    if (list.at(i).contains("Intel")) continue;
                    temp << list.at(i);
                }
                if (temp.size() > 0) list = temp;
            }
            specs[HWKeys::GPU_MODEL] = list.first();
        }
    }

    search_results = searchPNPInfo(PNP_KEY_DESC,specs.value(HWKeys::GPU_MODEL),true);
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

QVariantMap HWRecognizer::getHardwareSpecsAsVariantMap() const {
    QVariantMap map;
    QStringList keys = specs.keys();
    for (qint32 i = 0; i < keys.size(); i++){
        map[keys.at(i)] = specs.value(keys.at(i));
    }
    return map;
}

QStringList HWRecognizer::getErrors() const{
    return lastErrors;
}

QStringList HWRecognizer::getWarnings() const {
    return lastWarnings;
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

TableOutputParser::TableColumnList HWRecognizer::parseWMICOutputAsTable(const QString &cmd){

    TableOutputParser::TableColumnList parsedInformation;
    QStringList args = cmd.split(" ");
    QString command = args.first();
    args.removeFirst();
    bool ok = true;

    QString cmd_output = runCommand(command,args,&ok);

    if (!ok) return parsedInformation;

    cmd_output = cmd_output.remove('\r');

    // Now that we have the output of the command, we parse it.
    TableOutputParser top;
    if (!top.parseCMDTableOuput(cmd_output)){
        this->lastErrors.append("For command '" + cmd + "', failed in parsing the output. Reason: " + top.getErrors().join(" & "));
        return parsedInformation;
    }

    return top.getParsedOutputAsTableRowList();


}

TableOutputParser::ParseTableOutput HWRecognizer::getDevicePropertiesByID(const QString &deviceID){

    QString cmd = CMD_GET_DEVICE_PROPERTY;
    cmd = cmd.replace("<<DEVICE_ID>>",deviceID);
    TableOutputParser::ParseTableOutput pto;

    // For whatever reason this command (which needs to run through powershell) does not work correctly
    // With Qt Process. It works but returns nothing. As a ByPass we create a script with the command.

    QString batchFile = "get_device_properties.bat";
    QFile file(batchFile);
    if (!file.open(QFile::WriteOnly)){
        this->lastErrors.append("Failed to open batch file for writing for device ID '" + deviceID + "'");
    }
    QTextStream writer(&file);
    writer << "@ECHO OFF\n"; // This we need so the first line is the actual instance ID and not the
    writer << cmd;
    file.close();

    bool ok = true;
    QString cmd_output = this->runCommand(batchFile,QStringList(),&ok);

    //    // FOR DEBUGGING Getting the output from the list.
    //    QFile file1("pnp_input.txt");
    //    file1.open(QFile::ReadOnly);
    //    QTextStream reader(&file1);
    //    cmd_output = reader.readAll();


    // One way or the other we delete the file.
    QFile::remove(batchFile);

    if (!ok) return pto;

    cmd_output = cmd_output.remove('\r');
    TableOutputParser top;
    if (!top.parseCMDTableOuput(cmd_output)){
        this->lastErrors.append(top.getErrors());
        return pto;
    }

    return top.getParsedOutput();

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

QList< QMap<QString,QString> > HWRecognizer::searchPNPInfo(const QString &key, const QString &value, bool containsValue){

    QList< QMap<QString,QString> > ans;

    if (containsValue){

        QString tosearch = value;
        tosearch = tosearch.toLower();

        for (qint32 i = 0; i < pnputilinfo.size(); i++){
            //qDebug() << "Checking if Key" << key << "with value" << pnputilinfo.value(i).value(key) << " is the same as " << value;
            QString totest = pnputilinfo.value(i).value(key).toLower();
            if (totest.contains(tosearch)){
                ans << pnputilinfo.value(i);
            }
        }

    }
    else {
        for (qint32 i = 0; i < pnputilinfo.size(); i++){
            //qDebug() << "Checking if Key" << key << "with value" << pnputilinfo.value(i).value(key) << " is the same as " << value;
            if (pnputilinfo.value(i).value(key) == value){
                ans << pnputilinfo.value(i);
            }
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

    //    // FOR DEBUGGING Getting the output from the list.
    //    QFile file("pnp_list_input.txt");
    //    file.open(QFile::ReadOnly);
    //    QTextStream reader(&file);
    //    cmd_output = reader.readAll();

    if (!ok) return;

    cmd_output = cmd_output.remove('\r');

    //    // For DEBUGGING ONLY
    //    QFile file("pnp_util_output.txt");
    //    file.open(QFile::WriteOnly);
    //    QTextStream writer(&file);
    //    writer << cmd_output;
    //    file.close();

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

    //qDebug() << parsed;

    return parsed;

}

////////////////////////////////////////////////////////////////////////////////////////////
void HWRecognizer::searchForADeviceWithPropertyValue(const QString &search_for){

    QString tosearch = search_for.toLower();

    qint32 N = pnputilinfo.size();
    for (qint32 i = 0; i < N; i++){

        qDebug() << "Searching divice" << (i+1) << " out of " << N;
        QMap<QString,QString> entry = pnputilinfo.at(i);

        TableOutputParser::ParseTableOutput properties = this->getDevicePropertiesByID(entry.value(PNP_KEY_INSTID));
        qDebug() << "   Properties" << properties.size() << ". Device" <<  entry.value(PNP_KEY_DESC);

        for (qint32 j = 0; j < properties.size(); j++){
            // Now we get the key value.
            QMap<QString,QString> row = properties.at(j);
            QString tocompare = row.value(PNP_DEV_PROP_KEY_DATA).toLower();

            if ((tosearch == tocompare) || (tocompare.contains(tosearch))){
                qDebug() << "   MATCH FOUND: KEY:"  << row.value(PNP_DEV_PROP_KEY_KEYNAME) << ". VALUE: " << tocompare;
                QStringList keys = entry.keys();
                for (qint32 k = 0; k < keys.size(); k++){
                    qDebug() << "      " << keys.at(k) << "=>" << entry.value(keys.at(k));
                }
                break; // We search for only one property per device.
            }
        }
    }

    qDebug() << "Finished";

}


////////////////////////////////////////////////////////////////////////////////////////////
QString HWRecognizer::findHPOmniceptSN(){

    // The first thing we do is search for the device description we know to be for the HP Device.
    QList< QMap<QString,QString> > search_results = searchPNPInfo(PNP_KEY_DESC,HP_DEVICE_DESCRIPTION,false);
    if (search_results.count() != 1){
        //qDebug() << "Did not find the HP Device Description";
        this->lastWarnings << "Wrong number of entries when search for PNP HP Omnicept Device: " + QString::number(search_results.count());
        return "";
    }

    // If we have a single result we get the instance ID.
    QString instance_id = search_results.first().value(PNP_KEY_INSTID);
    //qDebug() << "The instance ID is " << instance_id;
    this->getDevicePropertiesByID(instance_id);

    // And now we get the Device property information.
    TableOutputParser::ParseTableOutput device_properties = this->getDevicePropertiesByID(instance_id);

    // We now search for the property key name which contains the Serial number.
    for (qint32 i = 0; i < device_properties.size(); i++){
        if (device_properties.at(i).value(PNP_DEV_PROP_KEY_KEYNAME) == HP_DEVICE_SN_PNP_PROPERTY_KEY){
            return device_properties.at(i).value(PNP_DEV_PROP_KEY_DATA);
        }
    }

    return "";

}

QString HWRecognizer::findVarjoAeroSN(){

    QList< QMap<QString,QString> > search_results = searchPNPInfo(PNP_KEY_INSTID,VARJO_SN_INST_ID_PREFIX,true);
    if (search_results.count() == 0){
        // We didn't find anything.
        qDebug() << "No varjo prefix found";
        return "";
    }

    // If we did then the last part of the instance id is the serial number.
    QMap<QString,QString> entry = search_results.first();
    QString inst_id = entry.value(PNP_KEY_INSTID);
    QStringList parts = inst_id.split("\\",Qt::SkipEmptyParts);
    QString serial_number = parts.last();

    qDebug() << "Proposed serial number" << serial_number;

    if (serial_number.startsWith(VARJO_SN_INST_ID_PREFIX)){
        // The string starts with qvrjv however only the last v is part of the serial number so the qvrj is removed.
        serial_number = serial_number.remove(0,4);
        return serial_number;
    }
    return "";
}
