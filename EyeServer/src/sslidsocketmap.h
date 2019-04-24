#ifndef SSLIDSOCKETMAP_H
#define SSLIDSOCKETMAP_H

#define  FULL_LOG
#include "sslidsocket.h"

class SSLIDSocketMap
{
public:
    SSLIDSocketMap();
    void addSocket(SSLIDSocket *s) { sockets[s->getID()] = s;  lock[s->getID()] = 0;}
    SSLIDSocket* getSocketLock(quint64 sid, const QString &where);
    void releaseSocket(quint64 sid, const QString &where);
    void deleteSocket(quint64 sid, const QString &where);

private:
    LogInterface logger;
    QHash<quint64,SSLIDSocket*> sockets;
    QHash<quint64,qint32> lock;
    QSet<quint64> deleteLater;
    void removeSocket(quint64 sid, const QString &where);
    QString deleteLaterContains(quint64 sid);
};

#endif // SSLIDSOCKETMAP_H
