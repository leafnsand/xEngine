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

#include "SliceSystemComponent.h"
#include "SliceAsset.h"
#include "SliceBus.h"
#include <AzCore/Component/GameEntityContextBus.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace AZ
{
    class SliceInstantiationNotificationBusBehaviorHandler : public SliceInstantiationNotificationBus::Handler, public AZ::BehaviorEBusHandler
    {
    public:
        AZ_EBUS_BEHAVIOR_BINDER(SliceInstantiationNotificationBusBehaviorHandler, "{7BFF8922-DC37-4550-96C8-E219B708D52D}", AZ::SystemAllocator,
            OnSliceInstantiated, OnSliceInstantiationFailed);

        void OnSliceInstantiated(const AZ::EntityId& id) override
        {
            Call(FN_OnSliceInstantiated, id);
        }

        void OnSliceInstantiationFailed() override
        {
            Call(FN_OnSliceInstantiationFailed);
        }
    };

    void SliceSystemComponent::Reflect(ReflectContext* context)
    {
        if (SerializeContext* serializeContext = azrtti_cast<SerializeContext*>(context))
        {
            serializeContext->Class<SliceSystemComponent>();
            serializeContext->Class<SliceAsset>();
        }

        if (BehaviorContext* behaviorContext = azrtti_cast<BehaviorContext*>(context))
        {
            behaviorContext->EBus<SliceInstantiationNotificationBus>("SliceInstantiationNotificationBus")
                ->Handler<SliceInstantiationNotificationBusBehaviorHandler>();
        }
    }

    void SliceSystemComponent::GetProvidedServices(ComponentDescriptor::DependencyArrayType& services)
    {
        services.push_back(AZ_CRC("SliceSystemService", 0x1a5b7aad));
    }

    void SliceSystemComponent::GetIncompatibleServices(ComponentDescriptor::DependencyArrayType& services)
    {
        services.push_back(AZ_CRC("SliceSystemService", 0x1a5b7aad));
    }

    void SliceSystemComponent::GetDependentServices(ComponentDescriptor::DependencyArrayType& services)
    {
        services.push_back(AZ_CRC("AssetDatabaseService", 0x3abf5601));
    }

    void SliceSystemComponent::GetRequiredServices(ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC("AssetDatabaseService", 0x3abf5601));
    }

    void SliceSystemComponent::Activate()
    {
        // Register with AssetDatabase
        AZ_Error("SliceSystemComponent", Data::AssetManager::IsReady(), "Can't register slice asset handler with AssetManager.");
        if (Data::AssetManager::IsReady())
        {
            Data::AssetManager::Instance().RegisterHandler(&m_assetHandler, AzTypeInfo<SliceAsset>::Uuid());
        }

        EntityContextId contextId;
        EBUS_EVENT_RESULT(contextId, GameEntityContextRequestBus, GetGameEntityContextId);

        GameEntityContextEventBus::Handler::BusConnect();
    }

    void SliceSystemComponent::Deactivate()
    {
        GameEntityContextEventBus::Handler::BusDisconnect();
        if (Data::AssetManager::IsReady())
        {
            Data::AssetManager::Instance().UnregisterHandler(&m_assetHandler);
        }
    }

    void SliceSystemComponent::OnSliceInstantiated(const Data::AssetId& sliceAssetId, const SliceComponent::SliceInstanceAddress& instance, const SliceInstantiationTicket& ticket)
    {
        const auto& entities = instance.second->GetInstantiated()->m_entities;
        for (auto& currEntity : entities)
        {
            EBUS_EVENT_ID(ticket, SliceInstantiationNotificationBus, OnSliceInstantiated, currEntity->GetId());
        }
    }

    void SliceSystemComponent::OnSliceInstantiationFailed(const Data::AssetId& sliceAssetId, const SliceInstantiationTicket& ticket)
    {
        EBUS_EVENT_ID(ticket, SliceInstantiationNotificationBus, OnSliceInstantiationFailed);
    }
} // namespace AZ

#endif // AZ_UNITY_BUILD