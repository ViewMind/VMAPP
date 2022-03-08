// (c) Copyright 2019-2021 HP Development Company, L.P.

#pragma once

#include <omnicept/lib-abi-cpp/DomainData.h>
#include <omnicept/lib-abi-cpp/DataFrame.h>

namespace HP {
namespace Omnicept {
namespace Abi {

    // ------------------------------------------------------------------------
    /*! @brief Deprecated 
    * @deprecated Part of Abi::EyeTracking message type now
    */
    class EyePupilDiameter
    {
    public:
        EyePupilDiameter();
        EyePupilDiameter(float size, float confidence);
        virtual ~EyePupilDiameter() = default;

        float size; 
        float confidence;
        bool operator ==(const EyePupilDiameter& other) const;
        std::string toString() const;
    };

    std::ostream& operator<< (std::ostream& out, const EyePupilDiameter& epd);

    // ------------------------------------------------------------------------
    /*! @brief Deprecated 
    * @deprecated Part of Abi::EyeTracking message type now
    */
    class EyePupillometry : public DomainData
    {
    public:
        EyePupillometry();
        EyePupillometry(EyePupilDiameter left, EyePupilDiameter right);
        virtual ~EyePupillometry() = default;

        EyePupilDiameter rightPupilDiameter; 
        EyePupilDiameter leftPupilDiameter; 

        bool operator== (const EyePupillometry& other) const;
        bool dataEquals(const DomainData& other) const override;
        
        std::string toString() const;
    };

    std::ostream& operator<< (std::ostream& out, const EyePupillometry& ep);

    // ------------------------------------------------------------------------
    /*! @brief Deprecated  
    * @deprecated Part of Abi::EyeTrackingFrame message type now
    */
    class EyePupillometryFrame : public DataFrame<EyePupillometry>
    {
    public:
        EyePupillometryFrame();
        virtual ~EyePupillometryFrame() = default;
    };
}

}
}
