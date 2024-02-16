#include <QCoreApplication>
#include <QRegularExpression>

#include "iostream"
#include "../../../CommonClasses/HWRecog/hwrecognizer.h"
#include "../../../CommonClasses/HWRecog/processrecognizer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //=========================================================== HW Recog DEV ================================
    HWRecognizer recog;

    if (!recog.getErrors().empty()){
        qDebug() << "ERRORS";
        qDebug().noquote() << recog.getErrors().join("\n");
    }

    if (!recog.getWarnings().empty()){
        qDebug() << "WARNINGS";
        qDebug().noquote() << recog.getWarnings().join("\n");
    }

//    // We check if there is soemting to search.
//    QString filename = "search.txt";
//    QString search_for = "";
//    if (QFile::exists(filename)){
//        QFile file(filename);
//        if (!file.open(QFile::ReadOnly)){
//            qDebug() << "Cannot open search file for reading";
//            return 0;
//        }
//        QTextStream reader(&file);
//        search_for = reader.readAll();
//        file.close();
//    }
//    else {
//        qDebug() << "No search file found. Exiting";
//        return 0;
//    }

    qDebug().noquote() << recog.toString(true);

//    //QString search_for = "8CC047Z0HR";
//    search_for = search_for.trimmed();
//    qDebug().noquote().nospace() << "Searching For string '" << search_for << "'";
//    recog.searchForADeviceWithPropertyValue(search_for);


//    //=============================== Task List Table DEV ================================
//    ProcessRecognizer pr;
//    pr.refreshProcessList();

//    qDebug() << pr.getCurrentProcesses();

    return 0;

}
