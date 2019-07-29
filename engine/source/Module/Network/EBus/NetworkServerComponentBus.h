#pragma once

#if !defined(AZ_PLATFORM_EMSCRIPTEN)

#include <AzCore/Component/ComponentBus.h>

namespace Module
{
    class NetworkServerRequest :
        public AZ::ComponentBus
    {
    public:
        virtual void StartServer(const char *address, AZ::u32 port, AZ::u32 connectCount) = 0;

        virtual bool IsListening() const = 0;

        virtual void StopServer() = 0;
    };

    using NetworkServerRequestBus = AZ::EBus<NetworkServerRequest>;

    class NetworkServerNotification :
        public AZ::ComponentBus
    {
    public:
        static const bool EnableEventQueue = true;

        virtual void OnStarted(bool connected) = 0;

        virtual void OnClientConnect() = 0;

        virtual void OnClientDisconnect() = 0;

        virtual void OnMessage(AZ::u32 id, const void *buffer, size_t length) = 0;

        template<class Bus>
        struct ConnectionPolicy
            : public AZ::EBusConnectionPolicy<Bus>
        {
            static void Connect(typename Bus::BusPtr& busPtr, typename Bus::Context& context, typename Bus::HandlerNode& handler, const typename Bus::BusIdType& id = 0)
            {
                AZ::EBusConnectionPolicy<Bus>::Connect(busPtr, context, handler, id);

                bool isConnected = false;
                EBUS_EVENT_ID_RESULT(isConnected, id, NetworkServerRequestBus, IsConnected);
                typename Bus::template CallstackEntryIterator<typename Bus::InterfaceType**> callstack(nullptr, &id); // Workaround for GetCurrentBusId in callee
                handler->OnStarted(isConnected);
            }
        };
    };

    using NetworkServerNotificationBus = AZ::EBus<NetworkServerNotification>;
}
#endif
