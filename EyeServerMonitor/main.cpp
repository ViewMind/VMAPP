#include <QCoreApplication>
#include "control.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Control c(&a);
    QObject::connect(&c,SIGNAL(exitRequested()),&a,SLOT(quit()),Qt::QueuedConnection);
    c.launchServer();
    return a.exec();
}
