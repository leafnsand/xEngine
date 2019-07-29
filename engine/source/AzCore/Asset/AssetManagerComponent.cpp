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

#include <AzCore/Asset/AssetManagerComponent.h>
#include <AzCore/Asset/GenericAssetHandler.h>
#include <AzCore/Asset/BinaryAssetHandler.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/Asset/AssetManager.h>
#include <AzCore/Math/Crc.h>
#include <AzCore/IO/FileIO.h>

namespace AZ
{
    namespace Internal
    {
        AZStd::string AssetIdToString(const AZ::Data::AssetId& id)
        {
            AZStd::string result;
            id.ToString(result);
            return result;
        }
    }

    class BehaviorAssetBusHandler : public AZ::Data::AssetBus::Handler, public AZ::BehaviorEBusHandler
    {
    public:
        AZ_EBUS_BEHAVIOR_BINDER(BehaviorAssetBusHandler, "{BA8C6E79-DBF8-4064-9FE7-75DD6DD5912B}", AZ::SystemAllocator,
            OnAssetReady, OnAssetMoved, OnAssetPreReload, OnAssetReloaded, OnAssetReloadError, OnAssetUnloaded, OnAssetError);

        void OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset) override
        {
            Call(FN_OnAssetReady, asset);
        }

        void OnAssetMoved(AZ::Data::Asset<AZ::Data::AssetData> asset, void* oldDataPointer) override
        {
            Call(FN_OnAssetMoved, asset, oldDataPointer);
        }

        void OnAssetPreReload(AZ::Data::Asset<AZ::Data::AssetData> asset) override
        {
            Call(FN_OnAssetPreReload, asset);
        }

        void OnAssetReloaded(AZ::Data::Asset<AZ::Data::AssetData> asset) override
        {
            Call(FN_OnAssetReloaded, asset);
        }

        void OnAssetReloadError(AZ::Data::Asset<AZ::Data::AssetData> asset) override
        {
            Call(FN_OnAssetReloadError, asset);
        }

        void OnAssetUnloaded(const AZ::Data::AssetId assetId, const AZ::Data::AssetType assetType) override
        {
            Call(FN_OnAssetUnloaded, assetId, assetType);
        }

        void OnAssetError(AZ::Data::Asset<AZ::Data::AssetData> asset) override
        {
            Call(FN_OnAssetError, asset);
        }
    };

    //=========================================================================
    // AssetDatabaseComponent
    // [6/25/2012]
    //=========================================================================
    AssetManagerComponent::AssetManagerComponent()
    {
    }

    void AssetManagerComponent::Init()
    {
        AZStd::string path;

        EBUS_EVENT_RESULT(path, AZ::ApplicationRequests::Bus, GetAppRoot);
        EBUS_EVENT(ApplicationRequests::Bus, NormalizePathKeepCase, path);
        AZ::IO::FileIOBase::GetInstance()->SetAlias("@root@", path.c_str());

        EBUS_EVENT_RESULT(path, AZ::ApplicationRequests::Bus, GetEngineRoot);
        EBUS_EVENT(ApplicationRequests::Bus, NormalizePathKeepCase, path);
        AZ::IO::FileIOBase::GetInstance()->SetAlias("@engine@", path.c_str());

        EBUS_EVENT_RESULT(path, AZ::ApplicationRequests::Bus, GetAssetRoot);
        EBUS_EVENT(ApplicationRequests::Bus, NormalizePathKeepCase, path);
        AZ::IO::FileIOBase::GetInstance()->SetAlias("@assets@", path.c_str());
    }

    //=========================================================================
    // Activate
    // [6/25/2012]
    //=========================================================================
    void AssetManagerComponent::Activate()
    {
        Data::AssetManager::Descriptor desc;
        Data::AssetManager::Create(desc);
        TickBus::Handler::BusConnect();

        m_binaryAssetHandler.reset(aznew BinaryAssetHandler);

        Data::AssetManager::Instance().RegisterHandler(m_binaryAssetHandler.get(), AzTypeInfo<BinaryAsset>::Uuid());
    }

    //=========================================================================
    // Deactivate
    // [6/25/2012]
    //=========================================================================
    void AssetManagerComponent::Deactivate()
    {
        Data::AssetManager::Instance().UnregisterHandler(m_binaryAssetHandler.get());
        m_binaryAssetHandler.reset();

        TickBus::Handler::BusDisconnect();
        Data::AssetManager::Destroy();
    }

    //=========================================================================
    // OnTick
    // [6/25/2012]
    //=========================================================================
    void AssetManagerComponent::OnTick(float /*deltaTime*/, ScriptTimePoint /*time*/)
    {
        Data::AssetManager::Instance().DispatchEvents();
    }

    //=========================================================================
    // GetProvidedServices
    //=========================================================================
    void AssetManagerComponent::GetProvidedServices(ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("AssetDatabaseService", 0x3abf5601));
    }

    //=========================================================================
    // GetIncompatibleServices
    //=========================================================================
    void AssetManagerComponent::GetIncompatibleServices(ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("AssetDatabaseService", 0x3abf5601));
    }

    //=========================================================================
    // GetDependentServices
    //=========================================================================
    void AssetManagerComponent::GetDependentServices(ComponentDescriptor::DependencyArrayType& dependent)
    {
        dependent.push_back(AZ_CRC("DataStreamingService", 0xb1b981f5));
        dependent.push_back(AZ_CRC("JobsService", 0xd5ab5a50));
    }

    //=========================================================================
    // Reflect
    //=========================================================================
    void AssetManagerComponent::Reflect(ReflectContext* context)
    {
        if (SerializeContext* serializeContext = azrtti_cast<SerializeContext*>(context))
        {
            serializeContext->Class<Data::AssetId>()
                ->Field("guid", &Data::AssetId::m_guid)
                ->Field("subId", &Data::AssetId::m_subId)
                ;

            serializeContext->Class<AZ::Data::AssetData>()
                ;

            serializeContext->Class<AssetManagerComponent, AZ::Component>()
                ->SerializerForEmptyClass();
        }

        if (BehaviorContext* behaviorContext = azrtti_cast<BehaviorContext*>(context))
        {
            behaviorContext->Class<AZ::Data::AssetId>()
                ->Method("ToString", &Internal::AssetIdToString)
                    ->Attribute(AZ::Script::Attributes::Operator, AZ::Script::Attributes::OperatorType::ToString)
                ->Method("Equal", &AZ::Data::AssetId::operator==)
                    ->Attribute(AZ::Script::Attributes::Operator, AZ::Script::Attributes::OperatorType::Equal);

            behaviorContext->Class<AZ::Data::AssetData>()
                ->Method("Acquire", &AZ::Data::AssetData::Acquire)
                ->Method("Release", &AZ::Data::AssetData::Release)
                ->Method("IsReady", &AZ::Data::AssetData::IsReady)
                ->Method("IsError", &AZ::Data::AssetData::IsError)
                ->Method("IsLoading", &AZ::Data::AssetData::IsLoading)
                ->Method("GetStatus", &AZ::Data::AssetData::GetStatus)
                ->Method("GetId", &AZ::Data::AssetData::GetId)
                ->Method("GetType", &AZ::Data::AssetData::GetType)
                ->Method("GetUseCount", &AZ::Data::AssetData::GetUseCount);

            behaviorContext->Class<AZ::Data::Asset<AZ::Data::AssetData>>()
                ->Method("IsReady", &AZ::Data::Asset<AZ::Data::AssetData>::IsReady)
                ->Method("IsError", &AZ::Data::Asset<AZ::Data::AssetData>::IsError)
                ->Method("IsLoading", &AZ::Data::Asset<AZ::Data::AssetData>::IsLoading)
                ->Method("GetStatus", &AZ::Data::Asset<AZ::Data::AssetData>::GetStatus)
                ->Method("GetId", &AZ::Data::Asset<AZ::Data::AssetData>::GetId)
                ->Method("GetType", &AZ::Data::Asset<AZ::Data::AssetData>::GetType)
                ->Method("GetHint", &AZ::Data::Asset<AZ::Data::AssetData>::GetHint)
                ->Method("GetData", &AZ::Data::Asset<AZ::Data::AssetData>::GetData);

            behaviorContext->Class<AZ::Data::AssetManager>()
                ->Method("GetAsset", [](const char* path, const AZ::Data::AssetType& type) -> AZ::Data::Asset<AZ::Data::AssetData>
                {
                    return AZ::Data::AssetManager::Instance().GetAsset(path, type);
                })
                ->Method("CreateAsset", [](const char* path, const AZ::Data::AssetType& type)->AZ::Data::Asset<AZ::Data::AssetData>
                {
                    return AZ::Data::AssetManager::Instance().CreateAsset(path, type);
                })
                ->Method("FindAsset", [](const char* path, const AZ::Data::AssetType& type)->AZ::Data::Asset<AZ::Data::AssetData>
                {
                    return AZ::Data::AssetManager::Instance().FindAsset(path, type);
                });

            behaviorContext->EBus<AZ::Data::AssetBus>("AssetBus")
                ->Handler<BehaviorAssetBusHandler>();
        }
    }
}

#endif // #ifndef AZ_UNITY_BUILD
