#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

#include "NetworkClientComponent.h"

namespace Module
{
    class BehaviorNetworkClientNotificationBus : public NetworkClientNotificationBus::Handler, public AZ::BehaviorEBusHandler
    {
    public:
        AZ_EBUS_BEHAVIOR_BINDER(BehaviorNetworkClientNotificationBus, "{7235F292-03CC-4F41-B8BA-2735008B86E6}", AZ::SystemAllocator, OnConnected, OnMessage);

        void OnConnected(bool connected) override
        {
            Call(FN_OnConnected, connected);
        }

        void OnMessage(AZ::u32 id, const void *buffer, size_t length) override
        {
            Call(FN_OnMessage, id, buffer, length);
        }
    };

    void NetworkClientComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serialize_context = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize_context->Class<NetworkClientComponent>()->Version(1);
        }

        if (auto behavior_context = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behavior_context->EBus<NetworkClientRequestBus>("NetworkClientRequestBus")
                ->Attribute(AZ::Script::Attributes::DisallowBroadcast, true)
                ->Event("Connect", &NetworkClientRequestBus::Events::Connect)
                ->Event("Disconnect", &NetworkClientRequestBus::Events::Disconnect)
                ->Event("Send", &NetworkClientRequestBus::Events::Send);

            behavior_context->EBus<NetworkClientNotificationBus>("NetworkClientNotificationBus")
                ->Handler<BehaviorNetworkClientNotificationBus>();

            behavior_context->Class<NetworkClientComponent>("NetworkClientComponent");
        }
    }

    void NetworkClientComponent::Activate()
    {
#if defined(AZ_PLATFORM_EMSCRIPTEN)
        m_connection = aznew SocketConnection(GetEntityId());
#else
        m_connection = aznew ThreadedClientSocketConnection(GetEntityId());
#endif

        NetworkClientRequestBus::Handler::BusConnect(GetEntityId());
        AZ::SystemTickBus::Handler::BusConnect();
    }

    void NetworkClientComponent::Deactivate()
    {
        AZ::SystemTickBus::Handler::BusDisconnect();
        NetworkClientRequestBus::Handler::BusDisconnect();

        delete m_connection;
    }

    void NetworkClientComponent::Connect(const char *address, AZ::u32 port)
    {
        m_connection->Connect(address, port);
    }

    bool NetworkClientComponent::IsConnected() const
    {
        return m_connection->IsConnected();
    }

    void NetworkClientComponent::Disconnect()
    {
        m_connection->Disconnect();
    }

    void NetworkClientComponent::Send(AZ::u32 id, const void *buffer, size_t length)
    {
        m_connection->Send(id, buffer, length);
    }

    void NetworkClientComponent::OnSystemTick()
    {
        m_connection->Dispatch();
    }
}
