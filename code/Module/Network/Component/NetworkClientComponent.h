#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>

#include "Network/EBus/NetworkClientComponentBus.h"

#if defined(AZ_PLATFORM_EMSCRIPTEN)
#   include "Network/Base/SocketConnection.h"
#else
#   include "Network/Base/ThreadedClientSocketConnection.h"
#endif

namespace Module
{
    class NetworkClientComponent :
        public AZ::Component,
        public AZ::SystemTickBus::Handler,
        public NetworkClientRequestBus::Handler
    {
    public:
        AZ_COMPONENT(NetworkClientComponent, "{6A8F6A43-8D72-4725-B117-7F5DC3D2F234}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC("NetworkService"));
        }

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC("NetworkService"));
        }

        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
        {
        }

        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
        {
        }

    protected:
        /////////////////////////////////////////////////////////////////////////////////////
        // AZ::Component
        void Activate() override;
        void Deactivate() override;
        /////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////
        // AZ::SystemTickBus::Handler
        void Connect(const char *address, AZ::u32 port) override;
        bool IsConnected() const override;
        void Disconnect() override;
        void Send(AZ::u32 id, const void *buffer, size_t length) override;
        /////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////
        // NetworkClientRequestBus::Handler
        void OnSystemTick() override;
        /////////////////////////////////////////////////////////////////////////////////////

    private:
#if defined(AZ_PLATFORM_EMSCRIPTEN)
        SocketConnection* m_connection = nullptr;
#else
        ThreadedClientSocketConnection* m_connection = nullptr;
#endif
    };
}
