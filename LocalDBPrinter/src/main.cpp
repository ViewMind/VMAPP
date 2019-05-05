#include <QCoreApplication>
#include <QVariantMap>
#include "../../CommonClasses/LocalInformationManager/localinformationmanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //************************ Getting the path to the local DB file
    QStringList args = QCoreApplication::arguments();
    QString inputDB;
    if (args.size() == 2){
        inputDB = args.last();
    }
    else {
        qDebug() << "There should be one and only one argument, the path to the local DB file";
        exit(1);
    }

    //************************ Loading the local DB
    LocalInformationManager lim;
    lim.setWorkingFile(inputDB);
    lim.printDBToConsole();

    exit(0);
    return a.exec();
}
