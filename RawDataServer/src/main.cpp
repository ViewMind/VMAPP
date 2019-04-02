#include <QCoreApplication>
#include <QDebug>


#define VIEWMIND_DATA_REPO        "viewmind-raw-eye-data"

#include "s3interface.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

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
}
