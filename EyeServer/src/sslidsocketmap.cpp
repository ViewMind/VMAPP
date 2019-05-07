#include "sslidsocketmap.h"

SSLIDSocketMap::SSLIDSocketMap()
{

}

SSLIDSocket* SSLIDSocketMap::getSocketLock(quint64 sid, const QString &where){
#ifdef FULL_LOG
    logger.appendStandard("SOCKET " + QString::number(sid) +  " @ " + where  + ": Getting Lock: " + QString::number(lock.value(sid))+ ". DL: " + deleteLaterContains(sid));
#else
    Q_UNUSED(where)
#endif
    if (sockets.contains(sid) && !deleteLater.contains(sid)){
        lock[sid]++;
        return sockets.value(sid);
    }
    else return nullptr;
}

void SSLIDSocketMap::releaseSocket(quint64 sid, const QString &where){
#ifdef FULL_LOG
    logger.appendStandard("SOCKET " + QString::number(sid) +  " @ " + where  + ": Releasing Lock: " + QString::number(lock.value(sid)) + ". DL: " + deleteLaterContains(sid));
#endif
    if (sockets.contains(sid)){        
        lock[sid]--;
        if (deleteLater.contains(sid) && (lock.value(sid) == 0)){
            removeSocket(sid,where);
        }
    }
}

void SSLIDSocketMap::deleteSocket(quint64 sid, const QString &where){
#ifdef FULL_LOG
    logger.appendStandard("SOCKET " + QString::number(sid) +  " @ " + where  + ": Request deletion " + QString::number(lock.value(sid))+ ". DL: " + deleteLaterContains(sid));
#endif
    if (sockets.contains(sid)){
        if (lock.value(sid) == 0) removeSocket(sid,where);
        else deleteLater << sid;
    }
}

void SSLIDSocketMap::removeSocket(quint64 sid, const QString &where){
    if (sockets.contains(sid)) {
#ifdef FULL_LOG
        logger.appendStandard("SOCKET " + QString::number(sid) +  " @ " + where  + ": effective deletion ");
#else
    Q_UNUSED(where)
#endif
        sockets.remove(sid);
        lock.remove(sid);
        deleteLater.remove(sid);
    }
}

QString SSLIDSocketMap::deleteLaterContains(quint64 sid){
    if (deleteLater.contains(sid)) return "true";
    else return "false";
}
