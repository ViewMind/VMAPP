#include <QCoreApplication>
#include <QDebug>

#include "control.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#ifdef COMPILE_FOR_S3_TEST

    S3Interface::S3LSReturn lsRet;
    S3Interface s3; s3.setS3Bin(VIEWMIND_DATA_REPO);

    lsRet = s3.listInPath("AR21483418/");
    for (qint32 i = 0; i < lsRet.directories.size(); i++){
        qWarning() << lsRet.directories.at(i);
    }

    for (qint32 i = 0; i < lsRet.fileNames.size(); i++){
        qWarning() << lsRet.fileNames.at(i) << lsRet.dates.at(i) << lsRet.sizes.at(i) << lsRet.times.at(i);
    }

    s3.copyRecursively("AR21483418/");

    qWarning() << "Finished";
    return a.exec();
#else
    Control control(&a);
    QObject::connect(&control,SIGNAL(exitRequested()),&a,SLOT(quit()),Qt::QueuedConnection);
    control.startServer();

#endif

    std::cout << "================" << RAW_DATA_SERVER_NAME << "================" << std::endl;
    std::cout << "| Version: " << RAW_DATA_SERVER_VERSION << std::endl;
    std::cout << "| CTRL-C to kill the server" << std::endl;
    std::cout << "========================================" << std::endl;

    return a.exec();
}
