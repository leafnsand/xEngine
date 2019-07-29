#include <Window/Component/WindowSystemComponent.h>

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

extern void* CreateNativeWindow(int& width, int& height);

namespace Module
{
    void WindowSystemComponent::Reflect(AZ::ReflectContext* reflection)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection))
        {
            serializeContext->Class<WindowSystemComponent, AZ::Component>()
                ->Field("width", &WindowSystemComponent::m_width)
                ->Field("height", &WindowSystemComponent::m_height)
                ;
        }

        if (auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection))
        {
            behaviorContext->EBus<WindowSystemRequestBus>("WindowSystemRequestBus")
                ->Event("SetWindowSize", &WindowSystemRequestBus::Events::SetWindowSize);

            behaviorContext->Class<WindowSystemComponent>("WindowSystemComponent")
                ->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::ExcludeFlags::Preview)
                ->Constructor()
                ->Property("m_width", BehaviorValueProperty(&WindowSystemComponent::m_width))
                ->Property("m_height", BehaviorValueProperty(&WindowSystemComponent::m_height));
        }
    }

    void WindowSystemComponent::Init()
    {
        m_nativeWindowHandle = CreateNativeWindow(m_width, m_height);
    }

    void WindowSystemComponent::Activate()
    {
        AZ::SystemTickBus::Handler::BusConnect();
        WindowSystemRequestBus::Handler::BusConnect();
        WindowsSystemNotificationBus::Handler::BusConnect();
    }

    void WindowSystemComponent::Deactivate()
    {
        AZ::SystemTickBus::Handler::BusDisconnect();
        WindowSystemRequestBus::Handler::BusDisconnect();
        WindowsSystemNotificationBus::Handler::BusDisconnect();
    }

    void WindowSystemComponent::OnSystemTick()
    {
    }

    void* WindowSystemComponent::GetNativeWindowHandle()
    {
        return m_nativeWindowHandle;
    }

    void WindowSystemComponent::GetWindowSize(int& width, int& height)
    {
        width = m_width;
        height = m_height;
    }

    void WindowSystemComponent::SetWindowSize(int width, int height)
    {
        m_width = width;
        m_height = height;
    }

    void WindowSystemComponent::OnWindowSizeChanged(int width, int height)
    {
        m_width = width;
        m_height = height;

        AZ_TracePrintf("WindowSystemComponent", "OnWindowSizeChanged width = %d, height = %d\n", width, height);
    }
}
