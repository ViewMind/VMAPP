#ifndef OPENVRCONTROLWINDOW_H
#define OPENVRCONTROLWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QWidget>
#include "openvrcontrol.h"

/// The control window is necessary because without it, information was sent incorrectly to the HMD
/// Could never figure out why.

class OpenVRControlWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit OpenVRControlWindow(QWidget *parent = nullptr);    

    Q_INVOKABLE void startTest();
    Q_INVOKABLE void stopTest();

signals:

public slots:

private:
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    OpenVRControl *openGLWidget;
};

#endif // OPENVRCONTROLWINDOW_H
