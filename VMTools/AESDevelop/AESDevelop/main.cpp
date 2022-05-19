#include <QCoreApplication>
#include <QDir>
#include "../../../CommonClasses/Encryption/aesengine.h"
#include "../../../CommonClasses/Encryption/aesfilemanager.h"

#define AES_KEY_EXPANSION_128       "2b7e151628aed2a6abf7158809cf4f3c"
#define AES_KEY_EXPANSION_192       "8e73b0f7da0e6452c810f32b809079e562f8ead2522c6b7b"
#define AES_KEY_EXPANSION_256       "603deb1015ca71be2b73aef0857d77811f352c073b6108d72d9810a30914dff4"

#define AES_EXAMPLE_PLAINTEXT       "00112233445566778899aabbccddeeff"
#define AES_128_EXAMPLE_KEY         "000102030405060708090a0b0c0d0e0f"
#define AES_192_EXAMPLE_KEY         "000102030405060708090a0b0c0d0e0f1011121314151617"
#define AES_256_EXAMPLE_KEY         "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString password = "I'm a very very bad boy";
    QByteArray initializationVector;
    for (quint8 i = 0; i < AES_STATE_SIZE; i++) initializationVector.append(i);
    AES::KeyType keyType = AES::AES_128;

    QString source_directory    = "test_files/original";
    QString encrypted_directory = "test_files/encrypted";
    QString decrypted_directory = "test_files/decrypted";

    AESFileManager fileManager;
    if (!fileManager.configure(password,keyType,initializationVector)){
        qDebug() << "Failed to configure file manager. Reason" << fileManager.getLastError();
        return 0;
    }

    QStringList filters; //filters << "*.json";
    QStringList source_files = QDir(source_directory).entryList(filters,QDir::NoDotAndDotDot|QDir::Files);
    for (qint32 i = 0; i < source_files.size(); i++){
        QString original_filename = source_directory + "/" + source_files.at(i);
        qDebug() << "ORIGINAL FILE" << original_filename;


        qDebug() << "   Encrypting ....";
        qDebug() << "      Loading Original File";
        QString saveTo = encrypted_directory + "/" + source_files.at(i);
        QFile efile(original_filename);
        if (!efile.open(QFile::ReadOnly)){
            qDebug() << "      Failed to open " << original_filename << " for encrypting";
            return 0;
        }
        QByteArray content;
        QTextStream reader(&efile);
        content = reader.readAll().toLatin1();
        efile.close();

        qDebug() << "      Encrypting Original File. Size " << content.size();
        if (!fileManager.saveFile(&content,saveTo)){
            qDebug() << "      Failed saving encrypted file. Reson: " << fileManager.getLastError();
            return 0;
        }
        qDebug() << "      Saved File To " << saveTo << " Encrypted Size: " << content.size();
        qDebug() << "      Encryption Time: " << fileManager.getLastOperationTime() << " ms";
        content.clear();


        qDebug() << "  Decrypting ...";
        QString src = saveTo;
        saveTo = decrypted_directory + "/" + source_files.at(i);
        qDebug() << "      Decrypting file " << src;
        if (!fileManager.loadFile(&content,src)){
            qDebug() << "      Failed to decrypt file. Resason" << fileManager.getLastError();
            return 0;
        }
        qDebug() << "      Decryption Successfull. Decrypted Size: " << content.size();
        qDebug() << "      Decryption Time: " << fileManager.getLastOperationTime() << "ms. Storing decrypted file";
        QFile dfile(saveTo);
        if (!dfile.open(QFile::WriteOnly)){
            qDebug() << "      Failed to open decryption store file" << saveTo << "for writing";
        }
        QTextStream writer(&dfile);
        writer << content;
        dfile.close();
        qDebug() << "      Decrypted file saved to" << saveTo;

    }


//    AESEngine aes;
//    //aes.enableDebugOutput(true);

//    QByteArray key = AES::HexStringToByteArray(AES_128_EXAMPLE_KEY);
//    QByteArray iv;
//    for (quint8 i = 0; i < AES_STATE_SIZE; i++) iv.append(i);
//    aes.configure(key,iv);

//    QString plaintext = "This is a message that doesn't say a whole lot. Nice try. Hope it succeeds!!!!!";
//    //QString plaintext = "abcdefghijklmnopq";
//    QByteArray plaintext_bytes = plaintext.toLatin1();

//    qDebug() << "Size in bytes of before encryption" << plaintext_bytes.size();

//    aes.encrypt(&plaintext_bytes);

//    qDebug() << "Size in bytes of encrypted text" << plaintext_bytes.size();
//    qDebug() << "Encrypted Message: " << QString(plaintext_bytes);

//    // Now we decrypt.
//    if (!aes.decrypt(&plaintext_bytes)){
//        qDebug() << "Failed decrypting. Reason: " << aes.getLastError();
//        qDebug() << "Size after Decryption: " << plaintext_bytes.size();
//        qDebug() << "Decrypted Message: " << QString(plaintext_bytes);
//        return 0;
//    }

//    qDebug() << "Size after Decryption: " << plaintext_bytes.size();
//    qDebug() << "Decrypted Message: " << QString(plaintext_bytes);


    //return a.exec();
    qDebug() << "Finished";
    return 0;
}
