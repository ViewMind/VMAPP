#include <QCoreApplication>
#include <QRegularExpression>

#include "iostream"
#include "../../../CommonClasses/HWRecog/hwrecognizer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    HWRecognizer recog;

    if (!recog.getErrors().empty()){
        qDebug() << "ERRORS";
        qDebug().noquote() << recog.getErrors().join("\n");
    }

//    HWRecognizer::HardwareMap hw = recog.getHardwareSpecs();
//    QStringList keys = hw.keys();
//    for (qint32 i = 0; i < keys.size(); i++){
//        qDebug() << keys.at(i) << "->" << hw.value(keys.at(i));
//    }

    // qDebug() << recog.toString(false);

    qDebug().noquote() << recog.toString(true);

    //    QProcess process;
    //    QString path = "wmic";
    //    QString arg  = "cpu get name";
    //    QStringList arguments;
    //    arguments << "cpu" << "get" << "name";

    //    process.start(path,arguments);
    //    if (!process.waitForFinished()){
    //        qDebug() << "ERROR" << process.errorString();
    //    }
    //    else {
    //        qDebug() << "Printing output";
    //        qDebug().noquote() << process.readAllStandardOutput();
    //    }
    //    qDebug() << "Finished";

    return 0;


    //return a.exec();
}
