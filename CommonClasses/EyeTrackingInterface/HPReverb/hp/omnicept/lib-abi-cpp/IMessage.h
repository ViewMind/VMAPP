// (c) Copyright 2018-2021 HP Development Company, L.P.

#pragma once
#include <omnicept/lib-abi-cpp/SensorInfo.h>
#include <omnicept/lib-abi-cpp/MessageTypes.h>
#include <omnicept/lib-abi-cpp/ConnectionIdentifier.h>

#include <memory>
#include <stdint.h>
#include <string>

namespace HP {
namespace Omnicept {
namespace Abi {

    /*! @brief ABC for all messages, the intermediate step between domain types and serialized bytes.*/
    class IMessage
    {
    public:
        virtual ~IMessage() = default;

        /*! @brief Gets the message type, indicating what the payload is.
        */
        virtual MessageType getMessageType() const = 0;

        /*! @brief Sets the sender, where the message originated from.
        */
        virtual void setSender(std::string) = 0;

        /*! @brief Gets the sender, where the message originated from.
         */
        virtual std::string getSender() const = 0;

        /*!@brief Gets the sender's connection id, if the sender field of the message is a ConnectionIdentifier, otherwise returns nullptr.
        */
        virtual std::unique_ptr<ConnectionIdentifier> getSenderConnectionId() const = 0;

        /*! @brief Sets the destination, where the message should be sent. If empty will send to all connections.
        */
        virtual void setDestination(const std::string) = 0;

        /*! @brief Gets the destination, where the message is to be sent to.
         */
        virtual std::string getDestination() const = 0;

        /*! @brief Gets the length of the header, in bytes.
        */
        virtual uint64_t getHeaderByteLength() const = 0;

        /*! @brief Gets the length of the body, in bytes.
        */
        virtual uint64_t getBodyByteLength() const = 0;

        /*! @brief Get the associate sensorInfo for this message
         */
        virtual const SensorInfo& getSensorInfo() const = 0;

        /*! @brief Writes the header (PackageDeclaration) into a byte array.
        *
        * @param destination The byte buffer to write into.
        * @param maxLength the maximum number of bytes the function is allowed to write. If the function needs more space,
        * it will not write at all.
        *
        * @return The number of bytes written into the destination
        */
        virtual uint64_t serializeHeaderToByteArray(uint8_t* destination, uint64_t maxLength) = 0;

        /*! @brief Writes the body into a byte array.
        *
        * @param destination The byte buffer to write into.
        * @param maxLength the maximum number of bytes the function is allowed to write. If the function needs more space,
        * it will not write at all.
        *
        * @return The number of bytes written into the destination
        */
        virtual uint64_t serializeBodyToByteArray(uint8_t* destination, uint64_t maxLength) = 0;

        /*! @brief Checks if an IMessage is a serialized DomainType message
        *
        * Use instead of hardcoding a MessageType into a check
        *
        * @tparam DomainType an Omnicept Message Type
        * @returns true if \p msg can be serialized to a DomainType, else false
        */
        template <class DomainType> bool isMessageA() const
        {
            return MessageTypeUtils::doesEnumMatchType<DomainType>(getMessageType());
        };

    protected:
        IMessage() = default;
    };

}

}
}