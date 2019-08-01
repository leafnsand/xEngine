#pragma once

#include <AzCore/Component/ComponentBus.h>

namespace Module
{
    class Collider2dRequest : public AZ::ComponentBus
    {
    public:
        virtual ~Collider2dRequest() {}
    };

    using Collider2dRequestBus = AZ::EBus<Collider2dRequest>;

    class Collider2dNotification : public AZ::ComponentBus
    {
    public:

        virtual ~Collider2dNotification() {}
    };

    using Collider2dNotificationBus = AZ::EBus<Collider2dNotification>;
}
