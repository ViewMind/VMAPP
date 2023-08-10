#ifndef TABLEOUTPUTPARSER_H
#define TABLEOUTPUTPARSER_H

#include <QMap>
#include <QString>
#include <QList>
#include <QDebug>

class TableOutputParser
{
public:
    TableOutputParser();

    typedef QList < QMap<QString,QString> > ParseTableOutput;
    typedef QMap<QString, QStringList> TableColumnList;

    ParseTableOutput getParsedOutput() const;
    QStringList getErrors() const;

    /**
     * @brief getParsedOutputAsTableRowList
     * @details It will return the table as a single map. Each key of the map corresponds to each value column in the table.
     * And each value is a list of all the values for that column in the order of the table.
     * @return The table column list.
     */
    TableColumnList getParsedOutputAsTableRowList();

    /**
     * @brief parseTableOuput
     * @details This is meant to parse command line table outputs. It is assumed that the first line in the table are string with no
     * spaces and hence the beginning and end mark the beginning and end of the values for each column. So each row is parsed as a map
     * that contains a field for each header and the corresponding value in the table.
     * @param cmd - The command that will generate the table output.
     * @return true if no issues found. False otherwise.
     */
    bool parseCMDTableOuput(const QString &cmd_ouput);

private:
    ParseTableOutput result;
    QStringList errors;

};

#endif // TABLEOUTPUTPARSER_H
