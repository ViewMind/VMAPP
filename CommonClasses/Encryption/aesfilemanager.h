#ifndef AESFILEMANAGER_H
#define AESFILEMANAGER_H

#include <QFile>
#include <QDataStream>
#include <QElapsedTimer>
#include "aesengine.h"

class AESFileManager
{
public:
    AESFileManager();

    bool configure (const QString &key, AES::KeyType keyType, const QByteArray initializationVector);
    bool saveFile(QByteArray *content, const QString filename);
    bool loadFile(QByteArray *content, const QString filename);

    QString getLastError() const;

    // This value is in miliseconds.
    qint64 getLastOperationTime() const;

private:

    AESEngine aes;
    QString lastError;
    QElapsedTimer timer;
    qint64 lastOperationTime;

};

#endif // AESFILEMANAGER_H
