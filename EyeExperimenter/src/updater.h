#ifndef UPDATER_H
#define UPDATER_H

#include <QThread>
#include <QDir>
#include "eyexperimenter_defines.h"

class Updater : public QThread
{
public:
    Updater();
    void run();

private:

};

#endif // UPDATER_H
