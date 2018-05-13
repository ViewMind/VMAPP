#ifndef SSLCLIENT_H
#define SSLCLIENT_H

#include <QMainWindow>
#include <QtNetwork/QSslSocket>
#include <QMetaEnum>

namespace Ui {
class SSLClient;
}

class SSLClient : public QMainWindow
{
    Q_OBJECT

public:
    explicit SSLClient(QWidget *parent = 0);
    ~SSLClient();

private slots:
    void on_pbConnect_clicked();
    void on_encryptedSuccess();
    void on_socketStateChanged(QAbstractSocket::SocketState state);
    void on_socketError(QAbstractSocket::SocketError error);
    void on_sslErrors(const QList<QSslError> &errors);

private:
    Ui::SSLClient *ui;
    QSslSocket *socket;
};

#endif // SSLCLIENT_H
