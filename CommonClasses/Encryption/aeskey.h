#ifndef AESKEY_H
#define AESKEY_H

#include <QString>
#include <QByteArray>
#include <QDebug>
#include <QCryptographicHash>
#include <QList>
#include "aes.h"

class AESKey
{
public:

    AESKey();

    // Constructing an AES Key requires the "key phrase" and the type.
    // The constructor also expands the key.
    void create(const QString &key, AES::KeyType keyType);

    // This can be used when key construction is not carried out by this class.
    bool create(const QByteArray &key);

    // Get the Round Key.
    void getNextRoundKey(AES::State round_key);

    // We need to know what is the order for which to get keys.
    void resetKeyScheduling(bool forEncryption);

    // Retrieving the constants.
    quint8 getNk() const;
    quint8 getNr() const;

    // Retrieves the key size. A key type of 0 means that the key is invalid.
    quint16 getKeySize() const;

    // For debugging only.
    void printKeySchedule();

private:

    // Parameters of the standard.
    const quint8 Nb = 4; // Number of colums (32 bit words) in the state matrix.
    quint8 Nk;           // Number of 32 bit words comprising the key. Will depend on the key size.
    quint8 Nr;           // Number of rounds which will depend on the version of the algorithm which is the defined by the key size.

    // Flag to indicate key order.
    bool keyOrderForEncryption;

    // The generated round keys.
    AES::Word round_key_words[AES_MAX_ROUND_KEY_WORDS];     // This reserves data for as many round key words as the maximum key size which is 256.
    AES::Word key_words[AES_MAX_KEY_SIZE_WORDS];            // This reserves memory for the maximum number of words in the largest key size of 256
    AES::State round_keys[AES_MAX_N_KEYS_REQ];              // The maximum number of keys (rounds) that could be required by the algorithm, if key size is 256

    qint16 round_counter;

    // Used for computing the RConstants.
    // The zero at the beginning because the 0x01 is the first value used with a i value of 1.
    // Then 0x02 with an index value of 2 and so on.
    quint8 RC[11] = {0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36};

    // FIPS Standar functions used for key expansion.
    void SubWord(AES::Word word);
    void RotWord(AES::Word word);
    void XORRcon(AES::Word word, quint8 rcon_index);

    void initWords(QByteArray hashedKey);
    void KeyExpansion();


};

#endif // AESKEY_H
