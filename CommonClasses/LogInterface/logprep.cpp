#include "logprep.h"

LogPrep::LogPrep(const QString &logFileLocation, const QString inst_id, const QString inst_number) {

    this->logDirectoryName = "logs_" + inst_id + "_" + inst_number;

    QDir dir(logDirectoryName);
    this->logDirectoryFullPath = dir.absolutePath();

    QDir dir2("compressed_logs");
    this->compressedLogDirectory = dir2.absolutePath();

    QFileInfo fileinfo(logFileLocation);
    this->logFileLocation = fileinfo.absoluteFilePath();

    this->instanceNumber = inst_number;
    this->institutionID = inst_id;

    //qDebug() << "The log's absolute file path is " << this->logFileLocation;
}

QString LogPrep::getError() const {
    return this->error;
}

QString LogPrep::createFullLogBackup() {
    if (!this->findAndCopyLogFile()) return "";
    if (!this->findAndCopyRRSLogs()) return "";
    return this->createCompressedLogDirectory();
}

bool LogPrep::findAndCopyRRSLogs(){

    if (!this->createLogDirectory()) return false;

    QString unityDirPath = UnityLogPaths::DIR;
    unityDirPath = unityDirPath.replace("%USERPROFILE%",QDir::homePath());

    if (!QDir(unityDirPath).exists()){
        this->error = "Unable to locate the Unity Log Directory at: '" + unityDirPath + "'";
        return false;
    }

    QString logFilePath = unityDirPath + "/" + UnityLogPaths::LOG;
    QString prevLogFilePath = unityDirPath + "/" + UnityLogPaths::PREVLOG;

    QString ts = QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss");
    QString localRRSLogPath = this->logDirectoryFullPath + "/" + UnityLogPaths::LOCAL_LOG_PREFIX + ts + ".log";
    QString localRRSPrevLogPath = this->logDirectoryFullPath + "/" + UnityLogPaths::LOCAL_PREVLOG_PREFIX + ts + ".log";

    if (!this->copyIfExists(logFilePath,localRRSLogPath)) return false;
    if (!this->copyIfExists(prevLogFilePath,localRRSPrevLogPath)) return false;

    return true;
}

bool LogPrep::findAndCopyLogFile(){

    if (!this->createLogDirectory()) return false;

    if (!QFile(this->logFileLocation).exists()){
        this->error = "Unable to find LogFile at location: " + this->logFileLocation;
        return false;
    }

    QString ts = QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss");
    QString bkpLocation = this->logDirectoryFullPath + "/logfile_" + ts + ".log";

    QFile::copy(this->logFileLocation,bkpLocation);
    if (!QFile(bkpLocation).exists()){
        this->error = "Unable to copy main log file from '" + this->logFileLocation + "' to '" + bkpLocation + "'";
        return false;
    }

    // We try to remove the current log.
    QFile(this->logFileLocation).remove();

    return true;

}

QString LogPrep::createCompressedLogDirectory(){

    // First we make sure any other compressed log file is clean.
    if (!cleanAndCreateCompressedLogDir()) return "";

    QString ts = QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss");
    QString compressedTargeFile = this->compressedLogDirectory + "/" +  this->institutionID + "_" + this->instanceNumber + "_" + ts + ".tar.gz";

    QStringList arguments;
    arguments << "-c";
    arguments << "-z";
    arguments << "-f";
    arguments << compressedTargeFile;
    arguments << this->logDirectoryName;

    QProcess tar;
    //tar.setWorkingDirectory(QDir());
    tar.start("tar.exe",arguments);
    tar.waitForFinished();

    qint32 exit_code = tar.exitCode();
    QString tarOuput = "STD: " + QString(tar.readAllStandardOutput()) + "\n";
    tarOuput = tarOuput + "ERR: " + QString(tar.readAllStandardOutput());

    if (!QFile(compressedTargeFile).exists()){
        this->error = "Failed in creating the compressed log directory. Tar Exit code: " + QString::number(exit_code) + ". Tar output: " + tarOuput;
        return "";
    }

    return compressedTargeFile;

}

void LogPrep::cleanLogDir(){
    qDebug() << "Cleaning dir" << this->logDirectoryFullPath;
    QDir dir(this->logDirectoryFullPath);
    dir.removeRecursively();
}

bool LogPrep::cleanAndCreateCompressedLogDir() {
    QDir dir(this->compressedLogDirectory);
    dir.removeRecursively();
    dir.mkdir(".");
    if (!dir.exists()){
        this->error = "Unable to create compressed log directory at '" + this->compressedLogDirectory + "'";
        return false;
    }
    return true;
}

bool LogPrep::copyIfExists(const QString &source, const QString &destination){
    if (QFile(source).exists()) {
        QFile::copy(source,destination);
        // It is very possible for the log file to
        if (!QFile(destination).exists()){
            this->error = "Unable to copy file at '" + source + "' to it's destination at '" + destination + "'";
            return false;
        }
    }
    return true;
}

bool LogPrep::createLogDirectory(){
    QDir dir(this->logDirectoryFullPath);
    dir.mkdir(".");
    if (!dir.exists()){
        this->error = "Was unable to create log directory '" + this->logDirectoryFullPath + "'";
        return false;
    }
    return true;
}
