// (c) Copyright 2019-2021 HP Development Company, L.P.

#pragma once
#include <vector>
#include <omnicept/lib-abi-cpp/DomainData.h>
#include <sstream>
#include <algorithm>

namespace HP {
namespace Omnicept {
namespace Abi {

    /*! @brief Base class for framed DomainData. Includes a templated collection for the framed type.
     * Framed data is a sequence of values that are collected over a period of time and processed as a unit, usually for efficiency purposes.
     * A DataFrame contains one or more T data and a timestamp that represents the latest data.
     * @tparam T the type for this DataFrame collection.
     */
    template<class T>
    class DataFrame : public DomainData
    {
    public:
        DataFrame(MessageType value);
        virtual ~DataFrame() = default;
        std::vector<T> data;
        std::string toString() const;

        template <class T>
        bool operator == (const DataFrame<T>& other) const
        {
            bool matches = DomainData::operator==(other); 
            matches = matches && data == other.data;
            return matches;
        }
        
        bool dataEquals(const DomainData& other) const override
        {
            bool matches = getMessageType() == other.getMessageType();
            if(matches)
            {
                auto o = static_cast<const DataFrame<T>&>(other);
                matches = dataEquals(o);
            }
            return matches;
        }

        bool dataEquals(const DataFrame<T>& other) const
        {
            bool matches = std::equal(data.begin(), data.end(), other.data.begin(), other.data.end(),
                [](const T& lhs, const T& rhs)
                {
                    return lhs.dataEquals(rhs);
                });
            return matches;
        }

    };

    template <class T>
    DataFrame<T>::DataFrame(MessageType value)
        : DomainData {value}
    {
    }

    template <class T>
    std::ostream& operator<< (std::ostream& out, const DataFrame<T>& f)
    {
        return out << f.toString();
    }

    template <class T>
    std::string DataFrame<T>::toString() const
    {
        std::stringstream ss;
        ss << "DataFrame: ";
        ss << sensorInfo;
        ss << " ";
        ss << timestamp;
        ss << " ";
        for (const T& t : data)
        {
            ss << t << " ";
        }
        return ss.str();
    }
}

}
}
