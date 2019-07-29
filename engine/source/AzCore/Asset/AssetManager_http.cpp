#include <AzCore/Asset/AssetManager.h>
#include <AzCore/IO/GenericStreams.h>
#include <AzCore/IO/FileIO.h>
#include <AzCore/StringFunc/StringFunc.h>

#include <mongoose/mongoose.h>

namespace AZ
{
    namespace Data
    {
        class HttpLoadAssetJob
        {
        public:
            AZ_CLASS_ALLOCATOR(HttpLoadAssetJob, SystemAllocator, 0);

            HttpLoadAssetJob()
            {
                mg_mgr_init(&m_context, nullptr);
            }

            ~HttpLoadAssetJob()
            {
                mg_mgr_free(&m_context);
                if (m_buffer)
                {
                    azfree(m_buffer);
                }
            }

            IO::MemoryStream Fetch(const AZStd::string& url)
            {
                m_fetching = true;
                const auto connection = mg_connect_http(&m_context, EventHandler, url.c_str(), nullptr, nullptr);
                if (connection)
                {
                    connection->user_data = this;
                    while (m_fetching)
                    {
                        mg_mgr_poll(&m_context, 1000);
                    }
                }
                return AZStd::move(IO::MemoryStream(m_buffer, m_length));
            }

            static void EventHandler(mg_connection* connection, int event, void* data)
            {
                HttpLoadAssetJob* self = (HttpLoadAssetJob*)connection->user_data;
                
                switch (event)
                {
                case MG_EV_CONNECT:
                {
                    if (*(int*)data != 0)
                    {
                        AZ_Warning("HttpLoadAssetJob", false, "HttpLoadAssetJob::EventHandler, connect failed: %s\n", strerror(*(int*)data));
                        self->m_fetching = false;
                    }
                    break;
                }
                case MG_EV_HTTP_REPLY:
                {
                    connection->flags |= MG_F_CLOSE_IMMEDIATELY;
                    self->m_fetching = false;
                    http_message *message = (http_message *)data;
                    if (message->resp_code == 200)
                    {
                        self->m_length = message->body.len;
                        self->m_buffer = azmalloc(self->m_length);
                        memcpy(self->m_buffer, message->body.p, self->m_length);
                    }
                    else if (message->resp_code == 301)
                    {
                        const auto location = mg_get_http_header(message, "Location");
                        if (location)
                        {
                            AZStd::string url(location->p, location->len);
                            const auto redirect = mg_connect_http(&self->m_context, EventHandler, url.c_str(), nullptr, nullptr);
                            redirect->user_data = self;
                            self->m_fetching = true;
                        }
                    }
                    else
                    {
                        AZ_Warning("HttpLoadAssetJob", false, "HttpLoadAssetJob::EventHandler, fetch failed response code = %d\n", message->resp_code);
                    }
                    break;
                }
                case MG_EV_CLOSE:
                {
                    if (self->m_fetching) {
                        AZ_Warning("HttpLoadAssetJob", false, "HttpLoadAssetJob::EventHandler, server closed connection\n");
                        self->m_fetching = false;
                    }
                    break;
                }
                default:
                    break;
                }
            }

        private:
            mg_mgr m_context;
            bool   m_fetching = false;
            void*  m_buffer = nullptr;
            size_t m_length = 0;
        };

        void FetchAndCacheHttpData(const AZStd::string& relativePath)
        {
            AZStd::string url;
            EBUS_EVENT_RESULT(url, AZ::ApplicationRequests::Bus, GetCdnRoot);

            url.append(relativePath);

            HttpLoadAssetJob job;
            IO::MemoryStream stream = job.Fetch(url);

            if (stream.IsOpen())
            {
                AZStd::string cacheFolderPath = "@assets@/" + relativePath;
                StringFunc::Path::StripFullName(cacheFolderPath);
                IO::FileIOBase::GetInstance()->CreatePath(cacheFolderPath.c_str());
                IO::FileIOStream cacheWriteStream(("@assets@/" + relativePath).c_str(), IO::OpenMode::ModeWrite);
                if (cacheWriteStream.IsOpen())
                {
                    cacheWriteStream.Write(stream.GetLength(), stream.GetData());
                    AZ_TracePrintf("FetchAndCacheHttpData", "Cached %s to local filesystem!\n", relativePath.c_str());
                }
                else
                {
                    AZ_Warning("FetchAndCacheHttpData", false, "Cannot open file %s to write cache!\n", cacheWriteStream.GetFilename());
                }
            }
            else
            {
                AZ_Warning("FetchAndCacheHttpData", false, "Cannot load file %s from cdn!\n", relativePath.c_str());
            }
        }
    }
}
