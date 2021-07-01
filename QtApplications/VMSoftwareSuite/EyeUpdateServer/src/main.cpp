#include <QCoreApplication>
#include "servercontrol.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // The actual server that will listen for connections.
    ServerControl scontrol(&a);
    QObject::connect(&scontrol,SIGNAL(exitRequested()),&a,SLOT(quit()),Qt::QueuedConnection);
    scontrol.startServer();

    std::cout << "================" << EYE_UPDATE_SERVER_NAME << "================" << std::endl;
    std::cout << "| Version: " << EYE_UPDATE_SERVER_VERSION << std::endl;
    std::cout << "| Log file: " << LOG_FILE_LOG << std::endl;
    std::cout << "| CTRL-C to kill the server" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << ">> ";

    return a.exec();
}
