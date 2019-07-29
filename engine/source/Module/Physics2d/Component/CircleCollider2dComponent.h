#pragma once

#include <AzCore/Component/Component.h>

#include <Box2D/Collision/Shapes/b2CircleShape.h>

#include "Physics2d/Component/Collider2dComponent.h"

namespace Module
{
    class CircleCollider2dComponent
        : public Collider2dComponent
    {
    public:
        AZ_COMPONENT(CircleCollider2dComponent, "{DDF6CBC9-C55C-44D2-B5C3-516D24DDC0CF}", Collider2dComponent);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC("CircleCollider2dService"));
        }

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC("CircleCollider2dService"));
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
        
    protected:
        void InitShape() override;

    private:
        float m_radius = 0;
        b2CircleShape* m_shape = nullptr;
    };
}
