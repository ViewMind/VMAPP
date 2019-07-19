#include <QCoreApplication>
#include "control.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Control control(&a);
    QObject::connect(&control,SIGNAL(done()),&a,SLOT(quit()),Qt::QueuedConnection);
    control.run();

    return a.exec();
}
