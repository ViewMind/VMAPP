#include "openvrcontrolwindow.h"

OpenVRControlWindow::OpenVRControlWindow(QWidget *parent) : QMainWindow(parent)
{
    centralwidget = new QWidget(this);
    gridLayout = new QGridLayout(centralwidget);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    gridLayout->setContentsMargins(0, 0, 0, 0);
    openGLWidget = new OpenVRControl(centralwidget);
    openGLWidget->setObjectName(QString::fromUtf8("openGLWidget"));

    gridLayout->addWidget(openGLWidget, 0, 0, 1, 1);

    this->setCentralWidget(centralwidget);

    // This makes sure that the window is too small to see, but that it remais ALWAYS visible.
    this->setGeometry(0,0,1,1);
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::X11BypassWindowManagerHint|Qt::Window);
}


void OpenVRControlWindow::startTest(){
    this->show();
    openGLWidget->start();
}

void OpenVRControlWindow::stopTest(){
    openGLWidget->stop();
}


