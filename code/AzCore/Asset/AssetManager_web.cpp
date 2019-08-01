#include <AzCore/PlatformDef.h>

#if defined(AZ_PLATFORM_EMSCRIPTEN)

#include <AzCore/Asset/AssetManager.h>
#include <AzCore/IO/GenericStreams.h>

#include <emscripten/emscripten.h>

namespace AZ
{
    namespace Data
    {
        class WebLoadAssetJob
        {
        public:
            AZ_CLASS_ALLOCATOR(WebLoadAssetJob, SystemAllocator, 0);

            WebLoadAssetJob(const Asset<AssetData>& asset, AssetHandler* handler, const AssetFilterCB& assetLoadFilterCB)
            {
                m_asset = asset;
                m_assetHandler = handler;
                m_assetLoadFilterCB = assetLoadFilterCB;
            }

            static void OnAsyncLoad(void* userdata, void* buffer, int length)
            {
                WebLoadAssetJob* self = (WebLoadAssetJob*)userdata;

                AZ_TracePrintf("AssetManager", "OnAsyncLoad finish: relative path = %s\n", self->m_asset.GetHint().c_str());

                IO::MemoryStream stream(buffer, length);
                const bool loadSucceeded = self->m_assetHandler->LoadAssetData(self->m_asset, &stream, self->m_assetLoadFilterCB);
                self->m_assetHandler->InitAsset(self->m_asset, loadSucceeded, false);

                delete self;
            }

            static void OnAsyncError(void* userdata)
            {
                WebLoadAssetJob* self = (WebLoadAssetJob*)userdata;

                AZ_Warning("AssetManager", false, "ASyncLoadData error: relative path = %s\n", self->m_asset.GetHint().c_str());

                self->m_assetHandler->InitAsset(self->m_asset, false, false);

                delete self;
            }

        private:
            Asset<AssetData>    m_asset;
            AssetHandler*       m_assetHandler;
            AssetFilterCB       m_assetLoadFilterCB;
        };

        void AsyncLoadData(const Asset<AssetData>& asset, AssetHandler* handler, const AssetFilterCB& assetLoadFilterCB)
        {
            AZStd::string path;
            EBUS_EVENT_RESULT(path, AZ::ApplicationRequests::Bus, GetCdnRoot);

            path.append(asset.GetHint());

            auto job = aznew WebLoadAssetJob(asset, handler, assetLoadFilterCB);

            emscripten_async_wget_data(path.c_str(), job, WebLoadAssetJob::OnAsyncLoad, WebLoadAssetJob::OnAsyncError);
        }
    }
}

#endif