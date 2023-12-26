#ifndef TABLEOUTPUTPARSER_H
#define TABLEOUTPUTPARSER_H

#include <QMap>
#include <QString>
#include <QList>
#include <QDebug>
#include <QVariantMap>

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

    /**
     * @brief parseCMDTableOutputTableWithDivider
     * @details Same as parseCMDTableOutput except the column division are defined by the second line which is a divider column.
     * So more logic needs to be used as the header are actually found by divving up the first line once the values for the second line are found.
     * @return true if no issues found. False otherwise.
     */
    bool parseCMDTableOutputTableWithDivider(const QString &cmd_output);

private:
    ParseTableOutput result;
    QStringList errors;

    QVariantMap findKeysAndKeyStartPosition(const QString &defineLine);
    void fillOutResult(const QStringList &keys, const QList<qint32> &key_start_positions, const QStringList &lines);
    QStringList getStringParsedByPostion(const QString &line, const QList<qint32> &key_start_positions);

};

#endif // TABLEOUTPUTPARSER_H
