#include "configurationmanager.h"

ConfigurationManager::ConfigurationManager()
{
}

bool ConfigurationManager::loadConfiguration(const QString &file, const char* textCodec, const QString readText){

    QString text;
    if (readText.isEmpty()){
        QFile f(file);
        if (!f.open(QFile::ReadOnly)){
            error = "Could not open settings file " + file + " for reading.";
            return false;
        }

        // Reading in all data and closing the file afterwards, using the provided text codec.
        QTextStream reader(&f);
        reader.setCodec(textCodec);
        text = reader.readAll();
        f.close();
    }
    else text = readText;

    // Parsing the configuration data
    QString currentCommand = "";
    QString currentField = "";
    QStringList fields;
    data.clear();
    ParseState state = PS_NAME_OR_COMMENT;

    QSet<QString> keyWordsFound;

    for (int i = 0; i < text.size(); i++){

        QChar c = text.at(i);

        switch (state){
        case PS_NAME:
            if (c == delimiters.name){
                currentCommand = currentCommand.trimmed();
                if (keyWordsFound.contains(currentCommand)){
                    error = "Duplicate keyword " + currentCommand + " was found. Each keyword must ocurr only once";
                    return false;
                }
                else keyWordsFound << currentCommand;
                state = PS_FIELD;
            }
            else{
                currentCommand = currentCommand + c;
            }
            break;
        case PS_FIELD:
            if (c == delimiters.field){
                currentField = currentField.trimmed();
                fields << currentField;
                currentField = "";
            }
            else if (c == delimiters.statement){
                currentField = currentField.trimmed();
                fields << currentField;
                if (fields.size() == 1){
                    data[currentCommand] = fields.first();
                }
                else{
                    data[currentCommand] = fields;
                }
                currentField = "";
                fields.clear();
                currentCommand = "";
                state = PS_NAME_OR_COMMENT;
            }
            else{
                currentField = currentField + c;
            }
            break;
        case PS_END_COMMENT:
            if (c == '\n') state = PS_NAME_OR_COMMENT;
            break;
        case PS_NAME_OR_COMMENT:
            if (c.isSpace()){
                continue;
            }
            if (c == delimiters.comment){
                state = PS_END_COMMENT;
            }
            else{
                currentCommand = currentCommand + c;
                state = PS_NAME;
            }
        }
    }

    if ((state != PS_NAME_OR_COMMENT) && (state != PS_END_COMMENT)){
        error = "Parser finished in a invalid state. Make sure every statement is finished by a ";
        error = error + delimiters.statement;
        return false;
    }

    return verifyParsedCommands();
}

void ConfigurationManager::addKeyValuePair(const QString &name, const QVariant &value){
    data[name] = value;
}

QString ConfigurationManager::getString(const QString &name, bool *ok) const{
    if (data.value(name).canConvert(QMetaType::QString)){
        if (ok != nullptr){
            *ok = true;
        }
        return data.value(name).toString();
    }
    else{
        if (ok != nullptr){
            *ok = false;
        }
        return "";
    }
}

bool ConfigurationManager::getBool(const QString &name, bool *ok) const{
    if (data.value(name).canConvert(QMetaType::Bool)){
        if (ok != nullptr){
            *ok = true;
        }
        return data.value(name).toBool();
    }
    else{
        if (ok != nullptr){
            *ok = false;
        }
        return false;
    }
}

qint32 ConfigurationManager::getInt(const QString &name, bool *ok) const{
    if (data.value(name).canConvert(QMetaType::Int)){
        if (ok != nullptr){
            *ok = true;
        }
        return data.value(name).toInt();
    }
    else{
        if (ok != nullptr){
            *ok = false;
        }
        return 0;
    }
}

qreal ConfigurationManager::getReal(const QString &name, bool *ok) const{
    if (data.value(name).canConvert(QMetaType::Double)){
        if (ok != nullptr){
            *ok = true;
        }
        return data.value(name).toDouble();
    }
    else{
        if (ok != nullptr){
            *ok = false;
        }
        return 0;
    }
}

QStringList ConfigurationManager::getStringList(const QString &name) const{
    if (!data.contains(name)) return QStringList();
    QVariantList list;
    if (data.value(name).canConvert(QMetaType::QVariantList)){
        list = data.value(name).toList();
    }
    else{
        list << data.value(name);
    }
    QStringList ans;
    for (qint32 i = 0; i < list.size(); i++){
        ans << list.at(i).toString();
    }
    return ans;
}

bool ConfigurationManager::setupVerification(const CommandVerifications &cv){

    // Checking the structure.
    QStringList cmds = cv.keys();
    for (qint32 i = 0; i < cmds.size(); i++){

        Command cmd = cv.value(cmds.at(i));

        // Checking that no fields is a list
        for (qint32 j = 0; j < cmd.fields.size(); j++){
            if (cmd.fields.at(j) == VT_LIST){
                error = "A field in command " + cmds.at(i) + " was declared as a list.";
                return false;
            }
        }

        // Checking that list have only 1 field.
        if (cmd.type == VT_LIST){
            if (cmd.fields.size() != 1){
                error = "Command " + cmds.at(i) + " has been declared a list but it does not have just 1 field";
                return false;
            }
        }

    }

    verif = cv;
    return true;

}

bool ConfigurationManager::verifyParsedCommands(){

    if (verif.isEmpty()) return true;

    QStringList errors;

    // Set of mandatory keywords
    QStringList cmds = verif.keys();
    QSet<QString> mandatory;
    for (qint32 i = 0; i < cmds.size(); i++){
        if (!verif.value(cmds.at(i)).optional) mandatory << cmds.at(i);
    }

    QStringList parsed = data.keys();

    for (qint32 i = 0; i < parsed.size(); i++){

        if (!verif.contains(parsed.at(i))){
            errors << "Keyword: " + parsed.at(i) + " is invalid. Please check your syntax.";
            continue;
        }

        // If it was mandatory, it has been found.
        mandatory.remove(parsed.at(i));

        if (verif.value(parsed.at(i)).type == VT_LIST){

            QVariantList list;
            if (!data.value(parsed.at(i)).canConvert(QMetaType::QVariantList)){
                // Have to assume is 1 value list for a multi size one.
                list << data.value(parsed.at(i));
            }
            else list = data.value(parsed.at(i)).toList();

            if (verif.value(parsed.at(i)).fields.size() == 1){
                // All values in the list should be of the same type
                ValueType vt = verif.value(parsed.at(i)).fields.first();
                for (qint32 j = 0; j < list.size(); j++){
                    QString e = checkValidtyQVariant(list.at(j),parsed.at(i),vt);
                    if (!e.isEmpty()) errors << e;
                }
            }
            else{
                // All values have a type, so list have to be of the same size.
                if (list.size() != verif.value(parsed.at(i)).fields.size()){
                    errors << "Keyword " + parsed.at(i) + " should be a list of "
                              + QString::number(verif.value(parsed.at(i)).fields.size())
                              + " but " + QString::number(list.size()) + " were found.";
                    continue;
                }
                for (qint32 j = 0; j < list.size(); j++){
                    QString e = checkValidtyQVariant(list.at(j),parsed.at(i),verif.value(parsed.at(i)).fields.at(j));
                    if (!e.isEmpty()) errors << e;
                }
            }

        }
        else{
            QString e = checkValidtyQVariant(data.value(parsed.at(i)),
                                             parsed.at(i),
                                             verif.value(parsed.at(i)).type);
            if (!e.isEmpty()) errors << e;
        }

    }

    // Checking all mandatory keywords were found
    if (!mandatory.isEmpty()){
        QString e;
        QSetIterator<QString> i(mandatory);
        while (i.hasNext()){
            e = e + " " + i.next();
        }
        errors << "The following mandatory keywords were not found in the configuration file:" + e;
    }

    if (errors.isEmpty()) return true;
    error = errors.join("<br>");
    return false;

}

QString ConfigurationManager::checkValidtyQVariant(const QVariant &data, const QString &keyword, ValueType type){
    //qWarning() << keyword << "::" << data;
    bool ok;
    switch (type){
    case VT_BOOL:
        if (!data.canConvert(QMetaType::Bool)){
            return  "On keyword " + keyword + " value is invalid as boolean. It should be true or false";
        }
        break;
    case VT_INT:
        data.toInt(&ok);
        if (!ok){
            return  "On keyword " + keyword + " value is invalid integer";
        }
        break;
    case VT_REAL:
        data.toDouble(&ok);
        if (!ok){
            return  "On keyword " + keyword + " value is invalid as real number";
        }
        break;
    case VT_STRING:
        if (!data.canConvert(QMetaType::QString)){
            return  "On keyword " + keyword + " value is invalid string";
        }
        break;
    default:
        return "";
    }
    return "";
}

void ConfigurationManager::merge(const ConfigurationManager &configmng){
    QStringList keys = configmng.getAllKeys();
    for (qint32 i = 0; i < keys.size(); i++){
        data[keys.at(i)] = configmng.getVariant(keys.at(i));
    }
}


QString ConfigurationManager::saveValueToFile(const QString &fileName, const char *textCode, const QString &key){
    return setValue(fileName,textCode,key,data.value(key).toString());
}

QString ConfigurationManager::setValue(const QString &fileName,
                                       const char* textCodec,
                                       const QString &cmd,
                                       const QString &value, ConfigurationManager *configToChange,
                                       const Delimiters &delimiters){

    QString newCmd = cmd + " " + delimiters.name + " " + value + delimiters.statement;

    if (!QFile::exists(fileName)){
        // Need to create the file.
        QFile temp(fileName);
        if (!temp.open(QFile::WriteOnly)){
            return "File " + fileName + " does not exist and could not be created";
        }
        temp.close();
    }

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)){
        return "Could not open " + fileName + " for reading";
    }

    // Reading all the content
    QTextStream reader(&file);
    reader.setCodec(textCodec);
    QString contents = reader.readAll();
    file.close();

    // Searching for the command, and then replacing it or adding it. However, we must make sure that the command is not in a comment.
    qint32 i = 0;
    bool done = false;
    while (!done){
        i = contents.indexOf(cmd,i,Qt::CaseSensitive);
        if (i == -1) break;
        // Checking that this is not in a comment.
        bool notfound = true;
        for (qint32 j = i; j > 0; j--){
            if (contents.at(j) == delimiters.comment){
                // this is in a comment.
                i++;
                notfound = false;
                break;
            }
            else if (contents.at(j) == '\n'){
                // This is not in a comment.
                done = true;
                notfound = false;
                break;
            }
        }
        if (notfound) done = true;
    }

    if (i == -1){
        // Th command is added at the end of the file
        contents = contents + "\n" + newCmd;
    }
    else{
        // The command must be replaced, so the first ; after the cmd is located.
        qint32 end = contents.indexOf(delimiters.statement,i);
        if (end == -1){
            return QString("Could not locate end of statement (") + delimiters.statement + QString("), for keyword ")
                    + cmd + " in file " + fileName;
        }
        contents.replace(i,end-i+1,newCmd);
    }

    // Re writing the file
    QFile file2(fileName);
    if (!file2.open(QFile::WriteOnly)){
        return "Could not open " + fileName + " for writing";
    }

    QTextStream writer(&file2);
    writer.setCodec(textCodec);
    writer << contents;
    file2.close();

    if (configToChange != nullptr){
        configToChange->addKeyValuePair(cmd,value);
    }

    return "";

}
