#include "Physics2d/Component/EdgeCollider2dComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

#include "Physics2d/priv/util.h"

namespace Module
{
    void EdgeCollider2dComponent::Reflect(AZ::ReflectContext* reflection)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection))
        {
            serializeContext->Class<EdgeCollider2dComponent, Collider2dComponent>()
                ->Field("Count", &EdgeCollider2dComponent::m_count)
                ->Field("Vertices", &EdgeCollider2dComponent::m_vertices)
                ->Field("IsLoop", &EdgeCollider2dComponent::m_isLoop);
        }

        if (auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection))
        {
            behaviorContext->Class<EdgeCollider2dComponent>("EdgeCollider2dComponent")
                ->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::ExcludeFlags::Preview)
                ->Constructor()
                ->Property("m_count", BehaviorValueProperty(&EdgeCollider2dComponent::m_count))
                ->Property("m_vertices", BehaviorValueProperty(&EdgeCollider2dComponent::m_vertices))
                ->Property("m_isLoop", BehaviorValueProperty(&EdgeCollider2dComponent::m_isLoop));
        }
    }

    // AZ::Component
    void EdgeCollider2dComponent::Activate()
    {
        Collider2dComponent::Activate();
    }

    void EdgeCollider2dComponent::Deactivate()
    {
        if (m_shape)
        {
            delete m_shape;
            m_shape = nullptr;
        }
        Collider2dComponent::Deactivate();
    }

    void EdgeCollider2dComponent::InitShape()
    {
        m_shape = new b2ChainShape;
        if(m_count > 0 && m_vertices.size() == m_count)
        {
            AZStd::vector<b2Vec2> tmp;
            for (auto& vertex : m_vertices)
            {
                tmp.push_back(Physics2D::Fromfloat2(vertex));
            }
            if(m_isLoop)
            {
                m_shape->CreateLoop(tmp.data(), m_count);
            }
            else
            {
                m_shape->CreateChain(tmp.data(), m_count);
            }
        }

        Collider2dComponent::CreateFixture(m_shape);
    }
}
