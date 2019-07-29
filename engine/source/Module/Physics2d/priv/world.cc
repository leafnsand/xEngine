#include "world.h"

#include <AzCore/Component/Entity.h>

#include "Physics2d/priv/util.h"
#include "Physics2d/EBus/Physics2dSystemBus.h"

namespace Physics2D
{    
    float Physics::meter = 1.0f;

    void World::Setup(const AZ::Vector2& gravity)
    {
        Box2DWorld = new b2World(b2Vec2(gravity.GetX(), gravity.GetY()));
        this->Box2DWorld->SetAllowSleeping(true);
        this->Box2DWorld->SetContactListener(this);
        this->Box2DWorld->SetContactFilter(this);
        this->Box2DWorld->SetDestructionListener(this);
    }

    void World::Discard()
    {
        delete(this->Box2DWorld->GetdebugDraw());

        delete this->Box2DWorld;
        this->Box2DWorld = nullptr;
    }

    b2Body* World::CreateBody(const BodyDesc& desc)
    {
        b2BodyDef def = FromBodyDesc(desc);
        def.position = Physics::scaleDown(def.position);
        return this->Box2DWorld->CreateBody(&def);
    }

    void World::DestroyBody(b2Body* body)
    {
        this->Box2DWorld->DestroyBody(body);
    }
    
    void World::SetGravity(const AZ::Vector2& gravity)
    {
        this->Box2DWorld->SetGravity(Fromfloat2(gravity));
    }

    AZ::Vector2 World::GetGravity() const
    {
        return Fromb2Vec2(this->Box2DWorld->GetGravity());
    }

    void World::ShiftOrigin(const AZ::Vector2& newOrigin)
    {
        this->Box2DWorld->ShiftOrigin(Fromfloat2(newOrigin));
    }

    void World::BeginContact(b2Contact* contact)
    {
        auto fixtureA = contact->GetFixtureA();
        auto fixtureB = contact->GetFixtureB();
        if(fixtureA && fixtureB)
        {
            auto entityA = (AZ::Entity*)fixtureA->GetUserData();
            auto entityB = (AZ::Entity*)fixtureB->GetUserData();
            if(entityA && entityB)
            {
                EBUS_EVENT(Module::Physics2dSystemNotificationBus, OnBeginContact, entityA->GetId(), entityB->GetId());
            }
        }
    }

    void World::EndContact(b2Contact* contact)
    {
        auto fixtureA = contact->GetFixtureA();
        auto fixtureB = contact->GetFixtureB();
        if (fixtureA && fixtureB)
        {
            auto entityA = (AZ::Entity*)fixtureA->GetUserData();
            auto entityB = (AZ::Entity*)fixtureB->GetUserData();
            if (entityA && entityB)
            {
                EBUS_EVENT(Module::Physics2dSystemNotificationBus, OnEndContact, entityA->GetId(), entityB->GetId());
            }
        }
    }

    void World::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
    {
        auto fixtureA = contact->GetFixtureA();
        auto fixtureB = contact->GetFixtureB();
        if (fixtureA && fixtureB)
        {
            auto entityA = (AZ::Entity*)fixtureA->GetUserData();
            auto entityB = (AZ::Entity*)fixtureB->GetUserData();
            if (entityA && entityB)
            {

            }
        }
    }

    void World::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
    {
        auto fixtureA = contact->GetFixtureA();
        auto fixtureB = contact->GetFixtureB();
        if (fixtureA && fixtureB)
        {
            auto entityA = (AZ::Entity*)fixtureA->GetUserData();
            auto entityB = (AZ::Entity*)fixtureB->GetUserData();
            if (entityA && entityB)
            {

            }
        }
    }

    bool World::ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB)
    {
        return true;
    }

    void World::SayGoodbye(b2Fixture* fixture)
    {
    }

    void World::SayGoodbye(b2Joint* joint)
    {
    }
}
