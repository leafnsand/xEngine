#include "Physics2d/Component/CircleCollider2dComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

#include "Physics2d/priv/util.h"

namespace Module
{
    void CircleCollider2dComponent::Reflect(AZ::ReflectContext* reflection)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection))
        {
            serializeContext->Class<CircleCollider2dComponent, Collider2dComponent>()
                ->Field("Radius", &CircleCollider2dComponent::m_radius);
        }
		if (auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection))
		{
			behaviorContext->Class<CircleCollider2dComponent>("CircleCollider2dComponent")
				->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::ExcludeFlags::Preview)
				->Constructor()
				->Property("m_radius", BehaviorValueProperty(&CircleCollider2dComponent::m_radius));
		}
    }

    // AZ::Component
    void CircleCollider2dComponent::Activate()
    {
        Collider2dComponent::Activate();
    }

    void CircleCollider2dComponent::Deactivate()
    {
        if(m_shape)
        {
            delete m_shape;
            m_shape = nullptr;
        }
        Collider2dComponent::Deactivate();
    }

    void CircleCollider2dComponent::InitShape()
    {
        m_shape = new b2CircleShape;
        m_shape->m_p = Physics2D::Fromfloat2(m_offset);
        m_shape->m_radius = m_radius;
        Collider2dComponent::CreateFixture(m_shape);
    }
}
