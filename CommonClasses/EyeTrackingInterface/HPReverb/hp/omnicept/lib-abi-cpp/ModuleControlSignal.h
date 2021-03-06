// (c) Copyright 2019-2021 HP Development Company, L.P.

#pragma once
#include <omnicept/lib-abi-cpp/GenericMessage.h>

namespace HP {
namespace Omnicept {
namespace Abi {

    /**
    * @brief enum representing types of ModuleControlSignal
    * 
    */
    enum class ModuleControlSignalType : uint64_t
    {
        /*! @brief This indicates that the ModuleControlSignal does not contain a usable signal type. 
        * This can happen if the type was not set properly, or if the message type is not known to the receiver, such as if
        * there is a version mismatch.
        */
        MODULE_SIGNAL_UNKNOWN = 0, 

        /*! @brief Used to indicate that a connection being set up to the %HP %Omnicept Runtime has been finalized, and is ready to use.
        */
        MODULE_SIGNAL_READY = 10, 

        /*! @brief Used to notify that an existing connection to the %HP %Omnicept Runtime will be disconnected. Could be generated by either the local or remote side of the connection.
        */ 
        MODULE_SIGNAL_STOP = 20, 

        /*! @brief \attention Exclusively sent to internal connections. 
        * \internal
        *  Sent from the %HP %Omnicept Runtime exclusively to internal connection to wake up sleeping modules
        * \endinternal
        */ 
        MODULE_SIGNAL_WAKE = 30, 

        /*! @brief \attention Exclusively sent to internal connections.
        * \internal
        *  Sent from the %HP %Omnicept Runtime exclusively to internal connections to put modules to sleep
        * \endinternal
        */
        MODULE_SIGNAL_SLEEP = 40 
    };

    /*! @brief Domain type for status signals between the %HP %Omnicept Runtime and its connections.
    *
    */
    class ModuleControlSignal final : public GenericMessage
    {
    public:
        ModuleControlSignal();
        ModuleControlSignal(ModuleControlSignalType signal);

        ~ModuleControlSignal() = default;
        void SetSignal(ModuleControlSignalType signal);
        const ModuleControlSignalType GetSignal() const;

        bool operator== (const ModuleControlSignal& other) const;

    protected:
        ModuleControlSignalType m_signal;
    };
}


}
}
