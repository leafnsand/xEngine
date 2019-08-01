#pragma once

#include <AzCore/PlatformDef.h>

#if defined(AZ_PLATFORM_EMSCRIPTEN)

#include "Network/Base/Message.h"

#include <AzCore/Socket/AzSocket.h>
#include <AzCore/std/containers/queue.h>
#include <AzCore/std/string/string.h>
#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Component/EntityId.h>

namespace Module
{
    class SocketConnection
    {
    public:
        AZ_CLASS_ALLOCATOR(SocketConnection, AZ::SystemAllocator, 0);

        SocketConnection(AZ::EntityId entityId);

        ~SocketConnection();

        void Connect(const char *address, uint16_t port);

        void Disconnect();

        bool IsConnected() const { return AZ::AzSock::IsAzSocketValid(m_socket); }

        void Send(uint32_t id, const void *buffer, uint32_t length);

        void Dispatch();

    protected:
        void DispatchInternal();

        void ConnectInternal(const char *address, uint16_t port);

        void DisconnectInternal();

    private:
        AZ::EntityId m_entityId;

        AZSOCKET m_socket{ AZ_SOCKET_INVALID };

        AZStd::queue<Message> m_sendQueue;
        AZStd::string m_sendBuffer;
        AZStd::queue<Message> m_recvQueue;
        AZStd::string m_recvBuffer;
    };
}

#endif
