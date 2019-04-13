#include "s3interface.h"

S3Interface::S3Interface()
{

}

void S3Interface::copyRecursively(const QString &path, const QString outputPath){

    QString aws_command;
    aws_command = "aws s3 cp --recursive " + s3Address + "/" + path + " " + QString(SERVER_WORK_DIR)  + "/" + outputPath;

    QStringList shellComamnds;

#ifdef SERVER_LOCALHOST
    shellComamnds << "ssh -i " + QString(SSH_KEY_LOCATION) +  " " + QString(SSH_USER_DNS) + " \" rm -rf " + QString(SERVER_WORK_DIR)  + "\"";
    shellComamnds << "ssh -i " + QString(SSH_KEY_LOCATION) +  " " + QString(SSH_USER_DNS) + " \"" + aws_command + "\"";
    shellComamnds << "scp -i " + QString(SSH_KEY_LOCATION) +  " -q -r " + QString(SSH_USER_DNS) + ":\""+ QString(SERVER_WORK_DIR)  + "\" . ";
#else
    shellComamnds << aws_command;
#endif

    runShellCommands(shellComamnds);

}


bool S3Interface::runShellCommands(const QStringList &shellCommands){
    LogInterface log;
    for (qint32 i = 0; i < shellCommands.size(); i++){
        log.appendStandard("AWS CP DIR: RUNNING COMMAND: " + shellCommands.at(i));
        QProcess process;
//        process.start(shellCommands.at(i));
        process.start("sh",QStringList() << "-c" << shellCommands.at(i));
        process.closeReadChannel(QProcess::StandardOutput);
        process.closeReadChannel(QProcess::StandardError);
        process.waitForFinished(60000000);
        qint32 result = process.exitCode();
        if (result != 0){
           log.appendError("AWS CP DIR: COMMAND RESULT IS: " + QString::number(result));
           return false;
        }
    }
    return true;
}

S3Interface::S3LSReturn S3Interface::listInPath(const QString &path){

    QStringList cmdList;
    S3LSReturn ans;

    QString aws_command;
    aws_command = "aws s3 ls " + s3Address + "/" + path;

#ifdef SERVER_LOCALHOST
    cmdList << "ssh -i " + QString(SSH_KEY_LOCATION) +  " " + QString(SSH_USER_DNS) + " \"" + aws_command + " > " + QString(SERVER_OUTPUT_FILE) + "\"";
    cmdList << "scp -i " + QString(SSH_KEY_LOCATION) +  " -q " + QString(SSH_USER_DNS) + ":\"" + QString(SERVER_OUTPUT_FILE) + "\" . ";
#else
    cmdList << aws_command + " > " + QString(SERVER_OUTPUT_FILE);
#endif

    if (!runShellCommands(cmdList)) return ans;

    LogInterface log;
    QFile file(SERVER_OUTPUT_FILE);
    if (!file.open(QFile::ReadOnly)) {
        log.appendError("AWS LIST: Could not open temp output file for reading");
        return ans;
    }
    QTextStream reader(&file);
    QString contents = reader.readAll();
    file.close();

    QStringList lines = contents.split("\n",QString::SkipEmptyParts);
    for (qint32 i = 0; i < lines.size(); i++){

        QString line = lines.at(i).trimmed();

        if (line.isEmpty()) continue;

        QStringList parts = line.split(" ",QString::SkipEmptyParts);

        if (parts.size() == 2){
            // DIR ENTRY
            ans.directories << parts.last();
        }
        else if (parts.size() == 4){
            // FILE ENTRY
            ans.dates << parts.at(0);
            ans.fileNames << parts.at(3);
            ans.sizes << parts.at(2).toUInt();
            ans.times << parts.at(1);
        }
        else{
            log.appendError("AWS LIST: UNRECOGNIZED AWS LS Entry format: " + lines.at(i));
        }
    }

    // Deleting the temp file
    file.remove();

    return ans;

}
