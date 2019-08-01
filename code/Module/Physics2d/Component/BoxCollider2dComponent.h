#pragma once

#include <AzCore/Component/Component.h>

#include <Box2D/Collision/Shapes/b2PolygonShape.h>

#include "Physics2d/Component/Collider2dComponent.h"

namespace Module
{
    class BoxCollider2dComponent
        : public Collider2dComponent
    {
    public:
        AZ_COMPONENT(BoxCollider2dComponent, "{F47D1747-3C6A-41F7-B03C-E964C0CFB622}",Collider2dComponent);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC("BoxCollider2dService"));
        }

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC("BoxCollider2dService"));
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
		b2PolygonShape* m_shape = nullptr;
		AZ::Vector2 m_size = AZ::Vector2::CreateZero();
    };
}