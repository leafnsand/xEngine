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
#ifndef AZCORE_ASSET_DATABASE_BUS_H
#define AZCORE_ASSET_DATABASE_BUS_H

#include <AzCore/EBus/EBus.h>
#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/std/string/string.h>
#include <AzCore/std/containers/bitset.h>
#include <AzCore/Outcome/Outcome.h>

namespace AZ
{
    namespace Data
    {
        /** Asset Information (returned by bus queries to the catalog)
        * Note that Multiple UUIDs may point at the same "asset information"
        * so that legacy UUIDs (such as those generated using a different scheme) can still resolve to a valid asset
        * however, only one such entry will have 'canonical' set to true, meaning its the latest scheme.
        * UIs which enumerate assets should only use canonical assets.
        */
        class AssetInfo
        {
        public:
            AZ_TYPE_INFO(AssetInfo, "{E6D8372B-8419-4287-B478-1353709A972F}");
            AZ::Data::AssetId m_assetId; // this is in case you look up by a legacy Id or other remapping and it resolves to a new ID.
            AZ::Data::AssetType m_assetType = s_invalidAssetType;
            AZ::u64 m_sizeBytes = 0;
            AZStd::string m_relativePath; // (legacy asset name)
        };

        /*
         * Events that AssetManager listens for
         */
        class AssetManagerEvents
            : public EBusTraits
        {
        public:
            //////////////////////////////////////////////////////////////////////////
            // EBusTraits overrides
            static const EBusHandlerPolicy HandlerPolicy = EBusHandlerPolicy::Single;
            typedef AZStd::recursive_mutex  MutexType;
            //////////////////////////////////////////////////////////////////////////

            virtual ~AssetManagerEvents() {}

            /// Signal that an asset is ready for use
            virtual void OnAssetReady(const Asset<AssetData>& asset) = 0;
            /// Signal that an asset has been reloaded
            virtual void OnAssetReloaded(const Asset<AssetData>& asset) = 0;
            /// Signal that an asset failed to reload.
            virtual void OnAssetReloadError(const Asset<AssetData>& asset) = 0;
            /// Signal that an asset error has occurred
            virtual void OnAssetError(const Asset<AssetData>& asset) = 0;

            /// Notify listeners that all asset events have been dispatched.
            virtual void OnAssetEventsDispatched() {};
        };
        typedef EBus<AssetManagerEvents> AssetManagerBus;
    }   // namespace Data
}   // namespace AZ

#endif  // AZCORE_ASSET_DATABASE_BUS_H
#pragma once
