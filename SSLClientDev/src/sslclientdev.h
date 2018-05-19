#ifndef SSLCLIENTDEV_H
#define SSLCLIENTDEV_H

#include <QMainWindow>
#include "sslclient.h"

namespace Ui {
class SSLClientDev;
}

class SSLClientDev : public QMainWindow
{
    Q_OBJECT

public:
    explicit SSLClientDev(QWidget *parent = 0);
    ~SSLClientDev();

private:
    Ui::SSLClientDev *ui;
};

#endif // SSLCLIENTDEV_H
