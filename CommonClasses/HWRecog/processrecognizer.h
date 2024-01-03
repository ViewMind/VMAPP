#ifndef PROCESSRECOGNIZER_H
#define PROCESSRECOGNIZER_H

#include <QProcess>
#include "tableoutputparser.h"

class ProcessRecognizer
{
public:
    ProcessRecognizer();

    /**
     * @brief refreshProcessList - Get and parse the task list.
     * @return True if all good. False if there were errors.
     */
    bool refreshProcessList();

    /**
     * @brief getNumberOfInstancesRunningOf - Searches for a processe in the latest snapshot and returns how manty times it was found.
     * @param processName - The name of the process to search.
     * @return The number of times the process name was found. -1 means there was an error.
     */
    qint32 getNumberOfInstancesRunningOf(const QString &processName, bool searchAllColumns);


    TableOutputParser::TableColumnList getCurrentProcesses() const;
    QStringList getErrors() const;
    QString getLastTaskTable() const;


private:

    TableOutputParser::TableColumnList processesSnapShot;
    QStringList errors;
    QString lastTable;

    static inline const char * TASKLIST = "tasklist";

    static inline const char * COLUMN_PNAME  = "Image Name";
    static inline const char * COLUMN_PID    = "PID";
    static inline const char * COLUMN_SNAME  = "Session Name";
    static inline const char * COLUMN_SESNUM = "Session#";
    static inline const char * COLUMN_MEM    = "Mem Usage";

};

#endif // PROCESSRECOGNIZER_H
