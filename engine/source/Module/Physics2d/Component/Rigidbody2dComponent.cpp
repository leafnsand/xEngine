#include "Physics2d/Component/Rigidbody2dComponent.h"

#include <AzCore/Serialization/SerializeContext.h>

#include "Physics2d/EBus/Physics2dSystemBus.h"

namespace Module
{
    using namespace Physics2D;

    void Rigidbody2dComponent::Reflect(AZ::ReflectContext* reflection)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection))
        {
            serializeContext->Class<Rigidbody2dComponent, AZ::Component>()
                ->Field("BodyType", &Rigidbody2dComponent::m_bodyType)
                ->Field("LinearDrag", &Rigidbody2dComponent::m_linearDrag)
                ->Field("AngularDrag", &Rigidbody2dComponent::m_angularDrag)
                ->Field("GravityScale", &Rigidbody2dComponent::m_gravityScale)
                ->Field("IsAwake", &Rigidbody2dComponent::m_isAwake)
                ->Field("IsSleepingAllowed", &Rigidbody2dComponent::m_isSleepingAllowed)
                ->Field("Mass", &Rigidbody2dComponent::m_mass);
        }

        if (auto behavior_context = azrtti_cast<AZ::BehaviorContext *>(reflection))
        {
            behavior_context->EBus<Rigidbody2dRequestBus>("Rigidbody2dRequestBus")
                ->Event("SetTransform", &Rigidbody2dRequestBus::Events::SetTransform)
                ->Event("GetPosition", &Rigidbody2dRequestBus::Events::GetPosition)
                ->Event("GetAngle", &Rigidbody2dRequestBus::Events::GetAngle)
                ->Event("GetWorldCenter", &Rigidbody2dRequestBus::Events::GetWorldCenter)
                ->Event("GetLocalCenter", &Rigidbody2dRequestBus::Events::GetLocalCenter)
                ->Event("SetLinearVelocity", &Rigidbody2dRequestBus::Events::SetLinearVelocity)
                ->Event("GetLinearVelocity", &Rigidbody2dRequestBus::Events::GetLinearVelocity)
                ->Event("SetAngularVelocity", &Rigidbody2dRequestBus::Events::SetAngularVelocity)
                ->Event("GetAngularVelocity", &Rigidbody2dRequestBus::Events::GetAngularVelocity)
                ->Event("ApplyForce", &Rigidbody2dRequestBus::Events::ApplyForce)
                ->Event("ApplyForceToCenter", &Rigidbody2dRequestBus::Events::ApplyForceToCenter)
                ->Event("ApplyTorque", &Rigidbody2dRequestBus::Events::ApplyTorque)
                ->Event("ApplyLinearImpulse", &Rigidbody2dRequestBus::Events::ApplyLinearImpulse)
                ->Event("ApplyAngularImpulse", &Rigidbody2dRequestBus::Events::ApplyAngularImpulse)
                ->Event("GetMass", &Rigidbody2dRequestBus::Events::GetMass)
                ->Event("GetInertia", &Rigidbody2dRequestBus::Events::GetInertia)
                ->Event("ResetMassData", &Rigidbody2dRequestBus::Events::ResetMassData)
                ->Event("GetWorldPoint", &Rigidbody2dRequestBus::Events::GetWorldPoint)
                ->Event("GetWorldVector", &Rigidbody2dRequestBus::Events::GetWorldVector)
                ->Event("GetLocalPoint", &Rigidbody2dRequestBus::Events::GetLocalPoint)
                ->Event("GetLocalVector", &Rigidbody2dRequestBus::Events::GetLocalVector)
                ->Event("GetLinearVelocityFromWorldPoint", &Rigidbody2dRequestBus::Events::GetLinearVelocityFromWorldPoint)
                ->Event("GetLinearVelocityFromLocalPoint", &Rigidbody2dRequestBus::Events::GetLinearVelocityFromLocalPoint)
                ->Event("GetLinearDamping", &Rigidbody2dRequestBus::Events::GetLinearDamping)
                ->Event("SetLinearDamping", &Rigidbody2dRequestBus::Events::SetLinearDamping)
                ->Event("GetAngularDamping", &Rigidbody2dRequestBus::Events::GetAngularDamping)
                ->Event("SetAngularDamping", &Rigidbody2dRequestBus::Events::SetAngularDamping)
                ->Event("GetGravityScale", &Rigidbody2dRequestBus::Events::GetGravityScale)
                ->Event("SetGravityScale", &Rigidbody2dRequestBus::Events::SetGravityScale)
                ->Event("SetType", &Rigidbody2dRequestBus::Events::SetType)
                ->Event("GetType", &Rigidbody2dRequestBus::Events::GetType)
                ->Event("SetBullet", &Rigidbody2dRequestBus::Events::SetBullet)
                ->Event("IsBullet", &Rigidbody2dRequestBus::Events::IsBullet)
                ->Event("SetSleepingAllowed", &Rigidbody2dRequestBus::Events::SetSleepingAllowed)
                ->Event("IsSleepingAllowed", &Rigidbody2dRequestBus::Events::IsSleepingAllowed)
                ->Event("SetAwake", &Rigidbody2dRequestBus::Events::SetAwake)
                ->Event("IsAwake", &Rigidbody2dRequestBus::Events::IsAwake)
                ->Event("SetActive", &Rigidbody2dRequestBus::Events::SetActive)
                ->Event("IsActive", &Rigidbody2dRequestBus::Events::IsActive)
                ->Event("SetFixedRotation", &Rigidbody2dRequestBus::Events::SetFixedRotation)
                ->Event("IsFixedRotation", &Rigidbody2dRequestBus::Events::IsFixedRotation);

            behavior_context->Class<Rigidbody2dComponent>("Rigidbody2dComponent")
                ->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::ExcludeFlags::Preview)
                ->Constructor()
                ->Property("m_bodyType",
                    [](Rigidbody2dComponent* component) { return (int&)(component->m_bodyType); },
                    [](Rigidbody2dComponent* component, const int& i) { component->m_bodyType = (Physics2D::BodyType)i;})
                ->Property("m_linearDrag", BehaviorValueProperty(&Rigidbody2dComponent::m_linearDrag))
                ->Property("m_angularDrag", BehaviorValueProperty(&Rigidbody2dComponent::m_angularDrag))
                ->Property("m_gravityScale", BehaviorValueProperty(&Rigidbody2dComponent::m_gravityScale))
                ->Property("m_isAwake", BehaviorValueProperty(&Rigidbody2dComponent::m_isAwake))
                ->Property("m_isSleepingAllowed", BehaviorValueProperty(&Rigidbody2dComponent::m_isSleepingAllowed))
                ->Property("m_mass", BehaviorValueProperty(&Rigidbody2dComponent::m_mass));
        }
    }

    // AZ::Component
    void Rigidbody2dComponent::Activate()
    {
        AZ::SystemTickBus::Handler::BusConnect();
        AZ::TransformNotificationBus::Handler::BusConnect(GetEntityId());
        Rigidbody2dRequestBus::Handler::BusConnect(GetEntityId());

        AZ::Transform world;
        EBUS_EVENT_ID_RESULT(world, GetEntityId(), AZ::TransformBus, GetWorldTM);

        BodyDesc desc;
        desc.Type = m_bodyType;
        desc.LinearDamping = m_linearDrag;
        desc.AngularDamping = m_angularDrag;
        desc.Awake = m_isAwake;
        desc.AllowSleep = m_isSleepingAllowed;
        desc.GravityScale = m_gravityScale;
        desc.Position = AZ::Vector2(world.GetPosition().GetX(), world.GetPosition().GetY());

        EBUS_EVENT_RESULT(m_body, Physics2dSystemRequestBus, CreateBody, desc);
    }

    void Rigidbody2dComponent::Deactivate()
    {
        if(m_body)
        {
            EBUS_EVENT(Physics2dSystemRequestBus, DestroyBody, m_body);
            m_body = nullptr;
        }
        Rigidbody2dRequestBus::Handler::BusDisconnect();
        AZ::SystemTickBus::Handler::BusDisconnect();
        AZ::TransformNotificationBus::Handler::BusDisconnect();
    }

    void Rigidbody2dComponent::OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world)
    {
        if (m_body)
        {
            AZ::Vector3 worldRotation;
            EBUS_EVENT_ID_RESULT(worldRotation, GetEntityId(), AZ::TransformBus, GetWorldRotation);
            SetTransform(AZ::Vector2(world.GetPosition().GetX(), world.GetPosition().GetY()), worldRotation.GetZ());
        }
    }

    void Rigidbody2dComponent::OnSystemTick()
    {
        if (m_body)
        {
            EBUS_EVENT_ID(GetEntityId(), AZ::TransformBus, SetWorldTranslation, AZ::Vector3(GetPosition().GetX(), GetPosition().GetY(), 0));
        }
    }

    b2Fixture* Rigidbody2dComponent::CreateFixture(b2FixtureDef& def)
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        return this->m_body->CreateFixture(&def);
    }

    void Rigidbody2dComponent::SetTransform(const AZ::Vector2& position, float angle)
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        this->m_body->SetTransform(Physics::scaleDown(Fromfloat2(position)), angle);
    }

    AZ::Vector2 Rigidbody2dComponent::GetPosition() const
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        return Fromb2Vec2(Physics::scaleUp(this->m_body->GetPosition()));
    }

    float Rigidbody2dComponent::GetAngle() const
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        return this->m_body->GetAngle();
    }

    AZ::Vector2 Rigidbody2dComponent::GetWorldCenter() const
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        return Fromb2Vec2(Physics::scaleUp(this->m_body->GetWorldCenter()));
    }
    AZ::Vector2 Rigidbody2dComponent::GetLocalCenter() const
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        return Fromb2Vec2(Physics::scaleUp(this->m_body->GetLocalCenter()));
    }

    void Rigidbody2dComponent::SetLinearVelocity(const AZ::Vector2& v)
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        this->m_body->SetLinearVelocity(Physics::scaleDown(Fromfloat2(v)));
    }

    AZ::Vector2 Rigidbody2dComponent::GetLinearVelocity() const
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        return Fromb2Vec2(Physics::scaleUp(this->m_body->GetLinearVelocity()));
    }

    void Rigidbody2dComponent::SetAngularVelocity(float omega)
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        this->m_body->SetAngularVelocity(omega);
    }

    float Rigidbody2dComponent::GetAngularVelocity() const
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        return this->m_body->GetAngularVelocity();
    }

    void Rigidbody2dComponent::ApplyForce(const AZ::Vector2& force, const AZ::Vector2& point, bool wake)
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        this->m_body->ApplyForce(Physics::scaleDown(Fromfloat2(force)), Physics::scaleDown(Fromfloat2(point)), wake);
    }
    void Rigidbody2dComponent::ApplyForceToCenter(const AZ::Vector2& force, bool wake)
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        this->m_body->ApplyForceToCenter(Physics::scaleDown(Fromfloat2(force)), wake);
    }

    void Rigidbody2dComponent::ApplyTorque(float torque, bool wake)
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        this->m_body->ApplyTorque(Physics::scaleDown(Physics::scaleDown(torque)), wake);
    }

    void Rigidbody2dComponent::ApplyLinearImpulse(const AZ::Vector2& impulse, const AZ::Vector2& point, bool wake)
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        this->m_body->ApplyLinearImpulse(Physics::scaleDown(Fromfloat2(impulse)), Physics::scaleDown(Fromfloat2(point)), wake);
    }

    void Rigidbody2dComponent::ApplyAngularImpulse(float impulse, bool wake)
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        this->m_body->ApplyAngularImpulse(Physics::scaleDown(Physics::scaleDown(impulse)), wake);
    }

    float Rigidbody2dComponent::GetMass() const
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        return this->m_body->GetMass();
    }

    float Rigidbody2dComponent::GetInertia() const
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        return Physics::scaleUp(Physics::scaleUp(this->m_body->GetInertia()));
    }

    MassData Rigidbody2dComponent::GetMassData() const
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        b2MassData data;
        this->m_body->GetMassData(&data);
        return Fromb2MassData(data);
    }
    void Rigidbody2dComponent::SetMassData(const MassData* data)
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        b2MassData b2data = FromMassData(*data);
        this->m_body->SetMassData(&b2data);
    }
    void Rigidbody2dComponent::ResetMassData()
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        this->m_body->ResetMassData();
    }

    AZ::Vector2 Rigidbody2dComponent::GetWorldPoint(const AZ::Vector2& localPoint) const
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        return Fromb2Vec2(Physics::scaleUp(this->m_body->GetWorldPoint(Physics::scaleDown(Fromfloat2(localPoint)))));
    }
    AZ::Vector2 Rigidbody2dComponent::GetWorldVector(const AZ::Vector2& localVector) const
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        return Fromb2Vec2(Physics::scaleUp(this->m_body->GetWorldVector(Physics::scaleDown(Fromfloat2(localVector)))));
    }

    AZ::Vector2 Rigidbody2dComponent::GetLocalPoint(const AZ::Vector2& worldPoint) const
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        return Fromb2Vec2(Physics::scaleUp(this->m_body->GetLocalPoint(Physics::scaleDown(Fromfloat2(worldPoint)))));
    }
    AZ::Vector2 Rigidbody2dComponent::GetLocalVector(const AZ::Vector2& worldVector) const
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        return Fromb2Vec2(Physics::scaleUp(this->m_body->GetLocalVector(Physics::scaleDown(Fromfloat2(worldVector)))));
    }

    AZ::Vector2 Rigidbody2dComponent::GetLinearVelocityFromWorldPoint(const AZ::Vector2& worldPoint) const
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        return Fromb2Vec2(Physics::scaleUp(this->m_body->GetLinearVelocityFromWorldPoint(Physics::scaleDown(Fromfloat2(worldPoint)))));
    }
    AZ::Vector2 Rigidbody2dComponent::GetLinearVelocityFromLocalPoint(const AZ::Vector2& localPoint) const
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        return Fromb2Vec2(Physics::scaleUp(this->m_body->GetLinearVelocityFromLocalPoint(Physics::scaleDown(Fromfloat2(localPoint)))));
    }

    float Rigidbody2dComponent::GetLinearDamping() const
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        return this->m_body->GetLinearDamping();
    }
    void Rigidbody2dComponent::SetLinearDamping(float linearDamping)
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        this->m_body->SetLinearDamping(linearDamping);
    }

    float Rigidbody2dComponent::GetAngularDamping() const
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        return this->m_body->GetAngularDamping();
    }
    void Rigidbody2dComponent::SetAngularDamping(float angularDamping)
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        this->m_body->SetAngularDamping(angularDamping);
    }

    float Rigidbody2dComponent::GetGravityScale() const
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        return this->m_body->GetGravityScale();
    }
    void Rigidbody2dComponent::SetGravityScale(float scale)
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        this->m_body->SetGravityScale(scale);
    }

    void Rigidbody2dComponent::SetType(BodyType type)
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        this->m_body->SetType(b2BodyType(type));
    }
    BodyType Rigidbody2dComponent::GetType() const
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        return BodyType(this->m_body->GetType());
    }

    void Rigidbody2dComponent::SetBullet(bool flag)
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        this->m_body->SetBullet(flag);
    }
    bool Rigidbody2dComponent::IsBullet() const
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        return this->m_body->IsBullet();
    }

    void Rigidbody2dComponent::SetSleepingAllowed(bool flag)
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        this->m_body->SetSleepingAllowed(flag);
    }
    bool Rigidbody2dComponent::IsSleepingAllowed() const
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        return this->m_body->IsSleepingAllowed();
    }

    void Rigidbody2dComponent::SetAwake(bool flag)
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        this->m_body->SetAwake(flag);
    }
    bool Rigidbody2dComponent::IsAwake() const
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        return this->m_body->IsAwake();
    }

    void Rigidbody2dComponent::SetActive(bool flag)
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        this->m_body->SetActive(flag);
    }
    bool Rigidbody2dComponent::IsActive() const
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        return this->m_body->IsActive();
    }

    void Rigidbody2dComponent::SetFixedRotation(bool flag)
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        this->m_body->SetFixedRotation(flag);
    }
    bool Rigidbody2dComponent::IsFixedRotation() const
    {
        AZ_Assert(m_body != nullptr, "invalid body2d");

        return this->m_body->IsFixedRotation();
    }
}
