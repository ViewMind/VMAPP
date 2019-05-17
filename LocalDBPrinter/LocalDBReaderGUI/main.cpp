#include "localdbreader.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LocalDBReader w;
    w.show();

    return a.exec();
}
