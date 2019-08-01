#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Math/Vector2.h>

#include <Box2D/Dynamics/b2Fixture.h>

#include "Physics2d/EBus/Collider2dBus.h"

namespace Module
{
    class Collider2dComponent
        : public AZ::Component
        , protected Collider2dRequestBus::Handler
    {
    public:
        AZ_COMPONENT(Collider2dComponent, "{2D4A67D8-3CF4-455A-BD92-77D0475145EF}");

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC("Collider2dService"));
        }

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC("Collider2dService"));
        }

        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
        {
            dependent.push_back(AZ_CRC("Rigidbody2dService"));
        }

        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
        {

        }

        static void Reflect(AZ::ReflectContext* reflection);

        // AZ::Component
        void Activate() override;
        void Deactivate() override;

        virtual void InitShape() {};
        void CreateFixture(b2Shape* shape);

    protected:
        float m_density = 1.0f;
        AZ::Vector2 m_offset = AZ::Vector2::CreateZero();
        bool m_isTrigger = false;
        float m_bounciness = 0;
        float m_friction = 0;

        b2Fixture* m_fixture = nullptr;
    };
}