#include "Physics2d/Component/Physics2dSystemComponent.h"

#include <AzCore/Serialization/SerializeContext.h>

#include "Physics2d/priv/util.h"

namespace Module
{
    void Physics2dSystemComponent::Reflect(AZ::ReflectContext* reflection)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection))
        {
            serializeContext->Class<Physics2dSystemComponent, AZ::Component>()
                ->Field("Gravity", &Physics2dSystemComponent::m_gravity);
        }
        
        if (auto behavior_context = azrtti_cast<AZ::BehaviorContext *>(reflection))
        {
            behavior_context->EBus<Physics2dSystemRequestBus>("Physics2dSystemRequestBus")
                ->Event("SetGravity", &Physics2dSystemRequestBus::Events::SetGravity);
            behavior_context->EBus<Physics2dSystemNotificationBus>("Physics2dSystemNotificationBus")
                ->Handler<Physics2dSystemNotificationBusHandler>();
        }
    }

    Physics2dSystemComponent::Physics2dSystemComponent()
    {

    }

    void Physics2dSystemComponent::Init()
    {
        
    }

    void Physics2dSystemComponent::Activate()
    {
        this->m_world = aznew Physics2D::World;
        this->m_world->Setup(m_gravity);

        AZ::SystemTickBus::Handler::BusConnect();
        Physics2dSystemRequestBus::Handler::BusConnect();
    }

    void Physics2dSystemComponent::Deactivate()
    {
        Physics2dSystemRequestBus::Handler::BusDisconnect();
        AZ::SystemTickBus::Handler::BusDisconnect();

        if(m_world)
        {
            delete m_world;
            m_world = nullptr;
        }
    }

    void Physics2dSystemComponent::OnSystemTick()
    {
        AZ_Assert(m_world != nullptr, "invalid world2d");
        
        float dt = 0;
        EBUS_EVENT_RESULT(dt, AZ::TickRequestBus, GetTickDeltaTime);
        this->m_world->Box2DWorld->Step(dt, 8, 3);
    }

    b2Body* Physics2dSystemComponent::CreateBody(const Physics2D::BodyDesc& desc)
    {
        AZ_Assert(m_world != nullptr, "invalid world2d");

        return this->m_world->CreateBody(desc);
    }

    void Physics2dSystemComponent::DestroyBody(b2Body* body)
    {
        AZ_Assert(m_world != nullptr, "invalid world2d");

        if(body)
        {
            this->m_world->DestroyBody(body);
        }
    }

    void Physics2dSystemComponent::SetGravity(const AZ::Vector2& gravity)
    {
        AZ_Assert(m_world != nullptr, "invalid world2d");

        this->m_world->SetGravity(gravity);
    }

    AZ::Vector2 Physics2dSystemComponent::GetGravity()
    {
        AZ_Assert(m_world != nullptr, "invalid world2d");

        return this->m_world->GetGravity();
    }

    void Physics2dSystemComponent::ShiftOrigin(const AZ::Vector2& newOrigin)
    {
        AZ_Assert(m_world != nullptr, "invalid world2d");

    }
}
