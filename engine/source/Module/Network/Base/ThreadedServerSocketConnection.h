#pragma once

#include <AzCore/PlatformDef.h>

#if !defined(AZ_PLATFORM_EMSCRIPTEN)

#include "Network/Base/Message.h"

#include <AzCore/std/containers/queue.h>
#include <AzCore/std/string/string.h>
#include <AzCore/std/parallel/atomic.h>
#include <AzCore/std/parallel/thread.h>
#include <AzCore/std/parallel/mutex.h>
#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Component/EntityId.h>

namespace Module
{
    class ThreadedServerSocketConnection
    {
    public:
        struct ClientConnection;

        AZ_CLASS_ALLOCATOR(ThreadedServerSocketConnection, AZ::SystemAllocator, 0);

        ThreadedServerSocketConnection(AZ::EntityId entityId);

        ~ThreadedServerSocketConnection();

        void Bind(const AZStd::string &address, AZ::u16 port, AZ::u32 connectCount);

        void StopServer();

        bool IsListening() const { return m_running && m_listening; }

        void Dispatch();

    private:
        void WorkerThread(AZStd::string address, uint16_t port);

    private:
        AZ::EntityId m_entityId;

        AZStd::atomic_bool m_running{ false };
        AZStd::atomic_bool m_listening{ false };

        AZStd::thread m_workerThread;

        AZStd::vector<ClientConnection*> m_clientConnections;

        AZ::u32 m_maxConnectCount = 1;
    };
}

#endif
