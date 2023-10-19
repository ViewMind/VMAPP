#include <QCoreApplication>

#include "../../../CommonClasses/LogInterface/logprep.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // We do this to make sure the log file exists.
    QFile lfile("logfile.log");
    lfile.open(QFile::WriteOnly);
    QTextStream writer(&lfile);
    writer << "Writing some stuff to the log file";
    lfile.close();

    LogPrep lp("logfile.log","1","9");

//    if (!lp.findAndCopyRRSLogs()){
//        qDebug() << "ERROR copying RRS log files: " << lp.getError();
//        return a.exec();
//    }

//    if (!lp.findAndCopyLogFile()){
//        qDebug() << "ERROR copying local lof file: " << lp.getError();
//        return a.exec();
//    }

//    QString createdFile = lp.createCompressedLogDirectory();

//    if (createdFile == ""){
//        qDebug() << "ERROR failed in createing the compressed log directory " << lp.getError();
//        return a.exec();
//    }

    QString createdFile = lp.createFullLogBackup();
    lp.cleanLogDir();
    qDebug() << "Done: Compreseed log can be found at " << createdFile;

    return a.exec();
}
