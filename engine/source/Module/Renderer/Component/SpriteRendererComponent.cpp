#include "Renderer/Component/SpriteRendererComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace Module
{
    void SpriteRendererComponent::Reflect(AZ::ReflectContext* context)
    {

    }

    void SpriteRendererComponent::Activate()
    {
        RendererComponent::Activate();

        SpriteRendererRequestBus::Handler::BusConnect(GetEntityId());
    }

    void SpriteRendererComponent::Deactivate()
    {
        m_sprite = nullptr;

        SpriteRendererRequestBus::Handler::BusDisconnect();

        RendererComponent::Deactivate();
    }

    void SpriteRendererComponent::Render(size_t subMeshIndex)
    {
    }
}
