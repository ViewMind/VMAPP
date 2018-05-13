#ifndef SSLMANAGER_H
#define SSLMANAGER_H

#include <QMetaEnum>
#include <QHash>
#include "../../CommonClasses/SSLIDSocket/sslidsocket.h"
#include "ssllistener.h"

class SSLManager: public QObject
{
    Q_OBJECT
public:
    SSLManager(QObject *parent = Q_NULLPTR);

    // Start the server
    void startServer(quint16 port);

    // Get all messages.
    QStringList getMessages();

private slots:
    void on_newConnection();
    void on_newSSLSignal(quint64 socket, quint8 signaltype);

signals:
    void newMessages();

private:

    // Listens for incoming connections and enables de SSL capabilities by using a QSSLSocket
    SSLListener *listener;

    // The queue of information that needs to be processed
    QHash<quint64,SSLIDSocket*> sockets;
    QList<quint64> queue;

    // Generates ever increasing values.
    quint64 idGen;

    // The list of errors
    QStringList messages;

    // Helper fucntion to generate messages.
    void changedState(quint64 id);
    void socketErrorFound(quint64 id);
    void sslErrorsFound(quint64 id);

    // Remove form the map and the list
    void removeSocket(quint64 id);

    // Emmitting the message signal
    void addMessage(const QString &type, const QString &msg);

};

#endif // SSLMANAGER_H
