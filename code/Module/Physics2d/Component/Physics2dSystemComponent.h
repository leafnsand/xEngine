#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/RTTI/TypeInfo.h>

#include "Physics2d/EBus/Physics2dSystemBus.h"
#include "Physics2d/priv/world.h"

namespace Module
{
    class Physics2dSystemComponent
        : public AZ::Component
        , protected AZ::SystemTickBus::Handler
        , protected Physics2dSystemRequestBus::Handler
    {
    public:
        AZ_COMPONENT(Physics2dSystemComponent, "{DCA06120-BC1C-448F-BB9D-CBDB4E68F90C}");

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC("Physics2dSystemService"));
        }

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC("Physics2dSystemService"));
        }

        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
        {

        }

        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
        {

        }

        static void Reflect(AZ::ReflectContext* reflection);

        Physics2dSystemComponent();
        Physics2dSystemComponent(const Physics2dSystemComponent&) = delete;
        virtual ~Physics2dSystemComponent() = default;

    protected:
        // AZ::Component
        void Init() override;
        void Activate() override;
        void Deactivate() override;

        // AZ::SystemTickBus::Handler
        void OnSystemTick() override;

        // Physics2dSystemRequestBus::Handler
        b2Body* CreateBody(const Physics2D::BodyDesc& desc) override;
        void DestroyBody(b2Body* body) override;
        void SetGravity(const AZ::Vector2& gravity) override;
        AZ::Vector2 GetGravity() override;
        void ShiftOrigin(const AZ::Vector2& newOrigin) override;

    private:
        AZ::Vector2 m_gravity = AZ::Vector2(0, -9.8f);
        Physics2D::World* m_world = nullptr;
    };
}
