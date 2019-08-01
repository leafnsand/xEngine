#pragma once

#include <AzCore/EBus/EBus.h>

namespace Module
{
    class WindowSystemRequest : public AZ::EBusTraits
    {
    public:
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;

        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;

        virtual ~WindowSystemRequest() {}

        virtual void* GetNativeWindowHandle() = 0;

        virtual void GetWindowSize(int& width, int& height) = 0;

        virtual void SetWindowSize(int width, int height) = 0;
    };

    using WindowSystemRequestBus = AZ::EBus<WindowSystemRequest>;

    class WindowsSystemNotification : public AZ::EBusTraits
    {
    public:
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Multiple;

        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;

        virtual ~WindowsSystemNotification() {}

        virtual void OnWindowSizeChanged(int width, int height) = 0;
    };

    using WindowsSystemNotificationBus = AZ::EBus<WindowsSystemNotification>;
}