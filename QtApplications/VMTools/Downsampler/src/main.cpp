#include "eyedatadownsampler.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    EyeDataDownSampler w;
    w.show();
    return a.exec();
}
