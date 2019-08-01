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
#ifndef AZ_UNITY_BUILD

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/Asset/AssetManager.h>
#include <AzCore/Component/ComponentApplicationBus.h>
#include <AzCore/std/parallel/lock.h>

namespace AZ
{
    namespace Data
    {
        namespace AssetInternal
        {
            //=========================================================================
            // AssetLoad
            //=========================================================================
            Asset<AssetData> AssetLoad(AssetData* assetData)
            {
                return AssetManager::Instance().GetAsset(assetData->GetId(), assetData->GetType(), true, nullptr, true);
            }

            //=========================================================================
            // QueueAssetLoad
            //=========================================================================
            Asset<AssetData> QueueAssetLoad(AssetData* assetData, const AZ::Data::AssetFilterCB& assetLoadFilterCB)
            {
                return AssetManager::Instance().GetAsset(assetData->GetId(), assetData->GetType(), true, assetLoadFilterCB);
            }

            //=========================================================================
            // GetAsset
            //=========================================================================
            Asset<AssetData> GetAsset(const AssetId& id, const AssetType& type, bool queueLoad)
            {
                return AssetManager::Instance().GetAsset(id, type, queueLoad, nullptr, false);
            }

            //=========================================================================
            // GetAsset
            //=========================================================================
            Asset<AssetData> GetAsset(const char* path, const AssetType& type, bool queueLoad)
            {
                return AssetManager::Instance().GetAsset(path, type, queueLoad, nullptr, false);
            }

            //=========================================================================
            // ReloadAsset
            //=========================================================================
            bool ReloadAsset(AssetData* assetData)
            {
                AssetManager::Instance().ReloadAsset(assetData->GetId());
                return true;
            }

            //=========================================================================
            // GetAssetData
            //=========================================================================
            Asset<AssetData> GetAssetData(const AssetId& id)
            {
                if (AssetManager::IsReady())
                {
                    AZStd::lock_guard<AZStd::recursive_mutex> assetLock(AssetManager::Instance().m_assetMutex);
                    auto it = AssetManager::Instance().m_assets.find(id);
                    if (it != AssetManager::Instance().m_assets.end())
                    {
                        return it->second;
                    }
                }
                return nullptr;
            }

            AssetId ResolveAssetId(const AssetId& id)
            {
                AZ::Data::AssetInfo assetInfo = AZ::Data::AssetManager::Instance().GetAssetInfoById(id);
                if (assetInfo.m_assetId.IsValid())
                {
                    return assetInfo.m_assetId;
                }
                else
                {
                    return id;
                }

            }
        }
        //=========================================================================
        // RemoveFromDB
        // [6/19/2012]
        //=========================================================================
        void AssetData::RemoveFromDB()
        {
            AssetManager::Instance().ReleaseAsset(this);
        }
    }   // namespace Data
}   // namespace AZ

#endif // #ifndef AZ_UNITY_BUILD
