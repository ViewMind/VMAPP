// (c) Copyright 2019-2021 HP Development Company, L.P.

#pragma once

#include <omnicept/lib-abi-cpp/GenericMessage.h>
#include <vector>
#include <ostream>

namespace HP {
namespace Omnicept {
namespace Abi {
    /*! @brief Deprecated
    */
    class BackpackColorZone
    {
    public:
        BackpackColorZone() = default;
        BackpackColorZone(uint32_t r, uint32_t g, uint32_t b);

        virtual ~BackpackColorZone() = default;

        uint32_t r;
        uint32_t g;
        uint32_t b;

        bool operator == (const BackpackColorZone& other) const;
    };

    std::ostream& operator<< (std::ostream& out, const BackpackColorZone&);

    /*! @brief Deprecated
    */
    class BackpackColors : public GenericMessage
    {
    public:
        BackpackColors();
        BackpackColors(const std::vector<BackpackColorZone>& zones);

        virtual ~BackpackColors() = default;

        std::vector<BackpackColorZone>& getColorZones();
        const std::vector<BackpackColorZone>& getColorZones() const;

        bool operator== (const BackpackColors& other) const;
        friend std::ostream& operator<<(std::ostream& out, const BackpackColors&);

    protected:
        std::vector<BackpackColorZone> m_zones;
    };
}
}
}