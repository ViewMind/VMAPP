// (c) Copyright 2018-2021 HP Development Company, L.P.

#pragma once

#include <iostream>
#include <unordered_map>

namespace HP {
namespace Omnicept {
namespace Abi {

    enum MessageType : uint32_t
    {
        null = 0,// 0 Explicitly reserved and must not be used.
        declaration = 1,
        testPackage = 2,
        heartRate = 3,
        heartRateFrame = 4,
        sessionControlSignal = 5,
        moduleControlSignal = 6,
        eyeTracking = 9,
        eyeTrackingFrame = 10,
        backpackColors = 11,
        battery = 12,
        sync = 13,
        eyePupillometry = 14,
        eyePupillometryFrame = 15,
        byteMessage = 16,
        ppg = 17,
        ppgFrame = 18,
        facialExpression = 19,
        facialExpressionFrame = 20,
        cognitiveLoad = 21,
        cognitiveLoadInputFeature = 22,
        timestamp = 23,
        subscriptionList = 24,
        connectionParameter = 25,
        facialEmg = 26,
        facialEmgFrame = 27,
        vsync = 28,
        sceneColor = 29,
        sceneColorFrame = 30,
        dataVaultAction = 31,
        dataVaultResult = 32,
        connectionStatusSignal = 33,
        connectionInfo = 34,
        connectionInfoQuery = 35,
        heartRateVariability = 36,
        cameraImage = 37,
        imu = 38,
        imuFrame = 39,
    };

    const std::unordered_map<MessageType, std::string> MessageTypeStrings
    {
        { null, "null" },
        { declaration , "PackageDeclaration" },
        { testPackage , "TestPackage" },
        { heartRate , "HeartRate" },
        { heartRateFrame , "HeartRateFrame" },
        { sessionControlSignal , "SessionControlSignal" },
        { moduleControlSignal , "ModuleControlSignal" },
        { eyeTracking , "EyeTracking" },
        { eyeTrackingFrame , "EyeTrackingFrame" },
        { backpackColors , "BackpackColors" },
        { battery , "Battery" },
        { sync , "Sync" },
        { eyePupillometry , "EyePupillometry" },
        { eyePupillometryFrame , "EyePupillometryFrame" },
        { byteMessage , "ByteMessage" },
        { ppg , "PPG" },
        { ppgFrame , "PPGFrame" },
        { facialExpression , "FacialExpression" },
        { facialExpressionFrame , "FacialExpressionFrame" },
        { cognitiveLoad , "CognitiveLoad" },
        { cognitiveLoadInputFeature , "CognitiveLoadInputFeature" },
        { timestamp , "Timestamp" },
        { subscriptionList , "SubscriptionList" },
        { connectionParameter , "ConnectionParameter" },
        { facialEmg , "FacialEmg" },
        { facialEmgFrame , "FacialEmgFrame" },
        { vsync , "VSync" },
        { sceneColor , "SceneColor" },
        { sceneColorFrame , "SceneColorFrame" },
        { dataVaultAction , "DataVaultAction" },
        { dataVaultResult , "DataVaultResult" },
        { connectionStatusSignal , "ConnectionStatusSignal" },
        { connectionInfo , "ConnectionInfo"},
        { connectionInfoQuery , "ConnectionInfoQuery"},
        { heartRateVariability , "HeartRateVariability" },
        { cameraImage, "CameraImage" },
        { imu, "IMU" },
        { imuFrame, "IMUFrame" },
    };

    class MessageTypeUtils final
    {
        MessageTypeUtils() = delete;
        ~MessageTypeUtils() = delete;
    public:

        /*! @brief Gets the enum mapped to the DomainType
        *
        * Specializations implemented in .cpp file
        * Useful for mapping a type to an enum in template functions
        *
        * @tparam DomainType an Omnicept Message Type
        * @returns MessageType that represents the DomainType
        */
        template <class DomainType> static MessageType getEnumForType();


        /*! @brief Checks if a MessageType is mapped to the DomainType
        *
        * @param type the MessageType to check
        * @tparam DomainType an Omnicept Message Type
        * @returns true if the \p type represents DomainType false otherwise
        */
        template <class DomainType> static bool doesEnumMatchType(const Abi::MessageType type)
        {
            return type == getEnumForType<DomainType>();
        };

        template <class DomainType>
        static std::string getTypeName()
        {
            std::string name = "unkownType";
            MessageType type = getEnumForType<DomainType>();
            if (MessageTypeStrings.find(type) != MessageTypeStrings.end())
            {
                name = MessageTypeStrings.at(type);
            }
            return name;
        }
    };
}

}
}