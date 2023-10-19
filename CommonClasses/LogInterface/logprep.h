#ifndef LOGPREP_H
#define LOGPREP_H

/**
 * @brief The LogPrep class is basically a container for convenience function that will prepare logfiles for uploading to the server.
 */

#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QProcess>
#include <QDebug>

namespace UnityLogPaths {
   static const QString DIR = "%USERPROFILE%/AppData/LocalLow/ViewMind/RRS";
   static const QString LOG = "Player.log";
   static const QString PREVLOG = "Player-prev.log";
   static const QString LOCAL_LOG_PREFIX = "RRS_";
   static const QString LOCAL_PREVLOG_PREFIX = "RRS_prev_";
}

class LogPrep
{
public:

    LogPrep(const QString &logFileLocation, const QString inst_id, const QString inst_number);

    /**
     * @brief findAndCopyRRSLogs - Attempts to find the RRS Unity logs and copies them to the Log directory accordingly.
     * @return True if succssfull, false otherwise.
     */
    bool findAndCopyRRSLogs();

    /**
     * @brief findAndCopyLogFile - Checks existance of log file at specified location upon object creation. Uses the other paramters to create a backup and copies it to the local directory.
     * @return True if successfull, false otherwise.
     */
    bool findAndCopyLogFile();

    /**
     * @brief cleanLogDir - Deletes the log directory.
     */
    void cleanLogDir();

    /**
     * @brief createCompressedLogDirectory - Compresses the log directoy to a local file. It also cleans any compressed logs that might be present.
     * @return The full path of the created file if successfull. Empty string otherwise.
     */
    QString createCompressedLogDirectory();

    /**
     * @brief createFullLogBackup - Calls function findAndCopy and then compressed. Returns the created compressed file.
     * @return The created compressed file, if successfull. Or an empty string otherwise.
     */
    QString createFullLogBackup();

    /**
     * @brief getError - Gets the last error message.
     * @return - The last error message.
     */
    QString getError() const;

private:

    QString error;
    QString logDirectoryName;
    QString logDirectoryFullPath;
    QString compressedLogDirectory;
    QString logFileLocation;
    QString institutionID;
    QString instanceNumber;

    /**
     * @brief createLogDirectory - Checks if the log directory exists and creates it if it doesn't.
     * @return - True if successfull, false otherwise.
     */
    bool createLogDirectory();

    /**
     * @brief copyIfExists - Check if source exists. If it does it copies it to the destination. If it doesn't it does nothing but returns true.
     * @param source - The source path of the file.
     * @param destination - The destination path of the file
     * @return True if no errors. False otherwise.
     */
    bool copyIfExists(const QString &source, const QString &destination);

    /**
     * @brief cleanAndCreateCompressedLogDir - First deletes and then creates the compressed log directory.
     * @return true if the compressed log dir exists.
     */
    bool cleanAndCreateCompressedLogDir();

};

#endif // LOGPREP_H
