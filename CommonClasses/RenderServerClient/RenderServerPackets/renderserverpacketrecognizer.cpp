#include "renderserverpacketrecognizer.h"

const QString RenderServerPacketRecognizer::PACKET_PREAMBLE = "[[[<>]]]";

RenderServerPacketRecognizer::RenderServerPacketRecognizer()
{
    this->curlyBraceBalancer = 0;
    this->currentPreambleIndex = 0;
    this->preambleSize = PACKET_PREAMBLE.size();
}


RenderServerPacketRecognizer::RecogState RenderServerPacketRecognizer::isNewBytePartOfPacket(char c){

    if (this->currentPreambleIndex < this->preambleSize){
        if (PACKET_PREAMBLE.at(this->currentPreambleIndex) == c){
            this->currentPreambleIndex++;
        }
        else {
            this->currentPreambleIndex = 0;
        }
        return WAITING_FOR_PACKET;
    }
    else {

        // We are in the packet and we need to balance the curly braces.
        if (c == '{'){
            curlyBraceBalancer++;
        }
        else if (c == '}'){
            curlyBraceBalancer--;
            if (curlyBraceBalancer == 0){
                currentPreambleIndex = 0;
                return PACKET_DONE;
            }
        }

        if (curlyBraceBalancer > 0) return IN_PACKET;
        else return WAITING_FOR_PACKET;

    }

}
