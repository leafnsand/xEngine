#pragma once

#include <AzCore/Component/ComponentBus.h>

namespace Module
{
    class EdgeCollider2dRequest : public AZ::ComponentBus
    {
    public:

        virtual ~EdgeCollider2dRequest() {}
    };

    using EdgeCollider2dRequestBus = AZ::EBus<EdgeCollider2dRequest>;

    class EdgeCollider2dNotification : public AZ::ComponentBus
    {
    public:

        virtual ~EdgeCollider2dNotification() {}
    };

    using EdgeCollider2dNotificationBus = AZ::EBus<EdgeCollider2dNotification>;
}
