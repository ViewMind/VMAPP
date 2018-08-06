#include "bindinggenerator.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BindingGenerator w;
    w.show();

    return a.exec();
}
