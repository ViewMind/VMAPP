#include <QCoreApplication>
#include "inputcontrol.h"
#include "servercontrol.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // The actual server that will listen for connections.
    ServerControl scontrol(&a);
    QObject::connect(&scontrol,SIGNAL(exitRequested()),&a,SLOT(quit()),Qt::QueuedConnection);
    scontrol.startServer();

    // Listen to the input ...
    InputControl icontrol(&a);
    QObject::connect(&icontrol,SIGNAL(exitRequested()),&a,SLOT(quit()),Qt::QueuedConnection);
    icontrol.start();

    return a.exec();
}
