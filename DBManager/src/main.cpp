#include <QCoreApplication>
#include "control.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Control control(&a);
    //QObject::connect(&control,SIGNAL(exitRequested()),&a,SLOT(quit()),Qt::QueuedConnection);
    QObject::connect(&control,SIGNAL(exitRequested()),&a,SLOT(quit()));
    control.start();

    return a.exec();
}
