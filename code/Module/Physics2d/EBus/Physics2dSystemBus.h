#pragma once

#include <AzCore/EBus/EBus.h>
#include <AzCore/Math/Vector2.h>
#include <AzCore/RTTI/BehaviorContext.h>

#include "Physics2d/priv/util.h"

namespace Module
{
    class Physics2dSystemRequest : public AZ::EBusTraits
    {
    public:
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;

        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;

        virtual ~Physics2dSystemRequest() {}

        virtual b2Body* CreateBody(const Physics2D::BodyDesc& desc) = 0;

        virtual void DestroyBody(b2Body* body) = 0;

        virtual void SetGravity(const AZ::Vector2& gravity) = 0;

        virtual AZ::Vector2 GetGravity() = 0;

        virtual void ShiftOrigin(const AZ::Vector2& newOrigin) = 0;
    };

    using Physics2dSystemRequestBus = AZ::EBus<Physics2dSystemRequest>;

    class Physics2dSystemNotification : public AZ::EBusTraits
    {
    public:
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Multiple;

        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;

        virtual ~Physics2dSystemNotification() {}

        virtual void OnBeginContact(AZ::EntityId entityIdA, AZ::EntityId entityIdB) {};

        virtual void OnEndContact(AZ::EntityId entityIdA, AZ::EntityId entityIdB) {};
    };

    using Physics2dSystemNotificationBus = AZ::EBus<Physics2dSystemNotification>;

    class Physics2dSystemNotificationBusHandler : public Physics2dSystemNotificationBus::Handler, public AZ::BehaviorEBusHandler
    {
    public:
        AZ_EBUS_BEHAVIOR_BINDER(Physics2dSystemNotificationBusHandler, "{8A54349C-C018-4806-A7BF-285F8914A1AB}", AZ::SystemAllocator,
            OnBeginContact, OnEndContact);

        void OnBeginContact(AZ::EntityId entityIdA, AZ::EntityId entityIdB) override
        {
            Call(FN_OnBeginContact, entityIdA, entityIdB);
        }

        void OnEndContact(AZ::EntityId entityIdA, AZ::EntityId entityIdB) override
        {
            Call(FN_OnEndContact, entityIdA, entityIdB);
        }
    };
}
