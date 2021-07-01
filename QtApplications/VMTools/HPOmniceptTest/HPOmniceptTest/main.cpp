#include <QCoreApplication>
#include <QDebug>

#include <QElapsedTimer>
#include <QFile>
#include <QTextStream>
#include "receiver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    HPOmnicentProvider hpprovider;
    Receiver receiver;
    QObject::connect(&hpprovider,&HPOmnicentProvider::eyeDataAvailable,&receiver,&Receiver::newEyeData);

    if (!hpprovider.connectToHPRuntime()){
        qDebug() << "Could not connect to HP Runtime: " << hpprovider.getError();
        return 0;
    }
    qDebug() << "Connected to runtime. Starting Printer";
    hpprovider.start();

    if (hpprovider.getError() != ""){
        qDebug() << "HP Provider error" << hpprovider.getError();
    }

    return a.exec();
}

