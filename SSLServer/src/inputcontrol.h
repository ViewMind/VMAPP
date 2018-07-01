#ifndef INPUTCONTROL_H
#define INPUTCONTROL_H

#include <QThread>
#include <iostream>
#include "../../CommonClasses/LogInterface/loginterface.h"

#define  PROGRAM_NAME      "EyeSever"
#define  PROGRAM_VERSION   "2.0.1"

class InputControl : public QThread
{
    Q_OBJECT

public:
    explicit InputControl(QObject *parent = 0);
    void run();

signals:
    void exitRequested();

private:
    void printGreeting();
};

#endif // INPUTCONTROL_H
