#include "filedldev.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FileDLDev w;
    w.show();
    return a.exec();
}
