#include "sslserverwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SSLServerWindow w;
    w.show();

    return a.exec();
}
