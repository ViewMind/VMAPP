#include "devwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DevWindow w;
    w.show();
    return a.exec();
}
