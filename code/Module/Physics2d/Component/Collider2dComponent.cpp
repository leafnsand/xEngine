#include "Physics2d/Component/Collider2dComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include "Physics2d/EBus/Rigidbody2dBus.h"

namespace Module
{
    void Collider2dComponent::Reflect(AZ::ReflectContext* reflection)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection))
        {
            serializeContext->Class<Collider2dComponent, AZ::Component>()
                ->Field("Density", &Collider2dComponent::m_density)
                ->Field("Offset", &Collider2dComponent::m_offset)
                ->Field("IsTrigger", &Collider2dComponent::m_isTrigger)
                ->Field("Bounciness", &Collider2dComponent::m_bounciness)
                ->Field("Friction", &Collider2dComponent::m_friction);
        }

        if (auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection))
        {
            behaviorContext->Class<Collider2dComponent>("Collider2dComponent")
                ->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::ExcludeFlags::Preview)
                ->Constructor()
                ->Property("m_density", BehaviorValueProperty(&Collider2dComponent::m_density))
                ->Property("m_offset", BehaviorValueProperty(&Collider2dComponent::m_offset))
                ->Property("m_isTrigger", BehaviorValueProperty(&Collider2dComponent::m_isTrigger))
                ->Property("m_bounciness", BehaviorValueProperty(&Collider2dComponent::m_bounciness))
                ->Property("m_friction", BehaviorValueProperty(&Collider2dComponent::m_friction));
        }

    }

    // AZ::Component
    void Collider2dComponent::Activate()
    {
        InitShape();
    }

    void Collider2dComponent::Deactivate()
    {

    }

    void Collider2dComponent::CreateFixture(b2Shape* shape)
    {
        b2FixtureDef def;
        def.density = m_density;
        def.friction = m_friction;
        def.restitution = m_bounciness;
        def.shape = shape;
        def.userData = GetEntity();
        EBUS_EVENT_ID_RESULT(m_fixture, GetEntityId(), Rigidbody2dRequestBus, CreateFixture, def);
    }
}
