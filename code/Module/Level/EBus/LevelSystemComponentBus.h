#pragma once

#include <AzCore/EBus/EBus.h>
#include <AzCore/Slice/SliceBus.h>

namespace Module
{
    class LevelSystemRequest : public AZ::EBusTraits
    {
    public:
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;

        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;

        virtual ~LevelSystemRequest() {}

        virtual AZ::SliceInstantiationTicket LoadLevel(AZStd::string& path) = 0;

        virtual void UnloadLevel() = 0;

        virtual void SetDefaultLevel(AZStd::string path) = 0;
    };

    using LevelSystemRequestBus = AZ::EBus<LevelSystemRequest>;

    class LevelSystemNotification : public AZ::EBusTraits
    {
    public:
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Multiple;

        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;

        virtual ~LevelSystemNotification() {}
    };

    using LevelSystemNotificationBus = AZ::EBus<LevelSystemNotification>;
}
