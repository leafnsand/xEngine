#include "Physics2d/Component/BoxCollider2dComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

#include "Physics2d/priv/util.h"

namespace Module
{
    void BoxCollider2dComponent::Reflect(AZ::ReflectContext* reflection)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection))
        {
            serializeContext->Class<BoxCollider2dComponent, Collider2dComponent>()
                ->Field("Size", &BoxCollider2dComponent::m_size);
        }
        if (auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection))
        {
            behaviorContext->Class<BoxCollider2dComponent>("BoxCollider2dComponent")
                ->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::ExcludeFlags::Preview)
                ->Constructor()
                ->Property("m_size", BehaviorValueProperty(&BoxCollider2dComponent::m_size));
		}
    }

    // AZ::Component
    void BoxCollider2dComponent::Activate()
    {
		Collider2dComponent::Activate();
    }

    void BoxCollider2dComponent::Deactivate()
    {
		if (m_shape)
		{
			delete m_shape;
			m_shape = nullptr;
		}
		Collider2dComponent::Deactivate();
    }

	void BoxCollider2dComponent::InitShape()
	{
		m_shape = new b2PolygonShape;
		b2Vec2 center = Physics2D::Fromfloat2(m_offset);
		m_shape->SetAsBox(m_size.GetX() / 2, m_size.GetY() / 2, center, 0);
		Collider2dComponent::CreateFixture(m_shape);
	}
}
