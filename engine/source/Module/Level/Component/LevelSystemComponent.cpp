#include "Level/Component/LevelSystemComponent.h"
#include "Level/Component/LevelComponent.h"

#include <AzCore/Math/Transform.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/Asset/AssetManager.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Component/GameEntityContextBus.h>

namespace Module
{
    void LevelSystemComponent::Reflect(AZ::ReflectContext* reflection)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection))
        {
            serializeContext->Class<LevelSystemComponent>()
                ->Field("DefaultLevel", &LevelSystemComponent::m_defaultLevel)
                ;
        }

        if (auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection))
        {
            behaviorContext->EBus<LevelSystemRequestBus>("LevelSystemRequestBus")
                ->Event("LoadLevel", &LevelSystemRequestBus::Events::LoadLevel)
                ->Event("UnloadLevel", &LevelSystemRequestBus::Events::UnloadLevel)
                ->Event("SetDefaultLevel", &LevelSystemRequestBus::Events::SetDefaultLevel);

            behaviorContext->Class<LevelSystemComponent>("LevelSystemComponent")
                ->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::ExcludeFlags::Preview)
                ->Constructor()
                ->Property("m_defaultLevel", BehaviorValueProperty(&LevelSystemComponent::m_defaultLevel));
        }
    }

    void LevelSystemComponent::Activate()
    {
        LevelSystemRequestBus::Handler::BusConnect();
        LoadLevel(m_defaultLevel);
    }

    void LevelSystemComponent::Deactivate()
    {
        UnloadLevel();
        LevelSystemRequestBus::Handler::BusDisconnect();
    }

    AZ::SliceInstantiationTicket LevelSystemComponent::LoadLevel(AZStd::string& path)
    {
        UnloadLevel();

        auto asset = AZ::Data::AssetManager::Instance().FindAsset<AZ::SliceAsset>(path.c_str());
        if (!asset.IsReady() && !asset.IsLoading())
        {
            asset = AZ::Data::AssetManager::Instance().GetAsset<AZ::SliceAsset>(path.c_str());
        }

        AZ::SliceInstantiationTicket ticket;
        EBUS_EVENT_RESULT(ticket, AZ::GameEntityContextRequestBus, InstantiateDynamicSlice, asset, AZ::Transform::CreateIdentity(), nullptr);

        return ticket;
    }

    void LevelSystemComponent::UnloadLevel()
    {
        EBUS_EVENT(AZ::GameEntityContextRequestBus, ResetGameContext);
    }

    void LevelSystemComponent::SetDefaultLevel(AZStd::string path)
    {
        m_defaultLevel = path;
    }
}
