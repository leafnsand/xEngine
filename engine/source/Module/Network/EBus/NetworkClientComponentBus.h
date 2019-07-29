#pragma once

#include <AzCore/Component/ComponentBus.h>

namespace Module
{
    class NetworkClientRequest :
        public AZ::ComponentBus
    {
    public:
        virtual void Connect(const char *address, AZ::u32 port) = 0;

        virtual bool IsConnected() const = 0;

        virtual void Disconnect() = 0;

        virtual void Send(AZ::u32 id, const void *buffer, size_t length) = 0;
    };

    using NetworkClientRequestBus = AZ::EBus<NetworkClientRequest>;

    class NetworkClientNotification :
        public AZ::ComponentBus
    {
    public:
        static const bool EnableEventQueue = true;

        virtual void OnConnected(bool connected) = 0;

        virtual void OnMessage(AZ::u32 id, const void *buffer, size_t length) = 0;

        template<class Bus>
        struct ConnectionPolicy
            : public AZ::EBusConnectionPolicy<Bus>
        {
            static void Connect(typename Bus::BusPtr& busPtr, typename Bus::Context& context, typename Bus::HandlerNode& handler, const typename Bus::BusIdType& id = 0)
            {
                AZ::EBusConnectionPolicy<Bus>::Connect(busPtr, context, handler, id);

                bool isConnected = false;
                EBUS_EVENT_ID_RESULT(isConnected, id, NetworkClientRequestBus, IsConnected);
                typename Bus::template CallstackEntryIterator<typename Bus::InterfaceType**> callstack(nullptr, &id); // Workaround for GetCurrentBusId in callee
                handler->OnConnected(isConnected);
            }
        };
    };

    using NetworkClientNotificationBus = AZ::EBus<NetworkClientNotification>;
}
