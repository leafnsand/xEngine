#pragma once

#include <AzCore/EBus/EBus.h>

#include "Physics2d/priv/util.h"

namespace Module
{
    class Rigidbody2dRequest : public AZ::ComponentBus
    {
    public:
        virtual ~Rigidbody2dRequest() {}

        virtual b2Fixture* CreateFixture(b2FixtureDef& def) = 0;

        virtual void SetTransform(const AZ::Vector2& position, float angle) = 0;

        virtual AZ::Vector2 GetPosition() const = 0;

        virtual float GetAngle() const = 0;

        virtual AZ::Vector2 GetWorldCenter() const = 0;
        virtual AZ::Vector2 GetLocalCenter() const = 0;

        virtual void SetLinearVelocity(const AZ::Vector2& v) = 0;
        virtual AZ::Vector2 GetLinearVelocity() const = 0;

        virtual void SetAngularVelocity(float omega) = 0;
        virtual float GetAngularVelocity() const = 0;

        virtual void ApplyForce(const AZ::Vector2& force, const AZ::Vector2& point, bool wake) = 0;
        virtual void ApplyForceToCenter(const AZ::Vector2& force, bool wake) = 0;

        virtual void ApplyTorque(float torque, bool wake) = 0;

        virtual void ApplyLinearImpulse(const AZ::Vector2& impulse, const AZ::Vector2& point, bool wake) = 0;

        virtual void ApplyAngularImpulse(float impulse, bool wake) = 0;

        virtual float GetMass() const = 0;

        virtual float GetInertia() const = 0;

        virtual Physics2D::MassData GetMassData() const = 0;
        virtual void SetMassData(const Physics2D::MassData* data) = 0;
        virtual void ResetMassData() = 0;

        virtual AZ::Vector2 GetWorldPoint(const AZ::Vector2& localPoint) const = 0;
        virtual AZ::Vector2 GetWorldVector(const AZ::Vector2& localVector) const = 0;

        virtual AZ::Vector2 GetLocalPoint(const AZ::Vector2& worldPoint) const = 0;
        virtual AZ::Vector2 GetLocalVector(const AZ::Vector2& worldVector) const = 0;

        virtual AZ::Vector2 GetLinearVelocityFromWorldPoint(const AZ::Vector2& worldPoint) const = 0;
        virtual AZ::Vector2 GetLinearVelocityFromLocalPoint(const AZ::Vector2& localPoint) const = 0;

        virtual float GetLinearDamping() const = 0;
        virtual void SetLinearDamping(float linearDamping) = 0;

        virtual float GetAngularDamping() const = 0;
        virtual void SetAngularDamping(float angularDamping) = 0;

        virtual float GetGravityScale() const = 0;
        virtual void SetGravityScale(float scale) = 0;

        virtual void SetType(Physics2D::BodyType type) = 0;
        virtual Physics2D::BodyType GetType() const = 0;

        virtual void SetBullet(bool flag) = 0;
        virtual bool IsBullet() const = 0;

        virtual void SetSleepingAllowed(bool flag) = 0;
        virtual bool IsSleepingAllowed() const = 0;

        virtual void SetAwake(bool flag) = 0;
        virtual bool IsAwake() const = 0;

        virtual void SetActive(bool flag) = 0;
        virtual bool IsActive() const = 0;

        virtual void SetFixedRotation(bool flag) = 0;
        virtual bool IsFixedRotation() const = 0;
    };

    using Rigidbody2dRequestBus = AZ::EBus<Rigidbody2dRequest>;

    class Rigidbody2dNotification : public AZ::ComponentBus
    {
    public:
        virtual ~Rigidbody2dNotification() {}
    };

    using Rigidbody2dNotificationBus = AZ::EBus<Rigidbody2dNotification>;
}
