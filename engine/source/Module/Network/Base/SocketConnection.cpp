#include <AzCore/PlatformDef.h>

#if defined(AZ_PLATFORM_EMSCRIPTEN)

#include "SocketConnection.h"

#include "Network/EBus/NetworkComponentBus.h"

namespace Module
{
    SocketConnection::SocketConnection(AZ::EntityId entityId)
    {
        m_entityId = entityId;
    }

    SocketConnection::~SocketConnection()
    {
        Disconnect();
    }

    void SocketConnection::Connect(const char *address, uint16_t port)
    {
        if (!AZ::AzSock::IsAzSocketValid(m_socket))
        {
            ConnectInternal(address, port);
        }
    }

    void SocketConnection::Disconnect()
    {
        if (AZ::AzSock::IsAzSocketValid(m_socket))
        {
            DisconnectInternal();
        }
    }

    void SocketConnection::Send(uint32_t id, const void* buffer, uint32_t length)
    {
        Message message;
        message.id = id;
        message.length = length;
        message.data.resize_no_construct(length);
        if (length > 0)
        {
            memcpy(message.data.data(), buffer, length);
        }
        m_sendQueue.push(message);
    }

    void SocketConnection::Dispatch()
    {
        DispatchInternal();

        while (!m_recvQueue.empty())
        {
            const auto message = m_recvQueue.front();
            EBUS_EVENT_ID(m_entityId, NetworkClientNotificationBus, OnMessage, message.id, message.data.data(), message.data.size());
            m_recvQueue.pop();
        }
    }

    void SocketConnection::DispatchInternal()
    {
        if (!AZ::AzSock::IsAzSocketValid(m_socket)) return;
        // encode send message
        {
            while (!m_sendQueue.empty())
            {
                auto message = m_sendQueue.front();
                const auto offset = m_sendBuffer.size();
                m_sendBuffer.resize_no_construct(offset + message.size());
                message.Encode(m_sendBuffer.data() + offset, message.size());

                AZ_TracePrintf("Network", "SocketConnection::WorkerInternal: encode send message, id: %d, length: %d\n", message.id, message.length);

                m_sendQueue.pop();
            }
        }
        // send data
        {
            size_t sent = 0;
            size_t total = m_sendBuffer.size();
            while (sent < total)
            {
                auto send_result = AZ::AzSock::Send(m_socket, m_sendBuffer.data() + sent, int(total - sent), 0);
                if (AZ::AzSock::SocketErrorOccured(send_result))
                {
                    if (static_cast<AZ::AzSock::AzSockError>(send_result) == AZ::AzSock::AzSockError::eASE_EWOULDBLOCK)
                    {
                        break;
                    }
                    AZ_TracePrintf("Network", "SocketConnection::WorkerInternal: send error: %s\n", AZ::AzSock::GetStringForError(send_result));
                    Disconnect();
                    return;
                }
                sent += send_result;
            }
            if (sent == total)
            {
                m_sendBuffer.clear();
            }
            else
            {
                m_sendBuffer.erase(0, sent);
            }
        }
        // recv data
        {
            static const auto recv_data_buffer_size = 1024;
            static char recv_data_buffer[recv_data_buffer_size];
            while (auto recv_result = AZ::AzSock::Recv(m_socket, recv_data_buffer, recv_data_buffer_size, 0))
            {
                if (AZ::AzSock::SocketErrorOccured(recv_result))
                {
                    if (static_cast<AZ::AzSock::AzSockError>(recv_result) == AZ::AzSock::AzSockError::eASE_EWOULDBLOCK)
                    {
                        break;
                    }
                    AZ_TracePrintf("Network", "SocketConnection::WorkerInternal: recv error: %s\n", AZ::AzSock::GetStringForError(recv_result));
                    Disconnect();
                    return;
                }
                m_recvBuffer.append(recv_data_buffer, recv_result);
            }
        }
        // decode recv message
        {
            size_t decoded = 0;
            size_t total = m_recvBuffer.size();
            Message message;
            while (decoded < total)
            {
                if (message.Decode(m_recvBuffer.data() + decoded, total - decoded))
                {
                    decoded += message.size();

                    m_recvQueue.push(message);
                    message.length = 0;

                    AZ_TracePrintf("Network", "SocketConnection::WorkerInternal: recv message, id: %d, length: %d\n", message.id, message.length);
                }
                else
                {
                    break;
                }
            }
            if (decoded == total)
            {
                m_recvBuffer.clear();
            }
            else
            {
                m_recvBuffer.erase(0, decoded);
            }
        }
    }

    void SocketConnection::ConnectInternal(const char *address, uint16_t port)
    {
        AZ_TracePrintf("Network", "SocketConnection::ConnectInternal: connecting to %s:%u\n", address, port);

        m_socket = AZ::AzSock::Socket();
        if (!AZ::AzSock::IsAzSocketValid(m_socket))
        {
            AZ_TracePrintf("Network", "SocketConnection::ConnectInternal - could not open socket for (%s)\n", address);
            EBUS_EVENT_ID(m_entityId, NetworkClientNotificationBus, OnConnected, false);
            return;
        }

        AZ::AzSock::AzSocketAddress socketAddress;
        socketAddress.SetAddress(address, port);

        auto result = AZ::AzSock::Connect(m_socket, socketAddress);
        if (AZ::AzSock::SocketErrorOccured(result))
        {
            if (result == static_cast<int32_t>(AZ::AzSock::AzSockError::eASE_EWOULDBLOCK_CONN))
            {
                AZ_TracePrintf("Network", "SocketConnection::ConnectInternal - connect warning, %s!\n", AZ::AzSock::GetStringForError(result));
            }
            else
            {
                AZ_TracePrintf("Network", "SocketConnection::ConnectInternal - connect failed, %s!\n", AZ::AzSock::GetStringForError(result));
                AZ::AzSock::CloseSocket(m_socket);
                m_socket = AZ_SOCKET_INVALID;
                EBUS_EVENT_ID(m_entityId, NetworkClientNotificationBus, OnConnected, false);
                return;
            }
        }

        result = AZ::AzSock::SetSocketBlockingMode(m_socket, false);
        if (AZ::AzSock::SocketErrorOccured(result))
        {
            AZ_TracePrintf("Network", "SocketConnection::ConnectInternal - connect warning, set non-block failed, %s!\n", AZ::AzSock::GetStringForError(result));
        }

        result = AZ::AzSock::EnableTCPNoDelay(m_socket, true);
        if (AZ::AzSock::SocketErrorOccured(result))
        {
            AZ_TracePrintf("Network", "SocketConnection::ConnectInternal - connect warning, set tcp no delay failed, %s!\n", AZ::AzSock::GetStringForError(result));
        }

        EBUS_EVENT_ID(m_entityId, NetworkClientNotificationBus, OnConnected, true);

        AZ_TracePrintf("Network", "SocketConnection::ConnectInternal: connected to %s:%u\n", address, port);
    }

    void SocketConnection::DisconnectInternal()
    {
        AZ_TracePrintf("Network", "SocketConnection::DisconnectInternal - Disconnecting\n");

        AZ::AzSock::CloseSocket(m_socket);
        m_socket = AZ_SOCKET_INVALID;

        AZ_TracePrintf("Network", "SocketConnection::DisconnectInternal - Disconnected\n");
    }
}

#endif
