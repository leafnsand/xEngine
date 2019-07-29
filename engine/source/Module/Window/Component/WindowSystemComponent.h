#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>

#include <Window/EBus/WindowSystemComponentBus.h>

namespace Module
{
    class WindowSystemComponent
        : public AZ::Component
        , protected AZ::SystemTickBus::Handler
        , protected WindowSystemRequestBus::Handler
        , protected WindowsSystemNotificationBus::Handler
    {
    public:
        AZ_COMPONENT(WindowSystemComponent, "{FC352F48-C910-4BC1-9ADA-7EECC1E3AA3F}");

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC("WindowSystemService"));
        }

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC("WindowSystemService"));
        }

        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
        {
        }

        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
        {
        }

        static void Reflect(AZ::ReflectContext* reflection);

    protected:
        // AZ::Component
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        // AZ::SystemTickBus::Handler
        void OnSystemTick() override;
        // WindowSystemRequestBus::Handler
        void* GetNativeWindowHandle() override;
        void GetWindowSize(int& width, int& height) override;
        void SetWindowSize(int width, int height) override;
        //WindowsSystemNotificationBus
        void OnWindowSizeChanged(int width, int height) override;

    private:
        void* m_nativeWindowHandle = nullptr;
        int m_width = 0;
        int m_height = 0;
    };
}