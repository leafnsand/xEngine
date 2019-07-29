#pragma once

#include <AzCore/PlatformDef.h>

#if !defined(AZ_PLATFORM_EMSCRIPTEN)

#include "Network/Base/Message.h"

#include <AzCore/std/functional.h>
#include <AzCore/std/containers/queue.h>
#include <AzCore/std/string/string.h>
#include <AzCore/std/parallel/atomic.h>
#include <AzCore/std/parallel/thread.h>
#include <AzCore/std/parallel/mutex.h>
#include <AzCore/Socket/AzSocket_fwd.h>
#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Component/EntityId.h>

namespace Module
{
    class ThreadedClientSocketConnection
    {
    public:
        AZ_CLASS_ALLOCATOR(ThreadedClientSocketConnection, AZ::SystemAllocator, 0);

        ThreadedClientSocketConnection(AZ::EntityId entityId);

        ~ThreadedClientSocketConnection();

        void Connect(const AZStd::string &address, AZ::u16 port);

        void Disconnect();

        bool IsConnected() const { return m_running && m_connected; }

        void Send(AZ::u32 id, const void *buffer, AZ::u32 length);

        void Dispatch();

    private:
        void WorkerThread(AZStd::string address, uint16_t port);

    private:
        AZ::EntityId m_entityId;

        AZStd::atomic_bool m_running{ false };
        AZStd::atomic_bool m_connected{ false };

        AZStd::thread m_workerThread;

        AZStd::queue<Message> m_sendQueue;
        AZStd::mutex m_sendMutex;

        AZStd::queue<Message> m_recvQueue;
        AZStd::mutex m_recvMutex;
    };
}

#endif
