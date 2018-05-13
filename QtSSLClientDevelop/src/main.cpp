#include "sslclientwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SSLClient w;
    w.show();

    return a.exec();
}
