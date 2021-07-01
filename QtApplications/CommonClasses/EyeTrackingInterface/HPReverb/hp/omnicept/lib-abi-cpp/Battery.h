// (c) Copyright 2019-2021 HP Development Company, L.P.

#pragma once

#include <omnicept/lib-abi-cpp/GenericMessage.h>
#include <string>
#include <iostream>

namespace HP {
namespace Omnicept {
namespace Abi {

    /*! @brief Deprecated
    */
    class Battery : public GenericMessage
    {
    public:
        Battery();

        virtual ~Battery();
        unsigned int id;
        bool isPresent;
        float percent;

        bool operator == (const Battery& other) const;
        std::string toString() const;
    };
    
    std::ostream & operator << (std::ostream &out, const Battery&);
}

}
}
