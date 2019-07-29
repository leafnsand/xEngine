#include <AzCore/PlatformDef.h>

#if !defined(AZ_PLATFORM_EMSCRIPTEN)

#include "ThreadedServerSocketConnection.h"

#include "Network/EBus/NetworkServerComponentBus.h"

#include <AzCore/std/smart_ptr/make_shared.h>
#include <AzCore/std/parallel/lock.h>
#include <AzCore/Socket/AzSocket.h>
#include <AzCore/Casting/numeric_cast.h>
#include <AzCore/std/parallel/thread.h>

namespace Module
{
    struct ThreadedServerSocketConnection::ClientConnection
    {
        AZ_CLASS_ALLOCATOR(ClientConnection, AZ::SystemAllocator, 0);

        AZSOCKET m_socket = AZ_SOCKET_INVALID;
        
        AZStd::queue<Message> m_sendQueue;
        AZStd::mutex m_sendMutex;

        AZStd::queue<Message> m_recvQueue;
        AZStd::mutex m_recvMutex;
    };

    ThreadedServerSocketConnection::ThreadedServerSocketConnection(AZ::EntityId entityId)
    {
        m_entityId = entityId;
    }

    ThreadedServerSocketConnection::~ThreadedServerSocketConnection()
    {
        StopServer();
    }

    void ThreadedServerSocketConnection::Bind(const AZStd::string &address, AZ::u16 port, AZ::u32 connectCount)
    {
        m_maxConnectCount = connectCount;

        if (!m_workerThread.joinable())
        {
            m_running = true;
            AZStd::thread_desc threadDesc;
            threadDesc.m_name = "Network worker thread";
            m_workerThread = AZStd::thread(AZStd::bind(&ThreadedServerSocketConnection::WorkerThread, this, AZStd::string(address), port), &threadDesc);
        }
    }

    void ThreadedServerSocketConnection::StopServer()
    {
        if (m_workerThread.joinable())
        {
            m_running = false;
            m_workerThread.join();
        }
    }

    void ThreadedServerSocketConnection::Dispatch()
    {
        NetworkServerNotificationBus::ExecuteQueuedEvents();

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

             AZ_TracePrintf("Network", "ThreadedServerSocketConnection::OnSystemTick: recv message id %d, length %d\n", message.id, message.length);

            EBUS_EVENT_ID(m_entityId, NetworkServerNotificationBus, OnMessage, message.id, message.data.data(), message.data.size());
            messageQueue.pop();
        }
    }

    void ThreadedServerSocketConnection::WorkerThread(AZStd::string address, uint16_t port)
    {
        AZ_TracePrintf("Network", "ThreadedServerSocketConnection::WorkerThread: try listen %s:%u\n", address.c_str(), port);

        const auto socket = AZ::AzSock::Socket();
        if (!AZ::AzSock::IsAzSocketValid(socket))
        {
            EBUS_QUEUE_EVENT_ID(m_entityId, NetworkServerNotificationBus, OnStarted, false);
            AZ_TracePrintf("Network", "ThreadedServerSocketConnection::WorkerThread - No valid socket available to connect\n");
            return;
        }

        AZ::AzSock::AzSocketAddress socketAddress;
        if (!socketAddress.SetAddress(address, port))
        {
            EBUS_QUEUE_EVENT_ID(m_entityId, NetworkServerNotificationBus, OnStarted, false);
            AZ_TracePrintf("Network", "ThreadedServerSocketConnection::WorkerThread - could not obtain numeric address from string (%s)\n", address.c_str());
            return;
        }

        auto result = AZ::AzSock::Bind(socket, socketAddress);
        if (AZ::AzSock::SocketErrorOccured(result))
        {
            EBUS_QUEUE_EVENT_ID(m_entityId, NetworkServerNotificationBus, OnStarted, false);
            AZ_TracePrintf("Network", "ThreadedServerSocketConnection::WorkerThread - AZ::AzSock::bind returned an error %s\n", AZ::AzSock::GetStringForError(result));
            AZ::AzSock::CloseSocket(socket);
            return;
        }

        result = AZ::AzSock::Listen(socket, 1);
        if (AZ::AzSock::SocketErrorOccured(result))
        {
            EBUS_QUEUE_EVENT_ID(m_entityId, NetworkServerNotificationBus, OnStarted, false);
            AZ_TracePrintf("Network", "ThreadedServerSocketConnection::WorkerThread - AZ::AzSock::listen returned an error %s\n", AZ::AzSock::GetStringForError(result));
            AZ::AzSock::CloseSocket(socket);
            return;
        }

        // non blocking
        result = AZ::AzSock::SetSocketBlockingMode(socket, false);
        if (AZ::AzSock::SocketErrorOccured(result))
        {
            EBUS_QUEUE_EVENT_ID(m_entityId, NetworkServerNotificationBus, OnStarted, false);
            AZ_TracePrintf("Network", "ThreadedServerSocketConnection::WorkerThread - AZ::AzSock::SetSocketBlockingMode returned an error %s\n", AZ::AzSock::GetStringForError(result));
            AZ::AzSock::CloseSocket(socket);
            return;
        }

        // no delay
        result = AZ::AzSock::EnableTCPNoDelay(socket, false);
        if (AZ::AzSock::SocketErrorOccured(result))
        {
            EBUS_QUEUE_EVENT_ID(m_entityId, NetworkServerNotificationBus, OnStarted, false);
            AZ_TracePrintf("Network", "ThreadedServerSocketConnection::WorkerThread - AZ::AzSock::EnableTCPNoDelay returned an error %s\n", AZ::AzSock::GetStringForError(result));
            AZ::AzSock::CloseSocket(socket);
            return;
        }

        m_listening = true;

        AZ_TracePrintf("Network", "ThreadedServerSocketConnection::WorkerThread: listening %s:%u\n", address.c_str(), port);

        EBUS_QUEUE_EVENT_ID(m_entityId, NetworkServerNotificationBus, OnStarted, true);

        AZStd::queue<Message> recvQueue, sendQueue;
        while (m_running)
        {
            AZFD_SET read;
            FD_ZERO(&read);

            AZSOCKET MaxSocket = socket;

            for (size_t i = 0; i < m_connections.size(); ++i)
            {
                auto connect = m_connections[i];
                if (connect->IsValid())
                {
                    AZSOCKET sock = connect->GetSocket();
                    FD_SET(sock, &read);

                    if (MaxSocket < sock)
                    {
                        MaxSocket = sock;
                    }
                    continue;
                }

                EBUS_QUEUE_EVENT_ID(m_entityId, NetworkServerNotificationBus, OnClientDisconnect, connect->GetId());

                // The Connection is invalid, remove it.
                m_connections[i] = m_connections.back();
                m_connections.pop_back();

                --i;
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

            while(!sendQueue.empty())
            {
                auto& message = sendQueue.front();
                for (auto& sock : m_connections)
                {
                    if(message.recvId.IsNull() || sock->GetId() == message.recvId)
                    {
                        sock->Send(message);
                    }
                }
                sendQueue.pop();
            }

            AZTIMEVAL timeOut = { 1, 0 };
            int r = AZ::AzSock::Select(MaxSocket, &read, nullptr, nullptr, &timeOut);
            if (r > 0)
            {
                for (auto& sock : m_connections)
                {
                    if (!FD_ISSET(sock->GetSocket(), &read))
                    {
                        continue;
                    }
                    sock->Receive(recvQueue);
                }
            }

            // copy message
            {
                AZStd::lock_guard<AZStd::mutex> lock(m_recvMutex);
                if (m_recvQueue.empty() && !recvQueue.empty())
                {
                    m_recvQueue = AZStd::move(recvQueue);
                    recvQueue = AZStd::queue<Message>();
                }
            }

            AZ::AzSock::AzSocketAddress ad;
            auto client = AZ::AzSock::Accept(socket, ad);
            auto client_address = ad.GetAddress();

            if (AZ::AzSock::SocketErrorOccured(client))
            {
                if (static_cast<AZ::AzSock::AzSockError>(client) == AZ::AzSock::AzSockError::eASE_EWOULDBLOCK)
                {
                    AZStd::this_thread::sleep_for(AZStd::chrono::milliseconds(1));
                    continue;
                }
                AZ_TracePrintf("Network", "ThreadedServerSocketConnection::WorkerThread: accept to %s\n", client_address.c_str());

                goto close;
            }

            if (m_connections.size() >= m_maxConnectCount)
            {
                AZ::AzSock::CloseSocket(client);
                AZStd::this_thread::sleep_for(AZStd::chrono::milliseconds(1));
                continue;
            }

            result = AZ::AzSock::SetSocketBlockingMode(client, false);
            if (AZ::AzSock::SocketErrorOccured(result))
            {
                AZ_TracePrintf("Network", "ThreadedServerSocketConnection::WorkerThread - AZ::AzSock::SetSocketBlockingMode returned an error %s\n", AZ::AzSock::GetStringForError(result));
                AZ::AzSock::CloseSocket(client);
                return;
            }

            AZ_TracePrintf("Network", "ThreadedServerSocketConnection::WorkerThread: connected to %s\n", client_address.c_str());

            auto connect = AZStd::make_shared<ConnectionBase>(client);
            m_connections.emplace_back(connect);

            EBUS_QUEUE_EVENT_ID(m_entityId, NetworkServerNotificationBus, OnClientConnect, connect->GetId());
        }

    close:
        m_connections.clear();

        AZ_TracePrintf("Network", "ThreadedServerSocketConnection::WorkerThread - Disconnecting\n");

        if (AZ::AzSock::IsAzSocketValid(socket))
        {
            AZ_TracePrintf("Network", "ThreadedServerSocketConnection::WorkerThread - shutdown(%d)\n", socket);

            result = AZ::AzSock::Shutdown(socket, SD_BOTH);
            if (AZ::AzSock::SocketErrorOccured(result))
            {
                AZ_TracePrintf("Network", "ThreadedServerSocketConnection::WorkerThread - AZ::AzSock::shutdown returned an error %s\n", AZ::AzSock::GetStringForError(result));
            }

            AZ_TracePrintf("Network", "ThreadedServerSocketConnection::WorkerThread - closesocket(%d)\n", socket);

            result = AZ::AzSock::CloseSocket(socket);
            if (AZ::AzSock::SocketErrorOccured(result))
            {
                AZ_TracePrintf("Network", "ThreadedServerSocketConnection::WorkerThread - AZ::AzSock::closesocket returned an error %s\n", AZ::AzSock::GetStringForError(result));
            }
        }

        m_listening = false;

        AZ_TracePrintf("Network", "ThreadedServerSocketConnection::WorkerThread - Disconnected %d\n", socket);
    }    
}

#endif
