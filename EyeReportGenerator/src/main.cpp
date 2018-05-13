#include "eyereportui.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    EyeReportUI w;
    w.show();

    return a.exec();
}
