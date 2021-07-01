// (c) Copyright 2019-2021 HP Development Company, L.P.

#pragma once

#include <vector>
#include <omnicept/lib-abi-cpp/Subscription.h>

namespace HP {
namespace Omnicept {
namespace Abi {

    /*! @brief Domain type for a SubscriptionList.
     *
     * A SubscriptionList is a collection of Subscriptions, indicating what kinds of MessageTypes you want to receive.
     * You will not be notified when you request types that you cannot receive. For example, if your license
     * does not include access to a message type, or there are no attached sensors that produce the message type.
     * 
     * A narrow SubscriptionList can improve performance for both the Runtime and the extension. The Runtime performs a 
     * filtering function, so serialization and further processing is avoided for undesired messages.
     * 
     * When used for filtering, a message is allowed through the filter (ie. sent to an extension) when the message 
     * satisfies at least one Subscription in the subscription list.
     */
    class SubscriptionList
    {
    public:
        SubscriptionList() = default; /*!< @brief The default constructor should not be used in place of GetSubscriptionListToNone(). */
        virtual ~SubscriptionList() = default;

        const std::vector<Subscription>& getSubscriptions() const;
        std::vector<Subscription>& getSubscriptions();

        void setSubscriptions(const std::vector<Subscription>& subscriptions);

        bool operator == (const SubscriptionList& other) const;
        friend std::ostream & operator << (std::ostream &out, const Subscription&);

        static std::shared_ptr<SubscriptionList> GetSubscriptionListToAll(); /*!< @brief Get a subscription list for all message types. */
        static std::shared_ptr<SubscriptionList> GetSubscriptionListToNone(); /*!< @brief Get a well-formed but empty SubscriptionList. */

    protected:
        std::vector<Subscription> m_subscriptions;
    };
}
}
}