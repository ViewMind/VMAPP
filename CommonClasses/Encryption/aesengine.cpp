#include "aesengine.h"

AESEngine::AESEngine(){
    debug = false;
    timeProfile = false;
    configuredFlag = false;
}

QString AESEngine::getLastError() const {
    return lastError;
}

//////////////////////////////// CBC BLOCK ENCRYPTION/DECRYPTION FUNCTIONS ////////////////////////////////

void AESEngine::OnesAndZerosPadding(){
    data->append(0x80); // Adding the '1';
    // And then as many 'zeros' as necessary to complete the block size.
    while ((data->size() % AES_STATE_SIZE) != 0){
        data->append((char)0x00);
    }
}

bool AESEngine::RemoveOnesAndZerosPadding(){
    quint64 last = data->size()-1;
    while (data->size() > 0){
        quint8 byte = data->at(last);
        if (byte == 0x80){
            data->remove(last,1);
            break;
        }
        else if (byte == 0){
            data->remove(last,1);
            last = data->size()-1;
        }
        else {
            // There should NOT be ANY other value othern than zero before we find 0x80. Something went wrong.
            lastError = "Found a non zero byte value before the first 0x80";
            return false;
        }
    }

    if (data->size() > 0) return true;
    // Something went wrong we removed all the data.
    else {
        lastError = "Attempting to remove padding was left with empty data";
        return false;
    }
}

bool AESEngine::encrypt(QByteArray *plaintext){

    if (!configuredFlag) {
        this->lastError = "AES Engine has not been properly configured";
        return false;
    }

    data = plaintext;

    // First step is to pad the data.
    OnesAndZerosPadding();

    // Then we can start the CBC encrypting.
    AES::State previousBlock;
    AES::CopyAESState(initializationVector,previousBlock);
    dataByteArrayIndex = 0;
    quint64 data_size = static_cast<quint64>(data->size());


    while (dataByteArrayIndex < data_size){

        // We first load the state block with the data.
        if (timeProfile) timer.start();
        loadNextBlockIntoState();
        if (timeProfile) {
            qDebug() << "Loading Next Block" << timer.elapsed();
        }

        // Then we XOR the block with the results of the previous block.
        //if (timeProfile) timer.start();
        AES::AESStateXOR(currentState,previousBlock);
        if (timeProfile) {
            qDebug() << "CBC Previous Block XOR" << timer.elapsed();
        }

        // Then we Encrypt.
        //if (timeProfile) timer.start();
        EncryptBlock();
        if (timeProfile) {
            qDebug() << "Encrypt Block" << timer.elapsed();
        }

        // Finally we stored the data back in data vector.
        //if (timeProfile) timer.start();
        loadStateIntoBlock();
        if (timeProfile) {
            qDebug() << "Storing block result" << timer.elapsed();
        }

        // And we save the data as the previous block
        //if (timeProfile) timer.start();
        AES::CopyAESState(currentState,previousBlock);
        if (timeProfile) {
            qDebug() << "CBC Copying the current state to the previous block" << timer.elapsed();
            qDebug() << "===";
        }

    }

    return true;

}

bool AESEngine::decrypt(QByteArray *ciphertext){

    if (!configuredFlag) {
        this->lastError = "AES Engine has not been properly configured";
        return false;
    }

    data = ciphertext;

    // CBC Decryption is straight forward.
    AES::State previousBlock;
    AES::State temporaryStore;

    AES::CopyAESState(initializationVector,previousBlock);
    dataByteArrayIndex = 0;

    quint64 data_size = static_cast<quint64>(data->size());

    // First check. If the data is NOT a multiple of 128 bits. Something is wrong.
    if ((data_size % AES_STATE_SIZE) != 0){
        lastError = "Wrong data size. '" + QString::number(data_size) + "'. This value is not a multiple of 16";
        return false;
    }

    while (dataByteArrayIndex < data_size){

        // Outside of the loop we load the first block into the state.
        loadNextBlockIntoState();

        // We copy current state to the temporaty store. We need to do this to preverve the previous block vlaue
        // As the in-place decryption changes teh original value required for the XOR.
        AES::CopyAESState(currentState,temporaryStore);

        // As the first step in the loop we decrpyt.
        DecryptBlock();

        // We XOR with the previous block. The very first time should be the initializaton vector.
        AES::AESStateXOR(currentState,previousBlock);

        loadStateIntoBlock();

        // Now the previous block becomes the orignal ciphertext.
        AES::CopyAESState(temporaryStore,previousBlock);
    }

    // The Last Step is to remove the padding.
    return RemoveOnesAndZerosPadding();


}

//////////////////////////////// AES BLOCK DECRYPTION FUNCTIONS ////////////////////////////////

void AESEngine::EncryptBlock(){

    // This function assumues that the data to encrypt is in the current state variable. So it has no inputs or outputs.
    if (debug) printCurrentState("Start Encryption");
    key.resetKeyScheduling(true);

    key.getNextRoundKey(roundKey);
    AddRoundKey();

    if (debug) printCurrentState("After Adding Round Key");

    for (quint8 round = 1; round < key.getNr(); round++){

        SubBytes();
        if (debug) printCurrentState("On Round " + QString::number(round) + " SubBytes");

        ShiftRows();
        if (debug) printCurrentState("On Round " + QString::number(round) + " ShiftRows");

        MixColumns();
        if (debug) printCurrentState("On Round " + QString::number(round) + " MixColumns");

        key.getNextRoundKey(roundKey);
        AddRoundKey();

        if (debug) printCurrentState("On Round " + QString::number(round) + " AddRoundKey");

    }

    // Last Round

    SubBytes();
    if (debug) printCurrentState("On Final Round SubBytes");
    ShiftRows();
    if (debug) printCurrentState("On Final Round ShiftRows");

    key.getNextRoundKey(roundKey);
    AddRoundKey();

    // At this point the currentState contains the encrypted data.
    if (debug) printCurrentState("On Final Round AddRoundKey");

}

void AESEngine::DecryptBlock(){

    // As with encryption this function assumes that
    if (debug) printCurrentState("Start Decryption");
    key.resetKeyScheduling(false);

    key.getNextRoundKey(roundKey);
    if (debug) printCurrentKey();
    AddRoundKey();
    if (debug) printCurrentState("After Adding Round Key");

    for (quint8 round = 1; round < key.getNr(); round++){

        InvShiftRows();
        if (debug) printCurrentState("On Round " + QString::number(round) + " InvShiftRows");

        InvSubBytes();
        if (debug) printCurrentState("On Round " + QString::number(round) + " InvSubBytes");

        key.getNextRoundKey(roundKey);
        if (debug) printCurrentKey();
        AddRoundKey();
        if (debug) printCurrentState("On Round " + QString::number(round) + " AddRoundKey");

        InvMixColumns();
        if (debug) printCurrentState("On Round " + QString::number(round) + " InvMixColumns");
    }

    // Last Round.
    InvShiftRows();
    if (debug) printCurrentState("On Final Round InvShiftRows");
    InvSubBytes();
    if (debug) printCurrentState("On Final Round InvSubBytes");

    key.getNextRoundKey(roundKey);
    if (debug) printCurrentKey();
    AddRoundKey();

    // At this point the currentState contains the decrypted data.
    if (debug) printCurrentState("On Final Round AddRoundKey");

}

//////////////////////////////// DEBUG FUNCTIONS ////////////////////////////////

void AESEngine::printCurrentState(const QString &title){
    if (title != ""){
        qDebug().noquote() << title;
    }
    AES::PrintState(currentState,false);
}

void AESEngine::printCurrentKey(){
    qDebug().noquote() << "  KEY = " << AES::PrintState(roundKey,false,true);
}

void AESEngine::loadNextBlockIntoState(){
    for (quint8 i = 0; i < AES_STATE_SIZE; i++){
        currentState[i] = data->at(dataByteArrayIndex+i);
    }
}

void AESEngine::loadStateIntoBlock(){

    QByteArray temp;
    for (quint8 i = 0; i < AES_STATE_SIZE; i++){
        temp.append(currentState[i]);
    }
    data->replace(dataByteArrayIndex,AES_STATE_SIZE,temp);
    dataByteArrayIndex = dataByteArrayIndex + AES_STATE_SIZE;

//    for (quint8 i = 0; i < AES_STATE_SIZE; i++){
//        // In order to replace a byte we first must insert it at i and then remove the
//        // byte at i+i
//        data->insert(dataByteArrayIndex+i,currentState[i]);
//        data->remove(dataByteArrayIndex+i+1,1);
//    }
//    dataByteArrayIndex = dataByteArrayIndex + AES_STATE_SIZE;
}

void AESEngine::enableDebugOutput(bool enable){
    debug = enable;
}

void AESEngine::enableTimeProfile(bool enable){
    timeProfile = enable;
}


//////////////////////////////// KEY CREATION FUNCTIONS ////////////////////////////////

bool AESEngine::configure(const QString &string_key, AES::KeyType keyType, const QByteArray &iv){
    configuredFlag = false;
    key.create(string_key,keyType);
    if (iv.size() != AES_STATE_SIZE) return false;
    for (quint8 i = 0; i < AES_STATE_SIZE; i++){
        initializationVector[i] = iv.at(i);
    }
    configuredFlag = true;
    return true;
}

bool AESEngine::configure(QByteArray byte_key,const QByteArray &iv){
    configuredFlag = false;
    if (!key.create(byte_key)) return false;
    if (iv.size() != AES_STATE_SIZE) return false;
    for (quint8 i = 0; i < AES_STATE_SIZE; i++){
        initializationVector[i] = iv.at(i);
    }
    configuredFlag = true;
    return true;
}


//////////////////////////////// BLOCK ENCRYPTION/ DECRIPTION STANDARD FUNCTIONS ////////////////////////////////

void AESEngine::SubBytes() {
    for (quint8 i = 0; i < AES_STATE_SIZE; i++){
        currentState[i] = AES::SBox[currentState[i]];
    }
}

void AESEngine::InvSubBytes() {
    for (quint8 i = 0; i < AES_STATE_SIZE; i++){
        currentState[i] = InvSBox[currentState[i]];
    }
}


void AESEngine::ShiftRows(){

    AES::CopyAESState(currentState,storedCurrentState);

    currentState[1]  = storedCurrentState[5];
    currentState[5]  = storedCurrentState[9];
    currentState[9]  = storedCurrentState[13];
    currentState[13] = storedCurrentState[1];

    currentState[2]  = storedCurrentState[10];
    currentState[6]  = storedCurrentState[14];
    currentState[10] = storedCurrentState[2];
    currentState[14] = storedCurrentState[6];

    currentState[3]  = storedCurrentState[15];
    currentState[7]  = storedCurrentState[3];
    currentState[11] = storedCurrentState[7];
    currentState[15] = storedCurrentState[11];

}

void AESEngine::InvShiftRows() {
    // Since the first row of the state remains the same, the assignments are skipped.

    AES::CopyAESState(currentState,storedCurrentState);

    currentState[1]  = storedCurrentState[13];
    currentState[5]  = storedCurrentState[1];
    currentState[9]  = storedCurrentState[5];
    currentState[13] = storedCurrentState[9];

    currentState[2]  = storedCurrentState[10];
    currentState[6]  = storedCurrentState[14];
    currentState[10] = storedCurrentState[2];
    currentState[14] = storedCurrentState[6];

    currentState[3]  = storedCurrentState[7];
    currentState[7]  = storedCurrentState[11];
    currentState[11] = storedCurrentState[15];
    currentState[15] = storedCurrentState[3];
}


void AESEngine::MixColumns() {

    quint8 s00 = currentState[0];   quint8 s01 = currentState[4];  quint8 s02 = currentState[8];  quint8 s03 = currentState[12];
    quint8 s10 = currentState[1];   quint8 s11 = currentState[5];  quint8 s12 = currentState[9];  quint8 s13 = currentState[13];
    quint8 s20 = currentState[2];   quint8 s21 = currentState[6];  quint8 s22 = currentState[10]; quint8 s23 = currentState[14];
    quint8 s30 = currentState[3];   quint8 s31 = currentState[7];  quint8 s32 = currentState[11]; quint8 s33 = currentState[15];

    currentState[0]  = GFMultBy2[s00] ^ GFMultBy3[s10] ^ s20            ^ s30;                  //s00
    currentState[1]  = s00            ^ GFMultBy2[s10] ^ GFMultBy3[s20] ^ s30;                  //s10
    currentState[2]  = s00            ^ s10            ^ GFMultBy2[s20] ^ GFMultBy3[s30];       //s20
    currentState[3]  = GFMultBy3[s00] ^ s10            ^ s20            ^ GFMultBy2[s30];       //s30

    currentState[4]  = GFMultBy2[s01] ^ GFMultBy3[s11] ^ s21            ^ s31;
    currentState[5]  = s01            ^ GFMultBy2[s11] ^ GFMultBy3[s21] ^ s31;
    currentState[6]  = s01            ^ s11            ^ GFMultBy2[s21] ^ GFMultBy3[s31];
    currentState[7]  = GFMultBy3[s01] ^ s11            ^ s21            ^ GFMultBy2[s31];

    currentState[8]  = GFMultBy2[s02] ^ GFMultBy3[s12] ^ s22            ^ s32;
    currentState[9]  = s02            ^ GFMultBy2[s12] ^ GFMultBy3[s22] ^ s32;
    currentState[10] = s02            ^ s12            ^ GFMultBy2[s22] ^ GFMultBy3[s32];
    currentState[11] = GFMultBy3[s02] ^ s12            ^ s22            ^ GFMultBy2[s32];

    currentState[12] = GFMultBy2[s03] ^ GFMultBy3[s13] ^ s23            ^ s33;
    currentState[13] = s03            ^ GFMultBy2[s13] ^ GFMultBy3[s23] ^ s33;
    currentState[14] = s03            ^ s13            ^ GFMultBy2[s23] ^ GFMultBy3[s33];
    currentState[15] = GFMultBy3[s03] ^ s13            ^ s23            ^ GFMultBy2[s33];

}


void AESEngine::InvMixColumns() {

    quint8 s00 = currentState[0];   quint8 s01 = currentState[4];  quint8 s02 = currentState[8];  quint8 s03 = currentState[12];
    quint8 s10 = currentState[1];   quint8 s11 = currentState[5];  quint8 s12 = currentState[9];  quint8 s13 = currentState[13];
    quint8 s20 = currentState[2];   quint8 s21 = currentState[6];  quint8 s22 = currentState[10]; quint8 s23 = currentState[14];
    quint8 s30 = currentState[3];   quint8 s31 = currentState[7];  quint8 s32 = currentState[11]; quint8 s33 = currentState[15];

    currentState[0]  = GFMultByE[s00] ^ GFMultByB[s10] ^ GFMultByD[s20] ^ GFMultBy9[s30];
    currentState[1]  = GFMultBy9[s00] ^ GFMultByE[s10] ^ GFMultByB[s20] ^ GFMultByD[s30];
    currentState[2]  = GFMultByD[s00] ^ GFMultBy9[s10] ^ GFMultByE[s20] ^ GFMultByB[s30];
    currentState[3]  = GFMultByB[s00] ^ GFMultByD[s10] ^ GFMultBy9[s20] ^ GFMultByE[s30];

    currentState[4]  = GFMultByE[s01] ^ GFMultByB[s11] ^ GFMultByD[s21] ^ GFMultBy9[s31];
    currentState[5]  = GFMultBy9[s01] ^ GFMultByE[s11] ^ GFMultByB[s21] ^ GFMultByD[s31];
    currentState[6]  = GFMultByD[s01] ^ GFMultBy9[s11] ^ GFMultByE[s21] ^ GFMultByB[s31];
    currentState[7]  = GFMultByB[s01] ^ GFMultByD[s11] ^ GFMultBy9[s21] ^ GFMultByE[s31];

    currentState[8]  = GFMultByE[s02] ^ GFMultByB[s12] ^ GFMultByD[s22] ^ GFMultBy9[s32];
    currentState[9]  = GFMultBy9[s02] ^ GFMultByE[s12] ^ GFMultByB[s22] ^ GFMultByD[s32];
    currentState[10] = GFMultByD[s02] ^ GFMultBy9[s12] ^ GFMultByE[s22] ^ GFMultByB[s32];
    currentState[11] = GFMultByB[s02] ^ GFMultByD[s12] ^ GFMultBy9[s22] ^ GFMultByE[s32];

    currentState[12] = GFMultByE[s03] ^ GFMultByB[s13] ^ GFMultByD[s23] ^ GFMultBy9[s33];
    currentState[13] = GFMultBy9[s03] ^ GFMultByE[s13] ^ GFMultByB[s23] ^ GFMultByD[s33];
    currentState[14] = GFMultByD[s03] ^ GFMultBy9[s13] ^ GFMultByE[s23] ^ GFMultByB[s33];
    currentState[15] = GFMultByB[s03] ^ GFMultByD[s13] ^ GFMultBy9[s23] ^ GFMultByE[s33];

}

void AESEngine::AddRoundKey() {
    for (quint8 i = 0; i < AES_STATE_SIZE; i++){
        currentState[i] = currentState[i] ^ roundKey[i];
    }
}
