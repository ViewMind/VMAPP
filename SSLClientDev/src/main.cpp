#include "sslclientdev.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SSLClientDev w;
    w.show();

    return a.exec();
}
