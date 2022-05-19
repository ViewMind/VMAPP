#include "aesfilemanager.h"

AESFileManager::AESFileManager()
{
    lastError = "";
    lastOperationTime = 0;
}

QString AESFileManager::getLastError() const{
    return this->lastError;
}

qint64 AESFileManager::getLastOperationTime() const {
    return this->lastOperationTime;
}

bool AESFileManager::configure(const QString &key, AES::KeyType keyType, const QByteArray initializationVector){
    if (!aes.configure(key,keyType,initializationVector)){
        this->lastError = "Failed to configure AES Engine. Reason: " + aes.getLastError();
        return false;
    }
    aes.enableTimeProfile(false);
    return true;
}

bool AESFileManager::saveFile(QByteArray *content, const QString filename){

    timer.start();

    // First we open the file.
    QFile file(filename);
    if (!file.open(QFile::WriteOnly)){
        this->lastError = "Failed to open file '" + filename + "' for writing";
        return false;
    }

    // Then we do the encryption.
    if (!aes.encrypt(content)){
        this->lastError = "Encryption failed. Reason: " + aes.getLastError();
        file.close();
        return false;
    }

    // Data is now encrypted and must be saved.
    QDataStream writer(&file);
    writer << *content;
    file.close();

    lastOperationTime = timer.elapsed();

    return true;

}


bool AESFileManager::loadFile(QByteArray *content, const QString filename){

    timer.start();

    // First we open the file.
    QFile file(filename);
    if (!file.open(QFile::ReadOnly)){
        this->lastError = "Failed to open file '" + filename + "' for reading";
        return false;
    }

    // Now we load the data into the provided variable.
    QDataStream reader(&file);
    reader >> *content;
    file.close();

    // Then we do the decryption.
    if (!aes.decrypt(content)){
        this->lastError = "Decryption failed. Reason: " + aes.getLastError();
        file.close();
        return false;
    }

    lastOperationTime = timer.elapsed();

    return true;

}
