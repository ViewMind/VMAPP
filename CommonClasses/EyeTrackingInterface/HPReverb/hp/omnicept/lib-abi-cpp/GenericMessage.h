// (c) Copyright 2019-2021 HP Development Company, L.P.

#pragma once

#include <omnicept/lib-abi-cpp/MessageTypes.h>

namespace HP {
namespace Omnicept {
namespace Abi {

    /*! @brief Base class for all domain types*/
    class GenericMessage
    {
    public:
        GenericMessage() = delete;
        GenericMessage(MessageType msgType);
        virtual ~GenericMessage() = default;

        MessageType getMessageType() const;

        bool operator== (const GenericMessage& other) const;
    protected:
        MessageType m_messageType;
    };
}

}
}