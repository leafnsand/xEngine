#pragma once

#include <AzCore/Component/Component.h>

#include <Box2D/Collision/Shapes/b2ChainShape.h>

#include "Physics2d/Component/Collider2dComponent.h"

namespace Module
{
    class EdgeCollider2dComponent
        : public Collider2dComponent
    {
    public:
        AZ_COMPONENT(EdgeCollider2dComponent, "{93F6382A-A70B-4318-A8DD-0425D9F78435}", Collider2dComponent);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC("EdgeCollider2dService"));
        }

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC("EdgeCollider2dService"));
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
        b2ChainShape* m_shape = nullptr;

        int m_count = 0;
        AZStd::vector<AZ::Vector2> m_vertices;
        bool m_isLoop = false;
    };
}
