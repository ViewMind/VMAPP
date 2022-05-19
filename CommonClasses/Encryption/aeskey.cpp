#include "aeskey.h"

AESKey::AESKey(){
    Nk = 0;
    Nr = 0;

}

void AESKey::create(const QString &key, AES::KeyType keyType){

    quint16 key_length = 0;

    // I don't really think it's a good idea. But I need a generic way of generating the key
    QByteArray hashed_key;
    QByteArray hash_result = QCryptographicHash::hash(key.toLatin1(),QCryptographicHash::Sha3_512);

    switch (keyType) {
    case AES::AES_128:
        Nk = 4;
        Nr = 10;
        key_length = 128;
        break;
    case AES::AES_192:
        Nk = 6;
        Nr = 12;
        key_length = 192;
        break;
    case AES::AES_256:
        Nk = 8;
        Nr = 14;
        key_length = 256;
        break;
    }

    // Taking only the first bytes that I require.
    for (quint16 i = 0; i < key_length; i++){
        hashed_key.append(hash_result.at(i));
    }

    initWords(hashed_key);
    KeyExpansion();

}

bool AESKey::create(const QByteArray &key){
    switch (key.size()){
    case 16:
        Nk = 4;
        Nr = 10;
        break;
    case 24:
        Nk = 6;
        Nr = 12;
        break;
    case 32:
        Nk = 8;
        Nr = 14;
        break;
    default:
        Nk = 0;
        Nr = 0;
    }

    if (Nk != 0){
        initWords(key);
        KeyExpansion();
        return true;
    }

    return false;
}

void AESKey::initWords(QByteArray hashedKey){

    quint8 nwords = hashedKey.size()/4; // Each word has 4 bytes.
    //qDebug() << "Init Words. Key Size in Words: " << nwords;

    quint8 word_index = 0;
    quint8 byte_index = 0;

    for (quint8 k = 0; k < hashedKey.size(); k++){
        key_words[word_index][byte_index] = hashedKey.at(k);
        byte_index++;
        if (byte_index == AES_WORD_SIZE){
            byte_index = 0;
            word_index++;
        }
    }

    //qDebug() << "Initialzing round key words";

    // Resetting the round key words.
    for (quint8 w = 0; w < AES_MAX_ROUND_KEY_WORDS; w++){
        for (quint8 j = 0; j < AES_WORD_SIZE; j++){
            round_key_words[w][j] = 0;
        }
    }

    // Resetting the actual round keys
    for (quint8 i = 0; i < AES_MAX_N_KEYS_REQ; i++){
        for (quint8 j = 0; j < AES_STATE_SIZE; j++){
            round_keys[i][j] = 0;
        }
    }


}


quint8 AESKey::getNk() const{
    return Nk;
}

quint8 AESKey::getNr() const {
    return Nr;
}

quint16 AESKey::getKeySize() const {
    return Nk*32;
}


void AESKey::SubWord(AES::Word word){
    for (quint8 i = 0; i < AES_WORD_SIZE; i++){
        word[i] = AES::SBox[word[i]];
    }
}

void AESKey::RotWord(AES::Word word){
    AES::Word non_rotated_word;
    AES::CopyAESWord(word,non_rotated_word);
    word[0] = non_rotated_word[1];
    word[1] = non_rotated_word[2];
    word[2] = non_rotated_word[3];
    word[3] = non_rotated_word[0];
}

void AESKey::XORRcon(quint8 *word, quint8 rcon_index){
    AES::Word Rcon;
    Rcon[0] = RC[rcon_index];
    Rcon[1] = 0;
    Rcon[2] = 0;
    Rcon[3] = 0;

    //qDebug() << "RC of index" << rcon_index << " is " << AES::PrintAESWord(Rcon,true);

    AES::AESWordXOR(word,Rcon);

}

void AESKey::KeyExpansion(){

    quint8 i = 0;
    //qDebug() << "Key Expansion";

    while (i < Nk){
        AES::CopyAESWord(key_words[i],round_key_words[i]);
        i++;
    }

    i = Nk;
    AES::Word temp;

    while (i < Nb*(Nr+1)){

        //qDebug() << i;

        AES::CopyAESWord(round_key_words[i-1],temp);

        //qDebug().noquote() << "TEMP" << AES::PrintAESWord(temp,true);

        if ((i % Nk) == 0){
            RotWord(temp);
            //qDebug().noquote() << "After Rot Word TEMP" << AES::PrintAESWord(temp,true);
            SubWord(temp);
            //qDebug().noquote() << "After Sub Word TEMP" << AES::PrintAESWord(temp,true);
            XORRcon(temp,i/Nk);
            //qDebug().noquote() << "After XORCon TEMP" << AES::PrintAESWord(temp,true);
        }
        else if ( (Nk > 6) && ((i % Nk) == 4) ){
            SubWord(temp);
            //qDebug().noquote() << "After Sub Word TEMP" << AES::PrintAESWord(temp,true);
        }

        //qDebug().noquote() << "W[i-Nk]" << AES::PrintAESWord(round_key_words[i-Nk]);

        for (quint8 j = 0; j < AES_WORD_SIZE; j++){
            round_key_words[i][j] = round_key_words[i-Nk][j] ^ temp[j];
        }

        i++;

    }

    // Filling the round_keys as states as that is how they will be required.
    quint8 round_key_index = 0;
    quint8 round_key_byte_index = 0;
    quint8 word_index = 0;
    quint8 byte_index = 0;

    while (true) {

        round_keys[round_key_index][round_key_byte_index] = round_key_words[word_index][byte_index];
        byte_index++;
        round_key_byte_index++;
        if (round_key_byte_index == AES_STATE_SIZE){
            round_key_byte_index = 0;
            round_key_index++;
        }
        if (byte_index == AES_WORD_SIZE){
            word_index++;
            byte_index = 0;
            if (word_index == (Nr+1)*Nb) break;
        }

    }

    round_counter = 0;

}

void AESKey::getNextRoundKey(AES::State round_key){
    //round_key = round_keys[round_counter];
    AES::CopyAESState(round_keys[round_counter],round_key);

    if (keyOrderForEncryption){
        round_counter++;
        if (round_counter == (Nr+1)) round_counter = 0;
    }
    else {
        round_counter--;
        if (round_counter == -1) round_counter = (Nr+1)-1;
    }
}

void AESKey::resetKeyScheduling(bool forEncryption){
    if (forEncryption){
        keyOrderForEncryption = true;
        round_counter = 0;
    }
    else {
        keyOrderForEncryption = false;
        round_counter = (Nr+1)-1;
    }
}

void AESKey::printKeySchedule(){

    qDebug().noquote() << "KEY SCHEDULE. Key Size: " << getKeySize();

    for (quint8 i = 0; i < AES_MAX_ROUND_KEY_WORDS; i++){
        QString word = AES::PrintAESWord(round_key_words[i],true);
        qDebug().noquote() << i << ":" << word;
    }

}


