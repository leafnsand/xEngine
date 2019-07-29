#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Math/Vector2.h>
#include <AzCore/Component/TransformBus.h>
#include <AzCore/Component/TickBus.h>

#include <Box2D/Dynamics/b2Body.h>

#include "Physics2d/EBus/Rigidbody2dBus.h"
#include "Physics2d/priv/util.h"

namespace Module
{
    class Rigidbody2dComponent
        : public AZ::Component
        , protected Rigidbody2dRequestBus::Handler
        , protected AZ::TransformNotificationBus::Handler
        , protected AZ::SystemTickBus::Handler
    {
    public:
        AZ_COMPONENT(Rigidbody2dComponent, "{55A64751-887B-4A53-9E8A-C0D58B6CA9A7}");

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC("Rigidbody2dService"));
        }

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC("Rigidbody2dService"));
        }

        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
        {

        }

        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
        {

        }

        static void Reflect(AZ::ReflectContext* reflection);

        // AZ::Component
        void Activate() override;
        void Deactivate() override;

        // AZ::TransformNotificationBus::Handler
        void OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world) override;

        // AZ::SystemTickBus::Handler
        void OnSystemTick() override;

        // Rigidbody2dRequestBus::Handler
        b2Fixture* CreateFixture(b2FixtureDef& def) override;

        void SetTransform(const AZ::Vector2& position, float angle) override;

        AZ::Vector2 GetPosition() const override;

        float GetAngle() const override;

        AZ::Vector2 GetWorldCenter() const override;
        AZ::Vector2 GetLocalCenter() const override;

        void SetLinearVelocity(const AZ::Vector2& v) override;
        AZ::Vector2 GetLinearVelocity() const override;

        void SetAngularVelocity(float omega) override;
        float GetAngularVelocity() const override;

        void ApplyForce(const AZ::Vector2& force, const AZ::Vector2& point, bool wake) override;
        void ApplyForceToCenter(const AZ::Vector2& force, bool wake) override;

        void ApplyTorque(float torque, bool wake) override;

        void ApplyLinearImpulse(const AZ::Vector2& impulse, const AZ::Vector2& point, bool wake) override;

        void ApplyAngularImpulse(float impulse, bool wake) override;

        float GetMass() const override;

        float GetInertia() const override;

        Physics2D::MassData GetMassData() const override;
        void SetMassData(const Physics2D::MassData* data) override;
        void ResetMassData() override;

        AZ::Vector2 GetWorldPoint(const AZ::Vector2& localPoint) const override;
        AZ::Vector2 GetWorldVector(const AZ::Vector2& localVector) const override;

        AZ::Vector2 GetLocalPoint(const AZ::Vector2& worldPoint) const override;
        AZ::Vector2 GetLocalVector(const AZ::Vector2& worldVector) const override;

        AZ::Vector2 GetLinearVelocityFromWorldPoint(const AZ::Vector2& worldPoint) const override;
        AZ::Vector2 GetLinearVelocityFromLocalPoint(const AZ::Vector2& localPoint) const override;

        float GetLinearDamping() const override;
        void SetLinearDamping(float linearDamping) override;

        float GetAngularDamping() const override;
        void SetAngularDamping(float angularDamping) override;

        float GetGravityScale() const override;
        void SetGravityScale(float scale) override;

        void SetType(Physics2D::BodyType type) override;
        Physics2D::BodyType GetType() const override;

        void SetBullet(bool flag) override;
        bool IsBullet() const override;

        void SetSleepingAllowed(bool flag) override;
        bool IsSleepingAllowed() const override;

        void SetAwake(bool flag) override;
        bool IsAwake() const override;

        void SetActive(bool flag) override;
        bool IsActive() const override;

        void SetFixedRotation(bool flag) override;
        bool IsFixedRotation() const override;
    private:
        b2Body* m_body = nullptr;

        Physics2D::BodyType m_bodyType = Physics2D::BodyType::Dynamic;
        float m_linearDrag = 0;
        float m_angularDrag = 0.05f;
        float m_gravityScale = 1.0f;
        bool m_isAwake = true;
        bool m_isSleepingAllowed = false;
        float m_mass = 1.0f;
    };
}
