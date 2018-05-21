#include "reportviewerdev.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ReportViewerDev w;
    w.show();

    return a.exec();
}
