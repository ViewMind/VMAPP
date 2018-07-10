#include <QCoreApplication>

#include "sslwrapper.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qWarning() << "Starting";
    SSLWrapper wrapper(nullptr);

//    wrapper.sendSomeRandomPatientData(10,true);
//    wrapper.addSomeRandomDoctorData(10);

//    QStringList uids;
//    uids << "AR21447144" << "AR31775787";
//    wrapper.requestPatientData(uids);

    wrapper.sendDataToProcess();

    return a.exec();
}
