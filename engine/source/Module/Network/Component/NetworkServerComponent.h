#pragma once

#if !defined(AZ_PLATFORM_EMSCRIPTEN)

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>

#include "Network/EBus/NetworkServerComponentBus.h"
#include "Network/Base/ThreadedServerSocketConnection.h"

namespace Module
{
    class NetworkServerComponent :
        public AZ::Component,
        public AZ::SystemTickBus::Handler,
        public NetworkServerRequestBus::Handler
    {
    public:
        AZ_COMPONENT(NetworkServerComponent, "{D7308697-5EBA-4E35-97FD-CA3FD2700F89}");

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
        void StartServer(const char *address, AZ::u32 port, AZ::u32 connectCount) override;
        bool IsListening() const override;
        void StopServer() override;
        /////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////
        // NetworkRequestBus::Handler
        void OnSystemTick() override;
        /////////////////////////////////////////////////////////////////////////////////////

    private:
        ThreadedServerSocketConnection* m_connection = nullptr;
    };
}
#endif