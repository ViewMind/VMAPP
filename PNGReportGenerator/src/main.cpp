#include "pngreportgenerator.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PNGReportGenerator w;
    w.show();

    return a.exec();
}
