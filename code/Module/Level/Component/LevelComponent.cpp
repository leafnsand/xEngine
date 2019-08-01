#include "Level/Component/LevelComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/Component/TransformBus.h>

#include "Renderer/EBus/CameraComponentBus.h"

namespace Module
{
    void LevelComponent::Reflect(AZ::ReflectContext* reflection)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection))
        {
            serializeContext->Class<LevelComponent, AZ::Component>()
                ->Field("SkyBoxName", &LevelComponent::m_skyBoxName)
                ->Field("Divisions", &LevelComponent::m_divisions);
        }

        if (auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection))
        {
            behaviorContext->EBus<LevelRequestBus>("LevelRequestBus");

            behaviorContext->Class<LevelComponent>("LevelComponent")
                ->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::ExcludeFlags::Preview)
                ->Constructor()
                ->Property("m_skyBoxName", BehaviorValueProperty(&LevelComponent::m_skyBoxName))
                ->Property("m_divisions", BehaviorValueProperty(&LevelComponent::m_divisions));
        }
    }

    LevelComponent::LevelComponent()
    {
        
    }

    LevelComponent::LevelComponent(const AZStd::string& name)
    {
        m_skyBoxName = name;
    }

    void LevelComponent::Init()
    {
        if (!m_skyBoxName.empty())
        {
        }
    }

    void LevelComponent::Activate()
    {
        LevelRequestBus::Handler::BusConnect(GetEntityId());

    }

    void LevelComponent::Deactivate()
    {
        LevelRequestBus::Handler::BusDisconnect();
    }
}
