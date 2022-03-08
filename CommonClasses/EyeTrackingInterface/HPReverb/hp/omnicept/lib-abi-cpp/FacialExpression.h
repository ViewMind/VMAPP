// (c) Copyright 2019-2021 HP Development Company, L.P.

#pragma once

#include <omnicept/lib-abi-cpp/Experimental.h>
#include <omnicept/lib-abi-cpp/SensorInfo.h>
#include <omnicept/lib-abi-cpp/DomainData.h>
#include <omnicept/lib-abi-cpp/DataFrame.h>
#include <map>

namespace HP {
namespace Omnicept {
namespace Abi {

    /*! @brief Domain type for facial expression expression data.
     *
     * \warning FacialExpression development is unfinished, and it should not be used 
     * \internal 
     *  Each facial expression is given a strength from 0.0 to 1.0; where 0 indicates the expression is absent
     *  and 1.0 indicates the expression is fully present. Different module suppliers may or may not be able to
     *  measure more than one expression at a time.
     * \endinternal
     */
    class FacialExpression : public DomainData
    {
        //! @cond
    public:
        FacialExpression();

        virtual ~FacialExpression();

        double neutral; /*!< A double normalized from 0.0 to 1.0 representing how strongly the expression is being made */
        double winkLeft; /*!< A double normalized from 0.0 to 1.0 representing how strongly the expression is being made */
        double winkRight; /*!< A double normalized from 0.0 to 1.0 representing how strongly the expression is being made */
        double smile; /*!< A double normalized from 0.0 to 1.0 representing how strongly the expression is being made */
        double browRaise; /*!< A double normalized from 0.0 to 1.0 representing how strongly the expression is being made */
        double angry; /*!< A double normalized from 0.0 to 1.0 representing how strongly the expression is being made */
        double pursingLips; /*!< A double normalized from 0.0 to 1.0 representing how strongly the expression is being made */
        double smirkLeft; /*!< A double normalized from 0.0 to 1.0 representing how strongly the expression is being made */
        double smirkRight; /*!< A double normalized from 0.0 to 1.0 representing how strongly the expression is being made */
        double clenchTeeth; /*!< A double normalized from 0.0 to 1.0 representing how strongly the expression is being made */

        /*! A boolean indicating if the user blinked. Note that some FacialExpression suppliers provide infrequent updates,
         * so you may wish to represent a blink as only a momentary blink and not a continuous 'eyes closed' indicator.
         */
        bool blink;

        bool operator == (const FacialExpression& other) const;
        bool dataEquals(const DomainData& other) const override;
        //! @endcond
    };

    std::ostream & operator << (std::ostream &out, const FacialExpression&);

    /*! @brief Domain type for FacialExpression Frames.
    *
    * \warning FacialEMGFrame development is unfinished, and it should not be used
    */
    class FacialExpressionFrame : public DataFrame<FacialExpression>
    {
    public:
        //! @cond
        FacialExpressionFrame();
        virtual ~FacialExpressionFrame() = default;
        //! @endcond
    };
}

}
}