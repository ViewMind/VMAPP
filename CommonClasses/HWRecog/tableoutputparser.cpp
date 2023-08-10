#include "tableoutputparser.h"

TableOutputParser::TableOutputParser(){

}

TableOutputParser::ParseTableOutput TableOutputParser::getParsedOutput() const {
    return result;
}

TableOutputParser::TableColumnList TableOutputParser::getParsedOutputAsTableRowList(){

    TableColumnList ans;

    // So first we get a list of keys from the very first element.
    if (result.size() == 0) return ans; // Unless there is nothing to do.

    QStringList columns = result.first().keys();

    for (qint32 i = 0; i < columns.size(); i++){
        ans[columns.at(i)].clear();
        for (qint32 j = 0; j < result.size(); j++){
            ans[columns.at(i)].append(result.at(j).value(columns.at(i)));
        }
    }

    return ans;

}

QStringList TableOutputParser::getErrors() const {
    return errors;
}

bool TableOutputParser::parseCMDTableOuput(const QString &cmd_ouput){

    result.clear();
    errors.clear();

    QStringList lines = cmd_ouput.split("\n",Qt::SkipEmptyParts);

    //qDebug() << "Parsing " << lines.count();

    if (lines.size() < 2) {
        errors.append("Number of lines obtained in output is too low");
        return false;
    }

    // The first line is a table. The starting position of it's Name and It's value provide the formatting.
    QString header = lines.first().trimmed();

    QStringList keys;
    QList<qint32> key_start_positions;
    QString key = "";
    qint32 position_start = 0;
    bool searching_for_key = false;


    // Here we search  for the key starts point which delineate columns.
    for (qint32 i = 0; i < header.size(); i++){
        QChar c = header.at(i);
        if (isspace(c.toLatin1())){

            if (!searching_for_key){
                if (key == ""){
                    errors.append("Could not parse header. Found a space with empty key. Header: '" + header + "'");
                    return false;
                }
                else {
                    keys << key;
                    key_start_positions << position_start;
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
    }



    // Now armed with this information we can process all other lines.
    for (qint32 i = 1; i < lines.size(); i++){
        QString line = lines.at(i);

        QString acc = "";
        qint32 pos_in_str = 0;

        QMap<QString,QString> row;

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
            row[keys.at(j)] = acc.trimmed();
            acc = "";
        }

        result << row;

    }

    return true;


}
