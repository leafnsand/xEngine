#pragma once

#include <AzCore/Component/ComponentBus.h>

namespace Module
{
    class CircleCollider2dRequest : public AZ::ComponentBus
    {
    public:

        virtual ~CircleCollider2dRequest() {}
    };

    using CircleCollider2dRequestBus = AZ::EBus<CircleCollider2dRequest>;

    class CircleCollider2dNotification : public AZ::ComponentBus
    {
    public:

        virtual ~CircleCollider2dNotification() {}
    };

    using CircleCollider2dNotificationBus = AZ::EBus<CircleCollider2dNotification>;
}
