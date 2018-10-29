#include "dbview.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DBView w;
    w.show();

    return a.exec();
}
