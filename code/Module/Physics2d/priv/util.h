#pragma once

#include <Box2D/Box2D.h>

#include <AzCore/Math/Vector2.h>

namespace Physics2D
{
    enum class BodyType
    {
        Invalid = -1,
        Static = 0,
        Kinematic,
        Dynamic,
    };

    struct BodyDesc
    {
        BodyType Type = BodyType::Static;
        AZ::Vector2 Position;
        float Angle = 0.0f;
        AZ::Vector2 LinearVelocity;
        float AngularVelocity = 0.0f;
        float LinearDamping = 0.0f;
        float AngularDamping = 0.0f;
        bool AllowSleep = true;
        bool Awake = true;
        bool FixedRotation = false;
        bool Bullet = false;
        bool Active = true;
        float GravityScale = 1.0f;
    };

    struct MassData
    {
        float Mass = 0;
        AZ::Vector2 Center = AZ::Vector2::CreateZero();
        float I = 0;
    };

    inline AZ::Vector2 Fromb2Vec2(const b2Vec2& vec)
    {
        return AZ::Vector2(vec.x, vec.y);
    }

    inline b2Vec2 Fromfloat2(const AZ::Vector2& vec)
    {
        return b2Vec2(vec.GetX(), vec.GetY());
    }

    inline MassData Fromb2MassData(const b2MassData& data)
    {
        MassData ret;
        ret.Mass = data.mass;
        ret.Center = Fromb2Vec2(data.center);
        ret.I = data.I;
        return ret;
    }

    inline b2MassData FromMassData(const MassData& data)
    {
        b2MassData ret;
        ret.mass = data.Mass;
        ret.center = Fromfloat2(data.Center);
        ret.I = data.I;
        return ret;
    }

    inline b2BodyDef FromBodyDesc(const BodyDesc& desc)
    {
        b2BodyDef ret;
        ret.type = b2BodyType(desc.Type);
        ret.position = Fromfloat2(desc.Position);
        ret.angle = desc.Angle;
        ret.linearVelocity = Fromfloat2(desc.LinearVelocity);
        ret.angularVelocity = desc.AngularVelocity;
        ret.linearDamping = desc.LinearDamping;
        ret.angularDamping = desc.AngularDamping;
        ret.allowSleep = desc.AllowSleep;
        ret.awake = desc.Awake;
        ret.fixedRotation = desc.FixedRotation;
        ret.bullet = desc.Bullet;
        ret.active = desc.Active;
        ret.gravityScale = desc.GravityScale;
        return ret;
    }

    class Physics
    {
    public:
        static float meter;

        static float scaleDown(float f)
        {
            return f / meter;
        }

        static float scaleUp(float f)
        {
            return f*meter;
        }

        static void scaleDown(float &x, float &y)
        {
            x /= meter;
            y /= meter;
        }

        static void scaleUp(float &x, float &y)
        {
            x *= meter;
            y *= meter;
        }

        static b2Vec2 scaleDown(const b2Vec2 &v)
        {
            b2Vec2 t = v;
            scaleDown(t.x, t.y);
            return t;
        }

        static b2Vec2 scaleUp(const b2Vec2 &v)
        {
            b2Vec2 t = v;
            scaleUp(t.x, t.y);
            return t;
        }

        static b2AABB scaleDown(const b2AABB &aabb)
        {
            b2AABB t;
            t.lowerBound = scaleDown(aabb.lowerBound);
            t.upperBound = scaleDown(aabb.upperBound);
            return t;
        }

        static b2AABB scaleUp(const b2AABB &aabb)
        {
            b2AABB t;
            t.lowerBound = scaleUp(aabb.lowerBound);
            t.upperBound = scaleUp(aabb.upperBound);
            return t;
        }
    };
}
