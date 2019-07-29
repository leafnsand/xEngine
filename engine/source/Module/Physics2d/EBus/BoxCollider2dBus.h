#pragma once

#include <AzCore/Component/ComponentBus.h>

namespace Module
{
    class BoxCollider2dRequest : public AZ::ComponentBus
    {
    public:
        virtual ~BoxCollider2dRequest() {}
    };

    using BoxCollider2dRequestBus = AZ::EBus<BoxCollider2dRequest>;

    class BoxCollider2dNotification : public AZ::ComponentBus
    {
    public:

        virtual ~BoxCollider2dNotification() {}
    };

    using BoxCollider2dNotificationBus = AZ::EBus<BoxCollider2dNotification>;
}
