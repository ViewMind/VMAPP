// (c) Copyright 2019-2021 HP Development Company, L.P.

#pragma once

#include <omnicept/lib-abi-cpp/MessageTypes.h>
#include <omnicept/lib-abi-cpp/SensorInfo.h>

#include <string>

namespace HP {
namespace Omnicept {
namespace Abi {

    /*! @brief  A Subscription is submitted to the %HP %Omnicept Runtime as part of a SubsriptionList to indicate 
     * that an extension wants to receive certain message types. 
     * 
     * A Subscription can filter on messageType, sensorInfo, and sender. These different fields are
     * combined through a logical AND. An unset field acts like a wildcard and is not used in the filtering
     * process. For example, if a Subscription has only MessageType set, the a message only needs to have that
     * MessageType in order to be allowed through. If a Subscription has a MessageType and sender, then a
     * message needs to be of that MessageType and from that sender to be allowed through.
     */
    class Subscription
    {
    public:
        Subscription();
        Subscription(MessageType messageType, const SensorInfo& sensorInfo, const std::string& sender);
        Subscription(uint32_t messageType, const SensorInfo& sensorInfo, const std::string& sender);
        virtual ~Subscription();

        const MessageType& getType() const;
        const SensorInfo& getSensorInfo() const;
        const std::string& getSender() const;

        void setType(const MessageType& type);
        void setSensorInfo(const SensorInfo& sensorInfo);
        void setSender(const std::string& sender);

        bool operator == (const Subscription& other) const;

        friend std::ostream & operator << (std::ostream &out, const Subscription&);

        template<class DomainType> 
        static Subscription generateSubscriptionForDomainType()
        {
            return Subscription{ MessageTypeUtils::getEnumForType<DomainType>(), SensorInfo{}, "" };
        }

    protected:
        MessageType m_messageType;
        SensorInfo m_sensorInfo;
        std::string m_sender;
    };
}

}
}