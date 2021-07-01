#ifndef SSLLISTENER_H
#define SSLLISTENER_H

#include <QTcpServer>
#include <QSslSocket>

class SSLListener: public QTcpServer
{
    Q_OBJECT
public:
    SSLListener(QObject *parent = 0);

signals:
    void lostConnection();

protected:
    void incomingConnection(qintptr socketDescriptor);
};

#endif // SSLLISTENER_H
