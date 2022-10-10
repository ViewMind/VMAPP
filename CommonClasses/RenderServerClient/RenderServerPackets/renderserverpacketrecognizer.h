#ifndef RENDERSERVERPACKETRECOGNIZER_H
#define RENDERSERVERPACKETRECOGNIZER_H

#include <QString>

class RenderServerPacketRecognizer
{
public:


    RenderServerPacketRecognizer();

    static const QString PACKET_PREAMBLE;

    typedef enum {PACKET_DONE, IN_PACKET, WAITING_FOR_PACKET} RecogState;

    RecogState isNewBytePartOfPacket(char c);

private:

    qsizetype currentPreambleIndex;
    qint32 curlyBraceBalancer;
    qsizetype preambleSize;


};

#endif // RENDERSERVERPACKETRECOGNIZER_H
