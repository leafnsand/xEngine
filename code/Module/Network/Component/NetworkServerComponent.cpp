#if !defined(AZ_PLATFORM_EMSCRIPTEN)

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

#include "NetworkServerComponent.h"

namespace Module
{
    class BehaviorNetworkServerNotificationBus : public NetworkServerNotificationBus::Handler, public AZ::BehaviorEBusHandler
    {
    public:
        AZ_EBUS_BEHAVIOR_BINDER(BehaviorNetworkServerNotificationBus, "{F8B3CFCE-6F6B-480F-AA5D-0F15E48A5507}", AZ::SystemAllocator, OnStarted, OnMessage, OnClientConnect, OnClientDisconnect);

        void OnStarted(bool connected) override
        {
            Call(FN_OnStarted, connected);
        }

        void OnMessage(AZ::u32 id, const void *buffer, size_t length) override
        {
            Call(FN_OnMessage, id, buffer, length);
        }

        void OnClientConnect() override
        {
            Call(FN_OnClientConnect);
        }

        void OnClientDisconnect() override
        {
            Call(FN_OnClientDisconnect);
        }
    };

    void NetworkServerComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serialize_context = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize_context->Class<NetworkServerComponent>()->Version(1);
        }

        if (auto behavior_context = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behavior_context->EBus<NetworkServerRequestBus>("NetworkServerRequestBus")
                ->Attribute(AZ::Script::Attributes::DisallowBroadcast, true)
                ->Event("StartServer", &NetworkServerRequestBus::Events::StartServer)
                ->Event("StopServer", &NetworkServerRequestBus::Events::StopServer)
                ->Event("IsListening", &NetworkServerRequestBus::Events::IsListening);

            behavior_context->EBus<NetworkServerNotificationBus>("NetworkServerNotificationBus")
                ->Handler<BehaviorNetworkServerNotificationBus>();
        }
    }

    void NetworkServerComponent::Activate()
    {
        m_connection = aznew ThreadedServerSocketConnection(GetEntityId());

        NetworkServerRequestBus::Handler::BusConnect(GetEntityId());
        AZ::SystemTickBus::Handler::BusConnect();
    }

    void NetworkServerComponent::Deactivate()
    {
        AZ::SystemTickBus::Handler::BusDisconnect();
        NetworkServerRequestBus::Handler::BusDisconnect();

        delete m_connection;
    }

    void NetworkServerComponent::StartServer(const char *address, AZ::u32 port, AZ::u32 connectCount)
    {
        m_connection->Bind(address, port, connectCount);
    }

    bool NetworkServerComponent::IsListening() const
    {
        return m_connection->IsListening();
    }

    void NetworkServerComponent::StopServer()
    {
        m_connection->StopServer();
    }

    void NetworkServerComponent::OnSystemTick()
    {
        m_connection->Dispatch();
    }
}

#endif
