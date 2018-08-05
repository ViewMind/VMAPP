#include <QCoreApplication>
#include <QTest>

#include "sslwrapper.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qWarning() << "Starting";
    SSLWrapper wrapper(nullptr);

//    wrapper.addSomeRandomDoctorData(2);

//    qWarning() << "Waiting for 10 seconds before creating patient data";
//    QTest::qWait(10000);

//    qWarning() << "Creating patient data";
//    wrapper.sendSomeRandomPatientData(10,true);


//    QStringList uids;
//    uids << "AR20584968" << "AR22912155";
//    wrapper.requestPatientData(uids);

    wrapper.sendDataToProcess();

    qWarning() << "Finished!";

    return a.exec();
}
