#include <AzCore/PlatformDef.h>

#if !defined(AZ_PLATFORM_EMSCRIPTEN)

#include "ThreadedClientSocketConnection.h"

#include "Network/EBus/NetworkClientComponentBus.h"
#include "Network/Base/ConnectionBase.h"

#include <AzCore/std/smart_ptr/make_shared.h>
#include <AzCore/std/parallel/lock.h>
#include <AzCore/Socket/AzSocket.h>
#include <AzCore/Casting/numeric_cast.h>
#include <AzCore/std/parallel/thread.h>

namespace Module
{
    ThreadedClientSocketConnection::ThreadedClientSocketConnection(AZ::EntityId entityId)
    {
        m_entityId = entityId;
    }

    ThreadedClientSocketConnection::~ThreadedClientSocketConnection()
    {
        Disconnect();
    }

    void ThreadedClientSocketConnection::Connect(const AZStd::string &address, AZ::u16 port)
    {
        if (!m_workerThread.joinable())
        {
            m_running = true;
            AZStd::thread_desc threadDesc;
            threadDesc.m_name = "Network worker thread";
            m_workerThread = AZStd::thread(AZStd::bind(&ThreadedClientSocketConnection::WorkerThread, this, AZStd::string(address), port), &threadDesc);
        }
    }

    void ThreadedClientSocketConnection::Disconnect()
    {
        if (m_workerThread.joinable())
        {
            m_running = false;
            m_workerThread.join();
        }
    }

    void ThreadedClientSocketConnection::Send(AZ::u32 id, const void* buffer, AZ::u32 length)
    {
        Message message;
        message.id = id;
        message.length = length;
        message.data.resize_no_construct(length);
        memcpy(message.data.data(), buffer, length);

        // AZ_TracePrintf("Network", "ThreadedClientSocketConnection::Send: Queue %d %d\n", message.id, message.length);

        {
            AZStd::lock_guard<AZStd::mutex> lock(m_sendMutex);
            m_sendQueue.push(message);
        }
    }

    void ThreadedClientSocketConnection::Dispatch()
    {
        NetworkClientNotificationBus::ExecuteQueuedEvents();

        AZStd::queue<Message> messageQueue;
        {
            AZStd::lock_guard<AZStd::mutex> lock(m_recvMutex);
            if (!m_recvQueue.empty())
            {
                messageQueue = AZStd::move(m_recvQueue);
                m_recvQueue = AZStd::queue<Message>();
            }
        }
        while (!messageQueue.empty())
        {
            auto &message = messageQueue.front();

            // AZ_TracePrintf("Network", "ThreadedClientSocketConnection::OnSystemTick: recv message id %d, length %d\n", message.id, message.length);

            EBUS_EVENT_ID(m_entityId, NetworkClientNotificationBus, OnMessage, message.id, message.data.data(), message.data.size());
            messageQueue.pop();
        }
    }

    void ThreadedClientSocketConnection::WorkerThread(AZStd::string address, uint16_t port)
    {
        AZ_TracePrintf("Network", "ThreadedClientSocketConnection::WorkerThread: connecting to %s:%u\n", address.c_str(), port);

        const auto socket = AZ::AzSock::Socket();
        if (!AZ::AzSock::IsAzSocketValid(socket))
        {
            EBUS_QUEUE_EVENT_ID(m_entityId, NetworkClientNotificationBus, OnConnected, false);
            AZ_TracePrintf("Network", "ThreadedClientSocketConnection::WorkerThread - No valid socket available to connect\n");
            return;
        }

        AZ::AzSock::AzSocketAddress socketAddress;
        if (!socketAddress.SetAddress(address, port))
        {
            EBUS_QUEUE_EVENT_ID(m_entityId, NetworkClientNotificationBus, OnConnected, false);
            AZ_TracePrintf("Network", "ThreadedClientSocketConnection::WorkerThread - could not obtain numeric address from string (%s)\n", address.c_str());
            return;
        }

        auto result = AZ::AzSock::Connect(socket, socketAddress);
        if (AZ::AzSock::SocketErrorOccured(result))
        {
            EBUS_QUEUE_EVENT_ID(m_entityId, NetworkClientNotificationBus, OnConnected, false);
            AZ_TracePrintf("Network", "ThreadedClientSocketConnection::WorkerThread - AZ::AzSock::connect returned an error %s\n", AZ::AzSock::GetStringForError(result));
            AZ::AzSock::CloseSocket(socket);
            return;
        }

        // non blocking
        result = AZ::AzSock::SetSocketBlockingMode(socket, false);
        if (AZ::AzSock::SocketErrorOccured(result))
        {
            EBUS_QUEUE_EVENT_ID(m_entityId, NetworkClientNotificationBus, OnConnected, false);
            AZ_TracePrintf("Network", "ThreadedClientSocketConnection::WorkerThread - AZ::AzSock::SetSocketBlockingMode returned an error %s\n", AZ::AzSock::GetStringForError(result));
            AZ::AzSock::CloseSocket(socket);
            return;
        }

        // no delay
        result = AZ::AzSock::EnableTCPNoDelay(socket, true);
        if (AZ::AzSock::SocketErrorOccured(result))
        {
            EBUS_QUEUE_EVENT_ID(m_entityId, NetworkClientNotificationBus, OnConnected, false);
            AZ_TracePrintf("Network", "ThreadedClientSocketConnection::WorkerThread - AZ::AzSock::EnableTCPNoDelay returned an error %s\n", AZ::AzSock::GetStringForError(result));
            AZ::AzSock::CloseSocket(socket);
            return;
        }

        m_connected = true;

        AZ_TracePrintf("Network", "ThreadedClientSocketConnection::WorkerThread: connected to %s:%u\n", address.c_str(), port);

        EBUS_QUEUE_EVENT_ID(m_entityId, NetworkClientNotificationBus, OnConnected, true);

        AZStd::queue<Message> sendQueue, recvQueue;
        AZStd::string sendBuffer, recvBuffer;
        const auto recvDataBufferSize = 1024;
        char recvDataBuffer[recvDataBufferSize];

        auto connection = AZStd::make_shared<ConnectionBase>(socket);
        while (m_running)
        {
            if (!connection->IsValid())
            {
                AZ_TracePrintf("Network", "ThreadedClientSocketConnection::WorkerThread: connection IsValid false\n");

                goto close;
            }
            // copy send data
            {
                AZStd::lock_guard<AZStd::mutex> lock(m_sendMutex);
                if (!m_sendQueue.empty())
                {
                    sendQueue = AZStd::move(m_sendQueue);
                    m_sendQueue = AZStd::queue<Message>();
                }
            }

            while (!sendQueue.empty())
            {
                auto& message = sendQueue.front();
                connection->Send(message);
                sendQueue.pop();
            }

            connection->Receive(recvQueue);

            // copy message
            {
                AZStd::lock_guard<AZStd::mutex> lock(m_recvMutex);
                if (m_recvQueue.empty())
                {
                    m_recvQueue = AZStd::move(recvQueue);
                    recvQueue = AZStd::queue<Message>();
                }
            }

            AZStd::this_thread::sleep_for(AZStd::chrono::milliseconds(1));
        }

    close:

        AZ_TracePrintf("Network", "ThreadedClientSocketConnection::WorkerThread - Disconnecting\n");

        if (AZ::AzSock::IsAzSocketValid(socket))
        {
            AZ_TracePrintf("Network", "ThreadedClientSocketConnection::WorkerThread - shutdown(%d)\n", socket);

            result = AZ::AzSock::Shutdown(socket, SD_BOTH);
            if (AZ::AzSock::SocketErrorOccured(result))
            {
                AZ_TracePrintf("Network", "ThreadedClientSocketConnection::WorkerThread - AZ::AzSock::shutdown returned an error %s\n", AZ::AzSock::GetStringForError(result));
            }

            AZ_TracePrintf("Network", "ThreadedClientSocketConnection::WorkerThread - closesocket(%d)\n", socket);

            result = AZ::AzSock::CloseSocket(socket);
            if (AZ::AzSock::SocketErrorOccured(result))
            {
                AZ_TracePrintf("Network", "ThreadedClientSocketConnection::WorkerThread - AZ::AzSock::closesocket returned an error %s\n", AZ::AzSock::GetStringForError(result));
            }
        }

        m_connected = false;

        AZ_TracePrintf("Network", "ThreadedClientSocketConnection::WorkerThread - Disconnected %d\n", socket);
    }
}

#endif
