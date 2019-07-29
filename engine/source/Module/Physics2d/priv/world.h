#pragma once
#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/Math/Vector2.h>

#include <Box2D/Box2D.h>

namespace Physics2D
{
    struct BodyDesc;

    class World
        : public b2ContactListener
        , public b2ContactFilter
        , public b2DestructionListener
    {
    public:
        AZ_CLASS_ALLOCATOR(World, AZ::SystemAllocator, 0);

        void Setup(const AZ::Vector2& gravity);

        void Discard();

        b2Body* CreateBody(const BodyDesc& desc);
        void DestroyBody(b2Body* body);

        void SetGravity(const AZ::Vector2& gravity);
        AZ::Vector2 GetGravity() const;

        void ShiftOrigin(const AZ::Vector2& newOrigin);

        // public b2ContactListener
        void BeginContact(b2Contact* contact) override;
        void EndContact(b2Contact* contact) override;
        void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override;
        void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) override;

        // public b2ContactFilter
        bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB) override;

        // public b2DestructionListener
        void SayGoodbye(b2Fixture* fixture) override;
        void SayGoodbye(b2Joint* joint) override;

        b2World* Box2DWorld = nullptr;
    };
}
