#ifndef SSLSERVERWINDOW_H
#define SSLSERVERWINDOW_H

#include <QMainWindow>


#include "../../CommonClasses/DataPacket/datapacket.h"
#include "../../CommonClasses/LogInterface/loginterface.h"
#include "sslmanager.h"

namespace Ui {
class SSLServerWindow;
}

class SSLServerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SSLServerWindow(QWidget *parent = 0);
    ~SSLServerWindow();

private slots:
    void on_messagesAvailable();

private:
    Ui::SSLServerWindow *ui;
    SSLManager sslManager;
    LogInterface log;
    ConfigurationManager config;
};

#endif // SSLSERVERWINDOW_H
