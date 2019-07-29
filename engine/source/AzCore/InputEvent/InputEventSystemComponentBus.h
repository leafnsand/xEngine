#pragma once

#include <AzCore/EBus/EBus.h>
#include <AzCore/Math/Vector3.h>

namespace AZ
{
    class InputEventSystemRequest : public AZ::EBusTraits
    {
    public:
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;

        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;

        virtual ~InputEventSystemRequest() {}
    };

    using InputEventSystemRequestBus = AZ::EBus<InputEventSystemRequest>;

    class InputEventSystemNotification : public AZ::EBusTraits
    {
    public:
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Multiple;

        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;

        virtual ~InputEventSystemNotification() {}

        virtual void OnKeyDown(AZ::Crc32 value) {}

        virtual void OnKeyHold(AZ::Crc32 value) {}

        virtual void OnKeyUp(AZ::Crc32 value) {}

        virtual void OnMouseDown(AZ::Crc32 mouseIndex, AZ::Vector3 position) {}

        virtual void OnMouseMove(AZ::Vector3 position) {}

        virtual void OnMouseUp(AZ::Crc32 mouseIndex, AZ::Vector3 position) {}

        virtual void OnMouseRollIn(float value) {}

        virtual void OnMouseRollOut(float value) {}

        virtual void OnTouchDown(AZ::Crc32 touchIndex, AZ::Vector3 position) {}

        virtual void OnTouchMove(AZ::Crc32 touchIndex, AZ::Vector3 position) {}

        virtual void OnTouchUp(AZ::Crc32 touchIndex, AZ::Vector3 position) {}
    };

    using InputEventSystemNotificationBus = AZ::EBus<InputEventSystemNotification>;
}
