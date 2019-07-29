/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#pragma once

#include <AzCore/API/ApplicationAPI.h>
#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/Asset/AssetManagerBus.h>
#include <AzCore/Memory/Memory.h>
#include <AzCore/Memory/SystemAllocator.h> // used as allocator for most components
#include <AzCore/std/parallel/mutex.h>
#include <AzCore/std/string/string.h>
#include <AzCore/std/containers/unordered_map.h>
#include <AzCore/std/containers/intrusive_list.h>

namespace AZ
{
    class JobManager;
    class JobContext;

    namespace IO
    {
        class GenericStream;
        enum class OpenMode : AZ::u32;
    }

    namespace Data
    {
        class AssetHandler;
        class AssetDatabaseJob;

        class AssetStreamInfo
        {
        public:
            AssetStreamInfo()
                : m_streamFlags(IO::OpenMode())
                , m_dataLen(0)
                , m_dataOffset(0)
                , m_isCustomStreamType(false)
            {}

            bool IsValid() const
            {
                return !m_streamName.empty();
            }

            AZStd::string   m_streamName;
            IO::OpenMode    m_streamFlags;
            u64             m_dataLen;
            u64             m_dataOffset;
            bool            m_isCustomStreamType;   // if true, AssetDatabase will not attempt to open the stream
                                                    // and will pass the stream name directly to the handler.
        };

        /*
         * This is the base class for Async AssetDatabase jobs
         */
        class AssetDatabaseJob
            : public AZStd::intrusive_list_node<AssetDatabaseJob>
        {
            friend class AssetManager;

        protected:
            AssetDatabaseJob(AssetManager* owner, const Asset<AssetData>& asset, AssetHandler* assetHandler);
            virtual ~AssetDatabaseJob();

            AssetManager*       m_owner;
            Asset<AssetData>    m_asset;
            AssetHandler*       m_assetHandler;
        };

        /**
         * AssetDatabase handles the creation, refcounting and automatic
         * destruction of assets.
         *
         * In general for any events while loading/saving/etc. create an AssetEventHandler and pass
         * it to AssetDatabase::GetAsset().
         * You can also connect to AssetBus if you want to listen for
         * events without holding an asset.
         * If an asset is ready at the time you connect to AssetBus or GetAsset() is called,
         * your handler will be notified immediately, otherwise all events are dispatched asynchronously.
         */
        class AssetManager
            : private AssetManagerBus::Handler
        {
            friend class AssetData;
            friend class AssetDatabaseJob;
            friend Asset<AssetData> AssetInternal::GetAssetData(const AssetId& id);

        public:
            struct Descriptor
            {
                Descriptor()
                    : m_maxWorkerThreads(4)
                {}

                AZ::u32 m_maxWorkerThreads; ///< Max size of thread pool for asset loading jobs.
            };

            typedef AZStd::unordered_map<AssetType, AssetHandler*> AssetHandlerMap;
            typedef AZStd::unordered_map<AssetId, AssetData*> AssetMap;

            AZ_CLASS_ALLOCATOR(AssetManager, SystemAllocator, 0);
            AZ_RTTI(AssetManager, "{E4B247B0-76F7-4044-B499-DB65975FC01F}");

            static bool Create(const Descriptor& desc);
            static void Destroy();
            static bool IsReady();
            static AssetManager& Instance();

            AZStd::string GetAssetPathById(const AZ::Data::AssetId& id);

            AZ::Data::AssetId GetAssetIdByPath(const char* path, const AZ::Data::AssetType& typeToRegister);

            AZ::Data::AssetInfo GetAssetInfoById(const AZ::Data::AssetId& id);

            // @{ Asset handler management
            /// Register handler with the system for a particular asset type.
            /// A handler should be registered for each asset type it handles.
            void RegisterHandler(AssetHandler* handler, const AssetType& assetType);
            /// Unregister handler from the asset system.
            void UnregisterHandler(AssetHandler* handler);
            // @}

            /**
             * Gets an asset from the database, if not present it loads it from the catalog/stream. For events register a handler by calling RegisterEventHandler().
             * \param assetId a valid id of the asset
             * \param queueLoadData if an asset is not found in the database we will queue a load (default). You can pass false if you don't want to queue a load.
             * \param assetLoadFilterCB optional filter predicate for dependent asset loads.
             * \param loadBlocking defaults to false, but if set, asset will be loaded directly on the calling thread. This should only be set within the asynchronous asset-loading system for cascading loads.
             * Keep in mind that this async operation, asset will not be loaded after the call to this function completes.
             */
            template<class AssetClass>
            Asset<AssetClass> GetAsset(const AssetId& assetId, bool queueLoadData = true, const AZ::Data::AssetFilterCB& assetLoadFilterCB = nullptr, bool loadBlocking = false);

            /**
            * Gets an asset from the database, if not present it loads it from the catalog/stream. For events register a handler by calling RegisterEventHandler().
            * \param assetId a valid id of the asset
            * \param queueLoadData if an asset is not found in the database we will queue a load (default). You can pass false if you don't want to queue a load.
            * \param assetLoadFilterCB optional filter predicate for dependent asset loads.
            * \param loadBlocking defaults to false, but if set, asset will be loaded directly on the calling thread. This should only be set within the asynchronous asset-loading system for cascading loads.
            * \param isCreate defaults to false.  True indicates this is a brand new asset with a randomly generated assetId, so the AssetManager will not attempt to look up the asset in the asset catalog
            * Keep in mind that this async operation, asset will not be loaded after the call to this function completes.
            */
            Asset<AssetData> GetAsset(const AssetId& assetId, const AssetType& assetType, bool queueLoadData = true, const AZ::Data::AssetFilterCB& assetLoadFilterCB = nullptr, bool loadBlocking = false);

            template<class AssetClass>
            Asset<AssetClass> GetAsset(const char* path, bool queueLoadData = true, const AZ::Data::AssetFilterCB& assetLoadFilterCB = nullptr, bool loadBlocking = false);

            Asset<AssetData> GetAsset(const char* path, const AssetType& assetType, bool queueLoadData = true, const AZ::Data::AssetFilterCB& assetLoadFilterCB = nullptr, bool loadBlocking = false);

            /// Creates a dynamic/asset and returns the pointer. If the asset already exists it will return NULL (the you should use GetAsset to obtain it).
            template<class AssetClass>
            Asset<AssetClass> CreateAsset(const AssetId& assetId);
            Asset<AssetData> CreateAsset(const AssetId& assetId, const AssetType& assetType);

            template<class AssetClass>
            Asset<AssetClass> CreateAsset(const char* path);
            Asset<AssetData> CreateAsset(const char* path, const AssetType& assetType);

            /// Locates an existing asset in the database. If the asset is unknown, a null asset pointer is returned.
            template<class AssetClass>
            Asset<AssetClass> FindAsset(const AssetId& assetId);
            Asset<AssetData> FindAsset(const AssetId& assetId);

            template<class AssetClass>
            Asset<AssetClass> FindAsset(const char* path);
            Asset<AssetData> FindAsset(const char* path, const AssetType& assetType);

            /**
             * Requests a reload of a given asset from storage.
             */
            void ReloadAsset(const AssetId& assetId);
            void ReloadAsset(const char* path, const AssetType& assetType);

            /**
             * Reloads an asset from provided in-memory data.
             * Ownership of the provided asset data is transferred to the asset manager.
             */
            void ReloadAssetFromData(const Asset<AssetData>& asset);

            /**
            * Assign new data for the specified asset Id. This is effectively reloading the asset
            * with the provided data. Listeners will be notified to process the new data.
            */
            void AssignAssetData(const Asset<AssetData>& asset);

            /**
            * Gets a pointer to an asset handler for a type.
            * Returns nullptr if a handler for that type does not exist.
            */
            const AssetHandler* GetHandler(const AssetType& assetType);

            JobManager* GetJobManager() const { return m_jobManager; }

            void        DispatchEvents();

        protected:
            AssetManager(const Descriptor& desc);
            ~AssetManager();

            void NotifyAssetReady(Asset<AssetData> asset);
            void NotifyAssetPreReload(Asset<AssetData> asset);
            void NotifyAssetReloaded(Asset<AssetData> asset);
            void NotifyAssetReloadError(Asset<AssetData> asset);
            void NotifyAssetError(Asset<AssetData> asset);
            void ReleaseAsset(AssetData* asset);

            void AddJob(AssetDatabaseJob* job);
            void RemoveJob(AssetDatabaseJob* job);

            //////////////////////////////////////////////////////////////////////////
            // AssetDatabaseBus
            void OnAssetReady(const Asset<AssetData>& asset) override;
            void OnAssetReloaded(const Asset<AssetData>& asset) override;
            void OnAssetReloadError(const Asset<AssetData>& asset) override;
            void OnAssetError(const Asset<AssetData>& asset) override;
            //////////////////////////////////////////////////////////////////////////

            AssetHandlerMap         m_handlers;
            AZStd::recursive_mutex  m_handlerMutex;     // lock when accessing the handler map
            AZStd::recursive_mutex  m_catalogMutex;     // lock when accessing the catalog map
            AssetMap                m_assets;
            AZStd::recursive_mutex  m_assetMutex;       // lock when accessing the asset map

            // special-case lock for GetAsset, to be locked around m_handlerMutex and m_assetMutex (which can't be held at the same time, to avoid deadlocking)
            // This is required to prevent a race condition where two threads call GetAsset for the same unloaded asset and both attempt to create and load it.
            AZStd::recursive_mutex m_getAssetMutex;
            AZStd::recursive_mutex  m_assetReadyMutex;  // special-case lock so marking an asset ready and firing the notifications is an atomic operation

            typedef AZStd::unordered_map<AssetId, Asset<AssetData> > ReloadMap;
            ReloadMap               m_reloads;          // book-keeping and reference-holding for asset reloads

            JobManager*             m_jobManager;
            JobContext*             m_jobContext;
            unsigned int            m_numberOfWorkerThreads;    ///< Number of worked threads to spawn for this process. If <= 0 we will use all cores.
            int                     m_firstThreadCPU;           ///< ID of the first thread, afterwards we just increment. If == -1, no CPU will be set.(TODO: We can have a full array)

            typedef AZStd::intrusive_list<AssetDatabaseJob, AZStd::list_base_hook<AssetDatabaseJob> > ActiveJobList;
            ActiveJobList           m_activeJobs;

            using AssetIdToInfoMap = AZStd::unordered_map < AZ::Data::AssetId, AZ::Data::AssetInfo >;
            AssetIdToInfoMap        m_assetIdToInfo;

            using AssetPathToIdMap = AZStd::unordered_map < AZ::Uuid, AZ::Data::AssetId >;
            AssetPathToIdMap        m_assetPathToId;

            AZStd::recursive_mutex  m_assetIdToInfoMutex;
            AZStd::recursive_mutex  m_assetInfoToIdMutex;

            static EnvironmentVariable<AssetManager*>  s_assetDB;
        };

        /**
         * AssetHandlers are responsible for loading and destroying assets
         * when the asset manager requests it.
         *
         * To create a handler for a specific asset type, derive from this class
         * and register an instance of the handler with the asset manager.
         *
         * Asset handling functions may be called from multiple threads, so the
         * handlers need to be thread-safe.
         * It is ok for the handler to block the calling thread during the actual
         * asset load.
         */
        class AssetHandler
        {
            friend class AssetManager;
        public:
            AZ_RTTI(AssetHandler, "{58BD1FDF-E668-42E5-9091-16F46022F551}");

            AssetHandler();
            virtual ~AssetHandler();

            // Called by the asset manager to create a new asset. No loading should occur during this call
            virtual AssetPtr CreateAsset(const AssetId& id, const AssetType& type) = 0;

            // Called by the asset manager to perform actual asset load.
            // At least one of these overloads must be implemented by the user.
            virtual bool LoadAssetData(const Asset<AssetData>& asset, IO::GenericStream* stream, const AZ::Data::AssetFilterCB& assetLoadFilterCB);

            // Called after the data loading stage and after all dependencies have been fulfilled.
            // Override this if the asset needs post-load init. If overriden, the handler is responsible
            // for notifying the asset manager when the asset is ready via AssetDatabaseBus::OnAssetReady.
            virtual void InitAsset(const Asset<AssetData>& asset, bool loadStageSucceeded, bool isReload);

            // Called by the asset manager when an asset should be deleted.
            virtual void DestroyAsset(AssetPtr ptr) = 0;

        private:
            AZStd::atomic_int   m_nActiveAssets;    // how many assets handled by this handler are still in existence.
            AZStd::atomic_int   m_nHandledTypes;    // how many asset types are currently being handled by this handler.
        };

        //=========================================================================
        // AssetDatabase::GetAsset
        // [6/19/2012]
        //=========================================================================
        template<class AssetClass>
        Asset<AssetClass> AssetManager::GetAsset(const AssetId& assetId, bool queueLoadData, const AZ::Data::AssetFilterCB& assetLoadFilterCB, bool loadBlocking)
        {
            Asset<AssetData> asset = GetAsset(assetId, AzTypeInfo<AssetClass>::Uuid(), queueLoadData, assetLoadFilterCB, loadBlocking);
            return static_pointer_cast<AssetClass>(asset);
        }

        template<class AssetClass>
        Asset<AssetClass> AssetManager::GetAsset(const char* path, bool queueLoadData, const AssetFilterCB& assetLoadFilterCB, bool loadBlocking)
        {
            return GetAsset<AssetClass>(GetAssetIdByPath(path, AzTypeInfo<AssetClass>::Uuid()), queueLoadData, assetLoadFilterCB, loadBlocking);
        }

        //=========================================================================
        // AssetDatabase::FindAsset
        //=========================================================================
        template<class AssetClass>
        Asset<AssetClass> AssetManager::FindAsset(const AssetId& assetId)
        {
            Asset<AssetData> asset = FindAsset(assetId);
            if (asset.GetAs<AssetClass>())
            {
                return static_pointer_cast<AssetClass>(asset);
            }
            return Asset<AssetData>();
        }

        template<class AssetClass>
        Asset<AssetClass> AssetManager::FindAsset(const char* path)
        {
            return FindAsset<AssetClass>(GetAssetIdByPath(path, AzTypeInfo<AssetClass>::Uuid()));
        }

        //=========================================================================
        // CreateAsset
        // [8/31/2012]
        //=========================================================================
        template<class AssetClass>
        Asset<AssetClass> AssetManager::CreateAsset(const AssetId& assetId)
        {
            Asset<AssetData> asset = CreateAsset(assetId, AzTypeInfo<AssetClass>::Uuid());
            return static_pointer_cast<AssetClass>(asset);
        }

        template<class AssetClass>
        Asset<AssetClass> AssetManager::CreateAsset(const char* path)
        {
            Asset<AssetData> asset = CreateAsset(GetAssetIdByPath(path, AzTypeInfo<AssetClass>::Uuid()));
            return static_pointer_cast<AssetClass>(asset);
        }
    }   // namespace Data
}   // namespace AZ