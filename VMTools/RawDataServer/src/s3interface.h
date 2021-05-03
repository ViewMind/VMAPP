#ifndef S3INTERFACE_H
#define S3INTERFACE_H

#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QProcess>

#include "../../../CommonClasses/server_defines.h"
#include "../../../CommonClasses/LogInterface/loginterface.h"

#define SERVER_OUTPUT_FILE        "temp_output_file"
#define SERVER_WORK_DIR           "work"
#define SSH_USER_DNS              "ec2-user@18.220.30.34"
#define LOCAL_FLAG_FILE           "local_flag_file"


class S3Interface
{
public:

    struct S3LSReturn {
        QStringList directories;
        QStringList fileNames;
        QStringList dates;
        QStringList times;
        QList<quint64> sizes;
    };

    S3Interface(LogInterface *l);
    void setS3Bin(const QString &s3bin) {s3Address = "s3://" + s3bin;}

    // Commands.
    S3LSReturn listInPath(const QString &path);

    // Copies the files in the path to a directory copydir inside the current working directory. Copydir directory is erased first
    void copyRecursively(const QString &path, const QString outputPath);

private:
    QString s3Address;

    LogInterface *log;

    bool runShellCommands(const QStringList &shellCommands);

    bool runningLocally;

};

#endif // S3INTERFACE_H
